#include "Logger.hpp"
#include <iostream>
#include <format>
#include <source_location>
#include <cassert>
#include <chrono>

using namespace util;

ILogger::~ILogger() {
	;
}

void ILogger::error(std::string_view sv, std::source_location ssrc) {
	log(LogLevel::error, sv, ssrc);
}

void ILogger::warning(std::string_view sv, std::source_location ssrc) {
	log(LogLevel::warning, sv, ssrc);
}

void ILogger::info(std::string_view sv, std::source_location ssrc) {
	log(LogLevel::info, sv, ssrc);
}

void ILogger::debug(std::string_view sv, std::source_location ssrc) {
	log(LogLevel::debug, sv, ssrc);
}

Logger::Options::Options(bool logTime, bool logThreadId, bool logThreadName, bool logLocation, bool logFunctionName, const std::string& _timeFmt)
	: logTime{ logTime }, logThreadId{ logThreadId }, logThreadName{ logThreadName }, logLocation{ logLocation }, logFunctionName{ logFunctionName }, timeFmt { _timeFmt }
{
	if (logTime && timeFmt.empty()) {
		timeFmt = "{:%d.%m.%Y %H:%M:%S}";
	}
}

Logger::Logger(LogLevel lvl, Options&& opts)
	: lvl{ lvl }, opts{ std::move(opts) }
{
	;
}

Logger::~Logger() {

}

void Logger::setOptions(Options&& _opts) {
	opts = std::move(_opts);
}

void Logger::setLogLevel(LogLevel l) {
	lvl = l;
}

void Logger::_logImpl(std::ostream& os, LogLevel curLvl, std::string_view sv, std::source_location ssrc) {
	if (curLvl > lvl) {
		return;
	}
	if (opts.logTime) {
		std::string fmt = std::format("[{}]", opts.timeFmt);
		os << std::vformat(fmt, std::make_format_args(std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::utc_clock::now())));
	}
	if (opts.logThreadId) {
		os << "[" << std::this_thread::get_id() << "]";
	}
	if (opts.logThreadName) {
#ifdef _WIN32 
		os << "[WINDOWS_THREAD_NAMES_NOT_SUPPORTED]";
#else
		// assuming linux
		static char ThreadNameBuf[16];
		if (pthread_getname_np(pthread_self(), ThreadNameBuf, 16) == 0) {
			os << std::format("[{}]", ThreadNameBuf);
		}
		else {
			os << "[LINUX_COULDNT_GET_THREAD_NAME]";
		}
#endif
	}
	if (opts.logLocation) {
		os << std::format("[{}:{}]", ssrc.file_name(), ssrc.line());
	}
	if (opts.logFunctionName) {
		os << std::format("[{}]", ssrc.function_name());
	}
	os << ": " << sv << std::endl;
	os.flush();
}

StreamLogger::StreamLogger(LogLevel curLvl, Options&& opts, std::ostream& os)
	: Logger{ curLvl, std::move(opts) }, os{ os }
{

}

StreamLogger::~StreamLogger() {

}

void StreamLogger::log(LogLevel curLvl, std::string_view sv, std::source_location ssrc) {
	_logImpl(os, curLvl, sv, ssrc);
}

FileLogger::FileLogger(LogLevel curLvl, Options&& opts, std::string_view fname, std::ios_base::openmode mode)
	: Logger{ curLvl, std::move(opts) }
{
	assert(mode & (std::ios_base::out | std::ios_base::app));
	ofs = std::ofstream(std::string(fname.data(), fname.size()), mode);
	if (!ofs) {
		throw std::runtime_error("Couldn't open log file");
	}
}

void FileLogger::log(LogLevel curLvl, std::string_view sv, std::source_location ssrc) {
	_logImpl(ofs, curLvl, sv, ssrc);
}

FileLogger::~FileLogger() {

}

ThreadsafeLogger::ThreadsafeLogger(std::unique_ptr<Logger>&& logger)
	: pLogger{ std::move(logger) }
{
	assert(pLogger);
}

void ThreadsafeLogger::log(Logger::LogLevel curLvl, std::string_view sv, std::source_location ssrc) {
	std::lock_guard lck{ mtx };
	pLogger->log(curLvl, sv, ssrc);
}

void ThreadsafeLogger::setOptions(Options&& opts) {
	std::lock_guard lck{ mtx };
	pLogger->setOptions(std::move(opts));
}

void ThreadsafeLogger::setLogLevel(LogLevel l) {
	std::lock_guard lck{ mtx };
	pLogger->setLogLevel(l);
}