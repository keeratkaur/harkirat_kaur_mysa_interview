#include "../include/Soil.h"

Soil::Soil(float retentionRate, float drainageFactor)
    : moisture(0.0f), retentionRate(retentionRate), drainageFactor(drainageFactor) {}

void Soil::update(float evapotranspiration, float rainfall, float irrigation) {
    // Add water from rainfall and irrigation
    moisture += (rainfall + irrigation) * retentionRate;
    // Remove water from evapotranspiration
    moisture -= evapotranspiration * (1.0f - drainageFactor);
    // Clamp moisture between 0 and 100
    if (moisture > 100.0f) moisture = 100.0f;
    if (moisture < 0.0f) moisture = 0.0f;
}

void Soil::simulateFailure() {
    failed = true;
}
void Soil::resetFailure() {
    failed = false;
}
float Soil::getMoisture() const {
    if (failed) return -1.0f;
    return moisture;
} 