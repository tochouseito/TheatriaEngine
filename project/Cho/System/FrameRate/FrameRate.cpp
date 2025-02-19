#include "PrecompiledHeader.h"
#include "FrameRate.h"

void FrameRate::Initialize()
{
	// 現在時間を記録
	reference_ = std::chrono::steady_clock::now();

}

void FrameRate::Update()
{
	// 現在のフレームレートの取得
	float FPS = MaxFrameRate();

	// フレームレートピッタリの時間
	std::chrono::microseconds kMinTime(static_cast<uint64_t>(1000000.0f / FPS));

	//// 1/60秒よりわずかに短い時間
	std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / FPS + 4.0f));

	// 現在時間を取得する
	auto now = std::chrono::steady_clock::now();
	// 前回記録からの経過時間を取得する
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒 (よりわずかに短い時間) 経っていない場合
	if (elapsed < kMinCheckTime) {
		// 1/60秒経過するまで微小なスリープを繰り返す
		auto wait_until = reference_ + kMinTime;
		while (std::chrono::steady_clock::now() < wait_until) {
			std::this_thread::yield(); // CPUに他のスレッドの実行を許可
		}
	}

	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();

	// スリープ後の正確な経過時間を計算
	elapsed = std::chrono::duration_cast<std::chrono::microseconds>(reference_ - now);
}

void FrameRate::StartFrame() {
	startTime_ = std::chrono::high_resolution_clock::now();
}

void FrameRate::EndFrame() {
	float frameTime = 0.0f;
	float fps = 0.0f;
	auto endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> elapsed = endTime - startTime_;
	frameTime = elapsed.count();

	if (frameTime > 0.0f) {
		fps = 1.0f / frameTime;
	}
	SystemState::GetInstance().SetNowFPS(fps);
	SystemState::GetInstance().SetDeltaTime(frameTime);
}


