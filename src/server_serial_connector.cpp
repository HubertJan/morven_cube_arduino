#include "generic_serial_connector.cpp"
#include "models/motor_settings.cpp"
#include "arduino_client_interface.cpp"

class ServerSerialConnector
{

    ServerSerialConnector(int baudrate, IArduinoClient *arduinoClient)
    {
        *connector = SerialConnector(baudrate);
        arduino = arduinoClient
    }

    void receiveAndHandleCommand()
    {
        bool *hasReceived;
        Command command = connector->receiveCommand(hasReceived);
        if (!hasReceived)
        {
            return;
        }
        if (command == "pause")
        {
            handlePause(command.arguments);
        }
        else if (command == "program")
        {
            handle
        }
    }

private:
    SerialConnector *connector;
    IArduinoClient *arduino;

    void handlePause(String arguments)
    {
        if (arguments == "true")
        {
            arduino->SwitchPause(true);
        }
        else if (arguments == "false")
        {
            arduino->SwitchPause(false);
        }
    }

    struct ProgramArguments
    {
        String instructions;
        String id;
        bool isDoubleMoves;
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
             seperateArguments[7].toInt()}
        };
    }

    void handleProgram(String arguments)
    {
        ProgramArguments programArguments = extractProgramArguments(arguments);
        arduino->SetProgram(
            programArguments.instructions,
            programArguments.id,
            true,
            programArguments.motorSettings
        );
    }
};