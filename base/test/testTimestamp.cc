#include <iostream>

#include "base/Timestamp.h"

int main(int argc, char const *argv[])
{
    
    Timestamp now(Timestamp::now());
    std::cout << now.toString() << std::endl;
    return 0;
}
