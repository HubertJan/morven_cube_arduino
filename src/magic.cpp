#include "pc_connector.cpp"
#include "motor_controller.cpp"

class Magic : ArduinoClient
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
            int programLength = (currentProgram.instructions.length() + 1) / 3 - 1;

            char currentInstruction[3] = {};
            char nextInstruction[3] = {};
            int finishedIns = 0;
            bool executeAsDouble = false;
            GetInstruction(currentProgram.instructions, currentProgram.currentInstruction, currentInstruction);
            if (setting.doubleInstruction == true && currentProgram.currentInstruction + 1 <= programLength)
            {
                connector->DebugPrint("Check Instruction");
                GetInstruction(currentProgram.instructions, currentProgram.currentInstruction + 1, nextInstruction);
                connector->DebugPrint("Double");
                connector->DebugPrint(String(currentInstruction));
                connector->DebugPrint("B");
                connector->DebugPrint(String(nextInstruction));
                executeAsDouble = motorController->CheckDoubleInstruction(currentInstruction, nextInstruction);
            }
            if (executeAsDouble)
            {
                connector->DebugPrint("Double Instruction");
                if (hasMoveInit)
                {
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
                if (hasMoveInit)
                {
                    hasMoveInit = true;
                }
                else
                {
                    if (motorController->IsCubeInstructionDone(currentInstruction))
                    {
                        finishedIns = 1;
                        AddLatestExecutedInstruction(currentInstruction);
                        hasMoveInit = false;
                    }
                    else
                    {
                        finishedIns = 0;
                    }
                }
            }

            if (currentProgram.timeOfProgramStart == 0)
            {
                currentProgram.timeOfProgramStart = millis();
            }
            else
            {
                currentProgram.runningTime = millis() - currentProgram.timeOfProgramStart;
            }

            if (programLength <= currentProgram.currentInstruction)
            {
                statusCode = STATUS::FINISHED;
            }
            else
            {
                currentProgram.currentInstruction += finishedIns;
            }
            stateChanged = true;
        }
        if (stateChanged)
        {
            if (setting.noLogMode == false || statusCode == STATUS::FINISHED)
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

    void SetProgram(String instructionString, String id, bool fastMode)
    {
        setting.noLogMode = fastMode;
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

    /*    void SetPrepareAndProgram(String programInstructionString, String id, String prepareInsructionString)
    {
        statusCode = STATUS::PREPARE;
        prepareInstructions = prepareInsructionString;
        SetProgram(programInstructionString, id);
    }
 */
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

    struct Program
    {
        String id = "";
        int currentInstruction = 0;
        String instructions = "";
        unsigned long runningTime = 0;
        unsigned long timeOfProgramStart = 0;
    };
    struct Setting
    {
        int speed = 0;                 //Duration of one instruction in ms; O = fast as possible
        bool doubleInstruction = true; //Two instruction in one move (if possible)
        int acceleration = 0;
        bool noLogMode = false; //Turn of serial prints between actions
    };

    enum class STATUS
    {
        IDLE,
        PREPARE,
        RUN,
        PAUSE,
        FINISHED,
    };

    void GetInstruction(String instructions, int id, char *outInstruction)
    {
        outInstruction[0] = instructions[id * 3];
        outInstruction[1] = instructions[id * 3 + 1];
        connector->DebugPrint(outInstruction);
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
        connector->DebugPrint(executedInstruction);
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