# Mysa Irrigation System

This project is an irrigation system controller.

## Project Structure

```
MysaIrrigationSystem/
├── include/    # Header files
├── src/        # Source files
├── test/       # Unit and integration tests
├── config/     # Configuration files (YAML/JSON)
├── output/     # Output data/logs
├── main.cpp    # Main entry point
├── Makefile    # Build instructions
├── README.md   # Project documentation
```

---

## Configuration

Edit `config/config.yaml` to set simulation parameters. Example:

```
plant_water_need_per_day=2.0
plant_stress_threshold=30.0
plant_absorption_rate=0.05
soil_retention_rate=0.8
soil_drainage_factor=0.2
moisture_threshold=40.0
pump_flow_rate=6.0
water_cost=0.1
simulation_duration=60
# Water Conservation Mode
conservation_mode_enabled=false
conservation_water_cost_threshold=0.5
conservation_drought_moisture_threshold=20.0
conservation_moisture_threshold=35.0
conservation_night_start_hour=22
conservation_night_end_hour=6
```

### Config File Format & Fields
- Each line is a `key=value` pair (no YAML parser required).
- All fields are required unless marked optional.
- **plant_water_need_per_day**: Liters/day each plant needs.
- **plant_stress_threshold**: Stress % above which plant is considered stressed.
- **plant_absorption_rate**: Fraction of available water absorbed per second.
- **soil_retention_rate**: Fraction of water retained by soil (0–1).
- **soil_drainage_factor**: Fraction of water lost to drainage (0–1).
- **moisture_threshold**: Soil moisture % below which irrigation is triggered.
- **pump_flow_rate**: Pump flow rate in L/min.
- **water_cost**: Cost per liter of water.
- **simulation_duration**: Duration of simulation in seconds.
- **conservation_mode_enabled**: Enable water conservation mode (true/false).
- **conservation_water_cost_threshold**: Water cost above which conservation mode triggers.
- **conservation_drought_moisture_threshold**: Soil moisture % below which drought is detected.
- **conservation_moisture_threshold**: Stricter threshold for irrigation in conservation mode.
- **conservation_night_start_hour**: Start hour (24h) for night-only watering in conservation mode.
- **conservation_night_end_hour**: End hour (24h) for night-only watering in conservation mode.
- **simulation_step**: Simulation step size in seconds (e.g., 1.0 for 1s per iteration; can be <1 for sub-second or >1 for multi-second steps).

---

## Building and Running (MinGW)

1. **Open MinGW terminal** (Command Prompt, PowerShell, or MSYS2 MinGW shell).
2. **Navigate to the project directory:**
   ```sh
   cd /c/Users/kkira/OneDrive/Desktop/Projects/Mysa/MysaIrrigationSystem
   ```
3. **Build the project:**
   ```sh
   mingw32-make
   ```
   - If you get `'make' is not recognized`, ensure MinGW's `bin` directory is in your `PATH`.
4. **Run the program:**
   ```sh
   ./mysa_irrigation.exe
   ```
   or
   ```sh
   mysa_irrigation.exe
   ```

---

## Troubleshooting

- **No Makefile found:**
  - Make sure you are in the `MysaIrrigationSystem` directory (where the `Makefile` is).
- **Config file errors:**
  - Ensure `config/config.yaml` exists and is formatted as `key=value` pairs.
- **Compilation errors:**
  - Make sure you are using a C++11 compatible compiler (MinGW g++ is recommended).
- **Executable not found:**
  - Check for `mysa_irrigation.exe` in the current directory after building.

---

## Customization
- Change simulation parameters in `config/config.yaml` as needed.
- Extend the code for more features or different config options.

---

## Assumptions

### Soil Behavior
- Soil moisture is tracked as a percentage (0–100). Values are clamped to this range.
- Water from rainfall and irrigation is added to soil, scaled by a retention rate (default: 0.8).
- Water is lost from soil due to evapotranspiration, scaled by (1 - drainage factor) (default: 0.2).

### Plant Needs
- Each plant has a daily water need (default: 10.0 liters/day).
- Plants absorb a fraction of available water per second (absorption rate, default: 0.05).
- If absorbed water is less than the per-second need, plant stress increases; otherwise, it decreases.
- Plant stress is clamped between 0 and 100.

### Rainfall and Weather Patterns
- Weather is simulated with a daily temperature cycle (sine wave).
- Humidity is inversely related to temperature.
- Rainfall events occur randomly (10% chance per simulated hour, 0.5–5 mm per event).
- Weather sensors can fail randomly (1% chance per update).
- Sensor failures are logged and the system uses the last known value as a fallback.

---

## Moisture Formula Breakdown

The effective soil moisture used for irrigation decisions is calculated as:

```
effectiveMoisture = measuredSoilMoisture + recentRainfall * retentionFactor - evapotranspiration
```
Where:
- `measuredSoilMoisture`: Current soil moisture (with sensor noise and fallback on failure).
- `recentRainfall`: Rainfall since the last update (per interval, not rolling sum).
- `retentionFactor`: Fraction of rainfall retained by the soil (from config).
- `evapotranspiration`: Water lost due to evaporation and plant transpiration, calculated as:
  ```
  evapotranspiration = (temperature / 30) * (1 - humidity / 100) * 0.05
  ```

---

## Power Consumption Tracking

**New Feature:**  
The system now tracks the electrical power consumed by the water pump.

- **Config Parameter:**  
  `pump_power_watts` — Power consumption of the pump in Watts (W).  
  Example:  
  ```
  pump_power_watts=60.0
  ```
- **How it works:**  
  - When the pump is ON, its power usage is logged every second.
  - Power usage per second is calculated as:  
    ```
    power_used = pump_power_watts / 3600.0  // in Watt-hours (Wh) per second
    ```
  - The total power used is accumulated and reported at the end of the simulation.

- **CSV Output:**  
  The output CSV now includes a `PowerUsed (Wh)` column, showing the energy consumed by the pump each second.

- **Simulation Summary:**  
  At the end of the simulation, the total power used (in Wh) is printed to stdout.

---

## Output CSV Columns

| Column                | Description                                      |
|-----------------------|--------------------------------------------------|
| Timestamp             | Date and time of the log entry                   |
| SoilMoisture (%)      | Current soil moisture percentage                 |
| EffectiveMoisture (%) | Calculated effective moisture for irrigation     |
| Temperature (°C)      | Simulated air temperature                        |
| Humidity (%)          | Simulated relative humidity                      |
| Rainfall (mm)         | Rainfall in the last second                      |
| PumpState             | ON/OFF state of the water pump                   |
| FlowRate (L/min)      | Pump flow rate                                   |
| WaterUsed (L)         | Water used by the pump in this second            |
| PlantStress (%)       | Plant stress indicator                           |
| SensorError           | TRUE if any sensor failed this second            |
| ZoneID                | Zone identifier                                  |
| SoilType              | Soil type for this zone                          |
| PowerUsed (Wh)        | Pump power consumption in Watt-hours this second  |
| SimulationStep (s)     | Simulation step size in seconds                      |

---

## Embedded/Efficiency Assumptions

- The code is designed for low memory usage:
  - Uses simple data structures (no dynamic allocation in main loop)
  - Per-second logging can be redirected to SD card or external storage
- All calculations are performed with basic arithmetic (no heavy libraries)
- Sensor failure handling and error checking are lightweight

---

## Safety and Error Handling

- **Pump Safety:**  
  - The pump has a maximum run time and cooldown period to prevent overheating or damage.
- **Overwatering Prevention:**  
  - Irrigation only occurs when soil moisture is below the threshold and no rain is forecast.
- **Sensor Failures:**  
  - If a sensor fails, the last known value is used and the event is logged.
- **Config Validation:**  
  - All required config fields are checked at startup; the program exits with an error if any are missing.

---

## Methodology Summary

- **Effective Moisture Formula:**  
  ```
  effectiveMoisture = measuredSoilMoisture + recentRainfall * retentionFactor - evapotranspiration
  ```
- **Evapotranspiration:**  
  ```
  evapotranspiration = (temperature / 30) * (1 - humidity / 100) * 0.05
  ```
- **Power Consumption:**  
  ```
  power_used = pump_power_watts / 3600.0  // Wh per second when pump is ON
  ```
- **Watering Logic:**  
  - See "Watering Strategy Logic" section above for full details.

---

## Watering Strategy Logic

### Standard Logic
- If effective soil moisture drops below the threshold and no rain is forecast, the pump is activated.
- If rain is forecast or effective moisture is sufficient, the pump remains off.
- If any sensor fails, the system logs the failure and uses the last known value.

### Weather-Aware Irrigation
- Before irrigating, the system checks if rainfall is forecast in the next X hours (configurable).
- If forecasted rain exceeds a threshold, irrigation is delayed.

### Water Conservation Mode
- Triggers if water cost exceeds a threshold or drought is detected (soil moisture below threshold).
- In conservation mode:
  - A stricter moisture threshold is used.
  - Watering is only allowed during a night window (configurable start/end hour).

### Predictive Watering
- The system tracks rainfall and soil moisture history for the last 2–3 days (configurable).
- A moving average is used to detect dry or wet trends:
  - If the trend is dry (low rain, low moisture), the system is more aggressive (lowers threshold).
  - If the trend is wet (high rain, high moisture), the system is more conservative (raises threshold).
- Assumptions: "Dry" means avgRain < 1mm/hr and avgMoisture < 30%. "Wet" means avgRain > 2mm/hr or avgMoisture > 60%.

### Multi-Zone Coordination
- Supports multiple zones with a shared limit on concurrent active pumps (default: 2).
- Each zone checks if it can activate its pump before turning on.

### Sensor Failure Handling
- If a sensor read fails (returns -1 or -999), the system logs the failure and uses the last known value or an estimate.
- Failures are recorded in the output log.

### Error Handling
- All config fields are validated before use.
- File I/O and sensor reads are wrapped in try/catch blocks.
- The system exits gracefully with an error message if a critical error occurs.

---

## License
MIT (or specify your license here) 

---

## Build & Run Instructions

### Windows (cmd/PowerShell)
1. Open Command Prompt or PowerShell.
2. Navigate to the project directory:
   ```
   cd path\to\MysaIrrigationSystem
   ```
3. Build and run:
   ```
   compile.bat
   ```
   This will compile and run `mysa_irrigation.exe`.

### Linux/macOS
1. Open a terminal.
2. Navigate to the project directory:
   ```sh
   cd /path/to/MysaIrrigationSystem
   ```
3. Build with Makefile:
   ```sh
   make
   ```
   Or use the provided script:
   ```sh
   ./compile.sh
   ```
4. Run the program:
   ```sh
   ./mysa_irrigation
   ```

### MSYS2/Git Bash (Windows)
1. Open MSYS2 or Git Bash.
2. Navigate to the project directory:
   ```sh
   cd /c/Users/kkira/OneDrive/Desktop/Projects/Mysa/MysaIrrigationSystem
   ```
3. Build:
   ```sh
   ./compile.sh
   ```
4. Run:
   ```sh
   ./mysa_irrigation.exe
   ```

### Command-Line Options
- To specify a custom config file:
  ```sh
  ./mysa_irrigation --configuration config/config.yaml
  ```
- To set simulation duration (e.g., 2 hours):
  ```sh
  ./mysa_irrigation --duration 2h
  ```
- To set simulation step size (e.g., 0.5 seconds):
  ```sh
  ./mysa_irrigation --step 0.5
  ```

--- 