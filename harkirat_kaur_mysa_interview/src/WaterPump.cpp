#include "../include/WaterPump.h"

WaterPump::WaterPump(float flowRateLpm, float powerWatts)
    : flowRate(flowRateLpm), powerWatts(powerWatts), on(false), runTime(0), maxRunTime(600), cooldownTime(300), cooldownLeft(0) {}

void WaterPump::turnOn() {
    if (canRun()) {
        on = true;
    }
}
void WaterPump::turnOff() {
    on = false;
    runTime = 0;
    cooldownLeft = cooldownTime;
}
bool WaterPump::isOn() const { return on; }
float WaterPump::getFlowRate() const { return flowRate; }
float WaterPump::getPowerWatts() const { return powerWatts; }

void WaterPump::update(int secondsElapsed) {
    if (on) {
        runTime++;
        if (runTime >= maxRunTime) {
            turnOff();
        }
    } else if (cooldownLeft > 0) {
        cooldownLeft--;
    }
}
bool WaterPump::canRun() const {
    return !on && cooldownLeft == 0;
}
void WaterPump::setMaxRunTime(int seconds) {
    maxRunTime = seconds;
}
void WaterPump::setCooldownTime(int seconds) {
    cooldownTime = seconds;
} 