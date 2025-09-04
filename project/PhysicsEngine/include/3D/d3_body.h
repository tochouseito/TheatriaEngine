#pragma once

#include <memory>
#include <chomath.h>

class btRigidBody;

namespace physics::d3
{
	class Id3World;

	enum class Id3BodyType
	{
		STATIC, // 静的
		DYNAMIC, // 動的
		KINEMATIC // 運動
	};

	struct Id3BodyDef
	{
		Vector3 position; // 位置
		Vector3 degrees; // オイラー角
		Vector3 halfsize; // 半径（ボックス形状の場合）
		float friction = 0.5f; // 摩擦係数
		float restitution = 0.2f; // 反発係数
		float mass = 1.0f; // 質量
		Id3BodyType bodyType = Id3BodyType::DYNAMIC; // ボディタイプ
		void* userData = nullptr; // ユーザーデータ
		int userIndex = -1; // ユーザーインデックス
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
		virtual Quaternion GetQuaternion() const = 0;
		virtual Vector3 GetRotation() const = 0; // オイラー角を取得
		// ボディの位置と角度を設定
		virtual void SetTransform(const Vector3& position, const Quaternion& rotation) = 0;
		virtual void SetTransform(const Vector3& position) = 0;
		virtual void SetTransform(const Quaternion& rotation) = 0;
		// ボディの速度を取得
		virtual Vector3 GetLinearVelocity() const = 0;
		// ボディの速度を設定
		virtual void SetLinearVelocity(const Vector3& velocity) = 0;
		virtual Vector3 GetAngularVelocity() const { return Vector3::Zero(); } // 角速度を取得（デフォルトはゼロ）
		virtual void SetAngularVelocity(const Vector3&) {} // 角速度を設定（デフォルトは何もしない）
		// gravityScaleの設定
		virtual void SetGravityScale(Id3World* world, const float&) = 0;
		// 有効化
		virtual bool IsActive() const = 0;
		// 有効化・無効化
		virtual void SetActive(bool active) = 0;
		// BodyTypeの設定
		virtual void SetBodyType(Id3World* world, Id3BodyType) = 0;
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
		Quaternion GetQuaternion() const override;
		Vector3 GetRotation() const override;
		void SetTransform(const Vector3& position, const Quaternion& rotation) override;
		void SetTransform(const Vector3& position) override;
		void SetTransform(const Quaternion& rotation) override;
		Vector3 GetLinearVelocity() const override;
		void SetLinearVelocity(const Vector3& velocity) override;
		Vector3 GetAngularVelocity() const override;
		void SetAngularVelocity(const Vector3& angularVelocity) override;
		void SetGravityScale(Id3World* world, const float& scale) override; // gravityScaleの設定
		btRigidBody* GetRigidBody() const; // Bulletの剛体データを取得
		bool IsActive() const override; // 有効かどうかを取得
		void SetActive(bool active) override; // 有効化・無効化
		void SetBodyType(Id3World* world,Id3BodyType bodyType) override; // BodyTypeの設定
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
		Quaternion GetQuaternion() const override;
		Vector3 GetRotation() const override { return Vector3::Zero(); } // オイラー角を取得
		void SetTransform(const Vector3&, const Quaternion&) override {}
		void SetTransform(const Vector3&) override {}
		void SetTransform(const Quaternion&) override {}
		Vector3 GetLinearVelocity() const override;
		void SetLinearVelocity(const Vector3& velocity) override;
		void SetGravityScale(Id3World*, const float& scale) override { scale; } // gravityScaleの設定
		bool IsActive() const override;
		void SetActive(bool active) override { active; }
		void SetBodyType(Id3World*, Id3BodyType bodyType) override { bodyType; }
	};
}

