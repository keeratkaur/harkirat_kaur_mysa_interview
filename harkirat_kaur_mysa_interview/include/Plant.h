#ifndef PLANT_H
#define PLANT_H

class Plant {
public:
    Plant(float waterNeedPerDay, float stressThreshold, float absorptionRate);
    void update(float availableWater);
    float getStress() const;
    float getWaterNeed() const;
private:
    float waterNeedPerDay;   // Liters
    float stressThreshold;   // Percentage (0-100)
    float absorptionRate;    // Fraction (0-1)
    float stress;            // Percentage (0-100)
};

#endif // PLANT_H 