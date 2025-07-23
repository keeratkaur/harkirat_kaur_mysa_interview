#ifndef GARDENZONE_H
#define GARDENZONE_H

#include "Plant.h"
#include "Soil.h"
#include "WeatherSensor.h"
#include "WaterPump.h"

class GardenZone {
public:
    GardenZone(Plant* plant, Soil* soil, WeatherSensor* weather, WaterPump* pump);
    void update(int secondsElapsed);
    // Multi-zone coordination
    static void setMaxConcurrentPumps(int max);
    static int getActivePumpCount();
    static bool canActivatePump();
    static void incrementActivePumps();
    static void decrementActivePumps();
private:
    Plant* plant;
    Soil* soil;
    WeatherSensor* weather;
    WaterPump* pump;
    static int activePumpCount;
    static int maxConcurrentPumps;
};

#endif // GARDENZONE_H 