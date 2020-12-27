#include <Arduino.h>
#include <AccelStepper.h>
#include <Servo.h>

#define stepperAcceleration 50000
#define stepperMaxSpeed 3000
#define stepperspeed 3000

#define stepperDelay 0
#define Mosfet 30

class MotorController
{
public:
    MotorController()
    {
        /*         pinMode(7, OUTPUT);
        pinMode(27, OUTPUT);

        stD.setAcceleration(stepperAcceleration); //set acceleration (steps/second^2) VERMUTLICH IDEAL Ac: 8000 MaxSpeed: 3000 Speed: 1100
        stD.setMaxSpeed(stepperMaxSpeed);         //set max speed the motor will turn (steps/second)
        stD.setSpeed(stepperspeed);
        stR.setAcceleration(stepperAcceleration); //set acceleration (steps/second^2)
        stR.setMaxSpeed(stepperMaxSpeed);         //set max speed the motor will turn (steps/second)
        stR.setSpeed(stepperspeed);
        stF.setAcceleration(stepperAcceleration); //set acceleration (steps/second^2)
        stF.setMaxSpeed(stepperMaxSpeed);         //set max speed the motor will turn (steps/second)
        stF.setSpeed(stepperspeed);
        stL.setAcceleration(stepperAcceleration); //set acceleration (steps/second^2)
        stL.setMaxSpeed(stepperMaxSpeed);         //set max speed the motor will turn (steps/second)
        stL.setSpeed(stepperspeed);
        stB.setAcceleration(stepperAcceleration); //set acceleration (steps/second^2)
        stB.setMaxSpeed(stepperMaxSpeed);         //set max speed the motor will turn (steps/second)
        stB.setSpeed(stepperspeed);
        stU.setAcceleration(stepperAcceleration); //set acceleration (steps/second^2)
        stU.setMaxSpeed(stepperMaxSpeed);         //set max speed the motor will turn (steps/second)
        stU.setSpeed(stepperspeed); */
    }
    void ExecuteCubeInstruction(char instruction[3])
    {
        /* 
        if (instruction == "F1")
        {
            stF.move(50);
            stF.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "F2")
        {
            stF.move(100);
            stF.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "F3")
        {
            stF.move(-50);
            stF.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "R1")
        {
            stR.move(50);
            stR.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "R2")
        {
            stR.move(100);
            stR.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "R3")
        {
            stR.move(-50);
            stR.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "U1")
        {
            stU.move(50);
            stU.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "U2")
        {
            stU.move(100);
            stU.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "U3")
        {
            stU.move(-50);
            stU.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "B1")
        {
            stB.move(50);
            stB.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "B2")
        {
            stB.move(100);
            stB.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "B3")
        {
            stB.move(-50);
            stB.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "L1")
        {
            stL.move(50);
            stL.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "L2")
        {
            stL.move(100);
            stL.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "L3")
        {
            stL.move(-50);
            stL.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "D1")
        {
            stD.move(50);
            stD.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "D2")
        {
            stD.move(100);
            stD.runToPosition();
            delay(stepperDelay);
        }
        else if (instruction == "D3")
        {
            stD.move(-50);
            stD.runToPosition();
            delay(stepperDelay);
        }
        else
        {
            delay(50);
        }
    }

    void MoveCamera(String command)
    {
        if (incoming == "TBS22")
        {
            FirstStepperSpin(-22);
        }
        else if (incoming == "TBS50")
        {
            stD.move(-50);
            stD.runToPosition();
        }
        else if (incoming == "TBS100")
        {
            FirstStepperSpin(-100);
        }
        else if (incoming == "TBS-75")
        {
            FirstStepperSpin(75);
        }
        else if (incoming == "TBS28")
        {
            stD.move(-28);
            stD.runToPosition();
        } */
    }
    void ExecuteCubeDoubleInstruction(char instructionA[3], char instructionB[3])
    {
    }

    bool CheckDoubleInstruction(char instructionA[3], char instructionB[3])
    {
        bool isDouble = false;
        if(instructionA[0] == instructionB[0]){
            return false;
        }

        for (int i = 0; i < 3; i = i + 1)
        {
            bool contains = false;
            for (int o = 0; o < 2; o = o + 1)
            {
                if (!contains)
                {
                    if (doubleInstructionList[i][o] == instructionA[0] || doubleInstructionList[i][o] == instructionB[0])
                    {
                        contains = true;
                    }
                }
                else
                {
                    if (doubleInstructionList[i][o] == instructionA[0] || doubleInstructionList[i][o] == instructionB[0])
                    {
                        isDouble = true;
                    }
                    i = 3;
                    o = 2;
                }
            }
        }

        return isDouble;
    }

private:
    AccelStepper stD = AccelStepper(1, 5, 52); // STEP, DIR
    AccelStepper stR = AccelStepper(1, 2, 22); // STEP, DIR
    AccelStepper stF = AccelStepper(1, 6, 26); // STEP, DIR
    AccelStepper stL = AccelStepper(1, 4, 24); // STEP, DIR
    AccelStepper stB = AccelStepper(1, 3, 23); // STEP, DIR
    AccelStepper stU = AccelStepper(1, 7, 27);

    int speeds = 0;
    int dir = 1;           //used to switch direction
    String incoming = "1"; // for incoming serial data

    void FirstStepperSpin(int steps)
    {
        stD.setAcceleration(5000);
        stD.setMaxSpeed(700);
        stD.move(steps);
        stD.runToPosition();
    }

    const char doubleInstructionList[3][2] = {
        {'L', 'R'},
        {'U', 'D'},
        {'F', 'B'},
    };
};