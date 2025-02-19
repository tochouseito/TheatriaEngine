#pragma once

#include <iostream>
#include<chrono>
#include<thread>

class FrameRate
{
public:// メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void StartFrame();

	void EndFrame();

	//float GetFPS() const;

private: // メンバ変数

	/*記録時間FPS固定用*/
	std::chrono::steady_clock::time_point reference_;

	std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
	//float frameTime_ = 0.0f;  // 1フレームの経過時間（秒）
	//float fps_ = 0.0f;        // フレームレート
};