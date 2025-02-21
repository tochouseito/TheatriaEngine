#include "pch.h"
#include "Cho/Core/Log/Log.h"

// ログファイルの名前を定義
const std::string LOG_FILE_NAME = "ChoEngineLog.txt";
// ミューテックスを使用してスレッドセーフにする
std::mutex logMutex;

void Log(const std::string& message) {
	OutputDebugStringA(message.c_str());
}

std::wstring ConvertString(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }

    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
    if (sizeNeeded == 0) {
        return std::wstring();
    }
    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
    return result;
}

std::string ConvertString(const std::wstring& str) {
    if (str.empty()) {
        return std::string();
    }

    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
    if (sizeNeeded == 0) {
        return std::string();
    }
    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
    return result;
}

// ログを出力する関数
void WriteLog(const std::string& message) {
    // 現在の時間を取得
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &time_t_now); // Windows環境
#else
    localtime_r(&time_t_now, &localTime); // Linux/Unix環境
#endif

    // ミューテックスで排他制御
    std::lock_guard<std::mutex> lock(logMutex);

    // ファイルに追記モードでログを出力
    std::ofstream logFile(LOG_FILE_NAME, std::ios::app);
    if (logFile.is_open()) {
        logFile << "["
            << (1900 + localTime.tm_year) << "-"
            << (localTime.tm_mon + 1) << "-"
            << localTime.tm_mday << " "
            << localTime.tm_hour << ":"
            << localTime.tm_min << ":"
            << localTime.tm_sec
            << "] " << message << std::endl;
    } else {
        std::cerr << "ログファイルを開けませんでした。" << std::endl;
    }
}

void ChoLog(const std::string& message) {
#ifdef _DEBUG
    Log(message);
#endif // _DEBUG
#ifdef NDEBUG
    WriteLog(message);
#endif // NDEBUG
}

void ChoAssertLog(const std::string& message, const HRESULT& hr, const std::string& file, const int line)
{
	if (SUCCEEDED(hr)) {// hrが成功している場合は何もしない
		return;
	}
	std::string logMessage = std::format("Error: {} in {} at line {}\n", hr, file, line);
	ChoLog(logMessage);
	ChoLog(message);
	assert(SUCCEEDED(hr));
}
