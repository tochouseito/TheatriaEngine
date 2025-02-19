#include "PrecompiledHeader.h"
#include "ComponentManager.h"

#include"D3D12/ResourceViewManager/ResourceViewManager.h"

void ComponentManager::SetRVManager(ResourceViewManager* RVManager)
{
    rvManager_ = RVManager;
}

// Entityに関連するすべてのコンポーネントを削除します。
void ComponentManager::RemoveComponent(Entity entity) {
	// すべてのコンポーネントを削除
	{
		auto& components = GetOrCreateComponentMap<TransformComponent>();
		if (components.contains(entity)) {
			TransformComponent& component = GetComponent<TransformComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<RenderComponent>();
		if (components.contains(entity)) {
			RenderComponent& component = GetComponent<RenderComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<PhysicsComponent>();
		if (components.contains(entity)) {
			PhysicsComponent& component = GetComponent<PhysicsComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<MeshComponent>();
		if (components.contains(entity)) {
			MeshComponent& component = GetComponent<MeshComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<AnimationComponent>();
		if (components.contains(entity)) {
			AnimationComponent& component = GetComponent<AnimationComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<ColliderComponent>();
		if (components.contains(entity)) {
			ColliderComponent& component = GetComponent<ColliderComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<CameraComponent>();
		if (components.contains(entity)) {
			CameraComponent& component = GetComponent<CameraComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<MaterialComponent>();
		if (components.contains(entity)) {
			MaterialComponent& component = GetComponent<MaterialComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<ScriptComponent>();
		if (components.contains(entity)) {
			ScriptComponent& component = GetComponent<ScriptComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<SpriteComponent>();
		if (components.contains(entity)) {
			SpriteComponent& component = GetComponent<SpriteComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<ParticleComponent>();
		if (components.contains(entity)) {
			ParticleComponent& component = GetComponent<ParticleComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<EmitterComponent>();
		if (components.contains(entity)) {
			EmitterComponent& component = GetComponent<EmitterComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<MapChipBlockComponent>();
		if (components.contains(entity)) {
			MapChipBlockComponent& component = GetComponent<MapChipBlockComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<EffectComponent>();
		if (components.contains(entity)) {
			EffectComponent& component = GetComponent<EffectComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<DirectionalLightComponent>();
		if (components.contains(entity)) {
			DirectionalLightComponent& component = GetComponent<DirectionalLightComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<PointLightComponent>();
		if (components.contains(entity)) {
			PointLightComponent& component = GetComponent<PointLightComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	{
		auto& components = GetOrCreateComponentMap<SpotLightComponent>();
		if (components.contains(entity)) {
			SpotLightComponent& component = GetComponent<SpotLightComponent>(entity).value();
			// 特殊処理
			RemoveSpecialProcess(entity, component);
			components.erase(entity);
		}
	}
	//for (auto& [typeIndex, componentData] : componentMap) {
	//	auto components = std::static_pointer_cast<std::unordered_map<Entity, std::shared_ptr<void>>>(componentData);
	//	auto it = components->find(entity);
	//	// コンポーネントを持っていない場合はスキップ
	//	if (it == components->end() || !it->second) {
	//		continue;
	//	}
	//	if (!it->second) {
	//		std::cerr << "Error: Invalid shared_ptr in component map" << std::endl;
	//		continue;
	//	}
	//	// すでに `use_count() == 0` なら削除をスキップ
	//	if (it->second.use_count() == 0) {
	//		std::cerr << "Warning: Attempting to access expired shared_ptr" << std::endl;
	//		continue;
	//	}
	//	// 削除前の特殊処理を実行
	//	if (typeIndex == typeid(TransformComponent)) {
	//		if (auto ptr = std::static_pointer_cast<TransformComponent>(it->second)) {
	//			RemoveSpecialProcess(entity, *std::static_pointer_cast<TransformComponent>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else if (typeIndex == typeid(CameraComponent)) {
	//		if (auto ptr = std::static_pointer_cast<CameraComponent>(it->second)) {
	//			RemoveSpecialProcess(entity, *std::static_pointer_cast<CameraComponent>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else if (typeIndex == typeid(MaterialComponent)) {
	//		if (auto ptr = std::static_pointer_cast<MaterialComponent>(it->second)) {
	//			RemoveSpecialProcess(entity, *std::static_pointer_cast<MaterialComponent>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else if (typeIndex == typeid(SpriteComponent)) {
	//		if (auto ptr = std::static_pointer_cast<SpriteComponent>(it->second)) {
	//			RemoveSpecialProcess(entity, *std::static_pointer_cast<SpriteComponent>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else if (typeIndex == typeid(ParticleComponent)) {
	//		if (auto ptr = std::static_pointer_cast<ParticleComponent>(it->second)) {
	//			RemoveSpecialProcess(entity, *std::static_pointer_cast<ParticleComponent>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else if (typeIndex == typeid(EmitterComponent)) {
	//		if (auto ptr = std::static_pointer_cast<EmitterComponent>(it->second)) {
	//			RemoveSpecialProcess(entity, *std::static_pointer_cast<EmitterComponent>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else if (typeIndex == typeid(EffectComponent)) {
	//		if (auto ptr = std::static_pointer_cast<EffectComponent>(it->second)) {
	//			RemoveSpecialProcess(entity, *std::static_pointer_cast<EffectComponent>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else if (typeIndex == typeid(DirectionalLightComponent)) {
	//		if (auto ptr = std::static_pointer_cast<DirectionalLightComponent>(it->second)) {
	//			RemoveSpecialProcess(entity, *std::static_pointer_cast<DirectionalLightComponent>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else if (typeIndex == typeid(PointLightComponent)) {
	//		if (auto ptr = std::static_pointer_cast<PointLightComponent>(it->second)) {
	//			RemoveSpecialProcess(entity, *std::static_pointer_cast<PointLightComponent>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else if (typeIndex == typeid(SpotLightComponent)) {
	//		if (auto ptr = std::static_pointer_cast<SpotLightComponent>(it->second)) {
	//			RemoveSpecialProcess(entity, *std::static_pointer_cast<SpotLightComponent>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	else {
	//		if (auto ptr = std::static_pointer_cast<void>(it->second)) {
	//			//RemoveSpecialProcess(entity, *std::static_pointer_cast<void>(it->second));
	//		}
	//		else {
	//			continue;
	//		}
	//	}
	//	// コンポーネント削除
	//	components->erase(it);
	//}
}

void ComponentManager::RemoveSpecialProcess(const Entity& entity, const TransformComponent& component)
{
	component;
	TransformComponent& transform = GetComponent<TransformComponent>(entity).value();
	if (transform.isInstance) {
		rvManager_->SRVRelease(transform.srvIndex);
	}
	else {
		// CBV解放
		rvManager_->CBVRelease(transform.cbvIndex);
	}
}

void ComponentManager::RemoveSpecialProcess(const Entity& entity, const CameraComponent& component)
{
	component;
	CameraComponent& camera = GetComponent<CameraComponent>(entity).value();
	rvManager_->CBVRelease(camera.cbvIndex);
}

void ComponentManager::RemoveSpecialProcess(const Entity& entity, const MaterialComponent& component)
{
	component;
	MaterialComponent& material = GetComponent<MaterialComponent>(entity).value();
	rvManager_->CBVRelease(material.cbvIndex);
}

void ComponentManager::RemoveSpecialProcess(const Entity& entity, const SpriteComponent& component)
{
	component;
	SpriteComponent& sprite = GetComponent<SpriteComponent>(entity).value();
	rvManager_->CBVRelease(sprite.cbvIndex);
	rvManager_->CBVRelease(sprite.material.cbvIndex);
	// スプライトデータ解放
	////////////
	/*ここに処理を追加*/
	////////////
}

void ComponentManager::RemoveSpecialProcess(const Entity& entity, const ParticleComponent& component)
{
	component;
	ParticleComponent& particle = GetComponent<ParticleComponent>(entity).value();

	// FreeList解放
	rvManager_->UAVRelease(particle.freeList.uavIndex);

	// FreeListIndex解放
	rvManager_->UAVRelease(particle.freeListIndex.uavIndex);

	// Counter解放
	rvManager_->UAVRelease(particle.counter.uavIndex);

	// PerFrame解放
	rvManager_->CBVRelease(particle.perFrame.cbvIndex);

	// UAV解放
	rvManager_->UAVRelease(particle.uavIndex);

	// マテリアル解放
	rvManager_->CBVRelease(particle.material.cbvIndex);
}

void ComponentManager::RemoveSpecialProcess(const Entity& entity, const EmitterComponent& component)
{
	component;
	EmitterComponent& emitter = GetComponent<EmitterComponent>(entity).value();
	rvManager_->CBVRelease(emitter.cbvIndex);
}

void ComponentManager::RemoveSpecialProcess(const Entity& entity, const EffectComponent& component)
{
	component;
	EffectComponent& effect = GetComponent<EffectComponent>(entity).value();
	// EffectNodeResource
	for (auto& effectNode : effect.effectNodes) {
		rvManager_->UAVRelease(effectNode.particle.particleUAVIndex);
		rvManager_->UAVRelease(effectNode.particle.freeListIndex.uavIndex);
		rvManager_->UAVRelease(effectNode.particle.freeList.uavIndex);
		rvManager_->SRVRelease(effectNode.srvIndex);
	}
	effect.effectNodes.clear();
	rvManager_->CBVRelease(effect.material.cbvIndex);
	rvManager_->CBVRelease(effect.timeManagerCBVIndex);
}

void ComponentManager::RemoveSpecialProcess(const Entity& entity, const DirectionalLightComponent& component)
{
	component;
	DirectionalLightComponent& light = GetComponent<DirectionalLightComponent>(entity).value();
	// 解放
	rvManager_->DirLightRelease(light.index);
}

void ComponentManager::RemoveSpecialProcess(const Entity& entity, const PointLightComponent& component)
{
	component;
	PointLightComponent& light = GetComponent<PointLightComponent>(entity).value();
	// 解放
	rvManager_->PointLightRelease(light.index);
}

void ComponentManager::RemoveSpecialProcess(const Entity& entity, const SpotLightComponent& component)
{
	component;
	SpotLightComponent& light = GetComponent<SpotLightComponent>(entity).value();
	// 解放
	rvManager_->SpotLightRelease(light.index);
}

void ComponentManager::InitCameraComponent(CameraComponent* camera)
{
    camera->cbvIndex=rvManager_->CreateCBV(sizeof(ConstBufferDataViewProjection));
    rvManager_->GetCBVResource(
        camera->cbvIndex)->Map(
            0, nullptr, reinterpret_cast<void**>(&camera->constData)
        );
}

void ComponentManager::SpecialProcess(const Entity& entity, const TransformComponent& component)
{
	component;
	TransformComponent& transform = GetComponent<TransformComponent>(entity).value();
	if (transform.isInstance) {
		// インスタンシング描画用
		transform.srvIndex = rvManager_->GetNewHandle();
		rvManager_->CreateSRVResource(
			transform.srvIndex,
			sizeof(ConstBufferDataWorldTransform) * transform.instanceCount
		);
		rvManager_->CreateSRVforStructuredBuffer(
			transform.srvIndex,
			transform.instanceCount,
			sizeof(ConstBufferDataWorldTransform)
		);
		rvManager_->GetHandle(transform.srvIndex).resource->Map(
			0, nullptr, reinterpret_cast<void**>(&transform.constData)
		);
		// 初期化
		transform.transforms.resize(transform.instanceCount);
		for (uint32_t i = 0; i < transform.instanceCount; i++) {
			transform.constData[i].matWorld = ChoMath::MakeIdentity4x4();
			transform.constData[i].worldInverse = Transpose(Matrix4::Inverse(ChoMath::MakeIdentity4x4()));
			transform.constData[i].rootNode = ChoMath::MakeIdentity4x4();
		}
	}
	else {
		transform.cbvIndex = rvManager_->CreateCBV(sizeof(ConstBufferDataWorldTransform));
		rvManager_->GetCBVResource(
			transform.cbvIndex)->Map(
				0, nullptr, reinterpret_cast<void**>(&transform.constData)
			);
		// 初期化
		transform.constData->matWorld = transform.matWorld;
		transform.constData->worldInverse = Transpose(Matrix4::Inverse(transform.matWorld));
		transform.constData->rootNode = transform.rootMatrix;
	}
}

void ComponentManager::SpecialProcess(const Entity& entity, const CameraComponent& component)
{
	component;
	CameraComponent& camera = GetComponent<CameraComponent>(entity).value();
	camera.cbvIndex = rvManager_->CreateCBV(sizeof(ConstBufferDataViewProjection));
	rvManager_->GetCBVResource(
		camera.cbvIndex)->Map(
			0, nullptr, reinterpret_cast<void**>(&camera.constData)
		);
	// 初期化
	camera.constData->matWorld = camera.matWorld;
	camera.constData->view = Matrix4::Inverse(camera.matWorld);
	camera.constData->projection = MakePerspectiveFovMatrix(
		camera.fovAngleY, camera.aspectRatio, camera.nearZ, camera.farZ
	);
	camera.constData->cameraPosition = camera.translation;
}

void ComponentManager::SpecialProcess(const Entity& entity, const MaterialComponent& component)
{
	component;
	MaterialComponent& material = GetComponent<MaterialComponent>(entity).value();
    std::string texName = "uvChecker.png";
    material.textureID = texName;
    material.preTexID = texName;
	material.cbvIndex = rvManager_->CreateCBV(sizeof(ConstBufferDataMaterial));
	rvManager_->GetCBVResource(
		material.cbvIndex)->Map(
			0, nullptr, reinterpret_cast<void**>(&material.constData)
		);
	// 初期化
	material.constData->matUV = ChoMath::MakeIdentity4x4();
	material.constData->color = { 1.0f,1.0f,1.0f,1.0f };
	material.constData->enableLighting = false;
	material.constData->shininess = 50.0f;
}

void ComponentManager::SpecialProcess(const Entity& entity, const SpriteComponent& component)
{
	component;
	SpriteComponent& sprite = GetComponent<SpriteComponent>(entity).value();
	sprite.cbvIndex = rvManager_->CreateCBV(sizeof(ConstBufferDataSprite));
	rvManager_->GetCBVResource(
		sprite.cbvIndex)->Map(
			0, nullptr, reinterpret_cast<void**>(&sprite.constData)
		);
	sprite.spriteIndex = rvManager_->CreateSpriteData();
	sprite.material.cbvIndex = rvManager_->CreateCBV(sizeof(ConstBufferDataMaterial));
	rvManager_->GetCBVResource(
		sprite.material.cbvIndex)->Map(
			0, nullptr, reinterpret_cast<void**>(&sprite.material.constData)
		);
	// 初期化
	sprite.constData->matWorld = sprite.matWorld;
}

void ComponentManager::SpecialProcess(const Entity& entity, const ParticleComponent& component)
{
	component;
	ParticleComponent& particle = GetComponent<ParticleComponent>(entity).value();
	particle.uavIndex = rvManager_->GetNewHandle();

	// マテリアル作成
	particle.material.cbvIndex = rvManager_->CreateCBV(sizeof(ConstBufferDataMaterial));
	rvManager_->GetCBVResource(
		particle.material.cbvIndex)->Map(
			0, nullptr, reinterpret_cast<void**>(&particle.material.constData)
		);

	// UAV作成
	rvManager_->CreateUAVResource(particle.uavIndex, sizeof(ConstBufferDataParticle) * kMaxParticle);
	rvManager_->CreateUAVforStructuredBuffer(
		particle.uavIndex,
		kMaxParticle,
		sizeof(ConstBufferDataParticle)
	);

	// PerFrame
	particle.perFrame.cbvIndex = rvManager_->CreateCBV(sizeof(ConstBufferDataPerFrame));
	rvManager_->GetCBVResource(particle.perFrame.cbvIndex)->Map(
		0, nullptr, reinterpret_cast<void**>(&particle.perFrame.constData)
	);

	// Counter,UAV作成
	particle.counter.uavIndex = rvManager_->GetNewHandle();
	rvManager_->CreateUAVResource(particle.counter.uavIndex, sizeof(ConstBufferDataCounter));
	rvManager_->CreateUAVforStructuredBuffer(
		particle.counter.uavIndex,
		1,
		sizeof(ConstBufferDataCounter)
	);

	// FreeList,UAV作成
	particle.freeListIndex.uavIndex = rvManager_->GetNewHandle();
	rvManager_->CreateUAVResource(particle.freeListIndex.uavIndex, sizeof(ConstBufferDataFreeListIndex));
	rvManager_->CreateUAVforStructuredBuffer(
		particle.freeListIndex.uavIndex,
		1,
		sizeof(ConstBufferDataFreeListIndex)
	);
	particle.freeList.uavIndex = rvManager_->GetNewHandle();
	rvManager_->CreateUAVResource(particle.freeList.uavIndex, sizeof(ConstBufferDataFreeList) * kMaxParticle);
	rvManager_->CreateUAVforStructuredBuffer(
		particle.freeList.uavIndex,
		kMaxParticle,
		sizeof(ConstBufferDataFreeList)
	);

	// 初期化

}

void ComponentManager::SpecialProcess(const Entity& entity, const EmitterComponent& component)
{
	component;
	EmitterComponent& emitter = GetComponent<EmitterComponent>(entity).value();
	emitter.cbvIndex = rvManager_->CreateCBV(sizeof(ConstBufferDataEmitter));
	rvManager_->GetCBVResource(
		emitter.cbvIndex)->Map(
			0, nullptr, reinterpret_cast<void**>(&emitter.constData)
		);
	// 初期化
}

void ComponentManager::SpecialProcess(const Entity& entity, const EffectComponent& component)
{
	component;
	EffectComponent& effect = GetComponent<EffectComponent>(entity).value();
	effect.render.visible = false;

	// TimeManager
	effect.timeManagerCBVIndex = rvManager_->CreateCBV(sizeof(TimeManager));
	rvManager_->GetCBVResource(
		effect.timeManagerCBVIndex)->Map(
			0, nullptr, reinterpret_cast<void**>(&effect.timeManager)
		);

	// 初期化
	effect.timeManager->globalTime = 0.0f;
	effect.timeManager->maxTime = 120.0f;
}

void ComponentManager::SpecialProcess(const Entity& entity, const DirectionalLightComponent& component)
{
	component;
	DirectionalLightComponent& light = GetComponent<DirectionalLightComponent>(entity).value();
	if (rvManager_->GetLightCBVIndex() == 0) {
		assert(0 && "None LightResource");
	}
	PunctualLightData* lightData = rvManager_->GetLightConstData();
	// 初期化
	light.index = rvManager_->DirLightAllocate();// ライトリソースのアロケータ
	light.color = { 1.0f,1.0f,1.0f };
	light.intensity = 1.0f;
	light.direction = { 0.0f,-1.0f,0.0f };
	lightData->dirLights[light.index].active = 1;
	lightData->dirLights[light.index].color = light.color;
	lightData->dirLights[light.index].intensity = light.intensity;
	lightData->dirLights[light.index].direction = light.direction;
}

void ComponentManager::SpecialProcess(const Entity& entity, const PointLightComponent& component)
{
	component;
	PointLightComponent& light = GetComponent<PointLightComponent>(entity).value();
	if (rvManager_->GetLightCBVIndex() == 0) {
		assert(0 && "None LightResource");
	}
	PunctualLightData* lightData = rvManager_->GetLightConstData();
	// 初期化
	light.index = rvManager_->PointLightAllocate();// ライトリソースのアロケータ
	light.color = { 1.0f,1.0f,1.0f };
	light.intensity = 1.0f;
	light.position = { 0.0f,0.0f,0.0f };
	light.radius = 10.0f;
	light.decay = 1.0f;
	lightData->pointLights[light.index].active = 1;
	lightData->pointLights[light.index].color = light.color;
	lightData->pointLights[light.index].intensity = light.intensity;
	lightData->pointLights[light.index].position = light.position;
	lightData->pointLights[light.index].radius = light.radius;
	lightData->pointLights[light.index].decay = light.decay;
}

void ComponentManager::SpecialProcess(const Entity& entity, const SpotLightComponent& component)
{
	component;
	SpotLightComponent& light = GetComponent<SpotLightComponent>(entity).value();
	if (rvManager_->GetLightCBVIndex() == 0) {
		assert(0 && "None LightResource");
	}
	PunctualLightData* lightData = rvManager_->GetLightConstData();
	// 初期化
	light.index = rvManager_->SpotLightAllocate();// ライトリソースのアロケータ
	light.color = { 1.0f,1.0f,1.0f };
	light.intensity = 1.0f;
	light.position = { 0.0f,0.0f,0.0f };
	light.direction = { 0.0f,-1.0f,0.0f };
	light.distance = 10.0f;
	light.cosAngle = 0.0f;
	light.cosFalloffStart = 0.0f;
	light.decay = 1.0f;
	lightData->spotLights[light.index].active = 1;
	lightData->spotLights[light.index].color = light.color;
	lightData->spotLights[light.index].intensity = light.intensity;
	lightData->spotLights[light.index].direction = light.direction;
	lightData->spotLights[light.index].distance = light.distance;
	lightData->spotLights[light.index].position = light.position;
	lightData->spotLights[light.index].decay = light.decay;
	lightData->spotLights[light.index].cosAngle = light.cosAngle;
	lightData->spotLights[light.index].cosFalloffStart = light.cosFalloffStart;
}


