#include "magic.cpp"

Magic *magic;

void setup()
{
    magic = new Magic(96);        
    Serial.begin(9600);
};

void loop()
{
    magic->Update();
    delay(3000);
};