/**
 * @file MS5837.cpp
 */

/*********************
 *      INCLUDES
 *********************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_system.h"
#include "driver/gpio.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "I2Cbus.hpp"

#include "esp_log.h"

#include "MS5837.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static const char *TAG = "MS5837 lib";

const float MS5837::Pa = 100.0f;
const float MS5837::bar = 0.001f;
const float MS5837::mbar = 1.0f;

MS5837::MS5837()
{
}

bool MS5837::initialize()
{
    fluidDensity = MS5837_SEAWATER_DENSITY;
    seaLevelAirPressure = MS5837_DEFAULT_SEA_LEVEL_AIR_PRESSURE;
    _active_request = 0;

    int ret = i2c0.writeByte(MS5837_ADDR, MS5837_RESET, 0);
    if (ret != 0)
    {
        ESP_LOGW(TAG, "MS5837 reset write failed: %d", ret);
        return false;
    }

    // Wait for reset to complete
    vTaskDelay(pdMS_TO_TICKS(10));

    // Read calibration values and CRC
    uint8_t data[2];
    for (uint8_t i = 0; i < 7; i++)
    {
        ret = i2c0.readBytes(MS5837_ADDR, MS5837_PROM_READ + i * 2, 2, data);
        if (ret != 0)
        {
            ESP_LOGW(TAG, "MS5837 PROM read failed at i=%d: %d", i, ret);
            return false;
        }
        C[i] = (data[0] << 8) | data[1];
        // ESP_LOGI(TAG, "C[%i] is %04X",i,C[i]);
    }

    // Check CRC
    uint8_t crcRead = C[0] >> 12;
    uint8_t crcCalculated = crc4(C);

    if (crcCalculated != crcRead)
    {
        ESP_LOGW(TAG, "MS5837 config CRC check failed");
        return false;
    }

    // ESP_LOGI(TAG, "c[0] is %04X",C[0]);
    uint8_t version = (C[0] >> 5) & 0x7F; // Extract the sensor version from PROM Word 0

    // ESP_LOGI(TAG, "Version is %02X",version);

    // Set _model according to the sensor version
    if (version == MS5837_VERSION_02BA01)
    {
        _model = MS5837_02BA;
        ESP_LOGI(TAG, "Recognised MS5837_02BA (02BA01)");
    }
    else if (version == MS5837_VERSION_02BA21)
    {
        _model = MS5837_02BA;
        ESP_LOGI(TAG, "Recognised MS5837_02BA (02BA21)");
    }
    else if (version == MS5837_VERSION_30BA26)
    {
        _model = MS5837_30BA;
        ESP_LOGI(TAG, "Recognised MS5837_30BA");
    }
    else
    {
        _model = MS5837_UNRECOGNISED;
        ESP_LOGW(TAG, "Did not recognised MS5837 version");
    }

    return true;
}

void MS5837::setModel(uint8_t model)
{
    _model = model;
}

uint8_t MS5837::getModel()
{
    return _model;
}

void MS5837::setOverSampling(MS5837_OVERSAMPLING oversampling)
{
    _oversampling = (int)oversampling;
}

void MS5837::setSeaLevelAirPressure(float pressure)
{
    seaLevelAirPressure = (pressure / 100);
}

float MS5837::getSeaLevelAirPressure()
{
    return (float)(seaLevelAirPressure / 100.0f);
}

int8_t MS5837::read_nonblocking(int64_t time_us)
{
    uint8_t data[3];
    int ret = 0;
    if (_active_request == 0)
    {
        // Get the pressure data
        ret = i2c0.writeByte(MS5837_ADDR, MS5837_CONVERT_D1_256 + 2 * _oversampling, 0);
        if (ret != 0)
            return -1;
        _readStartTime = time_us;
        _active_request = 1;
    }
    else if (_active_request == 1)
    {
        if (time_us > (_readStartTime + delay_lookup[_oversampling]))
        {
            ret = i2c0.readBytes(MS5837_ADDR, MS5837_ADC_READ, 3, data);
            if (ret != 0)
                return -2;
            D1 = 0;
            D1 = (data[0] << 16) | (data[1] << 8) | data[2];

            // Request the temperature data
            ret = i2c0.writeByte(MS5837_ADDR, MS5837_CONVERT_D2_256 + 2 * _oversampling, 0);
            if (ret != 0)
                return -3;
            _readStartTime = time_us;
            _active_request = 2;
        }
    }
    else if (_active_request == 2)
    {
        if (time_us > (_readStartTime + delay_lookup[_oversampling]))
        {
            ret = i2c0.readBytes(MS5837_ADDR, MS5837_ADC_READ, 3, data);
            if (ret != 0)
                return -4;
            D2 = 0;
            D2 = (data[0] << 16) | (data[1] << 8) | data[2];

            calculate();
            _active_request = 3;
        }
    }
    else if (_active_request == 3)
    {
        _active_request = 0; // Used to reset after a read
    }

    return _active_request;
}

bool MS5837::read()
{
    uint8_t data[3];
    // Get the pressure data
    int ret;
    ret = i2c0.writeByte(MS5837_ADDR, MS5837_CONVERT_D1_256 + 2 * _oversampling, 0);
    if (ret != 0)
        return false;
    vTaskDelay(pdMS_TO_TICKS(delay_lookup_ms[_oversampling]) + 1);
    ret = i2c0.readBytes(MS5837_ADDR, MS5837_ADC_READ, 3, data);
    if (ret != 0)
        return false;
    D1 = 0;
    D1 = (data[0] << 16) | (data[1] << 8) | data[2];

    // Get the temperature data
    ret = i2c0.writeByte(MS5837_ADDR, MS5837_CONVERT_D2_256 + 2 * _oversampling, 0);
    if (ret != 0)
        return false;
    vTaskDelay(pdMS_TO_TICKS(delay_lookup_ms[_oversampling]) + 1);
    ret = i2c0.readBytes(MS5837_ADDR, MS5837_ADC_READ, 3, data);
    if (ret != 0)
        return false;
    D2 = 0;
    D2 = (data[0] << 16) | (data[1] << 8) | data[2];

    // ESP_LOGI(TAG, "D1 = %i ; D2 = %i",D1,D2);

    calculate();

    return true;
}

void MS5837::calculate()
{
    // Given C1-C6 and D1, D2, calculated TEMP and P
    // Do conversion first and then second order temp compensation

    int32_t dT = 0;
    int64_t SENS = 0;
    int64_t OFF = 0;
    int32_t SENSi = 0;
    int32_t OFFi = 0;
    int32_t Ti = 0;
    int64_t OFF2 = 0;
    int64_t SENS2 = 0;

    // Terms called
    dT = D2 - uint32_t(C[5]) * 256l;
    if (_model == MS5837_02BA)
    {
        SENS = int64_t(C[1]) * 65536l + (int64_t(C[3]) * dT) / 128l;
        OFF = int64_t(C[2]) * 131072l + (int64_t(C[4]) * dT) / 64l;
        P = (D1 * SENS / (2097152l) - OFF) / (32768l);
    }
    else
    {
        SENS = int64_t(C[1]) * 32768l + (int64_t(C[3]) * dT) / 256l;
        OFF = int64_t(C[2]) * 65536l + (int64_t(C[4]) * dT) / 128l;
        P = (D1 * SENS / (2097152l) - OFF) / (8192l);
    }

    // Temp conversion
    TEMP = 2000l + int64_t(dT) * C[6] / 8388608LL;

    // Second order compensation
    if (_model == MS5837_02BA)
    {
        if ((TEMP / 100) < 20)
        { // Low temp
            Ti = (11 * int64_t(dT) * int64_t(dT)) / (34359738368LL);
            OFFi = (31 * (TEMP - 2000) * (TEMP - 2000)) / 8;
            SENSi = (63 * (TEMP - 2000) * (TEMP - 2000)) / 32;
        }
    }
    else
    {
        if ((TEMP / 100) < 20)
        { // Low temp
            Ti = (3 * int64_t(dT) * int64_t(dT)) / (8589934592LL);
            OFFi = (3 * (TEMP - 2000) * (TEMP - 2000)) / 2;
            SENSi = (5 * (TEMP - 2000) * (TEMP - 2000)) / 8;
            if ((TEMP / 100) < -15)
            { // Very low temp
                OFFi = OFFi + 7 * (TEMP + 1500l) * (TEMP + 1500l);
                SENSi = SENSi + 4 * (TEMP + 1500l) * (TEMP + 1500l);
            }
        }
        else if ((TEMP / 100) >= 20)
        { // High temp
            Ti = 2 * (dT * dT) / (137438953472LL);
            OFFi = (1 * (TEMP - 2000) * (TEMP - 2000)) / 16;
            SENSi = 0;
        }
    }

    OFF2 = OFF - OFFi; // Calculate pressure and temp second order
    SENS2 = SENS - SENSi;

    TEMP = (TEMP - Ti);

    if (_model == MS5837_02BA)
    {
        P = (((D1 * SENS2) / 2097152l - OFF2) / 32768l);
    }
    else
    {
        P = (((D1 * SENS2) / 2097152l - OFF2) / 8192l);
    }
}

float MS5837::pressure(float conversion)
{
    if (_model == MS5837_02BA)
    {
        return P * conversion / 100.0f;
    }
    else
    {
        return P * conversion / 10.0f;
    }
}

float MS5837::temperature()
{
    return TEMP / 100.0f;
}

float MS5837::depth()
{
    return (pressure(MS5837::Pa) - seaLevelAirPressure) / (fluidDensity * 9.80665);
}

uint8_t MS5837::crc4(uint16_t n_prom[]) // n_prom defined as 8x unsigned int (n_prom[8])
{
    uint16_t n_rem = 0;                 // crc remainder
    n_prom[0] = ((n_prom[0]) & 0x0FFF); // CRC byte is replaced by 0
    n_prom[7] = 0;                      // Subsidiary value, set to 0
    for (uint8_t i = 0; i < 16; i++)    // operation is performed on bytes
    {                                   // choose LSB or MSB
        if (i % 2 == 1)
        {
            n_rem ^= (uint16_t)((n_prom[i >> 1]) & 0x00FF);
        }
        else
        {
            n_rem ^= (uint16_t)(n_prom[i >> 1] >> 8);
        }
        for (uint8_t n_bit = 8; n_bit > 0; n_bit--)
        {
            if (n_rem & (0x8000))
            {
                n_rem = (n_rem << 1) ^ 0x3000;
            }
            else
            {
                n_rem = (n_rem << 1);
            }
        }
    }
    n_rem = ((n_rem >> 12) & 0x000F); // final 4-bit remainder is CRC code
    return (n_rem ^ 0x00);
}