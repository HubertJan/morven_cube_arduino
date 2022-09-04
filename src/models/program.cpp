#include <Arduino.h>

struct Program
{
    String id = "";
    int lastInstructionId = 0;
    String instructions = "";
    int totalInstructions;
    unsigned long runningTime = 0;
    unsigned long timeOfProgramStart = 0;
};