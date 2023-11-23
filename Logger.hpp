#pragma once
#include <string>
#include <string_view>
#include <ostream>
#include <source_location>
#include <fstream>
#include <type_traits>
#include <mutex>
#include <memory>

namespace util {

	class ILogger {
	public:
		enum class LogLevel {
			error,
			warning,
			info,
			debug
		};

		struct Options {
			Options(bool logTime, bool logLocation, bool logFunctionName, const std::string& timeFmt = "");
			bool logTime = false;
			bool logLocation = false;
			bool logFunctionName = false;
			// time fmt example: "{:%d.%m.%Y %H:%M:%S}", also there is no validation here
			std::string timeFmt;
		};

		virtual void log(LogLevel curLvl, std::string_view sv, std::source_location ssrc = std::source_location::current()) = 0;
		virtual void error(std::string_view sv, std::source_location ssrc = std::source_location::current());
		virtual void warning(std::string_view sv, std::source_location ssrc = std::source_location::current());
		virtual void info(std::string_view sv, std::source_location ssrc = std::source_location::current());
		virtual void debug(std::string_view sv, std::source_location ssrc = std::source_location::current());
		virtual void setOptions(Options&& opts) = 0;
		virtual void setLogLevel(LogLevel l) = 0;
	};

	class Logger : public ILogger {
	public:
		Logger(LogLevel lvl, Options&& opts);
		virtual ~Logger();
		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;
		void setOptions(Options&& opts) override;
		void setLogLevel(LogLevel l) override;
	protected:
		void _logImpl(std::ostream& os, LogLevel curLvl, std::string_view sv, std::source_location ssrc = std::source_location::current());
		LogLevel lvl;
		Options opts;
	};

	class StreamLogger : public Logger {
	public:
		StreamLogger(LogLevel curLvl, Options&& opts, std::ostream& os);
		virtual ~StreamLogger();
		virtual void log(LogLevel curLvl, std::string_view sv, std::source_location ssrc = std::source_location::current()) override;
	private:
		std::ostream& os;
	};

	class FileLogger : public Logger {
	public:
		FileLogger(LogLevel curLvl, Options&& opts, std::string_view fname, std::ios_base::openmode mode = std::ios_base::app);
		virtual ~FileLogger();
		virtual void log(LogLevel curLvl, std::string_view sv, std::source_location ssrc = std::source_location::current()) override;
	private:
		std::ofstream ofs;
	};

	class ThreadsafeLogger : public ILogger {
	public:
		ThreadsafeLogger(std::unique_ptr<Logger>&& logger);
		void log(Logger::LogLevel curLvl, std::string_view sv, std::source_location ssrc = std::source_location::current()) override;
		void setOptions(Options&& opts) override;
		void setLogLevel(LogLevel l) override;
	private:
		std::unique_ptr<Logger> pLogger;
		std::mutex mtx;
	};

	template<typename LoggerT, typename ArgsT>
	class SingletonLogger {
	public:
		template<typename ... Args>
		static void init(Args&&... _args) {
			ArgsT targs = std::forward_as_tuple(std::forward<decltype(_args)>(_args)...);
			_getImpl(&targs);
		}
		static LoggerT& get() {
			return _getImpl();
		}
		SingletonLogger(const SingletonLogger&) = delete;
		SingletonLogger& operator=(const SingletonLogger&) = delete;
	private:
		SingletonLogger(ArgsT* _args)
		{
			logger = std::unique_ptr<LoggerT>(std::apply([]<typename...Args>(Args&&... args) { return new LoggerT(std::forward<decltype(args)>(args)...); }, std::forward<ArgsT>(*_args)));
			//logger = std::make_unique<LoggerT>(std::make_from_tuple<LoggerT>(*_args));
			//args = std::make_tuple<Args...>(std::forward<Args>(_args)...);
		}
		static LoggerT& _getImpl(ArgsT* _args = nullptr) {
			static SingletonLogger logger(_args);
			return *logger.logger;
		}
		std::unique_ptr<LoggerT> logger = nullptr;
		//ArgsT args;
	};

	using StreamSingletonLogger = SingletonLogger<StreamLogger, std::tuple<Logger::LogLevel, Logger::Options&&, std::ostream&>>;
	using FileSingletonLogger = SingletonLogger<FileLogger, std::tuple<Logger::LogLevel, Logger::Options&&, std::string_view, std::ios_base::openmode>>;

	template<typename LoggerT>
	using TsSingletonLogger = SingletonLogger<ThreadsafeLogger, std::tuple<std::unique_ptr<LoggerT>&&>>;

}

