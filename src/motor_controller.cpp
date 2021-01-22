#include <Arduino.h>
#include <AccelStepper.h>
#include <Servo.h>

/*AccelStepper stD(1, 0, 6); // STEP, DIR
AccelStepper stR(1, 1, 7); // STEP, DIR
AccelStepper stF(1, 2, 8); // STEP, DIR
AccelStepper stL(1, 3, 9); // STEP, DIR
AccelStepper stB(1, 4, 10); // STEP, DIR
AccelStepper stU(1, 5, 11); // STEP, DIR
*/

#define stepperAcceleration 30000
#define stepper100Acceleration 35000 //Limit 60000
#define stepperMaxSpeed 3000
#define stepperspeed 3000

#define stepperDelay 0
#define cornerCuttingValue 2
#define cornerCutting100Value 5

class MotorController
{
public:
    AccelStepper stD = AccelStepper(1, 0, 6);  // STEP, DIR
    AccelStepper stR = AccelStepper(1, 1, 7);  // STEP, DIR
    AccelStepper stF = AccelStepper(1, 2, 8);  // STEP, DIR
    AccelStepper stL = AccelStepper(1, 3, 9);  // STEP, DIR
    AccelStepper stB = AccelStepper(1, 4, 10); // STEP, DIR
    AccelStepper stU = AccelStepper(1, 5, 11);
    void runMotors()
    {
        //stD.move(10);
        stD.run();
        stR.run();
        stF.run();
        stL.run();
        stB.run();
        stU.run();
    }
    MotorController()
    {
        pinMode(28, OUTPUT); // Enable
        pinMode(12, OUTPUT); //Enable

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
        stU.setSpeed(stepperspeed);
    }

    void moveMotor(char motor, int target)
    {
        target -= 50; //Da z.B. eine Zahl 2 als char toInt 52 ist
        if (target == 7)
        {
            target = -1;
        }
        int steps = target * 50;
        int acceleration = stepperAcceleration;
        if (target == 2)
        { // Falls es eine halbe umdrehung ist, wird die stepperAcceleration erhöht
            acceleration = stepper100Acceleration;
        }
        if (motor == 'D')
        {
            stD.setAcceleration(acceleration);
            stD.move(steps);
        }
        else if (motor == 'R')
        {
            stR.setAcceleration(acceleration);
            stR.move(steps);
        }
        else if (motor == 'F')
        {
            stF.setAcceleration(acceleration);
            stF.move(steps);
        }
        else if (motor == 'L')
        {
            stL.setAcceleration(acceleration);
            stL.move(steps);
        }
        else if (motor == 'B')
        {
            stB.setAcceleration(acceleration);
            stB.move(steps);
        }
        else if (motor == 'U')
        {
            stU.setAcceleration(acceleration);
            stU.move(steps);
        }
    }

    int getDistance(char motor)
    {
        if (motor == 'D')
        {
            return (stD.distanceToGo());
        }
        else if (motor == 'R')
        {
            return (stR.distanceToGo());
        }
        else if (motor == 'F')
        {
            return (stF.distanceToGo());
        }
        else if (motor == 'L')
        {
            return (stL.distanceToGo());
        }
        else if (motor == 'B')
        {
            return (stB.distanceToGo());
        }
        else if (motor == 'U')
        {
            return (stU.distanceToGo());
        }
    }
    bool IsCubeInstructionDone(char instruction[3])
    {
        if (instruction[1] == 2)
        {
            if (getDistance(instruction[0]) <= cornerCutting100Value)
            {
                return (true);
            }
            else
            {
                return (false);
            }
        }
        else
        {
            if (getDistance(instruction[0]) <= cornerCuttingValue)
            {
                return (true);
            }
            else
            {
                return (false);
            }
        }
    }
    bool ExecuteCubeInstruction(char instruction[3])
    { // Return: Bool - If instruction is finished, return true. Otherwise false
        stD.run();
        stR.run();
        stF.run();
        stL.run();
        stB.run();
        stU.run();

       moveMotor(instruction[0], instruction[1]);
    }
    bool CheckDoubleInstruction(char instructionA[3], char instructionB[3])
    {
        bool isDouble = false;
        if (instructionA[0] == instructionB[0])
        {
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
