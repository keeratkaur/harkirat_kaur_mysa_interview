#ifndef WATERPUMP_H
#define WATERPUMP_H

class WaterPump {
public:
    WaterPump(float flowRateLpm, float powerWatts); // Updated constructor
    void turnOn();
    void turnOff();
    bool isOn() const;
    float getFlowRate() const;
    float getPowerWatts() const; // Getter for power consumption
    void update(int secondsElapsed); // Call every second to update timers
    bool canRun() const;            // Returns true if not in cooldown and under max run time
    void setMaxRunTime(int seconds);    // Set max run time
    void setCooldownTime(int seconds);  // Set cooldown period
private:
    float flowRate; // Liters per minute
    float powerWatts; // Power consumption in Watts
    bool on;
    int runTime;        // Seconds pump has been running
    int maxRunTime;     // Max allowed run time in seconds
    int cooldownTime;   // Cooldown period in seconds
    int cooldownLeft;   // Seconds left in cooldown
};

#endif // WATERPUMP_H 