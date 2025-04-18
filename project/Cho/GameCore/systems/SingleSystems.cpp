#include "pch.h"
#include "SingleSystems.h"
#include "Resources/ResourceManager/ResourceManager.h"
#include "GameCore/IScript/IScript.h"
#include "Platform/FileSystem/FileSystem.h"
#include "Core/ChoLog/ChoLog.h"
using namespace Cho;
// 優先度順に更新する
void TransformUpdateSystem::priorityUpdate(ECSManager* ecs)
{
	std::vector<std::pair<Entity, TransformComponent*>> transforms;

	// すべてのTransformComponentを取得
	for (auto& [arch, container] : ecs->GetArchToEntities())
	{
		if ((arch & m_Required) == m_Required)
		{
			for (Entity e : container.GetEntities())
			{
				if (auto* t = ecs->GetComponent<TransformComponent>(e))
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
			} else if (aParent && *aParent == b.first) return false;
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

void TransformUpdateSystem::UpdateComponent(Entity e, TransformComponent& transform)
{
	e;
	// 度数からラジアンに変換
	Vector3 radians = ChoMath::DegreesToRadians(transform.degrees);

	//// 変更がなければreturn
	//if (transform.translation == transform.prePos &&
	//	radians == transform.preRot &&
	//	transform.scale == transform.preScale)
	//{
	//	if (transform.parent && *transform.parent == transform.preParent)
	//	{
	//		return;
	//	}
	//}

	// 差分計算
	Vector3 diff = radians - transform.preRot;

	// 各軸のクオータニオンを作成
	Quaternion qx = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(1.0f, 0.0f, 0.0f), diff.x);
	Quaternion qy = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 1.0f, 0.0f), diff.y);
	Quaternion qz = ChoMath::MakeRotateAxisAngleQuaternion(Vector3(0.0f, 0.0f, 1.0f), diff.z);

	// 同時回転を累積
	transform.rotation = transform.rotation * qx * qy * qz;//*transform.rotation;

	// 精度を維持するための正規化
	transform.rotation.Normalize();

	// アフィン変換
	transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

	// 次のフレーム用に保存する
	transform.prePos = transform.translation;
	transform.preRot = radians;
	transform.preScale = transform.scale;
	/*if (transform.parent)
	{
		transform.preParent = *transform.parent;
	}*/

	// 親があれば親のワールド行列を掛ける
	if (transform.parent)
	{
		transform.matWorld = ChoMath::Multiply(transform.matWorld, m_pECS->GetComponent<TransformComponent>(transform.parent.value())->matWorld);
	}

	// 行列の転送
	TransferMatrix(transform);
}

void TransformUpdateSystem::TransferMatrix(TransformComponent& transform)
{
	// マップデータ更新
	BUFFER_DATA_TF data = {};
	data.matWorld = transform.matWorld;
	data.worldInverse = ChoMath::Transpose(Matrix4::Inverse(transform.matWorld));
	data.rootMatrix = transform.rootMatrix;
	m_pIntegrationBuffer->UpdateData(data, transform.mapID.value());
}

void TransformInitializeSystem::Start(TransformComponent& transform)
{
	// 初期値保存
	transform.startValue.translation = transform.translation;
	transform.startValue.rotation = transform.rotation;
	transform.startValue.scale = transform.scale;
	transform.startValue.degrees = transform.degrees;
}

void TransformFinalizeSystem::Finalize(Entity entity,TransformComponent& transform)
{
	entity;
	// 初期値に戻す
	transform.translation = transform.startValue.translation;
	transform.rotation = transform.startValue.rotation;
	transform.scale = transform.startValue.scale;
	transform.degrees = transform.startValue.degrees;
}

void CameraUpdateSystem::UpdateMatrix(TransformComponent& transform, CameraComponent& camera)
{ 
	// アフィン変換
	transform.matWorld = ChoMath::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);

	TransferMatrix(transform, camera);
}

void CameraUpdateSystem::TransferMatrix(TransformComponent& transform, CameraComponent& camera)
{
	BUFFER_DATA_VIEWPROJECTION data = {};
	data.matWorld = transform.matWorld;
	data.view = Matrix4::Inverse(transform.matWorld);
	data.projection = ChoMath::MakePerspectiveFovMatrix(camera.fovAngleY, camera.aspectRatio, camera.nearZ, camera.farZ);
	data.projectionInverse = Matrix4::Inverse(data.projection);
	data.cameraPosition = transform.translation;
	ConstantBuffer<BUFFER_DATA_VIEWPROJECTION>* buffer = dynamic_cast<ConstantBuffer<BUFFER_DATA_VIEWPROJECTION>*>(m_pResourceManager->GetBuffer<IConstantBuffer>(camera.bufferIndex));
	buffer->UpdateData(data);
}

void ScriptInitializeSystem::LoadScript(ScriptComponent& script)
{
	if (script.scriptName.empty())
	{
		script.isActive = false;
		return;
	}
	std::string funcName = "Create" + script.scriptName + "Script";
	funcName.erase(std::remove_if(funcName.begin(), funcName.end(), ::isspace), funcName.end());
	// CreateScript関数を取得
	typedef IScript* (*CreateScriptFunc)();
	CreateScriptFunc createScript = (CreateScriptFunc)GetProcAddress(FileSystem::ScriptProject::m_DllHandle, funcName.c_str());
	if (!createScript)
	{
		script.isActive = false;
		return;
	}
	// スクリプトを生成
	IScript* scriptInstance = createScript();
	if (!scriptInstance)
	{
		script.isActive = false;
		return;
	}
	// スクリプトのStart関数とUpdate関数をラップ
	script.startFunc = [scriptInstance](ScriptContext& ctx) {
		scriptInstance->Start(ctx);
		};
	script.updateFunc = [scriptInstance](ScriptContext& ctx) {
		scriptInstance->Update(ctx);
		};
	// インスタンスの解放用のクロージャを設定
	script.cleanupFunc = [scriptInstance, this]() {
		delete scriptInstance;
		};
	// 衝突関数をラップ
	script.onCollisionEnterFunc = [scriptInstance](ScriptContext& ctx,ScriptContext& other) {
		scriptInstance->OnCollisionEnter(ctx,other);
		};
	script.onCollisionStayFunc = [scriptInstance](ScriptContext& ctx, ScriptContext& other) {
		scriptInstance->OnCollisionStay(ctx, other);
		};
	script.onCollisionExitFunc = [scriptInstance](ScriptContext& ctx, ScriptContext& other) {
		scriptInstance->OnCollisionExit(ctx, other);
		};
	script.isActive = true;
}

void ScriptInitializeSystem::Start(ScriptComponent& script)
{
	LoadScript(script);
	StartScript(script);
}

void ScriptInitializeSystem::StartScript(ScriptComponent& script)
{
	if (!script.isActive) return;
	// スクリプトコンテキストを作成
	ScriptContext ctx = MakeScriptContext(script.entity.value());
	try
	{
		// スクリプトのStart関数を呼び出す
		script.startFunc(ctx);
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

ScriptContext ScriptInitializeSystem::MakeScriptContext(Entity entity)
{
	ScriptContext ctx(m_pObjectContainer,m_pInputManager,m_pResourceManager, m_ECS, entity);
	ctx.Initialize();
	return ctx;
}

void ScriptUpdateSystem::UpdateScript(ScriptComponent& script)
{
	if (!script.isActive) return;
	// スクリプトコンテキストを作成
	ScriptContext ctx = MakeScriptContext(script.entity.value());
	
	try
	{
		// スクリプトのUpdate関数を呼び出す
		script.updateFunc(ctx);
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

ScriptContext ScriptUpdateSystem::MakeScriptContext(Entity entity)
{
	ScriptContext ctx(m_pObjectContainer,m_pInputManager,m_pResourceManager, m_ECS, entity);
	ctx.Initialize();
	return ctx;
}

void ScriptFinalizeSystem::FinalizeScript(ScriptComponent& script)
{
	if (!script.isActive) return;
	try
	{
		// スクリプトのCleanup関数を呼び出す
		script.cleanupFunc();
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

