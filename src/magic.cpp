#include "server_serial_connector.cpp"
#include "motor_controller.cpp"
#include "models/status.cpp"
#include "models/program.cpp"
#include "models/settings.cpp"

class Magic : IArduinoClient
{
public:
    Magic() = delete;
    Magic(int baudRate)
    {
        connector = new ServerSerialConnector(baudRate, this);
        motorController = new MotorController();
    }
    void Update()
    {
        connector->receiveAndHandleCommand();
        motorController->runMotors();
        if (statusCode == STATUS::PREPARE)
        {
            prepareProgram();
        }
        else if (statusCode == STATUS::RUN)
        {
            runProgram();
        }
        motorController->runMotors();

        reportChanges();
    }
    void changeIsPausing(bool shouldPause)
    {
        if (statusCode != STATUS::RUN && statusCode != STATUS::PAUSE)
        {
            return;
        }
        if (statusCode == STATUS::PAUSE && shouldPause == true)
        {
            return;
        }
        if (statusCode == STATUS::RUN && shouldPause == false)
        {
            return;
        }
        statusCode = shouldPause ? STATUS::PAUSE : STATUS::RUN;
        stateHasChanged = true;
    }

    void setupProgram(String instructionString, String id)
    {
        if (statusCode == STATUS::IDLE)
        {
            currentProgram.id = id;
            currentProgram.instructions = instructionString;
            currentProgram.lastInstructionId = 0;
            currentProgram.runningTime = 0;
            currentProgram.timeOfProgramStart = 0;
            currentProgram.totalInstructions = (currentProgram.instructions.length() + 1) / 3;
            if (statusCode != STATUS::PREPARE)
            {
                statusCode = STATUS::RUN;
            }
            stateHasChanged = true;
        }
    }

    void updateSettings(MagicSettings magicSettings,
                        MotorSettings motorSettings)
    {
        if (statusCode == STATUS::IDLE)
        {
            settings = magicSettings;
            motorController->motorSettings = motorSettings;
        }
    }

private:
    String prepareInstructions = "";
    bool hasMoveInit = false;

    void prepareProgram()
    {
        char lastInstruction[3] = {prepareInstructions[0], prepareInstructions[1], prepareInstructions[2]};
        motorController->ExecuteCubeInstruction(lastInstruction);
        AddLatestExecutedInstruction(lastInstruction);
        prepareInstructions.remove(0, 2);
        if (prepareInstructions == "")
        {
            statusCode = STATUS::RUN;
        }
    }

    void runProgram()
    {
        char nextInstruction[3] = {};
        char possibleSecondInstruction[3] = {};
        int finishedIns = 0;
        bool canBeExecutedAsDouble = false;
        int nextInstructionId = currentProgram.lastInstructionId + 1;
        GetInstruction(currentProgram.instructions, nextInstructionId, nextInstruction);
        if (settings.useDoubleInstruction == true && currentProgram.lastInstructionId < currentProgram.totalInstructions)
        {
            GetInstruction(currentProgram.instructions, nextInstructionId + 1, possibleSecondInstruction);
            canBeExecutedAsDouble = motorController->CheckDoubleInstruction(nextInstruction, possibleSecondInstruction);
        }
        if (canBeExecutedAsDouble)
        {
            if (hasMoveInit == false)
            {
                connector->sendDebugPrint(String(nextInstruction));
                connector->sendDebugPrint(String(possibleSecondInstruction));
                motorController->ExecuteCubeInstruction(nextInstruction);
                motorController->ExecuteCubeInstruction(possibleSecondInstruction);
                hasMoveInit = true;
            }
            else
            {
                if (motorController->IsCubeInstructionDone(nextInstruction) && motorController->IsCubeInstructionDone(possibleSecondInstruction))
                {
                    finishedIns = 2;
                    AddLatestExecutedInstruction(nextInstruction);
                    AddLatestExecutedInstruction(possibleSecondInstruction);
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
            // TODO: Rework
            if (hasMoveInit == false)
            {
                connector->sendDebugPrint("Move");
                connector->sendDebugPrint(String(nextInstruction));
                motorController->ExecuteCubeInstruction(nextInstruction);
                hasMoveInit = true;
            }
            else
            {
                if (motorController->IsCubeInstructionDone(nextInstruction))
                {
                    connector->sendDebugPrint("done!");
                    finishedIns = 1;
                    AddLatestExecutedInstruction(nextInstruction);
                    hasMoveInit = false;
                }
                else
                {
                    connector->sendDebugPrint("Still Moving");
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

        if (currentProgram.totalInstructions <= nextInstructionId + finishedIns)
        {
            currentProgram.runningTime = millis() - currentProgram.timeOfProgramStart;
            statusCode = STATUS::IDLE;

            digitalWrite(28, HIGH);
            digitalWrite(12, HIGH);
            delay(800);
            connector->sendDebugPrint("Send Finished Report");
            connector->sendProgramFinishedReport(currentProgram.id, currentProgram.runningTime);
        }
        else
        {
            currentProgram.lastInstructionId += finishedIns;
            stateHasChanged = true;
        }
    }

    void reportChanges()
    {
        if (stateHasChanged)
        {
            if (statusCode != STATUS::RUN) // war mal da: setting.isLogging == false ||
            {
                connector->sendDebugPrint("Send Full Status");
                connector->sendProgramProgressUpdate(currentProgram.id, currentProgram.lastInstructionId, currentProgram.runningTime);
                connector->sendDebugPrint("Sucessfull");
            }
            connector->sendStatusUpdate(StatusCodeAsString());
            stateHasChanged = false;
        }
    }

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
    bool stateHasChanged = true;
    String executedInstruction = "";

    ServerSerialConnector *connector;
    Program currentProgram;
    MagicSettings settings;
    MotorController *motorController;
};
