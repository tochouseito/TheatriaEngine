#pragma once
#include "GameCore/ECS/ECSManager.h"
#include "SDK/DirectX/DirectX12/GpuBuffer/GpuBuffer.h"
#include "Core/Utility/CompBufferData.h"
#include "GameCore/ScriptAPI/ScriptAPI.h"
#include "Platform/Timer/Timer.h"

class ResourceManager;
class GraphicsEngine;
class GameCoreCommand;
class ScriptContainer;
class ObjectContainer;
class InputManager;
struct ModelData;

// TransformComponentSystem
class TransformSystem : public ECSManager::System<TransformComponent>
{
public:
	TransformSystem():
		ECSManager::System<TransformComponent>(
			[&](Entity e, TransformComponent& transform)
			{
				UpdateComponent(e, transform);
			},
			[&](Entity e, TransformComponent& transform)
			{
				InitializeComponent(e, transform);
			},
			[&](Entity e, TransformComponent& transform)
			{
				FinalizeComponent(e, transform);
			})
	{
	}
private:
	void InitializeComponent([[maybe_unused]] Entity e,[[maybe_unused]] TransformComponent& transform)
	{
		// 初期値保存
		transform.startValue.translation = transform.position;
		transform.startValue.rotation = transform.rotation;
		transform.startValue.scale = transform.scale;
		transform.startValue.degrees = transform.degrees;

		// 度数からラジアンに変換
		Vector3 radians = ChoMath::DegreesToRadians(transform.degrees);

		// 各軸のクオータニオンを作成
		Quaternion qx = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), radians.x);
		Quaternion qy = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), radians.y);
		Quaternion qz = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), radians.z);

		// 同時回転を累積
		transform.rotation = qx * qy * qz;

		// 精度を維持するための正規化
		transform.rotation.Normalize();

		// アフィン変換
		transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.position);

		// 次のフレーム用に保存する
		transform.prePos = transform.position;
		transform.preRot = radians;
		transform.preScale = transform.scale;
	}
	// 優先度順に更新する
	void priorityUpdate()
	{
		std::vector<std::pair<Entity, TransformComponent*>> transforms;

		// すべてのTransformComponentを取得
		for (auto& [arch, container] : m_pEcs->GetArchToEntities())
		{
			if ((arch & GetRequired()) == GetRequired())
			{
				for (Entity e : container.GetEntities())
				{
					if (auto* t = m_pEcs->GetComponent<TransformComponent>(e))
					{
						transforms.emplace_back(e, t);
					}
				}
			}
		}

		// 親子関係優先 + tickPriority順で安定ソート
		std::sort(transforms.begin(), transforms.end(),
			[](const auto& a, const auto& b)
			{
				const auto& aParent = a.second->parent;
				const auto& bParent = b.second->parent;

				if (aParent && bParent)
				{
					if (*aParent == b.first) return false; // b is parent of a
					if (*bParent == a.first) return true;  // a is parent of b
				}
				else if (aParent && *aParent == b.first) return false;
				else if (bParent && *bParent == a.first) return true;

				// 優先度比較
				return a.second->tickPriority < b.second->tickPriority;
			});

		// 更新実行
		for (auto& [e, t] : transforms)
		{
			UpdateComponent(e, *t);
		}
	}
	void UpdateComponent([[maybe_unused]] Entity e, TransformComponent& transform)
	{
		// 度数からラジアンに変換
		Vector3 radians = ChoMath::DegreesToRadians(transform.degrees);

		if (m_isQuaternion)
		{
			// クォータニオンを使用する場合
			transform.rotation = MakeQuaternionRotation(radians, transform);
		}
		else
		{
			// オイラー角を使用する場合
			transform.rotation = MakeEulerRotation(radians);
		};

		// アフィン変換
		transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.position);

		// 次のフレーム用に保存する
		transform.prePos = transform.position;
		transform.preRot = radians;
		transform.preScale = transform.scale;

		// 親があれば親のワールド行列を掛ける
		if (transform.parent.has_value())
		{
			transform.matWorld = ChoMath::Multiply(transform.matWorld, m_pEcs->GetComponent<TransformComponent>(transform.parent.value())->matWorld);
		}

		// 物理コンポーネントがあれば、物理ボディの位置を更新
		Rigidbody2DComponent* rb = m_pEcs->GetComponent<Rigidbody2DComponent>(e);
		if (rb && rb->runtimeBody)
		{
			rb->runtimeBody->SetLinearVelocity(b2Vec2(rb->velocity.x, rb->velocity.y));
		}

		// アニメーションコンポーネントがあればスキニングの確認
		AnimationComponent* anim = m_pEcs->GetComponent<AnimationComponent>(e);
		if (anim && anim->boneOffsetID.has_value())
		{
			transform.boneOffsetID = anim->boneOffsetID.value();
		}

		// 各行列
		transform.matLocal = ChoMath::MakeTranslateMatrix(transform.position);
		transform.matRotation = ChoMath::MakeRotateMatrix(transform.rotation);
		transform.matScale = ChoMath::MakeScaleMatrix(transform.scale);

		// 前方ベクトル
		transform.forward = ChoMath::GetForwardVectorFromMatrix(transform.matRotation);
		transform.forward.Normalize();

		// 行列の転送
		TransferMatrix(transform);
	}
	// オイラー回転
	Quaternion MakeEulerRotation(const Vector3& rad)
	{
		// オイラー角からクォータニオンを作成
		// 各軸のクオータニオンを作成
		Quaternion qx = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), rad.x);
		Quaternion qy = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), rad.y);
		Quaternion qz = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), rad.z);

		// 同時回転を累積
		Quaternion q = qx * qy * qz;
		return q.Normalize(); // 正規化して返す
	}
	Quaternion MakeQuaternionRotation(const Vector3& rad,const TransformComponent& c)
	{
		// 差分計算
		Vector3 diff = rad - c.preRot;

		// 各軸のクオータニオンを作成
		Quaternion qx = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), diff.x);
		Quaternion qy = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), diff.y);
		Quaternion qz = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), diff.z);

		// 同時回転を累積
		Quaternion q = c.rotation * qx * qy * qz;
		return q.Normalize(); // 正規化して返す
	};
	
	// 転送
	void TransferMatrix(TransformComponent& transform)
	{
		// マップデータ更新
		BUFFER_DATA_TF data = {};
		data.matWorld = transform.matWorld;
		data.worldInverse = ChoMath::Transpose(Matrix4::Inverse(transform.matWorld));
		data.rootMatrix = transform.rootMatrix;
		if (transform.materialID.has_value())
		{
			data.materialID = transform.materialID.value();
		}
		else
		{
			data.materialID = 0;
		}
		if (transform.boneOffsetID.has_value())
		{
			data.isAnimated = 1;
			data.boneOffsetStartIndex = transform.boneOffsetID.value();
		}
		else
		{
			data.isAnimated = 0;
			data.boneOffsetStartIndex = 0;
		}
		m_pIntegrationBuffer->UpdateData(data, transform.mapID.value());
	}
	void FinalizeComponent([[maybe_unused]] Entity e, [[maybe_unused]] TransformComponent& transform)
	{
		// 初期値に戻す
		transform.position = transform.startValue.translation;
		transform.rotation = transform.startValue.rotation;
		transform.scale = transform.startValue.scale;
		transform.degrees = transform.startValue.degrees;
	}

	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;
	bool m_isQuaternion = false; // クォータニオンを使用するかどうか
};

class CameraSystem : public ECSManager::System<TransformComponent, CameraComponent>
{
	public:
		CameraSystem():
			ECSManager::System<TransformComponent, CameraComponent>(
				[&](Entity e, TransformComponent& transform, CameraComponent& camera)
				{
					UpdateComponent(e, transform, camera);
				},
				[&](Entity e, TransformComponent& transform, CameraComponent& camera)
				{
					InitializeComponent(e, transform, camera);
				},
				[&](Entity e, TransformComponent& transform, CameraComponent& camera)
				{
					FinalizeComponent(e, transform, camera);
				})
		{
		}
	~CameraSystem() = default;
private:
	void InitializeComponent([[maybe_unused]] Entity e, TransformComponent& transform, CameraComponent&);
	void UpdateComponent([[maybe_unused]] Entity e, TransformComponent& transform, CameraComponent& camera)
	{
		TransferMatrix(transform, camera);
	}
	void TransferMatrix(TransformComponent& transform, CameraComponent& camera)
	{
		BUFFER_DATA_VIEWPROJECTION data = {};
		data.matWorld = transform.matWorld;
		data.view = Matrix4::Inverse(transform.matWorld);
		data.projection = ChoMath::MakePerspectiveFovMatrix(camera.fovAngleY, camera.aspectRatio, camera.nearZ, camera.farZ);
		data.projectionInverse = Matrix4::Inverse(data.projection);
		data.cameraPosition = transform.position;
		m_pCameraBuffer->UpdateData(data);
	}
	void FinalizeComponent([[maybe_unused]] Entity e, TransformComponent& transform, CameraComponent& camera);

	ConstantBuffer<BUFFER_DATA_VIEWPROJECTION>* m_pCameraBuffer = nullptr;
	StructuredBuffer<BUFFER_DATA_TF>* m_pIntegrationBuffer = nullptr;
};

class ScriptInstanceGenerateSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptInstanceGenerateSystem() :
		ECSManager::System<ScriptComponent>(
			[&]([[maybe_unused]] Entity e, [[maybe_unused]] ScriptComponent& script)
			{
				// 何もしない
			},
			[&]([[maybe_unused]] Entity e, ScriptComponent& script)
			{
				GenerateInstance(script);
			})
	{
	}
	~ScriptInstanceGenerateSystem() = default;
private:
	void GenerateInstance(ScriptComponent& script)
	{
		GameObject* object = m_pObjectContainer->GetGameObject(script.objectID.value());
		if (script.scriptName.empty() || !object->IsActive())
		{
			script.isActive = false;
			return;
		}
		std::string funcName = "Create" + script.scriptName + "Script";
		funcName.erase(std::remove_if(funcName.begin(), funcName.end(), ::isspace), funcName.end());
		// CreateScript関数を取得
		typedef IScript* (*CreateScriptFunc)(GameObject&);
		CreateScriptFunc createScript = (CreateScriptFunc)GetProcAddress(Cho::FileSystem::ScriptProject::m_DllHandle, funcName.c_str());
		if (!createScript)
		{
			script.isActive = false;
			return;
		}
		// スクリプトを生成
		object->Initialize();// 初期化
		IScript* scriptInstance = createScript(*object);
		if (!scriptInstance)
		{
			script.isActive = false;
			return;
		}
		// スクリプトのStart関数とUpdate関数をラップ
		script.startFunc = [scriptInstance]() {
			scriptInstance->Start();
			};
		script.updateFunc = [scriptInstance]() {
			scriptInstance->Update();
			};
		// インスタンスの解放用のクロージャを設定
		script.cleanupFunc = [scriptInstance, this]() {
			delete scriptInstance;
			};
		// 衝突関数をラップ
		script.onCollisionEnterFunc = [scriptInstance](GameObject& other) {
			scriptInstance->OnCollisionEnter(other);
			};
		script.onCollisionStayFunc = [scriptInstance](GameObject& other) {
			scriptInstance->OnCollisionStay(other);
			};
		script.onCollisionExitFunc = [scriptInstance](GameObject& other) {
			scriptInstance->OnCollisionExit(other);
			};
		script.scriptInstance = scriptInstance;
		script.isActive = true;
	}
	ResourceManager* m_pResourceManager = nullptr;
	InputManager* m_pInputManager = nullptr;
	ObjectContainer* m_pObjectContainer = nullptr;
};

class ScriptSystem : public ECSManager::System<ScriptComponent>
{
public:
	ScriptSystem():
		ECSManager::System<ScriptComponent>(
			[&](Entity e, ScriptComponent& script)
			{
				UpdateComponent(e, script);
			},
			[&](Entity e, ScriptComponent& script)
			{
				InitializeComponent(e, script);
			},
			[&](Entity e, ScriptComponent& script)
			{
				FinalizeComponent(e, script);
			})
	{
	}
	~ScriptSystem() = default;
private:
	void InitializeComponent([[maybe_unused]] Entity e, [[maybe_unused]] ScriptComponent& script)
	{
		if (!script.isActive) return;
		try
		{
			// スクリプトのStart関数を呼び出す
			script.startFunc();
		}
		catch (const std::exception& e)
		{
			// スクリプトのエラー処理
			Log::Write(LogLevel::Debug, "Script error: " + std::string(e.what()));
			script.isActive = false;
		}
		catch (...)
		{
			// その他のエラー処理
			Log::Write(LogLevel::Debug, "Unknown script error");
			script.isActive = false;
		}
	}
	void UpdateComponent([[maybe_unused]] Entity e, ScriptComponent& script)
	{
		if (!script.isActive) return;
		try
		{
			// スクリプトのUpdate関数を呼び出す
			script.updateFunc();
		}
		catch (const std::exception& e)
		{
			// スクリプトのエラー処理
			Log::Write(LogLevel::Debug, "Script error: " + std::string(e.what()));
			script.isActive = false;
		}
		catch (...)
		{
			// その他のエラー処理
			Log::Write(LogLevel::Debug, "Unknown script error");
			script.isActive = false;
		}
	}
	void FinalizeComponent([[maybe_unused]] Entity e, ScriptComponent& script)
	{
		if (!script.isActive) return;
		try
		{
			// スクリプトのCleanup関数を呼び出す
			script.cleanupFunc();
			script.scriptInstance = nullptr;
		}
		catch (const std::exception& e)
		{
			// スクリプトのエラー処理
			Log::Write(LogLevel::Debug, "Script error: " + std::string(e.what()));
		}
		catch (...)
		{
			// その他のエラー処理
			Log::Write(LogLevel::Debug, "Unknown script error");
		}
		script.isActive = false;
	}
};

class Rigidbody2DSystem : public ECSManager::System<TransformComponent, Rigidbody2DComponent>
{
public:
	Rigidbody2DSystem():
		ECSManager::System<TransformComponent, Rigidbody2DComponent>(
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
			{
				UpdateComponent(e, transform, rb);
			},
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
			{
				InitializeComponent(e, transform, rb);
			},
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
			{
				FinalizeComponent(e, transform, rb);
			})
	{
	}
	~Rigidbody2DSystem() = default;
	void Update() override
	{
		// Step は一回だけ呼ぶ（エンティティループより前）
		StepSimulation();

		// いつもの処理（b2Body -> Transform同期）
		ECSManager::System<TransformComponent, Rigidbody2DComponent>::Update();
	}
private:
	void InitializeComponent([[maybe_unused]] Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
	{
		if (rb.runtimeBody != nullptr) return;
		b2BodyDef bodyDef;
		bodyDef.userData.pointer = static_cast<uintptr_t>(rb.selfEntity.value());
		bodyDef.type = rb.bodyType;
		bodyDef.gravityScale = rb.gravityScale;
		bodyDef.fixedRotation = rb.fixedRotation;
		bodyDef.position = b2Vec2(transform.position.x, transform.position.y);
		float angleZ = ChoMath::DegreesToRadians(transform.degrees).z;
		bodyDef.angle = angleZ;
		rb.runtimeBody = m_World->CreateBody(&bodyDef);
		rb.runtimeBody->SetAwake(true);
		rb.world = m_World;
		rb.velocity.Initialize();

		// Transformと同期（optional）
		transform.position.x = rb.runtimeBody->GetPosition().x;
		transform.position.y = rb.runtimeBody->GetPosition().y;
	}
	void StepSimulation()
	{
		float timeStep = Timer::GetDeltaTime();
		constexpr int velocityIterations = 6;
		constexpr int positionIterations = 2;
		m_World->Step(timeStep, velocityIterations, positionIterations);
	}
	void UpdateComponent([[maybe_unused]] Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
	{
		if (rb.runtimeBody == nullptr) return;

		if (rb.requestedPosition)
		{
			if (rb.fixedRotation)
			{
				rb.runtimeBody->SetTransform(*rb.requestedPosition, ChoMath::DegreesToRadians(transform.degrees).z);
			}
			else
			{
				rb.runtimeBody->SetTransform(*rb.requestedPosition, rb.runtimeBody->GetAngle());
			}
			rb.requestedPosition.reset();
		}
		const b2Vec2& pos = rb.runtimeBody->GetPosition();
		transform.position.x = pos.x;
		transform.position.y = pos.y;

		b2Vec2 velocity = rb.runtimeBody->GetLinearVelocity();
		rb.velocity.x = velocity.x;
		rb.velocity.y = velocity.y;

		Vector3 radians = {};
		if (!rb.fixedRotation)
		{
			radians.z = rb.runtimeBody->GetAngle(); // radians
		}
		else
		{
			radians.z = ChoMath::DegreesToRadians(transform.degrees).z;
		}
		Vector3 degrees = ChoMath::RadiansToDegrees(radians);
		transform.degrees.z = degrees.z;

		//Vector3 radians = ChoMath::DegreesToRadians(transform.degrees);
		//radians.z = rb.runtimeBody->GetAngle(); // radians
		//Vector3 diff = radians - transform.preRot;
		//Quaternion qx = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), diff.x);
		//Quaternion qy = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), diff.y);
		//Quaternion qz = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), diff.z);
		//transform.rotation = transform.rotation * qx * qy * qz;//;
		//transform.rotation = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0, 0, 1), angle);

	}
	void Reset(Rigidbody2DComponent& rb)
	{
		// Bodyがあるなら削除
		if (rb.runtimeBody)
		{
			m_World->DestroyBody(rb.runtimeBody);
			rb.runtimeBody = nullptr;
		}
		rb.isCollisionStay = false;
		rb.otherEntity.reset();
	}
	template<typename ColliderT>
	void ResetCollider(Entity e)
	{
		ColliderT* col = m_ECS->GetComponent<ColliderT>(e);
		if (col && col->runtimeFixture)
		{
			col->runtimeFixture = nullptr;
		}
	}
	void FinalizeComponent([[maybe_unused]] Entity e, TransformComponent& transform, Rigidbody2DComponent& rb)
	{
		Reset(rb);
		ResetCollider<BoxCollider2DComponent>(e);
	}
	b2World* m_World = nullptr;
};

class CollisionSystem : public ECSManager::System<ScriptComponent, Rigidbody2DComponent>
{
public:
	CollisionSystem() :
		ECSManager::System<ScriptComponent, Rigidbody2DComponent>(
			[&]([[maybe_unused]] Entity e, [[maybe_unused]] ScriptComponent& script, [[maybe_unused]] Rigidbody2DComponent& rb)
			{
			},
			[&]([[maybe_unused]] Entity e, ScriptComponent& script, Rigidbody2DComponent& rb)
			{
				CollisionStay(script, rb);
			},
			[&]([[maybe_unused]] Entity e, [[maybe_unused]] ScriptComponent& script, [[maybe_unused]] Rigidbody2DComponent& rb)
			{
			})
	{
	}
	~CollisionSystem() = default;
private:
	void CollisionStay(ScriptComponent& script, Rigidbody2DComponent& rb)
	{
		if (script.isActive && rb.isCollisionStay && rb.otherEntity)
		{
			// 相手のゲームオブジェクトを取得
			GameObject* otherObject = m_pObjectContainer->GetGameObject(rb.otherEntity.value());
			if (!otherObject) { return; }
			if (!rb.isActive) { return; }
			otherObject->Initialize(false);// これいる？？
			script.onCollisionStayFunc(*otherObject);
		}
	}
};

class Collider2DSystem : public ECSManager::System<TransformComponent, Rigidbody2DComponent, BoxCollider2DComponent>
{
public:
	Collider2DSystem():
		ECSManager::System<TransformComponent, Rigidbody2DComponent, BoxCollider2DComponent>(
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb, BoxCollider2DComponent& box)
			{
				UpdateComponent(e, transform, rb, box);
			},
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb, BoxCollider2DComponent& box)
			{
				e;
				InitializeComponent(e, transform, rb, box);
			},
			[this](Entity e, TransformComponent& transform, Rigidbody2DComponent& rb, BoxCollider2DComponent& box)
			{
				e;
				FinalizeComponent(e, transform, rb, box);
			})
	{
	}
	~Collider2DSystem() = default;
private:
	float ComputePolygonArea(const b2PolygonShape* shape)
	{
		float area = 0.0f;
		const int count = shape->m_count;
		const b2Vec2* verts = shape->m_vertices;

		for (int i = 0; i < count; ++i)
		{
			const b2Vec2& a = verts[i];
			const b2Vec2& b = verts[(i + 1) % count];
			area += a.x * b.y - a.y * b.x;
		}
		return 0.5f * std::abs(area);
	}
	void InitializeComponent([[maybe_unused]] Entity e, TransformComponent& transform, Rigidbody2DComponent& rb, BoxCollider2DComponent& box)
	{
		transform;
		if (!rb.runtimeBody || box.runtimeFixture != nullptr) return;

		b2PolygonShape shape;
		shape.SetAsBox(box.width / 2.0f, box.height / 2.0f, b2Vec2(box.offsetX, box.offsetY), 0.0f);

		// 面積を計算
		float area = ComputePolygonArea(&shape);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = rb.mass / area;
		fixtureDef.friction = box.friction;
		fixtureDef.restitution = box.restitution;
		fixtureDef.isSensor = box.isSensor;

		box.runtimeFixture = rb.runtimeBody->CreateFixture(&fixtureDef);
	}
	void UpdateComponent([[maybe_unused]] Entity e, [[maybe_unused]] TransformComponent& transform, Rigidbody2DComponent& rb, BoxCollider2DComponent& box)
	{
		// サイズの変更があった場合、フィクスチャを再作成
		if (box.runtimeFixture == nullptr) { return; }
		if (box.width != box.runtimeFixture->GetShape()->m_radius || box.height != box.runtimeFixture->GetShape()->m_radius)
		{
			rb.runtimeBody->DestroyFixture(box.runtimeFixture);
			b2PolygonShape shape;
			shape.SetAsBox(box.width / 2.0f, box.height / 2.0f, b2Vec2(box.offsetX, box.offsetY), 0.0f);
			b2FixtureDef fixtureDef;
			fixtureDef.shape = &shape;
			fixtureDef.density = box.density;
			fixtureDef.friction = box.friction;
			fixtureDef.restitution = box.restitution;
			fixtureDef.isSensor = box.isSensor;
			box.runtimeFixture = rb.runtimeBody->CreateFixture(&fixtureDef);
		}
	}
	void FinalizeComponent([[maybe_unused]] Entity e, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] Rigidbody2DComponent& rb, [[maybe_unused]] BoxCollider2DComponent& box)
	{
		
	}
};

class MaterialSystem : public ECSManager::System<MaterialComponent>
{
public:
	MaterialSystem() : 
		ECSManager::System<MaterialComponent>(
			[&](Entity e, MaterialComponent& material)
			{
				UpdateComponent(e, material);
			},
			[&](Entity e, MaterialComponent& material)
			{
				InitializeComponent(e, material);
			},
			[&](Entity e, MaterialComponent& material)
			{
				FinalizeComponent(e, material);
			})
	{
	}
	~MaterialSystem() = default;
private:
	void InitializeComponent([[maybe_unused]] Entity e, MaterialComponent& material)
	{
	}
	void UpdateComponent([[maybe_unused]] Entity e, MaterialComponent& material)
	{
		BUFFER_DATA_MATERIAL data = {};
		data.color = material.color;
		data.enableLighting = material.enableLighting;
		if (material.enableTexture)
		{
			data.enableTexture = true;
			if (!material.textureName.empty() && m_pResourceManager->GetTextureManager()->IsTextureExist(material.textureName))
			{
				if (material.textureID.has_value())
				{
					data.textureId = material.textureID.value();
				}
				else
				{
					data.textureId = m_pResourceManager->GetTextureManager()->GetTextureID(material.textureName);
				}
			}
			else
			{
				data.textureId = 0;
			}
		}
		else
		{
			data.enableTexture = 0;
			data.textureId = 0;
		}
		data.matUV = material.matUV.Identity();
		data.shininess = material.shininess;
		data.uvFlipY = material.uvFlipY;
		m_pIntegrationBuffer->UpdateData(data, material.mapID.value());
	}
	void FinalizeComponent([[maybe_unused]] Entity e, MaterialComponent& material)
	{
	}
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_MATERIAL>* m_pIntegrationBuffer = nullptr;
};

class ParticleEmitterSystem : public ECSManager::System<ParticleComponent, EmitterComponent>
{
public:
	ParticleEmitterSystem():
		ECSManager::System<ParticleComponent, EmitterComponent>(
			[&](Entity e, ParticleComponent& particle, EmitterComponent& emitter)
			{
				UpdateComponent(e, particle, emitter);
			},
			[&](Entity e, ParticleComponent& particle, EmitterComponent& emitter)
			{
				InitializeComponent(e, particle, emitter);
			},
			[&](Entity e, ParticleComponent& particle, EmitterComponent& emitter)
			{
				FinalizeComponent(e, particle, emitter);
			})
	{
	}
	~ParticleEmitterSystem() = default;
private:
	void InitializeComponent([[maybe_unused]] Entity e, ParticleComponent& particle, EmitterComponent& emitter)
	{
		particle.time = 0.0f;

		// CSで初期化
		// コンテキスト取得
		CommandContext* context = m_pGraphicsEngine->GetCommandContext();
		// コマンドリスト開始
		m_pGraphicsEngine->BeginCommandContext(context);
		// パイプラインセット
		context->SetComputePipelineState(m_pGraphicsEngine->GetPipelineManager()->GetParticleInitPSO().pso.Get());
		// ルートシグネチャセット
		context->SetComputeRootSignature(m_pGraphicsEngine->GetPipelineManager()->GetParticleInitPSO().rootSignature.Get());
		// パーティクルバッファをセット
		IRWStructuredBuffer* particleBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.bufferIndex);
		context->SetComputeRootDescriptorTable(0, particleBuffer->GetUAVGpuHandle());
		// フリーリストバッファをセット
		IRWStructuredBuffer* freeListBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.freeListBufferIndex);
		context->SetComputeRootDescriptorTable(1, freeListBuffer->GetUAVGpuHandle());
		// カウンターバッファをセット
		context->SetComputeRootUnorderedAccessView(2, freeListBuffer->GetCounterResource()->GetGPUVirtualAddress());
		// Dispatch
		context->Dispatch(1, 1, 1);
		// コマンドリストをクローズ
		m_pGraphicsEngine->EndCommandContext(context, QueueType::Compute);
		// GPUの処理が終わるまで待機
		m_pGraphicsEngine->WaitForGPU(QueueType::Compute);
	}
	void UpdateComponent([[maybe_unused]] Entity e, ParticleComponent& particle, EmitterComponent& emitter)
	{
		TransformComponent* transform = m_pECS->GetComponent<TransformComponent>(e);
		emitter.frequencyTime += DeltaTime();

		// 射出間隔を上回ったら射出許可を出して時間を調整
		//if (emitter.frequency <= emitter.frequencyTime)
		//{
		//	emitter.frequencyTime = 0.0f;
		//	emitter.emit = 1;
		//} else
		//{
		//	// 射出間隔を上回ってないので、許可は出せない
		//	emitter.emit = 1;
		//}
		BUFFER_DATA_EMITTER data = {};
		data.lifeTime = emitter.lifeTime;
		data.position = emitter.position;
		data.position.value.x.median = transform->position.x;
		data.position.value.y.median = transform->position.y;
		data.position.value.z.median = transform->position.z;
		data.rotation = emitter.rotation;
		data.scale = emitter.scale;
		data.frequency = emitter.frequency;
		data.frequencyTime = emitter.frequencyTime;
		data.emit = emitter.emit;
		data.emitCount = emitter.emitCount;
		data.isFadeOut = emitter.isFadeOut;
		data.isBillboard = emitter.isBillboard;
		if (transform->materialID.has_value())
		{
			data.materialID = transform->materialID.value();
		}
		else
		{
			data.materialID = 0;
		}
		StructuredBuffer<BUFFER_DATA_EMITTER>* buffer = dynamic_cast<StructuredBuffer<BUFFER_DATA_EMITTER>*>(m_pResourceManager->GetBuffer<IStructuredBuffer>(emitter.bufferIndex));
		buffer->UpdateData(data, 0);
		emitter.emit = false;



		particle.time += DeltaTime();
		particle.deltaTime = DeltaTime();

		{
			BUFFER_DATA_PARTICLE_PERFRAME data = {};
			data.time = particle.time;
			data.deltaTime = particle.deltaTime;
			ConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>* buffer = dynamic_cast<ConstantBuffer<BUFFER_DATA_PARTICLE_PERFRAME>*>(m_pResourceManager->GetBuffer<IConstantBuffer>(particle.perFrameBufferIndex));
			buffer->UpdateData(data);
		}
		// Emit
		CommandContext* context = m_pGraphicsEngine->GetCommandContext();
		m_pGraphicsEngine->BeginCommandContext(context);
		// パイプラインセット
		context->SetComputePipelineState(m_pGraphicsEngine->GetPipelineManager()->GetParticleEmitPSO().pso.Get());
		// ルートシグネチャセット
		context->SetComputeRootSignature(m_pGraphicsEngine->GetPipelineManager()->GetParticleEmitPSO().rootSignature.Get());
		// パーティクルバッファをセット
		IRWStructuredBuffer* particleBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.bufferIndex);
		context->SetComputeRootDescriptorTable(0, particleBuffer->GetUAVGpuHandle());
		// エミッターバッファをセット
		IStructuredBuffer* emitterBuffer = m_pResourceManager->GetBuffer<IStructuredBuffer>(emitter.bufferIndex);
		context->SetComputeRootDescriptorTable(1, emitterBuffer->GetSRVGpuHandle());
		// PerFrameバッファをセット
		IConstantBuffer* perFrameBuffer = m_pResourceManager->GetBuffer<IConstantBuffer>(particle.perFrameBufferIndex);
		context->SetComputeRootConstantBufferView(2, perFrameBuffer->GetResource()->GetGPUVirtualAddress());
		// フリーリストインデックスバッファをセット
		//IRWStructuredBuffer* freeListIndexBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.freeListIndexBufferIndex);
		//context->SetComputeRootDescriptorTable(3, freeListIndexBuffer->GetUAVGpuHandle());
		// フリーリストバッファをセット
		IRWStructuredBuffer* freeListBuffer = m_pResourceManager->GetBuffer<IRWStructuredBuffer>(particle.freeListBufferIndex);
		context->SetComputeRootDescriptorTable(3, freeListBuffer->GetUAVGpuHandle());
		// Dispatch
		context->Dispatch(1, 1, 1);

		// EmitとUpdateの並列を阻止
		context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleBuffer->GetResource());
		//context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, freeListIndexBuffer->GetResource());
		context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, freeListBuffer->GetResource());

		// Update
		// パイプラインセット
		context->SetComputePipelineState(m_pGraphicsEngine->GetPipelineManager()->GetParticleUpdatePSO().pso.Get());
		// ルートシグネチャセット
		context->SetComputeRootSignature(m_pGraphicsEngine->GetPipelineManager()->GetParticleUpdatePSO().rootSignature.Get());
		// パーティクルバッファをセット
		context->SetComputeRootDescriptorTable(0, particleBuffer->GetUAVGpuHandle());
		// PerFrameバッファをセット
		context->SetComputeRootConstantBufferView(1, perFrameBuffer->GetResource()->GetGPUVirtualAddress());
		// フリーリストインデックスバッファをセット
		//context->SetComputeRootDescriptorTable(2, freeListIndexBuffer->GetUAVGpuHandle());
		// フリーリストバッファをセット
		context->SetComputeRootDescriptorTable(2, freeListBuffer->GetUAVGpuHandle());
		// Dispatch
		context->Dispatch(1, 1, 1);

		// クローズ
		m_pGraphicsEngine->EndCommandContext(context, QueueType::Compute);
		// GPUの処理が終わるまで待機
		m_pGraphicsEngine->WaitForGPU(QueueType::Compute);
	}
	void FinalizeComponent([[maybe_unused]] Entity e, ParticleComponent& particle, EmitterComponent& emitter)
	{
	}
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
};

class UISpriteSystem : public ECSManager::System<UISpriteComponent>
{
public:
	UISpriteSystem() : 
		ECSManager::System<UISpriteComponent>(
			[&](Entity e, UISpriteComponent& uiSprite)
			{
				UpdateComponent(e, uiSprite);
			},
			[&](Entity e, UISpriteComponent& uiSprite)
			{
				InitializeComponent(e, uiSprite);
			},
			[&](Entity e, UISpriteComponent& uiSprite)
			{
				e;
				FinalizeComponent(e, uiSprite);
			})
	{
	}
	~UISpriteSystem() = default;
private:
	void InitializeComponent([[maybe_unused]] Entity e, UISpriteComponent& uiSprite)
	{
		
	}
	void UpdateComponent([[maybe_unused]] Entity e, UISpriteComponent& uiSprite)
	{
		// UIの更新
		float left = 0.0f - uiSprite.anchorPoint.x;
		float right = uiSprite.size.x - uiSprite.anchorPoint.x;
		float top = 0.0f - uiSprite.anchorPoint.y;
		float bottom = uiSprite.size.y - uiSprite.anchorPoint.y;

		float tex_left = uiSprite.textureLeftTop.x / uiSprite.size.x;
		float tex_right = (uiSprite.textureLeftTop.x + uiSprite.size.x) / uiSprite.size.x;
		float tex_top = uiSprite.textureLeftTop.y / uiSprite.size.y;
		float tex_bottom = (uiSprite.textureLeftTop.y + uiSprite.size.y) / uiSprite.size.y;

		//uiSprite.scale = uiSprite.size;
		Vector3 scale = Vector3(uiSprite.scale.x, uiSprite.scale.y, 1.0f);
		Vector3 rotation = Vector3(0.0f, 0.0f, uiSprite.rotation);
		Vector3 translation = Vector3(uiSprite.position.x, uiSprite.position.y, 0.0f);
		Matrix4 worldMatrixSprite = ChoMath::MakeAffineMatrix(scale, rotation, translation);

		Matrix4 viewMatrixSprite = ChoMath::MakeIdentity4x4();

		Matrix4 projectionMatrixSprite = ChoMath::MakeOrthographicMatrix(0.0f, 0.0f,
			static_cast<float>(WinApp::GetWindowWidth()), static_cast<float>(WinApp::GetWindowHeight()),
			0.0f, 100.0f
		);

		Matrix4 worldViewProjectionMatrixSprite = ChoMath::Multiply(worldMatrixSprite, ChoMath::Multiply(viewMatrixSprite, projectionMatrixSprite));

		uiSprite.matWorld = worldViewProjectionMatrixSprite;

		//uiSprite.material.matUV = MakeAffineMatrix(Vector3(uiSprite.uvScale.x, uiSprite.uvScale.y, 1.0f), Vector3(0.0f, 0.0f, uiSprite.uvRot), Vector3(uiSprite.uvPos.x, uiSprite.uvPos.y, 0.0f));

		//uiSprite.constData->matWorld = uiSprite.matWorld;

		BUFFER_DATA_UISPRITE data = {};
		data.matWorld = uiSprite.matWorld;
		data.left = left;
		data.right = right;
		data.top = top;
		data.bottom = bottom;
		data.tex_left = tex_left;
		data.tex_right = tex_right;
		data.tex_top = tex_top;
		data.tex_bottom = tex_bottom;
		MaterialComponent* material = m_pECS->GetComponent<MaterialComponent>(e);
		if (material && material->mapID)
		{
			data.materialID = material->mapID.value();
		}
		else
		{
			data.materialID = 0;
		}
		m_pIntegrationBuffer->UpdateData(data, uiSprite.mapID.value());
	}
	void FinalizeComponent([[maybe_unused]] Entity e, UISpriteComponent& uiSprite)
	{
		
	}
	ResourceManager* m_pResourceManager = nullptr;
	StructuredBuffer<BUFFER_DATA_UISPRITE>* m_pIntegrationBuffer = nullptr;
};

class LightSystem : public ECSManager::System<TransformComponent, LightComponent>
{
public:
	LightSystem() : ECSManager::System<TransformComponent, LightComponent>(
		[&](Entity e, TransformComponent& transform, LightComponent& light)
		{
			UpdateComponent(e, transform, light);
		},
		[&](Entity e, TransformComponent& transform, LightComponent& light)
		{
			e;
			InitializeComponent(e, transform, light);
		},
		[&](Entity e, TransformComponent& transform, LightComponent& light)
		{
			e;
			FinalizeComponent(e, transform, light);
		})
	{
	}
	~LightSystem() = default;
private:
	void InitializeComponent([[maybe_unused]] Entity e, TransformComponent& transform, LightComponent& light)
	{
	}
	void UpdateComponent([[maybe_unused]] Entity e, TransformComponent& transform, LightComponent& light)
	{
		e;
		Vector3 direction = ChoMath::TransformDirection(Vector3(0.0f, 0.0f, 1.0f), ChoMath::MakeRotateXYZMatrix(ChoMath::DegreesToRadians(transform.degrees)));
		//direction.Normalize();
		// ライトのワールド行列を転送
		BUFFER_DATA_LIGHT& data = m_pResourceManager->GetLightBuffer()->GetData();
		uint32_t i = light.mapID.value();
		data.lightData[i].color = light.color;
		data.lightData[i].direction = direction;
		data.lightData[i].intensity = light.intensity;
		data.lightData[i].range = light.range;
		data.lightData[i].decay = light.decay;
		data.lightData[i].spotAngle = light.spotAngle;
		data.lightData[i].spotFalloffStart = light.spotFalloffStart;
		data.lightData[i].type = static_cast<uint32_t>(light.type);
		data.lightData[i].active = light.active;
		data.lightData[i].transformMapID = transform.mapID.value();
		// 参照に書き込んでいるので転送は不要
	}
	void FinalizeComponent([[maybe_unused]] Entity e, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] LightComponent& light)
	{
	}
	ResourceManager* m_pResourceManager = nullptr;
};

class AnimationSystem : public ECSManager::System<AnimationComponent>
{
public:
	AnimationSystem(ECSManager* ecs, ResourceManager* resourceManager, GraphicsEngine* graphicsEngine)
		: ECSManager::System<AnimationComponent>(
			[&](Entity e, AnimationComponent& animation)
			{
				UpdateComponent(e, animation);
			},
			[&](Entity e, AnimationComponent& animation)
			{
				InitializeComponent(e, animation);
			},
			[&](Entity e, AnimationComponent& animation)
			{
				FinalizeComponent(e, animation);
			}),
	{
	}
	~AnimationSystem() = default;
private:
	void InitializeComponent([[maybe_unused]] Entity e, AnimationComponent& animation)
	{
	}
	void UpdateComponent([[maybe_unused]] Entity e, AnimationComponent& animation)
	{
		ModelData* model = m_pResourceManager->GetModelManager()->GetModelData(animation.modelName);
		timeUpdate(animation, model);
	}
	void FinalizeComponent([[maybe_unused]] Entity e, AnimationComponent& animation)
	{
	}
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, const float& time)
	{
		assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
		if (keyframes.size() == 1 || time <= keyframes[0].time)
		{// キーが一つか、時刻がキーフレーム前なら最初の値とする
			return keyframes[0].value;
		}
		for (size_t index = 0; index < keyframes.size() - 1; ++index)
		{
			size_t nextIndex = index + 1;
			// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
			if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
			{
				// 範囲内を補間する
				float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
				return Vector3::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
			}
		}
		// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
		return (*keyframes.rbegin()).value;
	}
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, const float& time)
	{
		assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
		if (keyframes.size() == 1 || time <= keyframes[0].time)
		{// キーが一つか、時刻がキーフレーム前なら最初の値とする
			return keyframes[0].value;
		}
		for (size_t index = 0; index < keyframes.size() - 1; ++index)
		{
			size_t nextIndex = index + 1;
			// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
			if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
			{
				// 範囲内を補間する
				float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
				return Quaternion::Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
			}
		}
		// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
		return (*keyframes.rbegin()).value;
	}
	Scale CalculateValue(const std::vector<KeyframeScale>& keyframes, const float& time)
	{
		assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
		if (keyframes.size() == 1 || time <= keyframes[0].time)
		{// キーが一つか、時刻がキーフレーム前なら最初の値とする
			return keyframes[0].value;
		}
		for (size_t index = 0; index < keyframes.size() - 1; ++index)
		{
			size_t nextIndex = index + 1;
			// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
			if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
			{
				// 範囲内を補間する
				float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
				return Scale::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
			}
		}
		// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
		return (*keyframes.rbegin()).value;
	}
	void timeUpdate(AnimationComponent& animation, ModelData* model)
	{
		if (animation.isRun)
		{
			// animationIndexが変更された場合、遷移を開始する
			if (animation.prevAnimationIndex != animation.animationIndex)
			{
				animation.transitionIndex = animation.prevAnimationIndex;
				animation.transition = true;
				animation.transitionTime = 0.0f;
			}

			// 遷移が終了した場合、遷移フラグをfalseにする
			if (animation.lerpTime >= 1.0f)
			{
				animation.transition = false;
				animation.lerpTime = 0.0f;
				animation.time = animation.transitionTime;
				animation.transitionTime = 0.0f;
			}

			/*エンジンのデルタタイムにする*/
			static float deltaTime = 1.0f / 60.0f;

			// 遷移中の場合、遷移時間を更新
			if (animation.transition)
			{
				animation.transitionTime += deltaTime;//DeltaTime();
				animation.lerpTime = animation.transitionTime / animation.transitionDuration;
				animation.lerpTime = std::clamp(animation.lerpTime, 0.0f, 1.0f);
			}

			// 時間更新
			animation.isEnd = false;
			animation.time += deltaTime;//DeltaTime();
			if (animation.time >= model->animations[animation.animationIndex].duration)
			{
				animation.isEnd = true;
				if (!animation.isRestart)
				{
					animation.isRun = false;
				}
			}
			else
			{
				animation.isEnd = false;
			}
			if (!animation.isRestart)
			{
				animation.time;
			}
			else
			{
				animation.time = std::fmod(animation.time, model->animations[animation.animationIndex].duration);
			}
		}
		// アニメーション更新
		ApplyAnimation(animation, model);
		if (model->isBone)
		{
			SkeletonUpdate(animation, model);
			SkinClusterUpdate(animation, model);
			//ApplySkinning(animation, model);
		}
		animation.prevAnimationIndex = animation.animationIndex;
	}
	void ApplyAnimation(AnimationComponent& animation, ModelData* model)
	{
		for (Joint& joint : animation.skeleton->joints)
		{
			// 対象のJointのAnimationがあれば、値の適用を行う。下記のif文はC++17から可能になった初期化付きif文
			if (auto it = model->animations[animation.animationIndex].nodeAnimations.find(joint.name); it != model->animations[animation.animationIndex].nodeAnimations.end())
			{
				const NodeAnimation& rootNodeAnimation = (*it).second;
				if (animation.transition)
				{
					if (auto it2 = model->animations[animation.transitionIndex].nodeAnimations.find(joint.name); it2 != model->animations[animation.transitionIndex].nodeAnimations.end())
					{
						const NodeAnimation& rootNodeAnimation2 = (*it2).second;
						Vector3 startTranslate = CalculateValue(rootNodeAnimation2.translate.keyframes, animation.time);
						Quaternion startRotate = CalculateValue(rootNodeAnimation2.rotate.keyframes, animation.time);
						Scale startScale = CalculateValue(rootNodeAnimation2.scale.keyframes, animation.time);
						Vector3 endTranslate = CalculateValue(rootNodeAnimation.translate.keyframes, animation.transitionTime);
						Quaternion endRotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animation.transitionTime);
						Scale endScale = CalculateValue(rootNodeAnimation.scale.keyframes, animation.transitionTime);
						joint.transform.translation = Vector3::Lerp(startTranslate, endTranslate, animation.lerpTime);
						joint.transform.rotation = Quaternion::Slerp(startRotate, endRotate, animation.lerpTime);
						joint.transform.scale = Scale::Lerp(startScale, endScale, animation.lerpTime);
					}
				}
				else
				{
					joint.transform.translation = CalculateValue(rootNodeAnimation.translate.keyframes, animation.time);
					joint.transform.rotation = CalculateValue(rootNodeAnimation.rotate.keyframes, animation.time);
					joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animation.time);
				}
			}
		}
	}
	void SkeletonUpdate(AnimationComponent& animation, ModelData* model)
	{
		model;
		// すべてのJointを更新。親が若いので通常ループで処理可能になっている
		for (Joint& joint : animation.skeleton->joints)
		{
			joint.localMatrix = ChoMath::MakeAffineMatrix(joint.transform.scale, joint.transform.rotation, joint.transform.translation);
			if (joint.parent)
			{
				joint.skeletonSpaceMatrix = joint.localMatrix * animation.skeleton->joints[*joint.parent].skeletonSpaceMatrix;
			}
			else
			{// 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
				joint.skeletonSpaceMatrix = joint.localMatrix;
			}
		}
	}
	void SkinClusterUpdate(AnimationComponent& animation, ModelData* model)
	{
		StructuredBuffer<ConstBufferDataWell>* paletteBuffer = dynamic_cast<StructuredBuffer<ConstBufferDataWell>*>(m_pResourceManager->GetBuffer<IStructuredBuffer>(model->boneMatrixBufferIndex));
		for (uint32_t jointIndex = 0; jointIndex < model->skeleton.joints.size(); ++jointIndex)
		{
			assert(jointIndex < model->skinCluster.inverseBindPoseMatrices.size());
			uint32_t offset = static_cast<uint32_t>(model->skeleton.joints.size() * animation.boneOffsetID.value());
			ConstBufferDataWell data = {};
			data.skeletonSpaceMatrix =
				animation.skinCluster->inverseBindPoseMatrices[jointIndex] * animation.skeleton->joints[jointIndex].skeletonSpaceMatrix;
			data.skeletonSpaceInverseTransposeMatrix =
				ChoMath::Transpose(Matrix4::Inverse(data.skeletonSpaceMatrix));
			paletteBuffer->UpdateData(data, jointIndex + offset);
		}
	}
	void ApplySkinning(AnimationComponent& animation, ModelData* model)
	{
		animation;
		model;
		//// コマンドリストの取得
		//CommandContext* context = m_pGraphicsEngine->GetCommandContext();
		//m_pGraphicsEngine->BeginCommandContext(context);
		//// スキニングOutputリソースを遷移
		//IVertexBuffer* skinningBuffer = m_pResourceManager->GetBuffer<IVertexBuffer>(animation.skinningBufferIndex);
		//context->BarrierTransition(skinningBuffer->GetResource(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		//// パイプラインセット
		//context->SetComputePipelineState(m_pGraphicsEngine->GetPipelineManager()->GetSkinningPSO().pso.Get());
		//// ルートシグネチャセット
		//context->SetComputeRootSignature(m_pGraphicsEngine->GetPipelineManager()->GetSkinningPSO().rootSignature.Get());
		//// パレットデータをセット
		//IStructuredBuffer* paletteBuffer = m_pResourceManager->GetBuffer<IStructuredBuffer>(animation.paletteBufferIndex);
		//context->SetComputeRootDescriptorTable(0, paletteBuffer->GetSRVGpuHandle());
		//// メッシュデータをセット
		//IVertexBuffer* vertexBuffer = m_pResourceManager->GetBuffer<IVertexBuffer>(model->meshes[0].vertexBufferIndex);
		//context->SetComputeRootDescriptorTable(1, vertexBuffer->GetSRVGpuHandle());
		//// 影響データをセット
		//IStructuredBuffer* influenceBuffer = m_pResourceManager->GetBuffer<IStructuredBuffer>(animation.influenceBufferIndex);
		//context->SetComputeRootDescriptorTable(2, influenceBuffer->GetSRVGpuHandle());
		//// スキニングデータをセット
		//context->SetComputeRootDescriptorTable(3, skinningBuffer->GetUAVGpuHandle());
		//// Skinning用情報
		//IConstantBuffer* infoBuffer = m_pResourceManager->GetBuffer<IConstantBuffer>(model->meshes[0].skinInfoBufferIndex);
		//context->SetComputeRootConstantBufferView(4, infoBuffer->GetResource()->GetGPUVirtualAddress());
		//// Dispatch
		//context->Dispatch(static_cast<UINT>(model->meshes[0].vertices.size() + 1023) / 1024, 1, 1);
		//// スキニングOutputリソースを元に戻す
		//context->BarrierTransition(skinningBuffer->GetResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		//// クローズ
		//m_pGraphicsEngine->EndCommandContext(context, QueueType::Compute);
		//// GPUの処理が終わるまで待機
		//m_pGraphicsEngine->WaitForGPU(QueueType::Compute);
	}
	ResourceManager* m_pResourceManager = nullptr;
	GraphicsEngine* m_pGraphicsEngine = nullptr;
};

class EffectEditorSystem : public ECSManager::System<EffectComponent>
{
public:
	EffectEditorSystem() :
		ECSManager::System<EffectComponent>(
			[&](Entity e, EffectComponent& effect)
			{
				UpdateComponent(e, effect);
			},
			[&](Entity e, EffectComponent& effect)
			{
				InitializeComponent(e, effect);
			},
			[&](Entity e, EffectComponent& effect)
			{
				e;
				FinalizeComponent(e, effect);
			})
	{
	}
	~EffectEditorSystem() = default;
	void Update() override
	{
		// いつもの処理
		ECSManager::System<EffectComponent>::Update();
		// エフェクトの初期化
		InitEffectParticle();
		// シェーダーの更新
		UpdateShader();
	}
private:
	void InitializeComponent([[maybe_unused]] Entity e, EffectComponent& effect)
	{
		
	}
	void UpdateComponent([[maybe_unused]] Entity e, EffectComponent& effect)
	{
		// EditorはTimeBaseでの更新

	// エフェクトの時間を更新
		effect.root.second.time.deltaTime = DeltaTime();
		if (effect.isRun)
		{
			effect.root.second.time.elapsedTime++;
		}
		if (effect.root.second.time.elapsedTime > effect.root.second.time.duration)
		{
			if (effect.isLoop)
			{
				// ループする場合は時間をリセット
				effect.root.second.time.elapsedTime = 0.0f;
			}
			else
			{
				// ループしない場合は停止
				effect.isRun = false;
				return;
			}
		}

		// バッファに転送
		StructuredBuffer<EffectRoot>* rootBuffer = dynamic_cast<StructuredBuffer<EffectRoot>*>(m_pEngineCommand->m_ResourceManager->GetIntegrationBuffer(IntegrationDataType::EffectRootInt));
		StructuredBuffer<EffectNode>* nodeBuffer = dynamic_cast<StructuredBuffer<EffectNode>*>(m_pEngineCommand->m_ResourceManager->GetIntegrationBuffer(IntegrationDataType::EffectNodeInt));
		StructuredBuffer<EffectSprite>* spriteBuffer = dynamic_cast<StructuredBuffer<EffectSprite>*>(m_pEngineCommand->m_ResourceManager->GetIntegrationBuffer(IntegrationDataType::EffectSpriteInt));

		// EffectRootを更新
		EffectRoot root;
		root.globalSeed = effect.root.second.globalSeed;
		root.nodeCount = effect.root.second.nodeCount;
		root.time = effect.root.second.time;
		uint32_t i = 0;
		for (const auto& node : effect.root.second.nodes)
		{
			root.nodeID[i] = node.nodeID;
			i++;
			// Nodeを更新
			EffectNode nodeData;
			nodeData.common = node.common;
			nodeData.position = node.position;
			nodeData.rotation = node.rotation;
			nodeData.scale = node.scale;
			nodeData.drawCommon = node.drawCommon;
			nodeData.draw = node.draw;
			nodeData.isRootParent = node.isRootParent;
			nodeData.parentIndex = node.parentIndex;
			nodeBuffer->UpdateData(nodeData, node.nodeID);
			EFFECT_MESH_TYPE meshType = static_cast<EFFECT_MESH_TYPE>(node.draw.meshType);
			switch (meshType)
			{
			case EFFECT_MESH_TYPE::NONE:
				break;
			case EFFECT_MESH_TYPE::SPRITE: {
				// スプライトの場合はスプライトバッファに転送
				EffectSprite spriteData;
				const EffectSprite* sprite = std::get_if<EffectSprite>(&node.drawMesh);
				spriteData = *sprite;
				spriteBuffer->UpdateData(spriteData, node.draw.meshDataIndex);
				break;
			}
			case EFFECT_MESH_TYPE::RIBBON:
				break;
			case EFFECT_MESH_TYPE::TRAIL:
				break;
			case EFFECT_MESH_TYPE::RING:
				break;
			case EFFECT_MESH_TYPE::MODEL:
				break;
			case EFFECT_MESH_TYPE::CUBE:
				break;
			case EFFECT_MESH_TYPE::SPHERE:
				break;
			default:
				break;
			}
		}
		rootBuffer->UpdateData(root, effect.root.first);
	}
	void FinalizeComponent([[maybe_unused]] Entity e, EffectComponent& effect)
	{
		
	}

	void UpdateEffect(EffectComponent& effect);
	void InitEffectParticle()
	{
		// ParticleBuffer
		IRWStructuredBuffer* particleBuffer = m_pEngineCommand->GetResourceManager()->GetEffectParticleBuffer();
		// ParticleListBuffer
		IRWStructuredBuffer* particleListBuffer = m_pEngineCommand->GetResourceManager()->GetEffectParticleFreeListBuffer();
		// コマンドリスト開始
		CommandContext* context = m_pEngineCommand->GetGraphicsEngine()->GetCommandContext();
		m_pEngineCommand->GetGraphicsEngine()->BeginCommandContext(context);
		// パイプラインセット
		context->SetComputePipelineState(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectInitPSO().pso.Get());
		// ルートシグネチャセット
		context->SetComputeRootSignature(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectInitPSO().rootSignature.Get());
		// Particleバッファをセット
		context->SetComputeRootDescriptorTable(0, particleBuffer->GetUAVGpuHandle());
		// ParticleListバッファをセット
		context->SetComputeRootDescriptorTable(1, particleListBuffer->GetUAVGpuHandle());
		// ListCounterバッファをセット
		context->SetComputeRootUnorderedAccessView(2, particleListBuffer->GetCounterResource()->GetGPUVirtualAddress());
		// Dispatch
		context->Dispatch(128, 1, 1);
		// 並列阻止
		context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleBuffer->GetResource());
		context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleListBuffer->GetResource());
		// クローズ
		m_pEngineCommand->GetGraphicsEngine()->EndCommandContext(context, QueueType::Compute);
	}
	void UpdateShader()
	{
		if (!m_pEngineCommand->m_ResourceManager->GetEffectRootUseListCount()) { return; }

		// バッファを取得
		IStructuredBuffer* useRootListBuffer = m_pEngineCommand->GetResourceManager()->GetEffectRootUseListBuffer();
		IStructuredBuffer* rootBuffer = m_pEngineCommand->GetResourceManager()->GetIntegrationBuffer(IntegrationDataType::EffectRootInt);
		IStructuredBuffer* nodeBuffer = m_pEngineCommand->GetResourceManager()->GetIntegrationBuffer(IntegrationDataType::EffectNodeInt);
		IStructuredBuffer* spriteBuffer = m_pEngineCommand->GetResourceManager()->GetIntegrationBuffer(IntegrationDataType::EffectSpriteInt);
		IRWStructuredBuffer* particleBuffer = m_pEngineCommand->GetResourceManager()->GetEffectParticleBuffer();
		IRWStructuredBuffer* particleListBuffer = m_pEngineCommand->GetResourceManager()->GetEffectParticleFreeListBuffer();

		// コマンドリスト取得
		CommandContext* context = m_pEngineCommand->GetGraphicsEngine()->GetCommandContext();
		// コマンドリスト開始
		m_pEngineCommand->GetGraphicsEngine()->BeginCommandContext(context);

		// 発生
		// パイプラインセット
		context->SetComputePipelineState(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectTimeBaseEmitPSO().pso.Get());
		// ルートシグネチャセット
		context->SetComputeRootSignature(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectTimeBaseEmitPSO().rootSignature.Get());
		// UseListをセット
		context->SetComputeRootDescriptorTable(0, useRootListBuffer->GetSRVGpuHandle());
		// Rootをセット
		context->SetComputeRootDescriptorTable(1, rootBuffer->GetSRVGpuHandle());
		// ノードバッファをセット
		context->SetComputeRootDescriptorTable(2, nodeBuffer->GetSRVGpuHandle());
		// スプライトバッファをセット
		context->SetComputeRootDescriptorTable(3, spriteBuffer->GetSRVGpuHandle());
		// Particleバッファをセット
		context->SetComputeRootDescriptorTable(4, particleBuffer->GetUAVGpuHandle());
		// ParticleListバッファをセット
		context->SetComputeRootDescriptorTable(5, particleListBuffer->GetUAVGpuHandle());
		// ListCounterバッファをセット
		context->SetComputeRootUnorderedAccessView(6, particleListBuffer->GetCounterResource()->GetGPUVirtualAddress());
		// Dispatch
		context->Dispatch(static_cast<UINT>(m_pEngineCommand->m_ResourceManager->GetEffectRootUseListCount()), 1, 1);

		// 並列阻止
		context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleBuffer->GetResource());
		context->BarrierUAV(D3D12_RESOURCE_BARRIER_TYPE_UAV, D3D12_RESOURCE_BARRIER_FLAG_NONE, particleListBuffer->GetResource());

		// 更新
		// パイプラインセット
		context->SetComputePipelineState(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectTimeBaseUpdatePSO().pso.Get());
		// ルートシグネチャセット
		context->SetComputeRootSignature(m_pEngineCommand->GetGraphicsEngine()->GetPipelineManager()->GetEffectTimeBaseUpdatePSO().rootSignature.Get());
		// Rootをセット
		context->SetComputeRootDescriptorTable(0, rootBuffer->GetSRVGpuHandle());
		// ノードバッファをセット
		context->SetComputeRootDescriptorTable(1, nodeBuffer->GetSRVGpuHandle());
		// スプライトバッファをセット
		context->SetComputeRootDescriptorTable(2, spriteBuffer->GetSRVGpuHandle());
		// Particleバッファをセット
		context->SetComputeRootDescriptorTable(3, particleBuffer->GetUAVGpuHandle());
		// ParticleListバッファをセット
		context->SetComputeRootDescriptorTable(4, particleListBuffer->GetUAVGpuHandle());
		// Dispatch
		context->Dispatch(128, 1, 1);
		// クローズ
		m_pEngineCommand->GetGraphicsEngine()->EndCommandContext(context, QueueType::Compute);
	}

	EngineCommand* m_pEngineCommand = nullptr;
};