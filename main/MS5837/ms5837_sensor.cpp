
#include "ms5837_sensor.h"
#include "driver/gpio.h"
#include <driver/i2c.h>
#include "esp_log.h"
#include "I2Cbus.hpp"
#include "MS5837.h"

#define SDA_PIN (gpio_num_t)11 // Your chosen SDA pin
#define SCL_PIN (gpio_num_t)10 // Your chosen SCL pin
#define I2C_FREQ 100000
#define I2C_PORT I2C_NUM_0

#define TAG "MS5837_sensor"

MS5837 sensor;
float pressure_psi;
float pressure_mbar;
float temperature_celsius;
float temperature_fahrenheit;

bool ms5837_sensor_init(void)
{
    // i2c0.begin(SDA_PIN, SCL_PIN, I2C_FREQ);
    ESP_LOGI(TAG, "i2c initialised");
    if (!sensor.initialize())
    {
        ESP_LOGW(TAG, "MS5837 initialisation failed");
        return false;
    }
    else
    {
        ESP_LOGI(TAG, "MS5837 initialisation passed");
    }

    // Check the pressure sensor model
    // Note at least one device returned no ID, so mis-characterised as 02BA
    if (sensor.getModel() != MS5837_30BA)
    {
        ESP_LOGI(TAG, "Current MS5837 model unexpected.  Setting to version 30BA");
        sensor.setModel(MS5837_30BA);
    }

    ESP_LOGI(TAG, "Current sea level air pressure set to %4.2f", sensor.getSeaLevelAirPressure());

    return true;
}

bool ms5837_sensor_loop(void)
{
    bool ret = sensor.read();
    pressure_psi = 0.0145038 * sensor.pressure();                      // Convert mbar to PSI
    pressure_mbar = sensor.pressure();                                 // Get pressure in mbar
    temperature_celsius = sensor.temperature();                        // Get temperature in Celsius
    temperature_fahrenheit = (temperature_celsius * 9.0 / 5.0) + 32.0; // Convert to Fahrenheit
    if (ret == false)
        ESP_LOGW(TAG, "MS5837 read failed");
    else
        ESP_LOGI(TAG, "Pressure %5.4f , temp %2.4f ", pressure_mbar, temperature_celsius);
    return ret;
}

float get_pressure_psi(void)
{
    return pressure_psi;
}

float get_pressure_mbar(void)
{
    return pressure_mbar;
}

float get_temperature_celsius(void)
{
    return temperature_celsius;
}

float get_temperature_fahrenheit(void)
{
    return temperature_fahrenheit;
}
