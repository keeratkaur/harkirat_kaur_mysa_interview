#ifndef IRRIGATIONCONTROLLER_H
#define IRRIGATIONCONTROLLER_H

#include "Soil.h"
#include "WeatherSensor.h"
#include "WaterPump.h"
#include <vector>
#include "Logger.h"

class IrrigationController {
public:
    IrrigationController(Soil* soil, WeatherSensor* weather, WaterPump* pump, Logger* logger);
    void setMoistureThreshold(float threshold);
    void update(int secondsElapsed);
    void setForecastRain(bool rainLikely);
    // Set the number of hours to look ahead for rain forecast
    void setRainForecastHours(int hours);
    // Set the rain threshold (mm) above which irrigation is delayed
    void setRainForecastThreshold(float mm);
    // Water conservation mode configuration
    void setConservationModeEnabled(bool enabled);
    void setConservationWaterCostThreshold(float threshold);
    void setConservationDroughtMoistureThreshold(float threshold);
    void setConservationMoistureThreshold(float threshold);
    void setConservationNightWindow(int startHour, int endHour);
    void setCurrentWaterCost(float cost);
    // Predictive watering configuration
    void setHistoryWindowDays(int days);
    // Getters for last known sensor values (for fallback display)
    float getLastKnownSoilMoisture() const { return lastKnownSoilMoisture; }
    float getLastKnownTemperature() const { return lastKnownTemperature; }
    float getLastKnownHumidity() const { return lastKnownHumidity; }
    float getLastKnownRainfall() const { return lastKnownRainfall; }
private:
    Soil* soil;
    WeatherSensor* weather;
    WaterPump* pump;
    Logger* logger = nullptr; // For logging sensor failures
    float moistureThreshold;
    bool forecastRain;
    float getNoisyMoisture() const;
    int rainForecastHours = 6; // Number of hours to look ahead for rain forecast
    float rainForecastThreshold = 2.0f; // Rainfall threshold (mm) to delay irrigation
    // Water conservation mode state/config
    bool conservationModeEnabled = false;
    float conservationWaterCostThreshold = 0.5f;
    float conservationDroughtMoistureThreshold = 20.0f;
    float conservationMoistureThreshold = 35.0f;
    int conservationNightStartHour = 22;
    int conservationNightEndHour = 6;
    float currentWaterCost = 0.1f;
    // Predictive watering: store recent rainfall and soil moisture history (per hour)
    std::vector<float> rainfallHistory;
    std::vector<float> moistureHistory;
    int historyWindowDays = 3;
    int historyWindowHours = 72; // 3 days * 24 hours
    // Last known sensor values for fallback
    float lastKnownSoilMoisture = 50.0f;
    float lastKnownTemperature = 20.0f;
    float lastKnownHumidity = 50.0f;
    float lastKnownRainfall = 0.0f;
};

#endif // IRRIGATIONCONTROLLER_H 