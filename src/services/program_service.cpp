#include "../models/program.cpp"
class ProgramService
{
public:
    void continueProgram(){
        if(instructionState_ == 0){
            instructionState_ = &initInstruction();
            return;
        }
    }
private:
    InstructionState initInstruction(){

    }

    

Program *currentProgram_;
ProgramState *programState_;
InstructionState *instructionState_;
}