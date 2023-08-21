#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include "MD_AD9833.h"

int main(void)
{
    AD_init();
    static uint16_t lastv = 0;
    uint16_t v;
    ADCSRA = 0x87;
    ADMUX = 0x40;
    AD_setMode(MODE_SINE);
    while(1)
    {
        ADCSRA |= (1<<ADSC);
        while((ADCSRA & (1<<ADIF)) == 0)
            ;
        v = ADC;

        if(abs(v-lastv) > 20)
        {
            AD_setFrequency(CHAN_0, 2000+10*v);
            lastv = v;
        }
    }

    return 0;
}
