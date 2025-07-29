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
		// ボディの速度を取得
		virtual Vector3 GetLinearVelocity() const = 0;
		// ボディの速度を設定
		virtual void SetLinearVelocity(const Vector3& velocity) = 0;
		virtual Vector3 GetAngularVelocity() const { return Vector3::Zero(); } // 角速度を取得（デフォルトはゼロ）
		virtual void SetAngularVelocity(const Vector3&) {} // 角速度を設定（デフォルトは何もしない）
		// 有効化
		virtual bool IsActive() const = 0;
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
		Vector3 GetLinearVelocity() const override;
		void SetLinearVelocity(const Vector3& velocity) override;
		Vector3 GetAngularVelocity() const override;
		void SetAngularVelocity(const Vector3& angularVelocity) override;
		btRigidBody* GetRigidBody() const; // Bulletの剛体データを取得
		bool IsActive() const override; // 有効かどうかを取得
		void SetActive(bool active); // 有効化・無効化
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
		Vector3 GetLinearVelocity() const override;
		void SetLinearVelocity(const Vector3& velocity) override;
		bool IsActive() const override;
	};
}

