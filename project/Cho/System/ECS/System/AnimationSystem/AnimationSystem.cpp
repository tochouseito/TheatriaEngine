#include "PrecompiledHeader.h"
#include "AnimationSystem.h"
#include"D3D12/D3DCommand/D3DCommand.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"Graphics/GraphicsSystem/GraphicsSystem.h"
#include"SystemState/SystemState.h"

#include"MeshPattern/MeshPattern.h"

void AnimationSystem::Initialize(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<AnimationComponent>> animationComp = componentManager.GetComponent<AnimationComponent>(entity);
		if (animationComp) {
			AnimationComponent& animation = animationComp.value();
			animation;
		}
	}
}

void AnimationSystem::Update(EntityManager& entityManager, ComponentManager& componentManager)
{
	for (Entity entity : entityManager.GetActiveEntities()) {
		std::optional<std::reference_wrapper<AnimationComponent>> animationComp = componentManager.GetComponent<AnimationComponent>(entity);
		if (!animationComp) { continue; }
		AnimationComponent& animation = animationComp.value();
		std::optional<std::reference_wrapper<MeshComponent>> meshComp = componentManager.GetComponent<MeshComponent>(entity);
		if (!meshComp) { continue; }
		MeshComponent& mesh = meshComp.value();
		ModelData* model = nullptr;
		// 頂点データ取得キー
		uint32_t meshesIndex;
		if (rvManager_->GetModelData(mesh.meshesName)) {
			meshesIndex = rvManager_->GetModelData(mesh.meshesName)->meshIndex;
		}
		else {
			meshesIndex = mesh.meshID;
		}
		if (rvManager_->GetMesh(meshesIndex)->meshData[0].isAnimation) {
			model = rvManager_->GetModelData(rvManager_->GetMesh(meshesIndex)->meshesName);
			animation.numAnimation = static_cast<uint32_t>(model->animations.size());
		}
		if (!model) { continue; }
			timeUpdate(animation, model);
	}
	// スキニング後のリソース遷移
	ResourceTransition();
}

void AnimationSystem::SetPtrs(D3DCommand* d3dCommand, ResourceViewManager* rvManager, GraphicsSystem* graphicsSystem)
{
	d3dCommand_ = d3dCommand;
	rvManager_ = rvManager;
	graphicsSystem_ = graphicsSystem;
}

Vector3 AnimationSystem::CalculateValue(const std::vector<KeyframeVector3>& keyframes,const float& time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Vector3::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

Quaternion AnimationSystem::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes,const float& time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Quaternion::Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

Scale AnimationSystem::CalculateValue(const std::vector<KeyframeScale>& keyframes,const float& time)
{
	assert(!keyframes.empty());// キーがないものは返す値がわからないのでだめ
	if (keyframes.size() == 1 || time <= keyframes[0].time) {// キーが一つか、時刻がキーフレーム前なら最初の値とする
		return keyframes[0].value;
	}
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexの二つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Scale::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	// ここまで来た場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;
}

void AnimationSystem::timeUpdate(AnimationComponent& animation, ModelData* model)
{
		if (animation.isRun) {
			// animationIndexが変更された場合、遷移を開始する
			if (animation.prevAnimationIndex != animation.animationIndex) {
				animation.transitionIndex = animation.prevAnimationIndex;
				animation.transition = true;
				animation.transitionTime = 0.0f;
			}

			// 遷移が終了した場合、遷移フラグをfalseにする
			if (animation.lerpTime >= 1.0f) {
				animation.transition = false;
				animation.lerpTime = 0.0f;
				animation.time = animation.transitionTime;
				animation.transitionTime = 0.0f;
			}

			/*エンジンのデルタタイムにする*/
			static float deltaTime = 1.0f / 60.0f;

			// 遷移中の場合、遷移時間を更新
			if (animation.transition) {
				animation.transitionTime += deltaTime;//DeltaTime();
				animation.lerpTime = animation.transitionTime / animation.transitionDuration;
				animation.lerpTime = std::clamp(animation.lerpTime, 0.0f, 1.0f);
			}

			// 時間更新
			animation.isEnd = false;
			animation.time += deltaTime;//DeltaTime();
			if (animation.time >= model->animations[animation.animationIndex].duration) {
				animation.isEnd = true;
				if (!animation.isRestart) {
					animation.isRun = false;
				}
			}
			else {
				animation.isEnd = false;
			}
			if (!animation.isRestart) {
				animation.time;
			}
			else {
				animation.time = std::fmod(animation.time, model->animations[animation.animationIndex].duration);
			}
		}
	// アニメーション更新
	ApplyAnimation(animation, model);
	if (model->isBone) {
		SkeletonUpdate(animation, model);
		SkinClusterUpdate(animation, model);
		ApplySkinning(animation, model);
	}

	animation.prevAnimationIndex = animation.animationIndex;
	
}

void AnimationSystem::ApplyAnimation(AnimationComponent& animation, ModelData* model)
{
	for (Joint& joint : model->skeleton.joints) {
		// 対象のJointのAnimationがあれば、値の適用を行う。下記のif文はC++17から可能になった初期化付きif文
		if (auto it = model->animations[animation.animationIndex].nodeAnimations.find(joint.name); it != model->animations[animation.animationIndex].nodeAnimations.end()) {
			const NodeAnimation& rootNodeAnimation = (*it).second;
			if (animation.transition) {
				if(auto it2 = model->animations[animation.transitionIndex].nodeAnimations.find(joint.name); it2 != model->animations[animation.transitionIndex].nodeAnimations.end()) {
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
			else {
				joint.transform.translation = CalculateValue(rootNodeAnimation.translate.keyframes, animation.time);
				joint.transform.rotation = CalculateValue(rootNodeAnimation.rotate.keyframes, animation.time);
				joint.transform.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animation.time);
			}
		}
	}
}

void AnimationSystem::SkeletonUpdate(AnimationComponent& animation, ModelData* model)
{
	animation;
	// すべてのJointを更新。親が若いので通常ループで処理可能になっている
	for (Joint& joint : model->skeleton.joints) {
		joint.localMatrix = MakeAffineMatrix(joint.transform.scale, joint.transform.rotation, joint.transform.translation);
		if (joint.parent) {
			joint.skeletonSpaceMatrix = joint.localMatrix * model->skeleton.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else {// 親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void AnimationSystem::SkinClusterUpdate(AnimationComponent& animation, ModelData* model)
{
	animation;
	for (size_t jointIndex = 0; jointIndex < model->skeleton.joints.size(); ++jointIndex) {
		assert(jointIndex < model->skinCluster.inverseBindPoseMatrices.size());
		model->skinCluster.paletteData.map[jointIndex].skeletonSpaceMatrix =
			model->skinCluster.inverseBindPoseMatrices[jointIndex] * model->skeleton.joints[jointIndex].skeletonSpaceMatrix;
		model->skinCluster.paletteData.map[jointIndex].skeletonSpaceInverseTransposeMatrix =
			ChoMath::Transpose(Matrix4::Inverse(model->skinCluster.paletteData.map[jointIndex].skeletonSpaceMatrix)
			);
	}
}

void AnimationSystem::ApplySkinning(AnimationComponent& animation, ModelData* model)
{
	animation;
	// コマンドリストの取得
	ID3D12GraphicsCommandList6* commandList = d3dCommand_->GetCommand(CommandType::Compute).list.Get();

	commandList->SetComputeRootSignature(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::SkinningCS).rootSignature.Get());
	commandList->SetPipelineState(graphicsSystem_->GetPipeline()->GetPSO(PSOMode::SkinningCS).Blend[kBlendModeNone].Get());
	for (auto& meshData : rvManager_->GetMesh(model->meshIndex)->meshData) {
		commandList->SetComputeRootDescriptorTable(0, rvManager_->GetHandle(model->skinCluster.paletteData.srvIndex).GPUHandle);
		commandList->SetComputeRootDescriptorTable(1, rvManager_->GetHandle(meshData.srvVBVIndex).GPUHandle);
		commandList->SetComputeRootDescriptorTable(2, rvManager_->GetHandle(model->skinCluster.influenceData.srvIndex).GPUHandle);
		commandList->SetComputeRootDescriptorTable(3, rvManager_->GetHandle(model->skinCluster.skinningData.outputUAVIndex).GPUHandle);
		for (auto& pair : model->objects) {
			commandList->SetComputeRootConstantBufferView(4, rvManager_->GetCBVResource(model->objects[pair.first].infoCBVIndex)->GetGPUVirtualAddress());
		}
		commandList->Dispatch(static_cast<UINT>(meshData.size.vertices + 1023) / 1024, 1, 1);

		//// リソース遷移
		//d3dCommand_->BarrierTransition(
		//	CommandType::Compute,
		//	rvManager_->GetHandle(
		//		model->skinCluster.skinningData.outputUAVIndex).resource.Get(),
		//	D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		//	D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		//);
	}
}

void AnimationSystem::ResourceTransition()
{
	for (auto& model : rvManager_->GetModels()) {
		if (model.second->isBone) {
			// リソース遷移
			d3dCommand_->BarrierTransition(
				CommandType::Compute,
				rvManager_->GetHandle(
					model.second->skinCluster.skinningData.outputUAVIndex).resource.Get(),
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
			);
		}
	}
}
