#include "../models/motor_settings.cpp"
#include "serial_connector.cpp"

class RequestHandler
{
public:
    RequestHandler(SomeState state)
    {
        state_ = state;
    }
    void onProgramUpdate(Program updatedProgram)
    {
        arduino->setupProgram(
            programArguments.instructions,
            programArguments.id);
        arduino->updateSettings(
            {0,
             programArguments.useDoubleInstruction,
             false},
            programArguments.motorSettings);
        connector->sendResponse();
    }

    void onPause(bool shouldPause)
    {
    }

private:
    SomeState service_;
};

class CommunicationService
{
public:
    CommunicationService(int baudrate, RequestHandler *requestHandler)
    {
        connector = new SerialConnector(baudrate);
        handler = requestHandler;
    }

    void receiveAndHandleCommand()
    {
        bool *hasReceived;
        Command command = connector->receiveCommand(hasReceived);
        if (!hasReceived)
        {
            return;
        }
        if (command.command == "pause")
        {
            handler->onPause(extractPauseArguments(command.arguments));
        }
        else if (command.command == "program")
        {
            handler->onProgramUpdate(extractProgramArguments(command.arguments));
        }
    }

    void sendProgramProgressUpdate(
        String programId,
        int latestInstructionFinishedId,
        int runtime)
    {
        connector->sendData(
            createProgramProgressUpdate(
                programId,
                latestInstructionFinishedId,
                runtime));
    }

    void sendProgramFinishedReport(
        String programId,
        int runtime)
    {
        connector->sendData(createFinishedReport(programId, runtime));
    }

    void sendStatusUpdate(String status)
    {
        connector->sendData(convertDataToString("ty", "st") + convertDataToString("st", status));
    }

    void sendDebugPrint(String message)
    {
        connector->printDebugMessage(message);
    }

private:
    SerialConnector *connector;
    RequestHandler *handler;

    String convertDataToString(String name, String value)
    {
        return name + "=" + value + ";";
    }

    bool extractPauseArguments(String arguments)
    {
        return arguments == "true" ? true : false;
    }

    struct ProgramArguments
    {
        String instructions;
        String id;
        bool useDoubleInstruction;
        MotorSettings motorSettings;
    };

    ProgramArguments extractProgramArguments(String arguments)
    {
        String seperateArguments[8];
        connector->seperateArguments(arguments, 8, seperateArguments);
        return {
            seperateArguments[0],
            seperateArguments[1],
            seperateArguments[2] == "1" ? true : false,
            {seperateArguments[3].toInt(),
             seperateArguments[4].toInt(),
             seperateArguments[5].toInt(),
             seperateArguments[6].toInt(),
             seperateArguments[7].toInt()}};
    }

    String createFinishedReport(String programId, int runtime)
    {
        return convertDataToString("ty", "fi") + convertDataToString("id", programId) + convertDataToString("rt", String(runtime));
    }

    String createProgramProgressUpdate(String programId,
                                       int latestInstructionFinishedId,
                                       int runtime)
    {
        return convertDataToString("ty", "up") + convertDataToString("id", programId) + convertDataToString("rt", String(runtime) + convertDataToString("in", String(latestInstructionFinishedId)));
    }

    String createDataString(String programInstructions,
                            String programId,
                            String latestInstructionId,
                            String status,
                            String runtime)
    {
        return convertDataToString("in", programInstructions) + convertDataToString("id", programId) + convertDataToString("ci", latestInstructionId) + convertDataToString("li", status) + convertDataToString("st", runtime);
    }
};