#include "serial_connector.cpp"
#include "models/motor_settings.cpp"
#include "arduino_client_interface.cpp"

class ServerSerialConnector
{
public:
    ServerSerialConnector(int baudrate, IArduinoClient *arduinoClient)
    {
        connector = new SerialConnector(baudrate);
        arduino = arduinoClient;
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
            handlePause(command.arguments);
        }
        else if (command.command == "program")
        {
            handleProgram(command.arguments);
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

    void sendDebugPrint(String message)
    {
        connector->printDebugMessage(message);
    }

private:
    SerialConnector *connector;
    IArduinoClient *arduino;

    String convertDataToString(String name, String value)
    {
        return name + "=" + value + ";";
    }

    void handlePause(String arguments)
    {
        if (arguments == "true")
        {
            arduino->changeIsPausing(true);
        }
        else if (arguments == "false")
        {
            arduino->changeIsPausing(false);
        }
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

    void handleProgram(String arguments)
    {
        ProgramArguments programArguments = extractProgramArguments(arguments);
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
};