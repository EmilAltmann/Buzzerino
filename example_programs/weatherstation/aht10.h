#ifndef AHT10_H
#define AHT10_H

#include <stdbool.h>
#include <stdint.h>
#include "hardware/i2c.h"

typedef struct {
    i2c_inst_t* i2c;
    uint8_t address;
    uint sda_pin;
    uint scl_pin;
    uint32_t baudrate;
    bool initialized;
} aht10_t;

// Initializes I2C and calibrates the AHT10.
// Example: aht10_begin(&dev)
bool aht10_begin(aht10_t* dev);

// Soft reset (optional, e.g. if calibration fails)
bool aht10_soft_reset(aht10_t* dev);

// Reads temperature (°C) and relative humidity (%RH) in one measurement cycle.
bool aht10_read(aht10_t* dev, float* temperature_c, float* humidity_rh);

// Convenience: only read temperature
// ok is set to true if the measurement was successful.
float aht10_read_temperature(aht10_t* dev, bool* ok);

// Convenience: only read humidity
float aht10_read_humidity(aht10_t* dev, bool* ok);

#endif // AHT10_H