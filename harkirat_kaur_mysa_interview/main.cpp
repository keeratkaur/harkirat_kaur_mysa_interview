#include <iostream>
#include <thread>
#include <chrono>
#include "include/WeatherSensor.h"
#include "include/Soil.h"
#include "include/Plant.h"
#include "include/WaterPump.h"
#include "include/GardenZone.h"
#include "include/IrrigationController.h"
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "include/Logger.h"
#include <regex>
#include <cstdlib> // For std::rand
#include <iomanip> // For std::fixed and std::setprecision

int main(int argc, char* argv[]) {
    try {
        std::string configPath = "config/config.yaml";
        int simulation_duration = -1; // -1 means not set by CLI
        // --- CLI ARG PARSING ---
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--configuration" && i + 1 < argc) {
                configPath = argv[++i];
            } else if (arg == "--duration" && i + 1 < argc) {
                std::string dur = argv[++i];
                std::smatch match;
                if (std::regex_match(dur, match, std::regex(R"((\d+)([smhd]?))"))) {
                    int val = std::stoi(match[1]);
                    char unit = match[2].str().empty() ? 's' : match[2].str()[0];
                    switch (unit) {
                        case 's': simulation_duration = val; break;
                        case 'm': simulation_duration = val * 60; break;
                        case 'h': simulation_duration = val * 3600; break;
                        case 'd': simulation_duration = val * 86400; break;
                        default:
                            std::cerr << "Invalid duration unit: " << unit << std::endl;
                            return 10;
                    }
                } else {
                    std::cerr << "Invalid duration format: " << dur << std::endl;
                    std::cerr << "Usage: --duration <number>[s|m|h|d]" << std::endl;
                    return 11;
                }
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "Usage: " << argv[0] << " [--configuration <file>] [--duration <number>[s|m|h|d]]" << std::endl;
                return 0;
            } else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                std::cerr << "Usage: " << argv[0] << " [--configuration <file>] [--duration <number>[s|m|h|d]]" << std::endl;
                return 12;
            }
        }
        std::cout << "Mysa Irrigation System starting..." << std::endl;

        // --- CONFIG PARSING ---
        std::map<std::string, std::string> config;
        std::ifstream configFile(configPath);
        if (!configFile) {
            std::cerr << "Failed to open " << configPath << std::endl;
            return 1;
        }
        std::string line;
        while (std::getline(configFile, line)) {
            if (line.empty() || line[0] == '#') continue;
            auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            config[key] = value;
        }
        // --- VALIDATE YAML FIELDS ---
        const char* requiredFields[] = {
            "plant_water_need_per_day", "plant_stress_threshold", "plant_absorption_rate",
            "soil_retention_rate", "soil_drainage_factor", "moisture_threshold",
            "pump_flow_rate", "water_cost", "simulation_duration"
        };
        for (const char* field : requiredFields) {
            if (config.find(field) == config.end()) {
                std::cerr << "Missing required config field: " << field << std::endl;
                return 2;
            }
        }
        // --- PARSE VALUES ---
        float plant_water_need_per_day = std::stof(config["plant_water_need_per_day"]);
        float plant_stress_threshold = std::stof(config["plant_stress_threshold"]);
        float plant_absorption_rate = std::stof(config["plant_absorption_rate"]);
        float soil_retention_rate = std::stof(config["soil_retention_rate"]);
        float soil_drainage_factor = std::stof(config["soil_drainage_factor"]);
        float moisture_threshold = std::stof(config["moisture_threshold"]);
        float pump_flow_rate = std::stof(config["pump_flow_rate"]);
        float water_cost = std::stof(config["water_cost"]);
        int config_duration = std::stoi(config["simulation_duration"]);
        float pump_power_watts = 60.0f; // Default value
        if (config.find("pump_power_watts") != config.end()) {
            pump_power_watts = std::stof(config["pump_power_watts"]);
        }
        if (simulation_duration < 0) {
            simulation_duration = config_duration > 0 ? config_duration : 86400; // Default 1 day
        }
        float simulation_step = 1.0f; // Default step size in seconds
        // Check for CLI --step argument
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--step" && i + 1 < argc) {
                simulation_step = std::stof(argv[++i]);
            }
        }
        if (config.find("simulation_step") != config.end()) {
            simulation_step = std::stof(config["simulation_step"]);
        }
        // --- INITIALIZE OBJECTS ---
        WeatherSensor weather;
        Soil soil(soil_retention_rate, soil_drainage_factor);
        Plant plant(plant_water_need_per_day, plant_stress_threshold, plant_absorption_rate);
        WaterPump pump(pump_flow_rate, pump_power_watts);
        GardenZone zone(&plant, &soil, &weather, &pump);
        Logger logger("output/output.csv");
        IrrigationController controller(&soil, &weather, &pump, &logger);
        controller.setMoistureThreshold(moisture_threshold);

        int steps = static_cast<int>(simulation_duration / simulation_step);
        float secondsElapsed = 0.0f;
        int weatherFailureStart = -1;
        int soilFailureStart = -1;
        int zones = 1;
        std::string zone_id = "Zone1";
        std::string soil_type = "Loam";
        float flow_rate = pump_flow_rate;
        for (int i = 0; i < steps; ++i) { // Simulate for configured duration
            // Simulate a simple forecast: if rain is likely in the next 10s, set forecastRain
            bool rainLikely = (weather.getRainfall() > 2.0f);
            controller.setForecastRain(rainLikely);
            controller.update(secondsElapsed);
            zone.update(secondsElapsed);
            // Detect and handle weather sensor failure
            bool weatherFailed = weather.hasFailed();
            if (weatherFailed && weatherFailureStart == -1) {
                weatherFailureStart = secondsElapsed;
                std::cout << "[WARN] Weather sensor failure detected. Using fallback values." << std::endl;
            }
            if (weatherFailed && weatherFailureStart != -1 && secondsElapsed - weatherFailureStart > 10) {
                weather.resetFailure();
                weatherFailureStart = -1;
                std::cout << "[INFO] Weather sensor automatically reset after 10s of failure." << std::endl;
            }
            // Detect and handle soil sensor failure
            bool soilFailed = (soil.getMoisture() < 0);
            if (soilFailed && soilFailureStart == -1) {
                soilFailureStart = secondsElapsed;
                std::cout << "[WARN] Soil sensor failure detected. Using fallback values." << std::endl;
            }
            if (soilFailed && soilFailureStart != -1 && secondsElapsed - soilFailureStart > 10) {
                soil.resetFailure();
                soilFailureStart = -1;
                std::cout << "[INFO] Soil sensor automatically reset after 10s of failure." << std::endl;
            }
            // Use fallback values for display if failed
            float displayTemp = weatherFailed ? controller.getLastKnownTemperature() : weather.getTemperature();
            float displayHumidity = weatherFailed ? controller.getLastKnownHumidity() : weather.getHumidity();
            float displayRain = weatherFailed ? controller.getLastKnownRainfall() : weather.getRainfall();
            float displaySoil = soilFailed ? controller.getLastKnownSoilMoisture() : soil.getMoisture();
            // Effective moisture calculation (match controller logic)
            float noise = (std::rand() % 100 - 50) / 100.0f;
            float noisyMoisture = displaySoil + noise;
            float evap = (displayTemp / 30.0f) * (1.0f - displayHumidity / 100.0f) * 0.05f;
            float effectiveMoisture = noisyMoisture + displayRain - evap;
            bool sensorError = weatherFailed || soilFailed;
            // All per-second calculations now scale by simulation_step
            float waterUsed = pump.isOn() ? pump_flow_rate * (simulation_step / 60.0f) : 0.0f; // L per step
            float powerUsed = pump.isOn() ? pump.getPowerWatts() * (simulation_step / 3600.0f) : 0.0f; // Wh per step
            // Log per-step data in new CSV format
            logger.logSecond(
                static_cast<int>(secondsElapsed),
                displaySoil,
                effectiveMoisture,
                displayTemp,
                displayHumidity,
                displayRain,
                pump.isOn(),
                flow_rate,
                waterUsed,
                plant.getStress(),
                sensorError,
                zone_id,
                soil_type,
                powerUsed // New field for power consumption
            );
            // Print per-second output (optional, can be commented for long runs)
            std::cout << "Time: " << secondsElapsed << "s | Temp: " << displayTemp
                      << "C | Humidity: " << displayHumidity << "% | Rain: " << displayRain
                      << "mm | Soil Moisture: " << displaySoil << "% | Effective Moisture: " << effectiveMoisture
                      << "% | Plant Stress: " << plant.getStress()
                      << "% | Pump: " << (pump.isOn() ? "ON" : "OFF")
                      << " | Forecast Rain: " << (rainLikely ? "YES" : "NO");
            if (weatherFailed) std::cout << " [FALLBACK:Weather]";
            if (soilFailed) std::cout << " [FALLBACK:Soil]";
            std::cout << std::endl;
            if (simulation_step >= 0.01f) {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(simulation_step * 1000)));
            }
            secondsElapsed += simulation_step;
        }
        logger.finalize();
        // --- END SUMMARY ---
        std::cout << std::fixed << std::setprecision(1);
        std::cout << "\n--- Simulation Summary ---\n";
        int daysSimulated = simulation_duration / 86400;
        std::cout << "Duration simulated: " << daysSimulated << " days (" << simulation_duration << " seconds)\n";
        std::cout << "Simulation step size: " << simulation_step << " seconds" << std::endl;
        std::cout << "\n💧 Total water used: " << logger.getTotalWaterUsed() << " liters" << std::endl;
        std::cout << "🔌 Total power used: " << logger.getTotalPowerUsed() << " Wh" << std::endl;
        std::cout << "💰 Average daily water cost: $" << logger.getAverageDailyCost(water_cost, simulation_duration) << std::endl;
        std::cout << "📊 Average plant stress level: " << logger.getAveragePlantStress() << "%" << std::endl;
        std::cout << "🌿 Watering efficiency: " << logger.getWaterEfficiency() << "%" << std::endl;
        std::cout << "🛠️ Sensor failure events: " << logger.getSensorFailureEvents() << std::endl;
        std::cout << "\nZones: " << zones << std::endl;
        std::cout << "Soil Type: " << soil_type << std::endl;
        std::cout << "Pump Flow Rate: " << flow_rate << " L/min" << std::endl;
        std::cout << "\nCSV log saved to: output/simulation_log.csv" << std::endl;
        std::cout << "-------------------------" << std::endl;
        return 0;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid value in config: " << e.what() << std::endl;
        return 3;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 4;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 5;
    }
} 