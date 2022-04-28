#ifndef MOTORDRIVER_H
#define MOTORDRIVER_H

#include "Arduino.h"

#define MAX_POWER 125 // Number from 1-255

class MotorDriver {
private:
    uint8_t _pwm_pin;
    uint8_t _dir_pin;

public:
    void begin(uint8_t pwm_pin, uint8_t dir_pin){
        _pwm_pin = pwm_pin;
        _dir_pin = dir_pin;
        pinMode(_pwm_pin, OUTPUT);
        pinMode(_dir_pin, OUTPUT);
        delay(100);
        digitalWrite(_dir_pin, HIGH);
        analogWrite(_pwm_pin, 0);
    }

    void set_power(uint8_t power){
        analogWrite(_pwm_pin, min(power, MAX_POWER));
    }

    void stop(){
        analogWrite(_pwm_pin, 0);
    }
};

#endif
