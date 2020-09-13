/*
 MC34933.h - Library for the Toshiba MC34933 motor driver.
 Created by TheDIYGuy999 June - November 2016
 Released into the public domain.
 
 This is Version 1.2
 
 The pin configuration was moved to the separate begin() function.
 Change your existing programs in accordance with the provided example
 */

#include "Arduino.h"
#include "MC34933.h"

// Member definition (code) ========================================================================

MC34933::MC34933() { // Constructor
    _state = 0;
    _previousMillis = 0;
}

// Begin function ************************************************************

// NOTE: The first pin must always be PWM capable, the second only, if the last parameter is set to "true"
// SYNTAX: IN1, IN2, min. input value, max. input value, neutral position width
// invert rotation direction, true = both pins are PWM capable
void MC34933::begin(int pin1, int pin2, int minInput, int maxInput, int neutralWidth, boolean invert) {
    _pin1 = pin1;
    _pin2 = pin2;
    _minInput = minInput;
    _maxInput = maxInput;
    _minNeutral = (_maxInput + _minInput) / 2 - (neutralWidth / 2);
    _maxNeutral = (_maxInput + _minInput) / 2 + (neutralWidth / 2);
    _controlValueRamp = (_minNeutral + _maxNeutral) / 2;
    _invert = invert;
    
    pinMode(_pin1, OUTPUT);
    pinMode(_pin2, OUTPUT);
    digitalWrite(_pin1, LOW);
    digitalWrite(_pin2, LOW);
}

// Drive function ************************************************************

// SYNTAX: Input value, max PWM, ramptime in ms per 1 PWM increment
// true = brake active, false = brake in neutral position inactive
boolean MC34933::drive(int controlValue, int minPWM, int maxPWM, int rampTime, boolean neutralBrake) {
    _controlValue = controlValue;
    _minPWM = minPWM;
    _maxPWM = maxPWM;
    _rampTime = rampTime;
    _neutralBrake = neutralBrake;
    
    
    
    if (_invert) {
        _controlValue = map (_controlValue, _minInput, _maxInput, _maxInput, _minInput); // invert driving direction
    }
    
    // Fader (allows to ramp the motor speed slowly up & down) --------------------
    if (_rampTime >= 1) {
        unsigned long _currentMillis = millis();
        if (_currentMillis - _previousMillis >= _rampTime) {
            // Increase
            if (_controlValue > _controlValueRamp && _controlValueRamp < _maxInput) {
                _controlValueRamp++;
                _upwards = true;
                _downwards = false;
            }
            // Decrease
            else if (_controlValue < _controlValueRamp && _controlValueRamp > _minInput) {
                _controlValueRamp--;
                _upwards = false;
                _downwards = true;
            }
            else {
                _upwards = false;
                _downwards = false;
            }
            _previousMillis = _currentMillis;
        }
    }
    else {
        _controlValueRamp = _controlValue;
        _upwards = false;
        _downwards = false;
    }
    
    // H bridge controller -------------------
    if (_controlValueRamp >= _maxNeutral) { // Forward
        digitalWrite(_pin1, HIGH);
        //digitalWrite(_pin2, LOW);
        analogWrite(_pin2, map(_controlValueRamp, _maxNeutral, _maxInput, _minPWM, _maxPWM));
        _forward = true;
        _reverse = false;
        return true;
    }
    else if (_controlValueRamp <= _minNeutral) { // Reverse
        digitalWrite(_pin1, LOW);
        //digitalWrite(_pin2, HIGH);
        analogWrite(_pin2, map(_controlValueRamp, _minNeutral, _minInput, _minPWM, _maxPWM));
        _forward = false;
        _reverse = true;
        return true;
    }
    else { // Neutral
        if (_neutralBrake) {
            digitalWrite(_pin1, LOW); // Brake in neutral position active
            digitalWrite(_pin2, LOW);
        }
        else {
            digitalWrite(_pin1, HIGH); // Brake in neutral position inactive
            digitalWrite(_pin2, HIGH);
        }
        _forward = false;
        _reverse = false;
        return false;
    }
}

// Brakelight detection function ************************************************************
boolean MC34933::brakeActive() {
    
    unsigned long _currentMillisBrake = millis();
    
    // Reset delay timer, if vehicle isn't decelerating
    if ( (!(_upwards && _reverse)) && (!(_downwards && _forward)) ) {
        _previousMillisBrake = _currentMillisBrake;
    }
    
    if (_currentMillisBrake - _previousMillisBrake >= 100) {
        return true;
    }
    else {
        return false;
    }
}

