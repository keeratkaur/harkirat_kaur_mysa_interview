#include <cassert>
#include <iostream>
#include "../include/Soil.h"
#include "../include/WeatherSensor.h"
#include "../include/WaterPump.h"
#include "../include/IrrigationController.h"

int main() {
    Soil soil(0.8f, 0.2f);
    WeatherSensor weather;
    WaterPump pump(6.0f);
    IrrigationController controller(&soil, &weather, &pump);
    controller.setMoistureThreshold(40.0f);

    // Case 1: Moisture below threshold, no rain forecast -> pump ON
    soil.update(0.0f, 0.0f, 0.0f); // keep moisture at 50
    for (int i = 0; i < 20; ++i) soil.update(10.0f, 0.0f, 0.0f); // dry out soil
    assert(soil.getMoisture() < 40.0f);
    weather.update(0); // no rain
    controller.setForecastRain(false);
    controller.update(0);
    std::cout << "Pump state (should be ON): " << pump.isOn() << std::endl;
    assert(pump.isOn());

    // Case 2: Moisture above threshold -> pump OFF
    for (int i = 0; i < 100; ++i) soil.update(0.0f, 100.0f, 100.0f); // saturate soil
    assert(soil.getMoisture() == 100.0f);
    controller.update(0);
    std::cout << "Pump state (should be OFF): " << pump.isOn() << std::endl;
    assert(!pump.isOn());

    // Case 3: Moisture below threshold, but rain forecast -> pump OFF
    for (int i = 0; i < 100; ++i) soil.update(10.0f, 0.0f, 0.0f); // dry out soil
    assert(soil.getMoisture() < 40.0f);
    controller.setForecastRain(true);
    controller.update(0);
    std::cout << "Pump state (should be OFF due to forecast): " << pump.isOn() << std::endl;
    assert(!pump.isOn());

    // Case 4: Sensor failure disables pump
    // Force sensor failure
    WeatherSensor failedSensor;
    for (int t = 0; t < 10000; ++t) {
        failedSensor.update(t);
        if (failedSensor.hasFailed()) break;
    }
    WaterPump pump2(6.0f);
    IrrigationController controller2(&soil, &failedSensor, &pump2);
    controller2.setMoistureThreshold(40.0f);
    controller2.setForecastRain(false);
    controller2.update(0);
    std::cout << "Pump state (should be OFF due to sensor failure): " << pump2.isOn() << std::endl;
    assert(!pump2.isOn());

    // Case 5: Pump max run time and cooldown
    WaterPump pump3(6.0f);
    pump3.setMaxRunTime(3); // 3 seconds max
    pump3.setCooldownTime(5); // 5 seconds cooldown
    WeatherSensor weather2;
    IrrigationController controller3(&soil, &weather2, &pump3);
    controller3.setMoistureThreshold(100.0f); // Always try to irrigate
    controller3.setForecastRain(false);
    // Run for 10 seconds
    int onCount = 0, offCount = 0;
    for (int t = 0; t < 10; ++t) {
        controller3.update(t);
        if (pump3.isOn()) ++onCount;
        else ++offCount;
    }
    std::cout << "Pump ON count (should be <= 3): " << onCount << ", OFF count (should be >= 7): " << offCount << std::endl;
    assert(onCount <= 3);
    assert(offCount >= 7);

    std::cout << "IrrigationController tests passed!" << std::endl;
    return 0;
} 