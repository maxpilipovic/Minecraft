#pragma once
#include "mc.h"

//MACROS
#define MC_CORE_INFO(...) Logger::GetCoreLogger()->info(__VA_ARGS__)
#define MC_CORE_ERROR(...) Logger::GetCoreLogger()->error(__VA_ARGS__)

class Logger
{
public:
	static void Init();
	static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return logger; }

private:
	static std::shared_ptr<spdlog::logger> logger;
};
