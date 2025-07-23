#include "../include/IrrigationController.h"
#include <cstdlib>
#include <ctime>

IrrigationController::IrrigationController(Soil* soil, WeatherSensor* weather, WaterPump* pump, Logger* loggerPtr)
    : soil(soil), weather(weather), pump(pump), logger(loggerPtr), moistureThreshold(40.0f), forecastRain(false) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void IrrigationController::setMoistureThreshold(float threshold) {
    moistureThreshold = threshold;
}

void IrrigationController::setForecastRain(bool rainLikely) {
    forecastRain = rainLikely;
}

void IrrigationController::setRainForecastHours(int hours) {
    rainForecastHours = hours;
}

void IrrigationController::setRainForecastThreshold(float mm) {
    rainForecastThreshold = mm;
}

void IrrigationController::setConservationModeEnabled(bool enabled) {
    conservationModeEnabled = enabled;
}
void IrrigationController::setConservationWaterCostThreshold(float threshold) {
    conservationWaterCostThreshold = threshold;
}
void IrrigationController::setConservationDroughtMoistureThreshold(float threshold) {
    conservationDroughtMoistureThreshold = threshold;
}
void IrrigationController::setConservationMoistureThreshold(float threshold) {
    conservationMoistureThreshold = threshold;
}
void IrrigationController::setConservationNightWindow(int startHour, int endHour) {
    conservationNightStartHour = startHour;
    conservationNightEndHour = endHour;
}
void IrrigationController::setCurrentWaterCost(float cost) {
    currentWaterCost = cost;
}

void IrrigationController::setHistoryWindowDays(int days) {
    historyWindowDays = days;
    historyWindowHours = days * 24;
    rainfallHistory.clear();
    moistureHistory.clear();
}

float IrrigationController::getNoisyMoisture() const {
    float noise = (std::rand() % 100 - 50) / 100.0f; // -0.5 to +0.5
    return soil->getMoisture() + noise;
}

void IrrigationController::update(int secondsElapsed) {
    pump->update(secondsElapsed);
    // --- Sensor failure handling and fallback ---
    float soilMoisture = soil->getMoisture();
    if (soilMoisture < 0) {
        soilMoisture = lastKnownSoilMoisture;
    } else {
        lastKnownSoilMoisture = soilMoisture;
    }
    float temp = weather->getTemperature();
    if (temp == -999.0f) {
        temp = lastKnownTemperature;
    } else {
        lastKnownTemperature = temp;
    }
    float humidity = weather->getHumidity();
    if (humidity == -999.0f) {
        humidity = lastKnownHumidity;
    } else {
        lastKnownHumidity = humidity;
    }
    float recentRain = weather->getRainfall();
    if (recentRain == -999.0f) {
        recentRain = lastKnownRainfall;
    } else {
        lastKnownRainfall = recentRain;
    }
    // --- End sensor fallback ---
    /*
     * Effective moisture calculation:
     *   effectiveMoisture = soilMoisture + recentRainfall * retentionFactor - evapotranspiration;
     *
     * - soilMoisture: Current soil moisture (with noise)
     * - recentRainfall: Rainfall measured during this update interval (not a rolling window)
     * - retentionFactor: Fraction of rainfall retained by the soil (from Soil::retentionRate)
     * - evapotranspiration: Estimated water loss due to evaporation and plant transpiration
     *
     * Note: In this implementation, soilMoisture already incorporates rainfall and retentionFactor via Soil::update().
     * Here, recentRainfall is used directly for short-term irrigation logic, not as a rolling sum.
     */
    float noise = (std::rand() % 100 - 50) / 100.0f; // -0.5 to +0.5
    float noisyMoisture = soilMoisture + noise;
    float evap = (temp / 30.0f) * (1.0f - humidity / 100.0f) * 0.05f; // evapotranspiration estimate
    float effectiveMoisture = noisyMoisture + recentRain - evap;
    // --- Force pump ON for first 5 seconds ---
    if (secondsElapsed < 5) {
        pump->turnOn();
        return;
    }
    // --- Predictive Watering: Track and use weather/moisture trends ---
    // Record hourly rainfall and soil moisture
    if (secondsElapsed % 3600 == 0) {
        if (rainfallHistory.size() >= (size_t)historyWindowHours) rainfallHistory.erase(rainfallHistory.begin());
        if (moistureHistory.size() >= (size_t)historyWindowHours) moistureHistory.erase(moistureHistory.begin());
        rainfallHistory.push_back(recentRain);
        moistureHistory.push_back(soil->getMoisture());
    }
    // Calculate moving averages over the history window
    float avgRain = 0.0f, avgMoisture = 0.0f;
    if (!rainfallHistory.empty()) {
        for (float r : rainfallHistory) avgRain += r;
        avgRain /= rainfallHistory.size();
    }
    if (!moistureHistory.empty()) {
        for (float m : moistureHistory) avgMoisture += m;
        avgMoisture /= moistureHistory.size();
    }
    /*
     * Predictive watering logic:
     * - If the last 2-3 days have been dry (low avgRain, low avgMoisture), be more aggressive (lower threshold).
     * - If wet (high avgRain, high avgMoisture), be more conservative (raise threshold).
     * - Assumptions: "Dry" means avgRain < 1mm/hr and avgMoisture < 30%. "Wet" means avgRain > 2mm/hr or avgMoisture > 60%.
     * - Adjust threshold by +/- 5%.
     */
    float predictiveThreshold = moistureThreshold;
    if (avgRain < 1.0f && avgMoisture < 30.0f) {
        predictiveThreshold -= 5.0f; // Be more aggressive
    } else if (avgRain > 2.0f || avgMoisture > 60.0f) {
        predictiveThreshold += 5.0f; // Be more conservative
    }
    // Weather-aware irrigation: delay if rain forecast exceeds threshold
    float rainForecast = weather->getRainForecast(rainForecastHours);
    if (rainForecast > rainForecastThreshold) {
        // Delay irrigation due to forecasted rain
        pump->turnOff();
        return;
    }
    // Water Conservation Mode
    bool drought = soil->getMoisture() < conservationDroughtMoistureThreshold;
    bool highCost = currentWaterCost > conservationWaterCostThreshold;
    bool conservationActive = conservationModeEnabled && (highCost || drought);
    float thresholdToUse = conservationActive ? conservationMoistureThreshold : predictiveThreshold;
    // Only water at night if conservation mode is active
    bool allowWatering = true;
    if (conservationActive) {
        // Calculate current hour (0-23) from secondsElapsed
        int hour = (secondsElapsed / 3600) % 24;
        if (conservationNightStartHour < conservationNightEndHour) {
            // Night window does not cross midnight
            allowWatering = (hour >= conservationNightStartHour && hour < conservationNightEndHour);
        } else {
            // Night window crosses midnight
            allowWatering = (hour >= conservationNightStartHour || hour < conservationNightEndHour);
        }
    }
    if (effectiveMoisture < thresholdToUse && !forecastRain && pump->canRun() && allowWatering) {
        pump->turnOn();
    } else {
        pump->turnOff();
    }
} 