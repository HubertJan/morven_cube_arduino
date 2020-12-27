#include "magic.cpp"

Magic *magic;

void setup()
{
    magic = new Magic(96);
};

void loop()
{
    magic->Update();
};