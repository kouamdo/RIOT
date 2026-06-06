/*
 * Practice 2, Exercise 8: LoRaWAN Smart Lock Security Analysis
 * 
 * Implement a simple LoRaWAN-based smart lock that opens when receiving
 * a specific payload. Analyze its vulnerability to replay and jamming attacks.
 * 
 * Objectives:
 * - Design a LoRaWAN smart lock protocol
 * - Identify security vulnerabilities
 * - Demonstrate or mitigate replay attacks
 * - Explain jamming vulnerability
 * - Propose security enhancements
 * 
 * Vulnerability Analysis:
 * - Simple payload matching is vulnerable to replay attacks
 * - No timestamps or sequence numbers for verification
 * - No encryption or authentication on application layer
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <xtimer.h>

/**
 * Smart Lock State
 */
typedef enum {
    LOCK_LOCKED = 0,
    LOCK_UNLOCKED = 1,
    LOCK_ERROR = 2
} lock_state_t;

/**
 * LoRaWAN received packet
 */
typedef struct {
    uint8_t payload[64];
    uint16_t payload_len;
    int8_t rssi;
    uint8_t snr;
    uint32_t timestamp;
} lorawan_packet_t;

/**
 * Smart Lock Device
 */
typedef struct {
    lock_state_t state;
    uint32_t unlock_time;
    uint32_t last_command_time;
    uint16_t failed_attempts;
    uint8_t is_jammed;
} smart_lock_t;

static smart_lock_t lock = {
    .state = LOCK_LOCKED,
    .failed_attempts = 0,
    .is_jammed = 0,
};

/* Expected unlock command */
#define UNLOCK_COMMAND_LEN 4
static const uint8_t unlock_command[] = {'o', 'p', 'e', 'n'};

/**
 * VULNERABLE: Simple payload matching without authentication
 */
static void vulnerable_lock_handler(const lorawan_packet_t *pkt)
{
    printf("[Vulnerable Implementation]\n");
    printf("  Packet length: %u bytes\n", pkt->payload_len);
    printf("  Payload: ");
    for (int i = 0; i < pkt->payload_len; i++) {
        printf("%02X ", pkt->payload[i]);
    }
    printf("\n");
    
    /* Simple comparison - VULNERABLE to replay attacks */
    if (pkt->payload_len == UNLOCK_COMMAND_LEN &&
        memcmp(pkt->payload, unlock_command, UNLOCK_COMMAND_LEN) == 0) {
        
        printf("  ✗ VULNERABILITY: Payload matches unlock command!\n");
        printf("  ✗ No authentication, no timestamp, no nonce\n");
        printf("  ✗ Attacker can replay this packet indefinitely\n");
        
        lock.state = LOCK_UNLOCKED;
        lock.unlock_time = xtimer_now_usec() / 1000;
        
        printf("  LOCK OPENED (insecure)\n");
    }
}

/**
 * SECURE: Implements nonce and timeout based authentication
 */
static void secure_lock_handler(const lorawan_packet_t *pkt)
{
    printf("\n[Secure Implementation with Counter]\n");
    printf("  Packet length: %u bytes\n", pkt->payload_len);
    
    /* Expected: 4 bytes for "open" + 2 bytes for sequence counter */
    if (pkt->payload_len != (UNLOCK_COMMAND_LEN + 2)) {
        printf("  Invalid packet format\n");
        return;
    }
    
    /* Verify command part */
    if (memcmp(pkt->payload, unlock_command, UNLOCK_COMMAND_LEN) != 0) {
        printf("  Invalid command\n");
        return;
    }
    
    /* Extract and verify sequence counter */
    uint16_t counter = (pkt->payload[4] << 8) | pkt->payload[5];
    uint16_t expected_counter = lock.failed_attempts + 1;
    
    printf("  Sequence counter: %u (expected: %u)\n", counter, expected_counter);
    
    if (counter == expected_counter) {
        printf("  ✓ Counter verification passed\n");
        lock.state = LOCK_UNLOCKED;
        lock.unlock_time = xtimer_now_usec() / 1000;
        lock.failed_attempts++;
        printf("  LOCK OPENED (secure)\n");
    } else if (counter < expected_counter) {
        printf("  ✗ Counter too low - possible replay attack detected!\n");
        lock.state = LOCK_ERROR;
        lock.failed_attempts++;
    } else {
        printf("  ✗ Counter mismatch\n");
    }
}

/**
 * Demonstrate vulnerability to replay attacks
 */
static void demonstrate_replay_attack(void)
{
    printf("=== Replay Attack Demonstration ===\n\n");
    
    /* Legitimate unlock command */
    lorawan_packet_t legitimate = {
        .payload = {'o', 'p', 'e', 'n'},
        .payload_len = 4,
        .rssi = -95,
        .snr = 10,
        .timestamp = xtimer_now_usec() / 1000,
    };
    
    printf("1. First legitimate unlock command:\n");
    vulnerable_lock_handler(&legitimate);
    
    printf("\n2. Attacker replays the same packet:\n");
    vulnerable_lock_handler(&legitimate);
    
    printf("\n3. Attacker replays multiple times:\n");
    for (int i = 0; i < 3; i++) {
        printf("   Replay attempt %d: ", i + 1);
        if (lock.state == LOCK_UNLOCKED) {
            printf("SUCCESS\n");
        }
    }
    
    printf("\nResult: Lock remains vulnerable to replayed packets\n");
}

/**
 * Demonstrate jamming vulnerability
 */
static void demonstrate_jamming_attack(void)
{
    printf("\n=== Jamming Attack Vulnerability ===\n\n");
    
    printf("LoRaWAN Jamming Attack Scenario:\n");
    printf("1. Attacker transmits high-power noise on LoRa frequency\n");
    printf("2. All legitimate communications are blocked\n");
    printf("3. Lock cannot receive unlock commands\n\n");
    
    printf("Vulnerability Details:\n");
    printf("  - Spread spectrum (LoRa) is moderately resistant\n");
    printf("  - But directed, sustained jamming will succeed\n");
    printf("  - No authentication prevents key confusion\n");
    printf("  - Lock has no anti-jamming measures\n\n");
    
    printf("Impact on Smart Lock:\n");
    printf("  - Lock becomes inaccessible (denial of service)\n");
    printf("  - No timeout mechanism to default to locked state\n");
    printf("  - Could be combined with physical attack\n");
}

/**
 * Mitigation strategies
 */
static void discuss_mitigations(void)
{
    printf("\n=== Security Mitigations ===\n\n");
    
    printf("1. Counter/Sequence Number Based:\n");
    printf("   - Lock tracks expected counter value\n");
    printf("   - Rejects packets with old counter values\n");
    printf("   - Prevents simple replay attacks\n\n");
    
    printf("2. Cryptographic Authentication:\n");
    printf("   - HMAC or signature on command\n");
    printf("   - Requires shared secret or public key\n");
    printf("   - Prevents forged commands\n\n");
    
    printf("3. Time-based OTP (One-Time Password):\n");
    printf("   - Uses TOTP algorithm\n");
    printf("   - Synchronized with server clock\n");
    printf("   - Each command valid for limited time\n\n");
    
    printf("4. Challenge-Response Protocol:\n");
    printf("   - Server sends challenge\n");
    printf("   - Lock responds with challenge+secret hash\n");
    printf("   - No single packet can unlock device\n\n");
    
    printf("5. Anti-Jamming Techniques:\n");
    printf("   - Frequency hopping (if supported)\n");
    printf("   - Lockout after jamming detection\n");
    printf("   - Alert on communication failure\n");
    printf("   - Fallback to local authentication\n");
}

int main(void)
{
    printf("=== Practice 2, Exercise 8: LoRaWAN Smart Lock Security ===\n\n");
    
    printf("Simple Smart Lock Protocol:\n");
    printf("  Command: 4-byte payload 'open' (0x6F70656E)\n");
    printf("  Action: Unlock the door\n");
    printf("  Network: LoRaWAN Class A\n\n");
    
    /* Demonstrate the vulnerability */
    demonstrate_replay_attack();
    
    /* Show jamming vulnerability */
    demonstrate_jamming_attack();
    
    /* Explain mitigations */
    discuss_mitigations();
    
    printf("\n=== Security Analysis Summary ===\n\n");
    printf("Vulnerabilities Found:\n");
    printf("  1. Vulnerable to replay attacks\n");
    printf("     Severity: HIGH\n");
    printf("     Exploit: Capture and retransmit unlock command\n\n");
    printf("  2. Vulnerable to jamming attacks\n");
    printf("     Severity: MEDIUM\n");
    printf("     Exploit: Jam frequency, then physically attack\n\n");
    printf("  3. No encryption or authentication\n");
    printf("     Severity: CRITICAL\n");
    printf("     Exploit: Forge unlock commands\n\n");
    
    printf("Recommended Security Improvements:\n");
    printf("  Priority 1: Implement sequence numbers\n");
    printf("  Priority 2: Add cryptographic signatures\n");
    printf("  Priority 3: Implement challenge-response\n");
    printf("  Priority 4: Add intrusion detection\n");
    
    printf("\n=== Exercise Complete ===\n");
    return 0;
}
