#pragma once
#include <iostream>
#include <string>
#include <filesystem>

namespace ody
{
    class Logger 
    {
    public:
        enum class LogLevel {
            Info,
            Warning,
            Error
        };

        template <LogLevel level>
        static void Log(const std::string& message) 
        {
            std::string level_str;
            switch (level) {
            case LogLevel::Info:
                level_str = "Info";
                break;
            case LogLevel::Warning:
                level_str = "Warning";
                break;
            case LogLevel::Error:
                level_str = "Error";
                break;
            }
            std::cerr << "\033[1;" << GetColorCode(level) << "m" << "[" << level_str << "] " << "File: " << GetCurrentFile() << " | Line: " << GetCurrentLine() << " | Message: " << message << "\033[0m" << std::endl;
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
            }
            return 0;
        }

        static std::string GetCurrentFile() 
        {
            auto path = std::filesystem::path(__FILE__);
            return path.filename().string();
        }

        static int GetCurrentLine() 
        {
            return __LINE__;
        }
    };
}