#include"PrecompiledHeader.h"
#include"LogGenerator.h"

// ミューテックスを使用してスレッドセーフにする
std::mutex logMutex;

// ログを出力する関数
void WriteLog(const std::string& message) {
	message;
//    // 現在の時間を取得
//    auto now = std::chrono::system_clock::now();
//    auto time_t_now = std::chrono::system_clock::to_time_t(now);
//    std::tm localTime{};
//#ifdef _WIN32
//    localtime_s(&localTime, &time_t_now); // Windows環境
//#else
//    localtime_r(&time_t_now, &localTime); // Linux/Unix環境
//#endif
//
//    // ミューテックスで排他制御
//    std::lock_guard<std::mutex> lock(logMutex);
//
//    // ファイルに追記モードでログを出力
//    std::ofstream logFile(LOG_FILE_NAME, std::ios::app);
//    if (logFile.is_open()) {
//        logFile << "["
//            << (1900 + localTime.tm_year) << "-"
//            << (localTime.tm_mon + 1) << "-"
//            << localTime.tm_mday << " "
//            << localTime.tm_hour << ":"
//            << localTime.tm_min << ":"
//            << localTime.tm_sec
//            << "] " << message << std::endl;
//    }
//    else {
//        std::cerr << "ログファイルを開けませんでした。" << std::endl;
//    }
}
