#include "../include/Logger.h"
#include <iomanip>
#include <ctime> // Required for std::time_t and std::tm

Logger::Logger(const std::string& filename) {
    file.open(filename);
    file << "Timestamp,SoilMoisture (%),EffectiveMoisture (%),Temperature (°C),Humidity (%),Rainfall (mm),PumpState,FlowRate (L/min),WaterUsed (L),PlantStress (%),SensorError,ZoneID,SoilType,PowerUsed (Wh)\n";
}

Logger::~Logger() {
    if (file.is_open()) file.close();
}

void Logger::logSecond(
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
    const std::string& zone_id,
    const std::string& soil_type,
    float power_used
) {
    // Timestamp: start at 2025-07-01 00:00:00
    std::time_t base = 1751328000; // 2025-07-01 00:00:00 UTC
    std::time_t t = base + time_s;
    std::tm* tm_ptr = std::gmtime(&t);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_ptr);
    file << buf << ','
         << std::fixed << std::setprecision(1)
         << soil_moisture << ','
         << effective_moisture << ','
         << temp << ','
         << humidity << ','
         << rain << ','
         << (pump_on ? "ON" : "OFF") << ','
         << flow_rate << ','
         << water_used << ','
         << plant_stress << ','
         << (sensor_error ? "TRUE" : "FALSE") << ','
         << zone_id << ','
         << soil_type << ','
         << std::fixed << std::setprecision(2) // Set higher precision for power_used
         << power_used << '\n';
    total_water_used += water_used;
    total_power_used += power_used;
    total_plant_stress += plant_stress;
    ++log_count;
    if (sensor_error) ++sensor_failure_events;
    if (plant_stress < 10.0f) ++healthy_time;
    file.flush();
}

void Logger::finalize() {
    if (file.is_open()) file.flush();
}

float Logger::getTotalWaterUsed() const {
    return total_water_used;
}

float Logger::getTotalPowerUsed() const {
    return total_power_used;
}

float Logger::getAverageDailyCost(float water_cost, int simulation_duration) const {
    float days = simulation_duration / 86400.0f;
    return days > 0 ? (total_water_used * water_cost) / days : 0.0f;
}

float Logger::getAveragePlantStress() const {
    return log_count > 0 ? total_plant_stress / log_count : 0.0f;
}

float Logger::getWaterEfficiency() const {
    return log_count > 0 ? (static_cast<float>(healthy_time) / log_count) * 100.0f : 0.0f;
}

int Logger::getSensorFailureEvents() const {
    return sensor_failure_events;
}

int Logger::getHealthyTime() const {
    return healthy_time;
}

void Logger::setZoneID(const std::string& id) { zone_id = id; }
void Logger::setSoilType(const std::string& type) { soil_type = type; } 