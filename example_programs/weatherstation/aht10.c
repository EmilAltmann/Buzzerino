#include "aht10.h"
#include "pico/stdlib.h"
#include <string.h>
#include <math.h>

#define AHT10_ADDR          0x38
#define AHT10_CMD_INIT      0xE1  // calibration
#define AHT10_CMD_TRIGGER   0xAC  // start mesurement
#define AHT10_CMD_RESET     0xBA  // soft-reset

static bool aht10_read_status(aht10_t* dev, uint8_t* status) {
    int r = i2c_read_blocking(dev->i2c, dev->address, status, 1, false);
    return (r == 1);
}

static bool aht10_wait_not_busy(aht10_t* dev, uint32_t timeout_ms) {
    absolute_time_t start = get_absolute_time();
    for (;;) {
        uint8_t st = 0;
        if (aht10_read_status(dev, &st)) {
            if ((st & 0x80) == 0) return true;
        }
        if (absolute_time_diff_us(start, get_absolute_time()) >= (int64_t)timeout_ms * 1000) {
            return false; // Timeout
        }
        sleep_ms(5);
    }
}

static bool aht10_calibrate(aht10_t* dev) {
    uint8_t buf[3] = {AHT10_CMD_INIT, 0x08, 0x00};
    int w = i2c_write_blocking(dev->i2c, dev->address, buf, 3, false);
    if (w < 0) return false;
    if (!aht10_wait_not_busy(dev, 500)) return false;

    // Optional: Check status to see if calibration is active (Bit 3 set)
    uint8_t st = 0;
    if (!aht10_read_status(dev, &st)) return false;
    if ((st & 0x08) == 0) {
        // Some sensors require a second attempt or reset.
        // Do not treat this as a hard error here; handle as needed:
        // return false;
    }
    return true;
}

bool aht10_soft_reset(aht10_t* dev) {
    uint8_t cmd = AHT10_CMD_RESET;
    int w = i2c_write_blocking(dev->i2c, dev->address, &cmd, 1, false);
    if (w < 0) return false;
    sleep_ms(20);
    return true;
}

bool aht10_begin(aht10_t* dev) {

    i2c_inst_t* i2c = i2c0;
    uint sda_pin = 0;
    uint scl_pin = 1;
    uint32_t baudrate = 100000;


    if (!dev) return false;
    dev->i2c = i2c;
    dev->address = AHT10_ADDR;
    dev->sda_pin = sda_pin;
    dev->scl_pin = scl_pin;
    dev->baudrate = baudrate;
    dev->initialized = false;

    i2c_init(i2c, baudrate);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    sleep_ms(20); // power-up delay


    if (!aht10_calibrate(dev)) {
        // try reset + (re-)calibration
        aht10_soft_reset(dev);
        sleep_ms(20);
        if (!aht10_calibrate(dev)) {
            return false;
        }
    }
    dev->initialized = true;
    return true;
}

bool aht10_read(aht10_t* dev, float* temperature_c, float* humidity_rh) {
    if (!dev || !dev->initialized) return false;

    uint8_t cmd[3] = {AHT10_CMD_TRIGGER, 0x33, 0x00};
    int w = i2c_write_blocking(dev->i2c, dev->address, cmd, 3, false);
    if (w < 0) return false;

    // wait for measurement
    if (!aht10_wait_not_busy(dev, 200)) return false;

    uint8_t data[6] = {0};
    int r = i2c_read_blocking(dev->i2c, dev->address, data, 6, false);
    if (r != 6) return false;

    // Data layout:
    // data[0] = Status
    // Humidity: 20-bit value from data[1], data[2], high-nibble of data[3]
    // Temperature: 20-bit value from low-nibble of data[3], data[4], data[5]
    uint32_t hum_raw = ((uint32_t)data[1] << 12) |
                       ((uint32_t)data[2] << 4)  |
                       ((uint32_t)(data[3] >> 4) & 0x0F);

    uint32_t temp_raw = (((uint32_t)(data[3] & 0x0F)) << 16) |
                        ((uint32_t)data[4] << 8)             |
                        (uint32_t)data[5];

    // Conversion according to datasheet:
    // RH[%] = hum_raw * 100 / 2^20
    // T[°C] = temp_raw * 200 / 2^20 - 50
    float rh = (hum_raw * 100.0f) / 1048576.0f;     // 2^20 = 1048576
    float tc = (temp_raw * 200.0f) / 1048576.0f - 50.0f;

    if (humidity_rh) *humidity_rh = rh;
    if (temperature_c) *temperature_c = tc;
    return true;
}

float aht10_read_temperature(aht10_t* dev, bool* ok) {
    float t = 0.0f, h = 0.0f;
    bool r = aht10_read(dev, &t, &h);
    if (ok) *ok = r;
    return r ? t : NAN;
}

float aht10_read_humidity(aht10_t* dev, bool* ok) {
    float t = 0.0f, h = 0.0f;
    bool r = aht10_read(dev, &t, &h);
    if (ok) *ok = r;
    return r ? h : NAN;
}