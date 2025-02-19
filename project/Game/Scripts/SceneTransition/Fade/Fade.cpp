#include "PrecompiledHeader.h"
#include "Fade.h"
#include "Scene/SceneManager/SceneManager.h"

Fade::~Fade() {  }

void Fade::Initialize() {
	sprite->material.textureID = "uvChecker.png";
	sprite->size = Vector2(static_cast<float>(WindowWidth()), static_cast<float>(WindowHeight()));
	SetColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
	sprite->blendMode = BlendMode::kBlendModeNormal;
	sprite->render.visible = false;
}

void Fade::Update() {
	// ウィンドウサイズが変更されたら
	if (sprite->size.x != WindowWidth() || sprite->size.y != WindowHeight()) {
		sprite->size = Vector2(static_cast<float>(WindowWidth()), static_cast<float>(WindowHeight()));
	}

	if (status_ == Status::None) {
		sprite->render.visible = false;
	}
	else {
		sprite->render.visible = true;
	}
	/*フェード状態による分岐*/
	switch (status_) {
	case Fade::Status::None:
		break;
	case Fade::Status::FadeIn:
		/*1フレーム分の秒数をカウントアップ*/
		counter_ += 1.0f / 60.0f;
		/*フェード継続時間に達したら打ち止め*/
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		/*0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする*/
		SetColor(Color(0.0f, 0.0f, 0.0f, std::clamp(1.0f - (counter_ / duration_), 0.0f, 1.0f)));
		break;
	case Fade::Status::FadeOut:
		/*1フレーム分の秒数をカウントアップ*/
		counter_ += 1.0f / 60.0f;
		/*フェード継続時間に達したら打ち止め*/
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		/*0.0fから1.0fの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする*/
		SetColor(Color(0.0f, 0.0f, 0.0f, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	}
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
	if (status == Status::FadeOut) {
		SetColor(Color(0.0f, 0.0f, 0.0f, 0.0f));
	}
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const {
	/*フェード状態による分岐*/
	switch (status_) {
	case Fade::Status::FadeIn:
	case Fade::Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		}
		else {
			return false;
		}
		break;
	}
	return true;
}
