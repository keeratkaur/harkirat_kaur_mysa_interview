#include "../include/GardenZone.h"

GardenZone::GardenZone(Plant* plant, Soil* soil, WeatherSensor* weather, WaterPump* pump)
    : plant(plant), soil(soil), weather(weather), pump(pump) {}

// Static members for multi-zone coordination
int GardenZone::activePumpCount = 0;
int GardenZone::maxConcurrentPumps = 2;

void GardenZone::setMaxConcurrentPumps(int max) {
    maxConcurrentPumps = max;
}
int GardenZone::getActivePumpCount() {
    return activePumpCount;
}
bool GardenZone::canActivatePump() {
    return activePumpCount < maxConcurrentPumps;
}
void GardenZone::incrementActivePumps() {
    ++activePumpCount;
}
void GardenZone::decrementActivePumps() {
    if (activePumpCount > 0) --activePumpCount;
}

void GardenZone::update(int secondsElapsed) {
    weather->update(secondsElapsed);
    float temp = weather->getTemperature();
    float humidity = weather->getHumidity();
    float rainfall = weather->getRainfall();
    // Simple evapotranspiration model
    float evapotranspiration = (temp / 30.0f) * (1.0f - humidity / 100.0f) * 0.05f; // mm/sec
    float irrigation = 0.0f;
    if (pump->isOn()) {
        irrigation = pump->getFlowRate() / 60.0f; // L/min to L/sec
    }
    soil->update(evapotranspiration, rainfall, irrigation);
    plant->update(soil->getMoisture());
    // Multi-zone pump coordination: only activate pump if allowed
    if (!pump->isOn() && canActivatePump()) {
        pump->turnOn();
        incrementActivePumps();
    } else if (pump->isOn() && !canActivatePump()) {
        pump->turnOff();
        decrementActivePumps();
    }
} 