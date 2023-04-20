#include "base/Log.h"

int main(int argc, char const *argv[])
{
    LOG_TRACE("TEST");
    LOG_DEBUG("TEST");
    LOG_INFO("TEST");
    LOG_WARNING("TEST");
    LOG_ERROR("TEST");
    LOG_FATAL("TEST");
    int a = 10;
    LOG_DEBUG("a = %d", a);
    return 0;
}
