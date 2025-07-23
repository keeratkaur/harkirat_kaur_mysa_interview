#include "../include/WeatherSensor.h"
#include <cmath>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

WeatherSensor::WeatherSensor() : temperature(20.0f), humidity(50.0f), rainfall(0.0f), failed(false) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void WeatherSensor::update(int secondsElapsed) {
    // 0.2% chance per update to simulate failure
    if (!failed && (std::rand() % 5000 < 10)) {
        failed = true;
    }
    simulateWeather(secondsElapsed);
}

void WeatherSensor::simulateWeather(int secondsElapsed) {
    if (failed) {
        temperature = -999.0f;
        humidity = -999.0f;
        rainfall = -999.0f;
        return;
    }
    // Simulate a daily temperature cycle (sine wave: 24h = 86400s)
    float dayFraction = (secondsElapsed % 86400) / 86400.0f;
    temperature = 15.0f + 10.0f * std::sin(2 * M_PI * dayFraction) + (std::rand() % 200 - 100) / 100.0f;
    humidity = 80.0f - (temperature - 15.0f) * 2.0f + (std::rand() % 100 - 50) / 100.0f;
    // Clamp temperature and humidity
    if (temperature < -10.0f) temperature = -10.0f;
    if (temperature > 40.0f) temperature = 40.0f;
    if (humidity < 0.0f) humidity = 0.0f;
    if (humidity > 100.0f) humidity = 100.0f;
    // Random rainfall event (10% chance per hour)
    if (std::rand() % 3600 < 360) {
        rainfall = (std::rand() % 10 + 1) * 0.5f; // 0.5 to 5 mm
    } else {
        rainfall = 0.0f;
    }
}

float WeatherSensor::getTemperature() const { return failed ? -999.0f : temperature; }
float WeatherSensor::getHumidity() const { return failed ? -999.0f : humidity; }
float WeatherSensor::getRainfall() const { return failed ? -999.0f : rainfall; }
bool WeatherSensor::hasFailed() const { return failed; }

void WeatherSensor::resetFailure() {
    failed = false;
}

float WeatherSensor::getRainForecast(int hours) const {
    if (failed) return -999.0f;
    float totalForecast = 0.0f;
    for (int h = 0; h < hours; ++h) {
        // 10% chance per hour for rain event, as in simulateWeather
        bool rainEvent = (std::rand() % 10 == 0); // 1 in 10
        if (rainEvent) {
            float rainAmount = (std::rand() % 10 + 1) * 0.5f; // 0.5 to 5 mm
            totalForecast += rainAmount;
        }
    }
    return totalForecast;
} 