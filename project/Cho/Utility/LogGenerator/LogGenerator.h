#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <mutex>

// ログファイルの名前を定義
const std::string LOG_FILE_NAME = "log.txt";

// ログを出力する関数
void WriteLog(const std::string& message);
