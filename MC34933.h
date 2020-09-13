/*
 MC34933.h - Library for the Toshiba MC34933 motor driver.
 Created by TheDIYGuy999 June - November 2016
 Released into the public domain.
 
 This is Version 1.2
 
 Change history:
 
 V1.1:
 The pin configuration was moved to the separate begin() function.
 Change your existing programs in accordance with the provided example
 
 V1.2:
 minPWM input variable added to the drive() function.
 Allows to eliminate the backlash in self balancing applications
 */


#ifndef MC34933H
#define MC34933H

#include "Arduino.h"

// Class definition (header) ========================================================================
class MC34933 {
    public:
    MC34933();
    void begin(int pin1, int pin2, int pwmPin, int minInput, int maxInput, int neutralWidth, boolean invert);
    boolean drive(int controlValue, int minPWM, int maxPWM, int rampTime, boolean neutralBrake);
    boolean brakeActive();
    
    private:
    int _pin1;
    int _pin2;
    int _pwmPin;
    int _minInput;
    int _maxInput;
    int _minNeutral;
    int _maxNeutral;
    int _controlValue;
    int _controlValueRamp;
    int _minPWM;
    int _maxPWM;
    int _rampTime;
    boolean _neutralBrake;
    boolean _invert;
    unsigned long _previousMillis = 0;
    unsigned long _previousMillisBrake = 0;
    byte _state = 0;
    byte _forward;
    byte _reverse;
    byte _upwards;
    byte _downwards;
};

#endif //MC34933H
