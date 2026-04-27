#ifndef AHT10_H
#define AHT10_H

#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"

typedef struct {
    i2c_inst_t *i2c;
    uint8_t address;
    uint sda_pin;
    uint scl_pin;
    uint32_t baudrate;
    bool initialized;
} aht10_t;

bool aht10_begin(aht10_t *dev);
bool aht10_begin_with_pins(aht10_t *dev, i2c_inst_t *i2c, uint sda_pin, uint scl_pin, uint32_t baudrate);

bool aht10_soft_reset(aht10_t *dev);

bool aht10_read(aht10_t *dev, float *temperature_c, float *humidity_rh);
float aht10_read_temperature(aht10_t *dev, bool *ok);
float aht10_read_humidity(aht10_t *dev, bool *ok);

#endif