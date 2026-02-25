#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Logger::logger;

void Logger::Init()
{
	auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console->set_pattern("[%T] [%^%l%$] %v");

	//CORE
	logger = std::make_shared<spdlog::logger>("MC_CORE", spdlog::sinks_init_list{ console });
	logger->set_level(spdlog::level::trace); //Everything is printed)
	logger->flush_on(spdlog::level::trace); //Send every message

	MC_CORE_INFO("Minecraft Core Logger has been Initialized!");

}
