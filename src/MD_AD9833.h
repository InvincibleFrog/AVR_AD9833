#ifndef MD_AD9833_H
#define MD_AD9833_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Default frequency and phase values.
 */
#define AD_DEFAULT_FREQ 1000
#define AD_DEFAULT_PHASE 0

/**
 * @brief Pin definitions for serial communication.
 */
#define DATA 3 /**< Serial Data Input (DATA is connected at PB3) */
#define CLK 5 /**< Serial Clock Input (SCK is connected at PB5) */
#define FSYNC 2 /**< Active Low Control Input (FSYNC is connected at PB2) */

/**
 * @brief Frequency and phase select masks.
 */
#define SEL_FREQ0 (1<<AD_FREQ0)
#define SEL_FREQ1 (1<<AD_FREQ1)
#define SEL_PHASE0 (1<<AD_FREQ0 | 1<<AD_FREQ1 | 0<<AD_PHASE)
#define SEL_PHASE1 (1<<AD_FREQ0 | 1<<AD_FREQ1 | 1<<AD_PHASE)

/**
 * @brief Frequency and phase calculation macros.
 */
#define AD_MCLK 25000000UL
#define AD_2POW28 (1UL << 28)

/**
 * @brief AD9833 register addresses.
 */
extern const uint8_t AD_B28;
extern const uint8_t AD_HLB;
extern const uint8_t AD_FSELECT;
extern const uint8_t AD_PSELECT;
extern const uint8_t AD_RESET;
extern const uint8_t AD_SLEEP1;
extern const uint8_t AD_SLEEP12;
extern const uint8_t AD_OPBITEN;
extern const uint8_t AD_DIV2;
extern const uint8_t AD_MODE;
extern const uint8_t AD_FREQ1;
extern const uint8_t AD_FREQ0;
extern const uint8_t AD_PHASE;

/**
 * @brief Channel enumeration for AD9833.
 */
typedef enum channel
{
    CHAN_0 = 0,
    CHAN_1 = 1,
} channel_t;

/**
 * @brief Mode enumeration for AD9833.
 */
typedef enum mode
{
    MODE_OFF,
    MODE_SINE,
    MODE_SQUARE1,
    MODE_SQUARE2,
    MODE_TRIANGLE,
} mode_t;

/**
 * @brief AD9833 control and configuration variables.
 */
uint16_t _regCtl;
uint32_t _regFreq[2];
uint32_t _regPhase[2];
mode_t _modeLast;
float _freq[2];
uint16_t _phase[2];

/**
 * @brief Initialize AD9833.
 */
void AD_init(void);

/**
 * @brief Reset AD9833.
 * @param hold If true, the reset pin will be held low.
 */
void AD_reset(bool hold);

/**
 * @brief Write data to AD9833.
 * @param data The 16-bit data to be written.
 */
void AD_write16(uint16_t data);

/**
 * @brief Get the current mode of AD9833.
 * @return The current mode.
 */
mode_t AD_getMode(void);

/**
 * @brief Set the mode of AD9833.
 * @param mode The mode to be set.
 * @return True if the mode was successfully set, false otherwise.
 */
bool AD_setMode(mode_t mode);

/**
 * @brief Get the active frequency channel of AD9833.
 * @return The active frequency channel.
 */
channel_t AD_getActiveFrequency(void);

/**
 * @brief Set the active frequency channel of AD9833.
 * @param chan The channel to be set as active frequency channel.
 * @return True if the channel was successfully set, false otherwise.
 */
bool AD_setActiveFrequency(channel_t chan);

/**
 * @brief Get the frequency value of a specific channel.
 * @param chan The channel to retrieve the frequency from.
 * @return The frequency value of the specified channel.
 */
const float AD_getFrequency(channel_t chan);

/**
 * @brief Set the frequency value of a specific channel.
 * @param chan The channel to set the frequency for.
 * @param freq The frequency value to be set.
 * @return True if the frequency was successfully set, false otherwise.
 */
bool AD_setFrequency(channel_t chan, float freq);

/**
 * @brief Get the active phase channel of AD9833.
 * @return The active phase channel.
 */
channel_t AD_getActivePhase(void);

/**
 * @brief Set the active phase channel of AD9833.
 * @param chan The channel to be set as active phase channel.
 * @return True if the channel was successfully set, false otherwise.
 */
bool AD_setActivePhase(channel_t chan);

/**
 * @brief Get the phase value of a specific channel.
 * @param chan The channel to retrieve the phase from.
 * @return The phase value of the specified channel.
 */
uint16_t AD_getPhase(channel_t chan);

/**
 * @brief Set the phase value of a specific channel.
 * @param chan The channel to set the phase for.
 * @param phase The phase value to be set.
 * @return True if the phase was successfully set, false otherwise.
 */
bool AD_setPhase(channel_t chan, uint16_t phase);

/**
 * @brief Calculate the frequency control word for a given frequency.
 * @param f The frequency value.
 * @return The frequency control word.
 */
uint32_t AD_calcFreq(float f);

/**
 * @brief Calculate the phase control word for a given phase.
 * @param a The phase value.
 * @return The phase control word.
 */
uint16_t AD_calcPhase(float a);

#endif // MD_AD9833_H
