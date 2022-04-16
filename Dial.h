#ifndef DIAL_H
#define DIAL_H

#include "Arduino.h"

#define NEG_FAST 70
#define NEG_SLOW 160
#define POS_SLOW 500
#define POS_FAST 850

class Dial {
private:
    uint8_t _pin;

public:
    void begin(uint8_t pin){
        _pin = pin;
        pinMode(_pin, INPUT);
    }

    int16_t get_value(){
        return analogRead(_pin);
    }

    int8_t get_state(){ // -2 (back fast), -1 (back slow), 0 (deadzone), 1 (forward slow), 2 (forward fast)
        int16_t read = analogRead(_pin);
        if (read <= NEG_FAST) return -2;
        if (read <= NEG_SLOW) return -1;
        if (read <= POS_SLOW) return 0;
        if (read <= POS_FAST) return 1;
        return 2;
    }
};

#endif