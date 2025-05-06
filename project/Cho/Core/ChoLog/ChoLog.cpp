#include "ChoLog.h"
#include <cassert>

std::string Cho::Log::LogFileName = "ChoEngineLog.txt";
std::mutex Cho::Log::logMutex;

std::wstring ConvertString(const std::string& str)
{
    if (str.empty())
    {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0)
    {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

std::string ConvertString(const std::wstring& str)
{
    if (str.empty())
    {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0)
    {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}

void Cho::Log::Write(LogLevel level,
    const std::string& message,
    std::optional<std::variant<bool, HRESULT>> flag,
    const std::source_location location)
{
    std::string suffix;

    if (flag.has_value())
    {
        if (std::holds_alternative<bool>(*flag))
        {
            bool b = std::get<bool>(*flag);
            suffix = b ? " : Succeeded" : " : Failed";

            if (level == LogLevel::Assert)
            {
                if (!b)
                {
                    std::string logMessage = std::format("[ASSERT] {}:{} -\n {}{}", location.file_name(), location.line(), message, suffix);
#ifdef _DEBUG
                    OutputDebugStringA((logMessage + "\n").c_str());
#endif
#ifdef NDEBUG
                    WriteToFile(logMessage);
#endif
                    CHO_ASSERT(false,logMessage.c_str()); // assert 発動
                    return;
                }
                // 成功時は単に出力（レベルはInfoにする）
                level = LogLevel::Info;
            }

        } else if (std::holds_alternative<HRESULT>(*flag))
        {
            HRESULT hr = std::get<HRESULT>(*flag);
            bool success = SUCCEEDED(hr);
            suffix = success ? " : Succeeded" : std::format(" : Failed (HRESULT: 0x{:08X})", static_cast<unsigned int>(hr));

            if (level == LogLevel::Assert)
            {
                if (FAILED(hr))
                {
                    std::string logMessage = std::format("[ASSERT] {}:{} -\n {}{}", location.file_name(), location.line(), message, suffix);
#ifdef _DEBUG
                    OutputDebugStringA((logMessage + "\n").c_str());
#endif
#ifdef NDEBUG
                    WriteToFile(logMessage);
#endif
                    CHO_ASSERT(false,logMessage.c_str()); // assert 発動
                    return;
                }
                // 成功時はInfoとして出力
                level = LogLevel::Info;
            }
        }
    } else
    {
        if (level == LogLevel::Assert)
        {
            std::string logMessage = std::format("[ASSERT] {}:{} -\n {}", location.file_name(), location.line(), message);
#ifdef _DEBUG
            OutputDebugStringA((logMessage + "\n").c_str());
#endif
#ifdef NDEBUG
            WriteToFile(logMessage);
#endif
			CHO_ASSERT(false,logMessage.c_str()); // assert 発動
            return;
        }
    }

    std::string prefix = std::format("[{}] {}:{} -\n ", ToString(level), location.file_name(), location.line());
    std::string fullMessage = prefix + message + suffix;

#ifdef _DEBUG
    OutputDebugStringA((fullMessage + "\n").c_str());
#endif
#ifdef NDEBUG
    WriteToFile(fullMessage);
#endif
}

void Cho::Log::WriteToFile(const std::string& message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile("ChoEngineLog.txt", std::ios::app);
    if (logFile.is_open())
    {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm localTime{};
        localtime_s(&localTime, &now);

        logFile << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] "
            << message << std::endl;
    }
}

std::string Cho::Log::ToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Info: return "INFO";
    case LogLevel::Warn: return "WARN";
    case LogLevel::Error: return "ERROR";
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Assert: return "ASSERT";
    default: return "UNKNOWN";
    }
}
