#pragma once

#include <memory>
#include <chomath.h>

class btRigidBody;

namespace physics::d3
{
	struct Id3BodyDef
	{
		Vector3 position; // 位置
		Vector3 halfsize; // 半径（ボックス形状の場合）
		float friction = 0.5f; // 摩擦係数
		float restitution = 0.2f; // 反発係数
		float mass = 1.0f; // 質量
		void* userData = nullptr; // ユーザーデータ
	};

	class Id3Body
	{
		public:
		virtual ~Id3Body() = default;
		// ボディの作成
		virtual void Create(const Id3BodyDef& bodyDef) = 0;
		// ボディの削除
		virtual void Destroy() = 0;
		// ボディの位置を取得
		virtual Vector3 GetPosition() const = 0;
		// ボディの角度を取得
		virtual Quaternion GetRotation() const = 0;
		// ボディの位置と角度を設定
		virtual void SetTransform(const Vector3& position, const Quaternion& rotation) = 0;
		// ボディの速度を取得
		virtual Vector3 GetLinearVelocity() const = 0;
		// ボディの速度を設定
		virtual void SetLinearVelocity(const Vector3& velocity) = 0;
		virtual Vector3 GetAngularVelocity() const { return Vector3::Zero(); } // 角速度を取得（デフォルトはゼロ）
		virtual void SetAngularVelocity(const Vector3&) {} // 角速度を設定（デフォルトは何もしない）
		// 有効化
		virtual bool IsActive() const = 0;
		// 有効化・無効化
		virtual void SetActive(bool active) = 0;
		// 運動剛体に設定
		virtual void SetKinematic(bool isKinematic) = 0;
		// センサーオブジェクト（すり抜け）設定
		virtual void SetSensor(bool) {}
	};

	class bulletBody : public Id3Body
	{
		friend class bulletWorld; // bulletWorldからのみアクセス可能
	public:
		bulletBody();
		~bulletBody() override = default;
		void Create(const Id3BodyDef& bodyDef) override;
		void Destroy() override;
		Vector3 GetPosition() const override;
		Quaternion GetRotation() const override;
		void SetTransform(const Vector3& position, const Quaternion& rotation) override;
		Vector3 GetLinearVelocity() const override;
		void SetLinearVelocity(const Vector3& velocity) override;
		Vector3 GetAngularVelocity() const override;
		void SetAngularVelocity(const Vector3& angularVelocity) override;
		btRigidBody* GetRigidBody() const; // Bulletの剛体データを取得
		bool IsActive() const override; // 有効かどうかを取得
		void SetActive(bool active) override; // 有効化・無効化
		void SetKinematic(bool isKinematic) override; // 運動剛体に設定（デフォルトは何もしない）
		void SetSensor(bool isSensor) override; // センサーオブジェクト（すり抜け）設定
	private:
		struct Impl; // 実装の詳細を隠蔽するための前方宣言
		std::unique_ptr<Impl> impl; // Bulletのボディデータを保持
	};

	class chophysicsBody : public Id3Body
	{
		// Id3Body を介して継承されました
		void Create(const Id3BodyDef& bodyDef) override;
		void Destroy() override;
		Vector3 GetPosition() const override;
		Quaternion GetRotation() const override;
		void SetTransform(const Vector3&, const Quaternion&) override {}
		Vector3 GetLinearVelocity() const override;
		void SetLinearVelocity(const Vector3& velocity) override;
		bool IsActive() const override;
		void SetActive(bool active) override { active; }
		void SetKinematic(bool isKinematic) override { isKinematic; } // 運動剛体に設定（デフォルトは何もしない）
	};
}

