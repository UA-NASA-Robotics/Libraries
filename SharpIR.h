#ifndef SharpIR_h
#define SharpIR_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
    #include <pins_arduino.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdbool.h>
struct GPY0A21YK;

void distanceBegin(struct GPY0A21YK *sensor, uint16_t distancePin);

void distanceBegin(struct GPY0A21YK *sensor, uint16_t distancePin, uint16_t vccPin);
    
void GP0A21YK_IOInit();

uint16_t captureAI();

uint16_t getDistanceRaw(struct GPY0A21YK *sensor);
    
uint16_t getDistanceVolt(struct GPY0A21YK *sensor);
    
uint16_t getDistanceCent(struct GPY0A21YK *sensor);
    
//uint16_t mapGPY0A21YK_V(uint16_t value);
    
//uint16_t mapGPY0A21YK_CM(uint16_t value);
    
void setEnabled(struct GPY0A21YK *sensor, bool status);
    
#endif