#pragma once



#include <string>
#include <fstream>

#include <boost/scoped_array.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/sources/record_ostream.hpp>


typedef enum log_level
{
	TRACE,
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATAL
}log_level;

extern "C"
{
	void log_format(log_level lv,const char* format, ...);
}

#ifndef NDEBUG
#define LOG_TRACE(format, ...) log_format(TRACE,format, ##__VA_ARGS__);
#define LOG_DEBUG(format, ...) log_format(DEBUG,format, ##__VA_ARGS__);
#else
#define LOG_DEBUG(format, ...)
#define LOG_TRACE(format, ...)
#endif
#define LOG_INFO(format, ...) log_format(INFO,format, ##__VA_ARGS__);
#define LOG_WARNING(format, ...) log_format(WARNING,format, ##__VA_ARGS__);
#define LOG_ERROR(format, ...) log_format(ERROR,format, ##__VA_ARGS__);
#define LOG_FATAL(format, ...) log_format(FATAL,format, ##__VA_ARGS__);

