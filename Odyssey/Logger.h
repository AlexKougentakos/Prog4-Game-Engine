#pragma once
#include <iostream>
#include <string>
#include <filesystem>

#define LOG_INFO(message) ody::Logger::Log<ody::Logger::LogLevel::Info>(message, __FILE__, __LINE__)
#define LOG_WARNING(message) ody::Logger::Log<ody::Logger::LogLevel::Warning>(message, __FILE__, __LINE__)
#define LOG_ERROR(message) ody::Logger::Log<ody::Logger::LogLevel::Error>(message, __FILE__, __LINE__)
#define LOG_SOUND(message) ody::Logger::Log<ody::Logger::LogLevel::Sound>(message, __FILE__, __LINE__)

namespace ody
{
    class Logger 
    {
    public:
        enum class LogLevel 
        {
            Info,
            Warning,
            Error,
            Sound
        };

        template <LogLevel level>
        static void Log(const std::string& message, const char* file, int line)
        {
#ifdef _DEBUG //Logging is debug only
            std::string level_str;
            switch (level) 
            {
            case LogLevel::Info:
                level_str = "Info";
                break;
            case LogLevel::Warning:
                level_str = "Warning";
                break;
            case LogLevel::Error:
                level_str = "Error";
                break;
            case LogLevel::Sound:
                level_str = "Sound";
                break;
            }
            std::cerr << "\033[1;" << GetColorCode(level) << "m" << "[" << level_str << "] " << "File: " << std::filesystem::path(file).filename().string() << " | Line: " << line << " | Message: " << message << "\033[0m" << std::endl;
#endif
        }

    private:
        static int GetColorCode(LogLevel level) 
        {
            switch (level) {
            case LogLevel::Info:
                return 37; // white
            case LogLevel::Warning:
                return 33; // yellow
            case LogLevel::Error:
                return 31; // red
            case LogLevel::Sound:
                return 36; //light blue
            }
            return 0;
        }
    };
}