class IArduinoClient
{
public:
    virtual void changeIsPausing(bool value) = 0;
    virtual void setupProgram(String instructionString, String id) = 0;
    virtual void updateSettings(MagicSettings magicSettings,
                                MotorSettings motorSettings) = 0;
};