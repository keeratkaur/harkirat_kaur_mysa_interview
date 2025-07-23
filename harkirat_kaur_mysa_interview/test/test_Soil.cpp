#include <cassert>
#include <iostream>
#include "../include/Soil.h"

int main() {
    Soil soil(0.8f, 0.2f); // High retention, low drainage
    // Initial moisture should be 50
    assert(soil.getMoisture() == 50.0f);
    // Add rainfall
    soil.update(0.0f, 10.0f, 0.0f); // 10mm rain
    std::cout << "After rain: " << soil.getMoisture() << std::endl;
    assert(soil.getMoisture() > 50.0f);
    // Add irrigation
    soil.update(0.0f, 0.0f, 5.0f); // 5L irrigation
    std::cout << "After irrigation: " << soil.getMoisture() << std::endl;
    assert(soil.getMoisture() > 50.0f);
    // Evapotranspiration
    float before = soil.getMoisture();
    soil.update(2.0f, 0.0f, 0.0f); // Remove 2mm
    std::cout << "After evapotranspiration: " << soil.getMoisture() << std::endl;
    assert(soil.getMoisture() < before);
    // Test clamping at 0
    for (int i = 0; i < 100; ++i) soil.update(10.0f, 0.0f, 0.0f);
    std::cout << "After excessive drying: " << soil.getMoisture() << std::endl;
    assert(soil.getMoisture() == 0.0f);
    // Test clamping at 100
    for (int i = 0; i < 100; ++i) soil.update(0.0f, 100.0f, 100.0f);
    std::cout << "After excessive watering: " << soil.getMoisture() << std::endl;
    assert(soil.getMoisture() == 100.0f);
    std::cout << "Soil tests passed!" << std::endl;
    return 0;
} 