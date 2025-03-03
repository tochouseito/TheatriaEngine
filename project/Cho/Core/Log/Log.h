#pragma once
#include <string>
#include <format>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <mutex>
#include <Windows.h>

void Log(const std::string& message);
std::wstring ConvertString(const std::string& str);
std::string ConvertString(const std::wstring& str);
// ログを出力する関数
void WriteLog(const std::string& message);
void ChoLog(const std::string& message);
void ChoAssertLog(const std::string& message, const HRESULT& hr,const std::string& file, const int line);
void ChoAssertLog(const std::string& message, const bool& flag, const std::string& file, const int line);