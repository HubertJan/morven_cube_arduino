class IArduinoClient
{
public:
    virtual void SwitchPause(bool value) = 0;
    virtual void SetProgram(String instructionString, String id, bool fastMode, MotorSetting motorSetting) = 0;
    virtual void SetSetting(int val, bool val2) = 0;
};