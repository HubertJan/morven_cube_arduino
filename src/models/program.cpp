#include <Arduino.h>

struct Program
{
    String id = "";
    String instructions = "";
    int totalInstructions;
};

struct ProgramState
{
    int lastFinishedInstructionId;
    bool isRunning;
    unsigned long runningTime = 0;
    unsigned long timeOfProgramStart = 0;
}

struct InstructionState
{
    bool isCurrentlyExecutingDouble;
    char firstCurrentlyExecutingInstruction[3];
    char secondCurrentlyExecutingInstruction[3];
};