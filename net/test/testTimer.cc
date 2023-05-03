#include <iostream>

#include "net/Timer.h"

void runt()
{
    std::cout << "runt()" << std::endl;
}

int main(int argc, char const *argv[])
{
    Timer t(std::bind(runt), Timestamp(), 0.0);
    return 0;
}
