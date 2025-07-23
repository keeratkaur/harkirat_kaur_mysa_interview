#include <cassert>
#include <iostream>
#include "../include/WeatherSensor.h"

int main() {
    WeatherSensor sensor;
    for (int t = 0; t < 86400; t += 3600) { // Test at each hour
        sensor.update(t);
        float temp = sensor.getTemperature();
        float hum = sensor.getHumidity();
        float rain = sensor.getRainfall();
        std::cout << "Hour " << t/3600 << ": Temp=" << temp << ", Humidity=" << hum << ", Rain=" << rain << std::endl;
        assert(temp >= -10.0f && temp <= 40.0f); // Reasonable temp range
        assert(hum >= 0.0f && hum <= 100.0f);   // Humidity in [0,100]
        assert(rain >= 0.0f);                   // Rainfall non-negative
    }
    // Test sensor failure
    bool failed = false;
    for (int t = 0; t < 10000; t += 1) {
        sensor.update(t);
        if (sensor.hasFailed()) {
            failed = true;
            assert(sensor.getTemperature() == -999.0f);
            assert(sensor.getHumidity() == -999.0f);
            assert(sensor.getRainfall() == -999.0f);
            std::cout << "Sensor failure detected at t=" << t << std::endl;
            break;
        }
    }
    assert(failed && "Sensor should eventually fail in 10,000 updates");
    std::cout << "WeatherSensor tests passed!" << std::endl;
    return 0;
} 