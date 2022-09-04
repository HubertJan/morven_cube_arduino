struct Setting
{
    int speed = 0;                 // Duration of one instruction in ms; O = fast as possible
    bool doubleInstruction = true; // Two instruction in one move (if possible)
    int acceleration = 0;
    bool noLogMode = true; // Turn of serial prints between actions
};