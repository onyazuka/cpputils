#include "testLogger.hpp"
#include <iostream>

using namespace util;
using namespace util::tests;
using namespace std;

void util::tests::testLogger() {
	StreamLogger slog(Logger::LogLevel::warning, Logger::Options(true, true, true), std::cout);
	slog.log(Logger::LogLevel::error, "error here");
	slog.log(Logger::LogLevel::info, "info here");
	slog.log(Logger::LogLevel::debug, "debug here");
	slog.log(Logger::LogLevel::warning, "warning here");

	slog.setLogLevel(Logger::LogLevel::debug);
	slog.setOptions(Logger::Options(true, false, false));
	slog.info("info here again");

	std::string ss = "err";
	FileLogger flog(Logger::LogLevel::warning, Logger::Options(true, true, true), "d:/1.txt");
	flog.log(Logger::LogLevel::error, ss);
	flog.log(Logger::LogLevel::info, "info here");
	flog.log(Logger::LogLevel::debug, "debug here");
	flog.log(Logger::LogLevel::warning, "warning here");

	Logger& log = slog;
	log.log(Logger::LogLevel::error, "error here");

	StreamSingletonLogger::init(Logger::LogLevel::warning, Logger::Options(true, true, true), std::cout);
	StreamSingletonLogger::get().log(Logger::LogLevel::error, "singleton error here");
	StreamSingletonLogger::get().log(Logger::LogLevel::warning, "singleton warning here");
	StreamSingletonLogger::get().log(Logger::LogLevel::info, "singleton info here");

	FileSingletonLogger::init(Logger::LogLevel::debug, Logger::Options(true, true, true), "d:/2.txt", std::ios_base::app);
	FileSingletonLogger::get().debug("hi");

	std::unique_ptr<StreamLogger> pslog = std::make_unique<StreamLogger>(Logger::LogLevel::warning, Logger::Options(true, true, true), std::cout);
	std::unique_ptr<FileLogger> pslog1 = std::make_unique<FileLogger>(Logger::LogLevel::warning, Logger::Options(true, true, true), "d:/3.txt", std::ios_base::app);

	TsSingletonLogger<StreamLogger>::init(std::move(pslog));
	TsSingletonLogger<StreamLogger>::get().warning("hi");

	TsSingletonLogger<FileLogger>::init(std::move(pslog1));
	TsSingletonLogger<FileLogger>::get().error("hi");
}