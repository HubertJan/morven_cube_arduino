#include <Arduino.h>

class ArduinoClient
{
public:
    virtual void SwitchPause(bool value) = 0;
    virtual void SetProgram(String instructionString, String id, bool fastMode) = 0;
    virtual void SetSetting(int val, bool val2) = 0;
};

class PCConnector
{
public:
    PCConnector(int b, ArduinoClient *ard)
    {
        arduino = ard;
        baud = b;
        Serial.begin(9600);
        while (!Serial.available())
        {
        }
        Serial.println("Started");
    }
    void Update()
    {
        String commandLine = ReceiveCommandLine();
        if (commandLine != "")
        {
            String command[2];
            SeparateCommandLine(commandLine, command);
            HandleCommand(command);
        }
    }

    void SendFullStatus(String prgIns, String prgId, String insId, String exeIns, String sta, String rt)
    {
        Serial.println("data;" + DataToString("in", prgIns) //programInstructions
                       + DataToString("id", prgId)          //programId
                       + DataToString("ci", insId)          //currentInstructionId
                       + DataToString("li", exeIns)         //latestInstructions
                       + DataToString("st", sta)            //status
                       + DataToString("rt", rt)             //runTime
        );
    }

    void DebugPrint(String msg)
    {
        Serial.println("debug;" + msg);
        delay(100);
    }

private:
    int baud = 0;
    ArduinoClient *arduino;

    String DataToString(String name, String value)
    {
        return name + "=" + value + ";";
    }

    void SetPause(String setToPause)
    {
        if (setToPause == "true")
        {
            arduino->SwitchPause(true);
        }
        else if (setToPause == "false")
        {
            arduino->SwitchPause(false);
        }
    }

    void SendProgramAsResponse(String prgIns, String prgId)
    {
        String responseString = String("response;");
        responseString.concat(DataToString("programInstructions", prgIns)).concat(DataToString("programId", prgId));
        Serial.println(responseString);
        delay(100);
    }

    void SetProgram(String argsString)
    {
        int argNumber = CountArguments(argsString);
        DebugPrint(String("argNumber " + String(argNumber)));
        String args[argNumber];
        ArgsToArray(argsString, argNumber, args);
        String instructions = args[0];
        String id = args[1];
        if (argNumber == 3)
        {
            DebugPrint(String("FAST MODE"));
            bool fastMode = false;
            if (args[2] == "1")
            {
                fastMode = true;
                DebugPrint(String("FAST MODE ACTIVATED"));
            }
            arduino->SetProgram(instructions, id, fastMode);
            SendProgramAsResponse(instructions, id);
        }
        else
        {
            arduino->SetProgram(instructions, id, false);
            SendProgramAsResponse(instructions, id);
        }
    }

    void ChangeSettings(String argsString)
    {
        String args[2];
        ArgsToArray(argsString, 2, args);
        int speed = args[0].toInt();
        bool doubleInstruction = true;
        if (args[1] == "false")
        {
            doubleInstruction = false;
        }
        arduino->SetSetting(speed, doubleInstruction);
    }

    int CountArguments(String inp)
    {
        int symbols = CountCharacter(inp, '"');
        int counter = symbols / 2;

        return counter;
    }

    int CountCharacter(String inp, char character)
    {
        int counter = 0;
        int charIndex = 0;
        bool finished = false;
        DebugPrint(": " + String(inp) + " ;");
        while (finished == false)
        {
            charIndex = inp.indexOf(character, charIndex);
            DebugPrint(": " + String(charIndex) + " ;");
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

    void ArgsToArray(String args, int numberOfArgs, String *resultArray)
    {
        int arrayIndex = 0;
        int stringIndex = 0;
        int firstQuoteMark = -1;
        int secondQuoteMark = -1;
        while (arrayIndex < numberOfArgs)
        {
            if (firstQuoteMark == -1)
            {
                firstQuoteMark = args.indexOf("\"", stringIndex);
                stringIndex = firstQuoteMark + 1;
            }
            else
            {
                secondQuoteMark = args.indexOf("\"", stringIndex);
                stringIndex = secondQuoteMark + 1;
                resultArray[arrayIndex] = args.substring(firstQuoteMark + 1, secondQuoteMark);
                firstQuoteMark = -1;
                secondQuoteMark = -1;
                arrayIndex += 1;
            }
        }
    }

    typedef void (PCConnector::*CommandHandler)(const String);
    const String cdKeywords[3] = {
        "pause",
        "program",
        "option"};
    const CommandHandler cdFunctions[3] = {
        &PCConnector::SetPause,
        &PCConnector::SetProgram,
        &PCConnector::ChangeSettings,
    };

    String receivedString = "";
    bool stringComplete = false;

    void SeparateCommandLine(String cdLine, String *resultCd)
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
        resultCd[0] = commandWord;
        resultCd[1] = argumentLine;
    }

    String ReceiveCommandLine()
    {

        while (Serial.available())
        {
            char inChar = (char)Serial.read();
            if (inChar == '\n')
            {
                DebugPrint(receivedString);
                String commandLine = receivedString;
                receivedString = "";
                return commandLine; //commandLine;
            }
            else
            {
                receivedString += inChar;
            }
        }
        return "";
    }

    void HandleCommand(const String command[2])
    {
        for (int i = 0; i < 2; i++)
        {
            if (cdKeywords[i] == command[0])
            {
                (*this.*cdFunctions[i])(command[1]);
            }
        }
    }
};