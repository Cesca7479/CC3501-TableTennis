#include "dc_bias.h"

void set_dc_biases() 
{
    uint32_t sum_piezos[3] = {0, 0, 0};
    uint16_t result;

    for (size_t i = 0; i < 10; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            Piezos[j].read(); // Ignore first read after swapping ADC channels for accurate results
            result = Piezos[j].read();
            sum_piezos[j] += result;
        }
        sleep_ms(100);
    }

    for (size_t i = 0; i < 3; i++)
    {
        State.piezo_dc_biases[i] = sum_piezos[i] / 10;
    }

    printf("DC Biases: %d, %d, %d\r\n", State.piezo_dc_biases[0], State.piezo_dc_biases[1], State.piezo_dc_biases[2]);
}