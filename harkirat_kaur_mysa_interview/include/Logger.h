#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>

class Logger {
public:
    Logger(const std::string& filename);
    ~Logger();
    void logSecond(
        int time_s,
        float soil_moisture,
        float effective_moisture,
        float temp,
        float humidity,
        float rain,
        bool pump_on,
        float flow_rate,
        float water_used,
        float plant_stress,
        bool sensor_error,
        const std::string& zone_id = "Zone1",
        const std::string& soil_type = "Loam",
        float power_used = 0.0f // New parameter for power consumption
    );
    void finalize();
    // For summary reporting
    float getTotalWaterUsed() const;
    float getTotalPowerUsed() const; // New getter for total power used
    float getAverageDailyCost(float water_cost, int simulation_duration) const;
    float getAveragePlantStress() const;
    float getWaterEfficiency() const;
    int getSensorFailureEvents() const;
    int getHealthyTime() const;
    void setZoneID(const std::string& id);
    void setSoilType(const std::string& type);
private:
    std::ofstream file;
    float total_water_used = 0.0f;
    float total_power_used = 0.0f; // New field for total power used
    float total_plant_stress = 0.0f;
    float total_effective_water = 0.0f;
    int log_count = 0;
    int sensor_failure_events = 0;
    int healthy_time = 0;
    std::string zone_id = "Zone1";
    std::string soil_type = "Loam";
}; 