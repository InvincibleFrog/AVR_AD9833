#include "MD_AD9833.h"
#include <avr/io.h>

const uint8_t AD_B28 = 13;
const uint8_t AD_HLB = 12;
const uint8_t AD_FSELECT = 11;
const uint8_t AD_PSELECT = 10;
const uint8_t AD_RESET = 8;
const uint8_t AD_SLEEP1 = 7;
const uint8_t AD_SLEEP12 = 6;
const uint8_t AD_OPBITEN = 5;
const uint8_t AD_DIV2 = 3;
const uint8_t AD_MODE = 1;
const uint8_t AD_FREQ1 = 15;
const uint8_t AD_FREQ0 = 14;
const uint8_t AD_PHASE = 13;


void AD_init(void)
{
    DDRB = (1<<DATA) | (1<<CLK) | (1<<FSYNC); //Sets DATA, CLK, and FSYNC pins as output
    PORTB &= ~(1<<CLK); //Makes CLK pin low
    PORTB |= (1<<FSYNC); //Makes FSYNC pin high
    SPCR = 0x59; //Sets SPI control register (SPCR) to configure SPI mode and enable SPI
    _regCtl = 0x00;
    _regCtl |= (1<<AD_B28); //complete word to be loaded into control register in two consecutive writes
    AD_write16(_regCtl); //writes control register value
    AD_reset(true); //resets internal registers to 0
    AD_setFrequency(CHAN_0, AD_DEFAULT_FREQ); //Sets default frequency for CHAN_0
    AD_setFrequency(CHAN_1, AD_DEFAULT_FREQ); // Sets default frequency for CHAN_1
    AD_setPhase(CHAN_0, AD_DEFAULT_PHASE); // Sets default phase for CHAN_0
    AD_setPhase(CHAN_1, AD_DEFAULT_PHASE); // Sets default phase for CHAN_1
    AD_reset(false); //disables reset
    AD_setMode(MODE_SINE); // Sets the mode to sine wave
    AD_setActiveFrequency(CHAN_0); // Sets CHAN_0 as the active frequency channel
    AD_setActivePhase(CHAN_0); // Sets CHAN_0 as the active phase channel
}

void AD_reset(bool hold)
{
    _regCtl |= (1<<AD_RESET); //Sets the AD_RESET bit in control register
    AD_write16(_regCtl); //Writes the updated control register value
    if (!hold)
    {
        _regCtl &= ~(1<<AD_RESET); //Clears the AD_RESET bit in control register
        AD_write16(_regCtl); //Writes the updated control register value
    }
}

void AD_write16(uint16_t data)
{
    PORTB &= ~(1<<FSYNC); //Sets FSYNC pin low to start SPI transmission
    SPDR = (data>>8); //Loads the upper byte of data into SPI data register
    while (!(SPSR & (1<<SPIF))) //waits until transmission is complete
        ;
    SPDR = (data&0xFF); //Loads the lower byte of data into SPI data register
    while (!(SPSR & (1<<SPIF))) //waits until transmission is complete
        ;
    PORTB |= (1<<FSYNC); //Sets FSYNC pin high to end SPI transmission
}

bool AD_setFrequency(channel_t chan, float freq)
{
    uint16_t freq_select=0;
    _freq[chan] = freq; //Updates the frequency value for the specified channel
    _regFreq[chan] = AD_calcFreq(freq); //Calculates and stores the register frequency value
    switch(chan)
    {
        case(CHAN_0):
            freq_select = SEL_FREQ0;
            break;

        case(CHAN_1):
            freq_select = SEL_FREQ1;
            break;

        default:
            break;
    }
    AD_write16(_regCtl); //Writes control register value
    AD_write16(freq_select | (uint16_t)(_regFreq[chan] & 0x3FFF)); //Writes lower 14 bits of frequency value
    AD_write16(freq_select | (uint16_t)((_regFreq[chan]>>14) & 0x3FFF)); //Writes upper 14 bits of frequency value
    return(true);
}

bool AD_setPhase(channel_t chan, uint16_t phase)
{
    uint16_t phase_select=0;
    _phase[chan] = phase; //Updates the phase value for the specified channel
    _regPhase[chan] = AD_calcPhase(phase); //Calculates and stores the register phase value
    switch(chan)
    {
        case(CHAN_0):
            phase_select = SEL_PHASE0;
            break;

        case(CHAN_1):
            phase_select = SEL_PHASE1;
            break;

        default:
            break;
    }
    AD_write16(phase_select | (uint16_t)(_regPhase[chan] & 0xFFF)); //Writes phase select value combined with lower 12 bits of phase value

    return(true);
}

bool AD_setMode(mode_t mode)
{
    _modeLast = mode; //Sets the last mode to the provided mode
    switch(mode)
    {
        case(MODE_OFF):
            _regCtl &= ~(1<<AD_OPBITEN); //Clears OPBITEN bit
            _regCtl &= ~(1<<AD_MODE); //Clears MODE bit
            _regCtl |= (1<<AD_SLEEP1); //Sets SLEEP1 bit
            _regCtl |= (1<<AD_SLEEP12); //Sets SLEEP12 bit
            break;
        case(MODE_SINE):
            _regCtl &= ~(1<<AD_OPBITEN); //Clears OPBITEN bit
            _regCtl &= ~(1<<AD_MODE); //Clears MODE bit
            _regCtl &= ~(1<<AD_SLEEP1); //Clears SLEEP1 bit
            _regCtl &= ~(1<<AD_SLEEP12); //Clears SLEEP12 bit
            break;
        case(MODE_SQUARE1):
            _regCtl |= (1<<AD_OPBITEN); //Sets OPBITEN bit
            _regCtl &= ~(1<<AD_MODE); //Clears MODE bit
            _regCtl |= (1<<AD_DIV2); //Sets DIV2 bit
            _regCtl &= ~(1<<AD_SLEEP1); //Clears SLEEP1 bit
            _regCtl &= ~(1<<AD_SLEEP12); //Clears SLEEP12 bit
            break;
        case(MODE_SQUARE2):
            _regCtl |= (1<<AD_OPBITEN); //Sets OPBITEN bit
            _regCtl &= ~(1<<AD_MODE); //Clears MODE bit
            _regCtl &= ~(1<<AD_DIV2); //Clears DIV2 bit
            _regCtl &= ~(1<<AD_SLEEP1); //Clears SLEEP1 bit
            _regCtl &= ~(1<<AD_SLEEP12); //Clears SLEEP12 bit
            break;
        case(MODE_TRIANGLE):
            _regCtl &= ~(1<<AD_OPBITEN); //Clears OPBITEN bit
            _regCtl |= (1<<AD_MODE); //Sets MODE bit
            _regCtl &= ~(1<<AD_SLEEP1); //Clears SLEEP1 bit
            _regCtl &= ~(1<<AD_SLEEP12); //Clears SLEEP12 bit
            break;
    }
    AD_write16(_regCtl); //Writes the control register value

    return(true);
}

bool AD_setActiveFrequency(channel_t chan)
{
    switch(chan)
    {
        case(CHAN_0):
            _regCtl &= ~(1<<AD_FSELECT); //Clears FSELECT bit
            break;
        case(CHAN_1):
            _regCtl |= (1<<AD_FSELECT); //Sets FSELECT bit
            break;
    }
    AD_write16(_regCtl); //Writes the control register value

    return(true);
}

bool AD_setActivePhase(channel_t chan)
{
    switch(chan)
    {
        case(CHAN_0):
            _regCtl &= ~(1<<AD_PSELECT); //Clears PSELECT bit
            break;
        case(CHAN_1):
            _regCtl |= (1<<AD_PSELECT); //Sets PSELECT bit
            break;
    }
    AD_write16(_regCtl); //Writes the control register value

    return(true);
}

channel_t AD_getActiveFrequency(void)
{
    if (_regCtl & (1<<AD_FSELECT)) //Checks if FSELECT bit is set
        return CHAN_1; //Returns CHAN_1 if FSELECT is set
    else
        return CHAN_0; //Returns CHAN_0 if FSELECT is not set
}


channel_t AD_getActivePhase(void)
{
    if (_regCtl & (1<<AD_PSELECT)) //Check if PSELECT bit is set
        return CHAN_1; //Returns CHAN_1 if PSELECT is set
    else
        return CHAN_0; //Returns CHAN_0 if PSELECT is not set
}

mode_t AD_getMode(void )
{
    return _modeLast;
}

uint32_t AD_calcFreq(float f)
{
    return (uint32_t)((f*AD_2POW28/AD_MCLK) + 0.5);
}

uint16_t AD_calcPhase(float a)
{
    return (uint16_t)((512.0*(a/10)/45) + 0.5);
}

const float AD_getFrequency(channel_t chan)
{
    return _freq[chan];
}

uint16_t AD_getPhase(channel_t chan)
{
    return _phase[chan];
}
