#pragma once

#include "Game/Scripts/Sprite/BaseSprite/BaseSprite.h"

/*シーンのフェーズ*/
enum class Phase {
	kFadeIn,//フェードイン
	kMain,//メイン
	kFadeOut,//フェードアウト
};

class Fade :
	public BaseSprite
{
public:
	enum class Status {
		None,//フェードなし
		FadeIn,//フェードイン中
		FadeOut,//フェードアウト中
	};
	~Fade();
	// BaseSprite を介して継承されました
	void Initialize() override;
	void Update() override;

	/// <summary>
	/// フェード開始
	/// </summary>
	/// <param name="status"></param>
	/// <param name="duration"></param>
	void Start(Status status, float duration);

	/// <summary>
	///	フェード停止
	/// </summary>
	void Stop();

	/// <summary>
	/// フェード終了判定
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const;

	Status GetStatus() { return status_; }
private:
	Status status_ = Status::None;
	/*フェードの持続時間*/
	float duration_ = 0.0f;
	/*経過時間カウンター*/
	float counter_ = 0.0f;
};
