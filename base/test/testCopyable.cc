
#include "base/noncopyable.h"
#include "base/copyable.h"

#include <iostream>

class Foo : private noncopyable
{
};

int main(int argc, char const *argv[])
{
    
    std::cout << sizeof(copyable) << std::endl;
    std::cout << sizeof(noncopyable) << std::endl;
    std::cout << sizeof(Foo) << std::endl;

    Foo foo;
    Foo foo1;

    // foo = foo1;


    return 0;
}
