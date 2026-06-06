/*
 * Practice 2, Exercise 5: ECG data processing for a BLE heart-rate service.
 *
 * ESP32-only mode: process a small ECG-like sample array, detect R peaks, and
 * compute a heart-rate value. The calculated BPM can later feed the standard
 * NimBLE Heart Rate Service characteristic.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "xtimer.h"

#define ECG_SAMPLE_RATE_HZ      (1000U)
#define ECG_THRESHOLD           (650)
#define ECG_REFRACTORY_MS       (250U)

static const int16_t ecg_samples[] = {
    508, 512, 515, 520, 530, 565, 720, 620, 545, 522,
    510, 506, 512, 518, 526, 560, 735, 625, 548, 524,
    511, 507, 513, 519, 528, 562, 742, 628, 550, 525,
    512, 508, 514, 520, 529, 566, 730, 622, 546, 523,
    510, 507, 513, 518, 527, 563, 738, 626, 549, 524,
};

typedef struct {
    uint16_t peaks;
    uint32_t first_peak_ms;
    uint32_t last_peak_ms;
} ecg_result_t;

static bool is_local_peak(size_t i)
{
    return (ecg_samples[i] > ecg_samples[i - 1]) &&
           (ecg_samples[i] > ecg_samples[i + 1]);
}

static ecg_result_t analyze_ecg(void)
{
    ecg_result_t result = { 0 };
    uint32_t last_peak_ms = 0;
    bool has_peak = false;

    for (size_t i = 1; i + 1 < (sizeof(ecg_samples) / sizeof(ecg_samples[0])); i++) {
        uint32_t now_ms = (uint32_t)((i * 1000U) / ECG_SAMPLE_RATE_HZ);
        bool above_threshold = ecg_samples[i] >= ECG_THRESHOLD;
        bool outside_refractory = !has_peak || ((now_ms - last_peak_ms) >= ECG_REFRACTORY_MS);

        if (is_local_peak(i) && above_threshold && outside_refractory) {
            if (!has_peak) {
                result.first_peak_ms = now_ms;
            }

            result.peaks++;
            result.last_peak_ms = now_ms;
            last_peak_ms = now_ms;
            has_peak = true;

            printf("R peak %u at %lu ms, sample=%d\n",
                   result.peaks, (unsigned long)now_ms, ecg_samples[i]);
        }
    }

    return result;
}

static uint32_t estimate_bpm(const ecg_result_t *result, uint32_t duration_ms)
{
    if (result->peaks >= 2U && result->last_peak_ms > result->first_peak_ms) {
        uint32_t rr_total_ms = result->last_peak_ms - result->first_peak_ms;
        return ((uint32_t)(result->peaks - 1U) * 60000U) / rr_total_ms;
    }

    if (duration_ms == 0U) {
        return 0U;
    }

    return ((uint32_t)result->peaks * 60000U) / duration_ms;
}

int main(void)
{
    size_t sample_count = sizeof(ecg_samples) / sizeof(ecg_samples[0]);
    uint32_t duration_ms = (uint32_t)((sample_count * 1000U) / ECG_SAMPLE_RATE_HZ);

    puts("=== Practice 2, Exercise 5: ECG Heart Rate Simulation ===");
    puts("ESP32-only mode: ECG processing before adding NimBLE Heart Rate Service.");
    puts("Replace ecg_samples with examples-miem/dsp/ecg.h data when available.\n");

    ecg_result_t result = analyze_ecg();
    uint32_t bpm = estimate_bpm(&result, duration_ms);

    printf("Samples: %u at %u Hz\n", (unsigned)sample_count, ECG_SAMPLE_RATE_HZ);
    printf("Duration: %lu ms\n", (unsigned long)duration_ms);
    printf("Detected R peaks: %u\n", result.peaks);
    printf("Estimated heart rate: %lu BPM\n", (unsigned long)bpm);

    xtimer_msleep(100);
    return 0;
}
