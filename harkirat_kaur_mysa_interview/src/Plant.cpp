#include "../include/Plant.h"

Plant::Plant(float waterNeedPerDay, float stressThreshold, float absorptionRate)
    : waterNeedPerDay(waterNeedPerDay), stressThreshold(stressThreshold), absorptionRate(absorptionRate), stress(0.0f) {}

void Plant::update(float availableWater) {
    float absorbed = availableWater * absorptionRate;
    if (absorbed < waterNeedPerDay / 86400.0f) { // per second
        stress += (waterNeedPerDay / 86400.0f - absorbed) * 10.0f;
    } else {
        stress -= 0.1f;
    }
    if (stress < 0.0f) stress = 0.0f;
    if (stress > 100.0f) stress = 100.0f;
}

float Plant::getStress() const { return stress; }
float Plant::getWaterNeed() const { return waterNeedPerDay; } 