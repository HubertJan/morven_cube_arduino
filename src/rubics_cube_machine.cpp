class RubicsCubeMaschine
{
public:
    RubicsCubeMaschine()
    {
        communicationService_ = CommunicationService();
    }
    void update()
    {
        communicationService_->receiveHandleCommand();
        motorService_->runMotors();
        
    }

private:
    CommunicationService &communicationService_;
    MotorService motorService_;
};
