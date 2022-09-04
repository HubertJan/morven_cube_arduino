#include "pc_connector.cpp"
#include "motor_controller.cpp"

class Magic : IArduinoClient
{
public:
    Magic() = delete;
    Magic(int baudRate)
    {
        connector = new PCConnector(baudRate, this);
        motorController = new MotorController();
    }
    void Update()
    {
        connector->Update();
        motorController->runMotors();
        if (statusCode == STATUS::PREPARE)
        {
            char currentInstruction[3] = {prepareInstructions[0], prepareInstructions[1], prepareInstructions[2]};
            motorController->ExecuteCubeInstruction(currentInstruction);
            AddLatestExecutedInstruction(currentInstruction);
            prepareInstructions.remove(0, 2);
            if (prepareInstructions == "")
            {
                statusCode = STATUS::RUN;
            }
        }
        else if (statusCode == STATUS::RUN)
        {
            int programLength = (currentProgram.instructions.length() + 1) / 3;

            char currentInstruction[3] = {};
            char nextInstruction[3] = {};
            int finishedIns = 0;
            bool executeAsDouble = false;
            GetInstruction(currentProgram.instructions, currentProgram.currentInstruction, currentInstruction);
            if (setting.doubleInstruction == true && currentProgram.currentInstruction < programLength)
            {
                GetInstruction(currentProgram.instructions, currentProgram.currentInstruction + 1, nextInstruction);
                executeAsDouble = motorController->CheckDoubleInstruction(currentInstruction, nextInstruction);
            }
            if (executeAsDouble)
            {
                if (hasMoveInit == false)
                {
                    connector->DebugPrint(String(currentInstruction));
                    connector->DebugPrint(String(nextInstruction));
                    motorController->ExecuteCubeInstruction(currentInstruction);
                    motorController->ExecuteCubeInstruction(nextInstruction);
                    hasMoveInit = true;
                }
                else
                {
                    if (motorController->IsCubeInstructionDone(currentInstruction) && motorController->IsCubeInstructionDone(nextInstruction))
                    {
                        finishedIns = 2;
                        AddLatestExecutedInstruction(currentInstruction);
                        AddLatestExecutedInstruction(nextInstruction);
                        hasMoveInit = false;
                    }
                    else
                    {
                        finishedIns = 0;
                    }
                }
            }
            else
            {
                if (hasMoveInit == false)
                {
                    connector->DebugPrint("Move");
                    connector->DebugPrint(String(currentInstruction));
                    motorController->ExecuteCubeInstruction(currentInstruction);
                    hasMoveInit = true;
                }
                else
                {
                    if (motorController->IsCubeInstructionDone(currentInstruction))
                    {
                        connector->DebugPrint("done!");
                        finishedIns = 1;
                        AddLatestExecutedInstruction(currentInstruction);
                        hasMoveInit = false;
                    }
                    else
                    {
                        connector->DebugPrint("Still Moving");
                        finishedIns = 0;
                    }
                }
            }
            motorController->runMotors();
            if (currentProgram.timeOfProgramStart == 0)
            {
              digitalWrite(28, LOW);
              digitalWrite(12, LOW);
                
                currentProgram.timeOfProgramStart = millis();
            }
            

            if (programLength <= currentProgram.currentInstruction)
            {
                currentProgram.runningTime = millis() - currentProgram.timeOfProgramStart;
                statusCode = STATUS::FINISHED;
                
                digitalWrite(28, HIGH);
                digitalWrite(12, HIGH);
                delay(800);
            }
            else
            {
                currentProgram.currentInstruction += finishedIns;
                
            }
            stateChanged = true;
        }
        motorController->runMotors();
        if (stateChanged)
        {
            if (statusCode != STATUS::RUN) //war mal da: setting.noLogMode == false || 
            {
                connector->DebugPrint("Send Full Status");
                connector->SendFullStatus(currentProgram.instructions, currentProgram.id, String(currentProgram.currentInstruction), GetLatestExecutedInstruction(), StatusCodeAsString(), String(currentProgram.runningTime));
                connector->DebugPrint("Sucessfull");
            }
            stateChanged = false;
        }
    }
    void SwitchPause(bool pause)
    {
        if (statusCode == STATUS::RUN && pause == true)
        {
            statusCode = STATUS::PAUSE;
        }
        else if (statusCode == STATUS::PAUSE && pause == false)
        {
            statusCode = STATUS::RUN;
        }
        else
        {
            return;
        }
        stateChanged = true;
    }

    void SetProgram(String instructionString, String id, bool isDoubleIns, MotorSettings motorSetting)
    {
        setting.doubleInstruction = isDoubleIns;
        motorController->setSettings(motorSetting.acc50, motorSetting.acc100, motorSetting.cc50, motorSetting.cc100, motorSetting.maxSp);
        if (statusCode == STATUS::IDLE || statusCode == STATUS::FINISHED)
        {
            currentProgram.id = id;
            currentProgram.instructions = instructionString;
            currentProgram.currentInstruction = 0;
            currentProgram.runningTime = 0;
            currentProgram.timeOfProgramStart = 0;
            if (statusCode != STATUS::PREPARE)
            {
                statusCode = STATUS::RUN;
            }
            stateChanged = true;
        }
    }

    void SetSetting(int newSpeed, bool doubleInstruction)
    {
        if (statusCode == STATUS::IDLE || statusCode == STATUS::FINISHED)
        {
            setting.speed = newSpeed;
            setting.doubleInstruction = doubleInstruction;
        }
    }

private:
    String prepareInstructions = "";
    bool hasMoveInit = false;

    void GetInstruction(String instructions, int id, char *outInstruction)
    {
        outInstruction[0] = instructions[id * 3];
        outInstruction[1] = instructions[id * 3 + 1];
    };

    String StatusCodeAsString()
    {
        switch (statusCode)
        {
        case (STATUS::IDLE):
            return "IDLE";
            break;
        case (STATUS::RUN):
            return "RUN";
            break;
        case (STATUS::PAUSE):
            return "PAUSE";
            break;
        case (STATUS::FINISHED):
            return "FINISHED";
            break;
        default:
            return "UNKNOWN";
            break;
        }
    }

    void AddLatestExecutedInstruction(char instruction[3])
    {
        if (executedInstruction != "")
        {
            executedInstruction += " ";
        }
        executedInstruction += String(instruction);
    }
    String GetLatestExecutedInstruction()
    {
        String ins = String(executedInstruction);
        executedInstruction = "";
        return ins;
    }

    STATUS statusCode = STATUS::IDLE;
    bool stateChanged = true;
    String executedInstruction = "";

    PCConnector *connector;
    Program currentProgram;
    Setting setting;
    MotorController *motorController;
};
