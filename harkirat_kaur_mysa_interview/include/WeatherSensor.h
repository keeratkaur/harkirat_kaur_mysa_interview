#ifndef WEATHERSENSOR_H
#define WEATHERSENSOR_H

#include <cstdlib>

class WeatherSensor {
public:
    WeatherSensor();
    void update(int secondsElapsed);
    float getTemperature() const; // Returns -999.0f if failed
    float getHumidity() const;    // Returns -999.0f if failed
    float getRainfall() const;    // Returns -999.0f if failed
    float getRainForecast(int hours = 6) const; // Returns forecasted rainfall (mm) for the next X hours
    bool hasFailed() const;       // True if sensor is in failure state
    void resetFailure(); // Reset sensor failure state
private:
    float temperature; // Celsius
    float humidity;    // Percentage
    float rainfall;    // mm
    bool failed;       // Sensor failure flag
    void simulateWeather(int secondsElapsed);
};

#endif // WEATHERSENSOR_H 