
#include "base/Log.h"

bool isLogInited = false;
bool isLogReady = false;

bool initLogEnvironment(std::string cfg)
{
    if(!boost::filesystem::exists(cfg))
    {
        std::cout << "no cfg:" << cfg << std::endl;
		return false;
    }

    if(!boost::filesystem::exists("./log/"))
    {
        boost::filesystem::create_directory("./log/");
    }

    boost::log::add_common_attributes();

    // namespace logging = boost::log;
	using namespace boost::log::trivial;

    boost::log::register_simple_formatter_factory<severity_level, char>("SeverityFormat");
	boost::log::register_simple_filter_factory<severity_level, char>("SeverityFilter");

    std::ifstream file(cfg);
    try
    {
        boost::log::init_from_stream(file);
    }
    catch(const std::exception& exce)
    {
        std::cout << "init_logger is fail, exception : " << exce.what() << std::endl;
    }

    return true;
}


void log_format(log_level lv, const char* format, ...)
{
    if(!isLogInited)
    {
        isLogReady = initLogEnvironment("./log.ini");
        isLogInited = true;
    }
    if(!isLogReady)
        return ;
    
    va_list argList;
    va_start(argList, format);
    char formattedString[10240] = {0};
    vsnprintf(formattedString, 10240, format, argList);
    
    BOOST_LOG_TRIVIAL(severity_level(lv)) << formattedString;

	va_end(argList);

    if(lv == FATAL)
        abort();
}
