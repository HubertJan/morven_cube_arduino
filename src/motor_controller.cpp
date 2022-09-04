#include <Arduino.h>
#include <AccelStepper.h>
#include <Servo.h>

#define stepperAcceleration 42000
#define stepper100Acceleration 43000 // Limit 60000
#define stepperMaxSpeed 4000
#define stepperspeed 4000

#define stepperDelay 0
#define cornerCuttingValue 19
#define cornerCutting100Value 20

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
        debugCounter += 1;
        return;
        // stD.move(10);
        // tone(22, 1000);
        stD.run();
        stR.run();
        stF.run();
        stL.run();
        stB.run();
        stU.run();
    }
    // void MotorsOn
    MotorController()
    {
        pinMode(28, OUTPUT); // Enable
        pinMode(12, OUTPUT); // Enable
        /*
                stD.setAcceleration(acc50); //set acceleration (steps/second^2) VERMUTLICH IDEAL Ac: 8000 MaxSpeed: 3000 Speed: 1100
                stD.setMaxSpeed(maxSp);         //set max speed the motor will turn (steps/second)
                stD.setSpeed(stepperspeed);
                stR.setAcceleration(acc50); //set acceleration (steps/second^2)
                stR.setMaxSpeed(maxSp);         //set max speed the motor will turn (steps/second)
                stR.setSpeed(stepperspeed);
                stF.setAcceleration(acc50); //set acceleration (steps/second^2)
                stF.setMaxSpeed(maxSp);         //set max speed the motor will turn (steps/second)
                stF.setSpeed(stepperspeed);
                stL.setAcceleration(acc50); //set acceleration (steps/second^2)
                stL.setMaxSpeed(maxSp);         //set max speed the motor will turn (steps/second)
                stL.setSpeed(stepperspeed);
                stB.setAcceleration(acc50); //set acceleration (steps/second^2)
                stB.setMaxSpeed(maxSp);         //set max speed the motor will turn (steps/second)
                stB.setSpeed(stepperspeed);
                stU.setAcceleration(acc50); //set acceleration (steps/second^2)
                stU.setMaxSpeed(maxSp);         //set max speed the motor will turn (steps/second)
                stU.setSpeed(stepperspeed); 
        */
    }

    void setSettings(int newAcc50, int newAcc100, int newCc50, int newCc100, int newMaxSp)
    {
        acc50 = newAcc50;
        acc100 = newAcc100;
        cc50 = newCc50;
        cc100 = newCc100;
        maxSp = newMaxSp;
    }

    void moveMotor(char motor, int target)
    {
        // tone(22, 1000); //Testton

        target -= 48; // Da z.B. eine Zahl 2 als char toInt 52 ist
        if (target == 7)
        {
            target = -1;
        }
        int steps = target * 50;
        int acceleration = acc50;

        if (target == 2)
        { // Falls es eine halbe umdrehung ist, wird die stepperAcceleration erhöht
            acceleration = stepper100Acceleration;
        }

        AccelStepper responsibleStepper = getResponsibleStepper(motor);

        responsibleStepper.setAcceleration(acceleration);
        responsibleStepper.move(steps);
    }

    AccelStepper getResponsibleStepper(char motor)
    {
        switch (motor)
        {
        case 'D':
            return stD;
        case 'R':
            return stR;
        case 'F':
            return stF;
        case 'L':
            return stL;
        case 'B':
            return stB;
        case 'U':
            return stU;
        }
    }

    int getDistance(char motor)
    {
        AccelStepper responsibleStepper = getResponsibleStepper(motor);

        return responsibleStepper.distanceToGo();

    }
    bool IsCubeInstructionDone(char instruction[3])
    {
        if (debugCounter >= 5)
        {
            return true;
        }
        return false;
        int distance = getDistance(instruction[0]);
        if (distance < 0)
        {
            distance *= -1;
        }
        if (instruction[1] == '2')
        {
            if (distance <= cornerCutting100Value)
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
            if (distance <= cornerCuttingValue)
            {
                return (true);
            }
            else
            {

                return (false);
            }
        }
    }
    void ExecuteCubeInstruction(char instruction[3])
    {
        debugCounter = 0;
        // Return: Bool - If instruction is finished, return true. Otherwise false
        // tone(22, 1000); //Testton
        /*         stD.run();
                stR.run();
                stF.run();
                stL.run();
                stB.run();
                stU.run();

               moveMotor(instruction[0], instruction[1]); */
    }
    void ExecuteCubeDoubleInstruction(char instructionA[3], char instructionB[3])
    {
        debugCounter = 0;
        /*      bool isFinished = false;
             while (isFinished == false)
             {
                 bool instr1 = ExecuteCubeInstruction(instructionA);
                 bool instr2 = ExecuteCubeInstruction(instructionB);
                 if (instr1 && instr2)
                 {
                     isFinished = true;
                 }
             } */
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
    int dir = 1;           // used to switch direction
    String incoming = "1"; // for incoming serial data
    int acc50 = 42000;
    int acc100 = 43000; // Limit 60000
    int maxSp = 4000;
    int cc50 = 19;
    int cc100 = 20;

    int debugCounter = 0;

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
