#include <Arduino.h>

struct Command
{
    String command;
    String arguments;
};


class SerialConnector
{
public:
    SerialConnector(int baudrate)
    {
        Serial.begin(baudrate);
        while (!Serial.available())
        {
        }
        Serial.println("Started");
    }

    Command receiveCommand(bool *hasReceived)
    {
        String commandLine = receiveCommandLine();
        if (commandLine != "")
        {
            String command[2];
            *hasReceived = true;
            return extractCommand(commandLine);
        }
        *hasReceived = false;
        return;
    }

    void seperateArguments(String arguments, int argumentsCount, String *outArguments)
    {
        int arrayIndex = 0;
        int stringIndex = 0;
        int firstQuoteMark = -1;
        int secondQuoteMark = -1;
        while (arrayIndex < argumentsCount)
        {
            if (firstQuoteMark == -1)
            {
                firstQuoteMark = arguments.indexOf("\"", stringIndex);
                stringIndex = firstQuoteMark + 1;
            }
            else
            {
                secondQuoteMark = arguments.indexOf("\"", stringIndex);
                stringIndex = secondQuoteMark + 1;
                outArguments[arrayIndex] = arguments.substring(firstQuoteMark + 1, secondQuoteMark);
                firstQuoteMark = -1;
                secondQuoteMark = -1;
                arrayIndex += 1;
            }
        }
    }

    int countArguments(String arguments)
    {
        int symbols = countCharacter(arguments, '"');
        int counter = symbols / 2;

        return counter;
    }

    void printDebugMessage(String message)
    {
        Serial.println("debug;" + message);
        delay(100);
    }

    void sendResponse()
    {
        Serial.println("response;");
    }

    void sendData(String dataString){
        Serial.println("data;" + dataString);
    }

private:
    int countCharacter(String text, char characterToBeCounted)
    {
        int counter = 0;
        int charIndex = 0;
        bool finished = false;
        // DebugPrint(": " + String(text) + " ;");
        while (finished == false)
        {
            charIndex = text.indexOf(characterToBeCounted, charIndex);
            if (charIndex != -1)
            {
                counter += 1;
                charIndex += 1;
            }
            else
            {
                finished = true;
            }
        }

        return counter;
    }

    String receiveCommandLine()
    {
        String receivedString = "";
        while (Serial.available())
        {
            char inChar = (char)Serial.read();
            if (inChar == '\n')
            {
                // DebugPrint(receivedString);
                String commandLine = receivedString;
                receivedString = "";
                return commandLine;
            }
            else
            {
                receivedString += inChar;
            }
        }
        return "";
    }

    Command extractCommand(String cdLine)
    {
        int argumentPosition = cdLine.indexOf(" ");
        String commandWord;
        String argumentLine;
        if (argumentPosition == -1)
        {
            commandWord = cdLine;
        }
        else
        {
            commandWord = cdLine.substring(0, argumentPosition);
            argumentLine = cdLine.substring(argumentPosition + 1);
        }
        return {commandWord, argumentLine};
    }
};