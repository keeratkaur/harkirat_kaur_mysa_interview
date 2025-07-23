#ifndef SOIL_H
#define SOIL_H

class Soil {
public:
    Soil(float retentionRate, float drainageFactor);
    void update(float evapotranspiration, float rainfall, float irrigation);
    float getMoisture() const;
    // Sensor failure simulation
    void simulateFailure();
    void resetFailure();
private:
    float moisture;         // Percentage (0-100)
    float retentionRate;    // Fraction (0-1)
    float drainageFactor;   // Fraction (0-1)
    bool failed = false;
};

#endif // SOIL_H 