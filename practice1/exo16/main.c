#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "board.h"
#include "irq.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "xtimer.h"

/* Modbus RTU slave on the standard console UART. */
#define MODBUS_UART             UART_DEV(0)
#define MODBUS_BAUD             (115200U)
#define MODBUS_SLAVE_ID         (0x01U)
#define MODBUS_RX_BUFSIZE       (128U)

#define MB_FC_READ_HOLDING      (0x03U)
#define MB_FC_WRITE_SINGLE      (0x06U)
#define MB_FC_WRITE_MULTIPLE    (0x10U)

#define MB_EX_ILLEGAL_FUNCTION  (0x01U)
#define MB_EX_ILLEGAL_ADDRESS   (0x02U)
#define MB_EX_ILLEGAL_VALUE     (0x03U)

#define MODBUS_FRAME_TIMEOUT_MS (5U)

#ifndef EXO16_LED_PIN
#define EXO16_LED_PIN GPIO2
#endif

#define HOLDING_REG_COUNT       (8U)
#define REG_TEMPERATURE         (0U)
#define REG_HUMIDITY            (1U)
#define REG_PRESSURE            (2U)
#define REG_LED_STATE           (3U)

static volatile uint8_t rx_buffer[MODBUS_RX_BUFSIZE];
static volatile size_t rx_len = 0;
static volatile uint32_t last_rx_time_ms = 0;

static uint16_t holding_registers[HOLDING_REG_COUNT] = {
    [REG_TEMPERATURE] = 234U,   /* 23.4 C */
    [REG_HUMIDITY]    = 452U,   /* 45.2 % */
    [REG_PRESSURE]    = 10132U, /* 1013.2 hPa */
    [REG_LED_STATE]   = 0U,
};

static uint32_t now_ms(void)
{
    return xtimer_now_usec() / 1000U;
}

static uint16_t modbus_crc16(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFFU;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (unsigned bit = 0; bit < 8; bit++) {
            if (crc & 0x0001U) {
                crc = (crc >> 1) ^ 0xA001U;
            }
            else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

static void append_crc(uint8_t *frame, size_t payload_len)
{
    uint16_t crc = modbus_crc16(frame, payload_len);
    frame[payload_len] = (uint8_t)(crc & 0xFFU);
    frame[payload_len + 1U] = (uint8_t)(crc >> 8);
}

static bool check_crc(const uint8_t *frame, size_t len)
{
    if (len < 4U) {
        return false;
    }

    uint16_t received_crc = (uint16_t)frame[len - 2U] |
                            ((uint16_t)frame[len - 1U] << 8);
    uint16_t expected_crc = modbus_crc16(frame, len - 2U);
    return (received_crc == expected_crc);
}

static void apply_outputs_from_registers(void)
{
    if (holding_registers[REG_LED_STATE] != 0U) {
        gpio_set(EXO16_LED_PIN);
        holding_registers[REG_LED_STATE] = 1U;
    }
    else {
        gpio_clear(EXO16_LED_PIN);
    }
}

static size_t build_exception_response(uint8_t slave_id, uint8_t function,
                                       uint8_t exception, uint8_t *response)
{
    response[0] = slave_id;
    response[1] = function | 0x80U;
    response[2] = exception;
    append_crc(response, 3U);
    return 5U;
}

static size_t handle_read_holding(const uint8_t *request, uint8_t *response)
{
    uint16_t start_addr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t quantity = ((uint16_t)request[4] << 8) | request[5];

    if ((quantity == 0U) || (quantity > 0x7DU)) {
        return build_exception_response(request[0], request[1],
                                        MB_EX_ILLEGAL_VALUE, response);
    }

    if ((start_addr + quantity) > HOLDING_REG_COUNT) {
        return build_exception_response(request[0], request[1],
                                        MB_EX_ILLEGAL_ADDRESS, response);
    }

    response[0] = request[0];
    response[1] = request[1];
    response[2] = (uint8_t)(quantity * 2U);

    for (uint16_t i = 0; i < quantity; i++) {
        uint16_t value = holding_registers[start_addr + i];
        response[3U + (2U * i)] = (uint8_t)(value >> 8);
        response[4U + (2U * i)] = (uint8_t)(value & 0xFFU);
    }

    size_t payload_len = 3U + (2U * quantity);
    append_crc(response, payload_len);
    return payload_len + 2U;
}

static size_t handle_write_single(const uint8_t *request, uint8_t *response)
{
    uint16_t reg_addr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t value = ((uint16_t)request[4] << 8) | request[5];

    if (reg_addr >= HOLDING_REG_COUNT) {
        return build_exception_response(request[0], request[1],
                                        MB_EX_ILLEGAL_ADDRESS, response);
    }

    holding_registers[reg_addr] = value;
    apply_outputs_from_registers();

    memcpy(response, request, 6U);
    append_crc(response, 6U);
    return 8U;
}

static size_t handle_write_multiple(const uint8_t *request, size_t req_len,
                                    uint8_t *response)
{
    uint16_t start_addr = ((uint16_t)request[2] << 8) | request[3];
    uint16_t quantity = ((uint16_t)request[4] << 8) | request[5];
    uint8_t byte_count = request[6];

    if ((quantity == 0U) || (quantity > 0x7BU)) {
        return build_exception_response(request[0], request[1],
                                        MB_EX_ILLEGAL_VALUE, response);
    }

    if ((start_addr + quantity) > HOLDING_REG_COUNT) {
        return build_exception_response(request[0], request[1],
                                        MB_EX_ILLEGAL_ADDRESS, response);
    }

    if (byte_count != (uint8_t)(quantity * 2U)) {
        return build_exception_response(request[0], request[1],
                                        MB_EX_ILLEGAL_VALUE, response);
    }

    if (req_len < (size_t)(7U + byte_count + 2U)) {
        return build_exception_response(request[0], request[1],
                                        MB_EX_ILLEGAL_VALUE, response);
    }

    for (uint16_t i = 0; i < quantity; i++) {
        uint16_t value = ((uint16_t)request[7U + (2U * i)] << 8) |
                         request[8U + (2U * i)];
        holding_registers[start_addr + i] = value;
    }

    apply_outputs_from_registers();

    response[0] = request[0];
    response[1] = request[1];
    response[2] = request[2];
    response[3] = request[3];
    response[4] = request[4];
    response[5] = request[5];
    append_crc(response, 6U);
    return 8U;
}

static size_t process_modbus_request(const uint8_t *request, size_t req_len,
                                     uint8_t *response)
{
    if (req_len < 8U) {
        return 0U;
    }

    if (request[0] != MODBUS_SLAVE_ID) {
        return 0U;
    }

    if (!check_crc(request, req_len)) {
        return 0U;
    }

    switch (request[1]) {
        case MB_FC_READ_HOLDING:
            return handle_read_holding(request, response);

        case MB_FC_WRITE_SINGLE:
            return handle_write_single(request, response);

        case MB_FC_WRITE_MULTIPLE:
            return handle_write_multiple(request, req_len, response);

        default:
            return build_exception_response(request[0], request[1],
                                            MB_EX_ILLEGAL_FUNCTION, response);
    }
}

static void uart_rx_cb(void *arg, uint8_t data)
{
    (void)arg;

    uint32_t now = now_ms();

    if ((rx_len > 0U) && ((now - last_rx_time_ms) > MODBUS_FRAME_TIMEOUT_MS)) {
        rx_len = 0U;
    }

    last_rx_time_ms = now;

    if (rx_len < MODBUS_RX_BUFSIZE) {
        rx_buffer[rx_len++] = data;
    }
    else {
        rx_len = 0U;
    }
}

int main(void)
{
    gpio_init(EXO16_LED_PIN, GPIO_OUT);
    apply_outputs_from_registers();

    if (uart_init(MODBUS_UART, MODBUS_BAUD, uart_rx_cb, NULL) != UART_OK) {
        return 1;
    }

    uint8_t request[MODBUS_RX_BUFSIZE];
    uint8_t response[MODBUS_RX_BUFSIZE];

    while (1) {
        size_t frame_len = 0U;
        uint32_t current_ms = now_ms();

        if ((rx_len > 0U) && ((current_ms - last_rx_time_ms) > MODBUS_FRAME_TIMEOUT_MS)) {
            unsigned state = irq_disable();
            frame_len = rx_len;
            if (frame_len > 0U) {
                memcpy(request, (const void *)rx_buffer, frame_len);
                rx_len = 0U;
            }
            irq_restore(state);
        }

        if (frame_len > 0U) {
            size_t response_len = process_modbus_request(request, frame_len, response);
            if (response_len > 0U) {
                uart_write(MODBUS_UART, response, response_len);
            }
        }

        /* Refresh fake sensor values to simulate changing data. */
        holding_registers[REG_TEMPERATURE] = 230U + (uint16_t)((current_ms / 1000U) % 10U);
        holding_registers[REG_HUMIDITY] = 450U + (uint16_t)((current_ms / 2000U) % 10U);
        holding_registers[REG_PRESSURE] = 10130U + (uint16_t)((current_ms / 3000U) % 5U);

        xtimer_msleep(1);
    }

    return 0;
}
