#include "PrecompiledHeader.h"
#include "ResourceViewManager.h"
#include"D3D12/D3DDevice/D3DDevice.h"
#include"D3D12/D3DCommand/D3DCommand.h"
#include"LogGenerator/LogGenerator.h"

void ResourceViewManager::Initialize(D3DDevice* d3dDevice, D3DCommand* d3dCommand)
{
	// デスクリプタヒープの生成
	descriptorHeap_ = d3dDevice->CreateDescriptorHeap(
		HEAP_TYPE,
		kMaxDescriptor,
		true
	);
	// デスクリプタ1個分のサイズを取得して記録
	descriptorSize_ = d3dDevice->GetDevice()->GetDescriptorHandleIncrementSize(
		HEAP_TYPE
	);

	d3dDevice_ = d3dDevice;
	d3dCommand_ = d3dCommand;
}

void ResourceViewManager::SetDescriptorHeap(ID3D12GraphicsCommandList6* commandList)
{
	// ディスクリプタヒープ
	ID3D12DescriptorHeap* descriptorHeaps[] = { descriptorHeap_.Get() };

	// コマンドリストにディスクリプタヒープをセット
	commandList->SetDescriptorHeaps(
		_countof(descriptorHeaps),
		descriptorHeaps
	);
}

void ResourceViewManager::Reset()
{
	indexLine_ = 0;
}

uint32_t ResourceViewManager::GetNewHandle()
{
	uint32_t index = SUVAllocate();

	ConstantHandleData& descriptorData = handles[index];

	descriptorData.CPUHandle = GetCPUDescriptorHandle(index);
	descriptorData.GPUHandle = GetGPUDescriptorHandle(index);

	return index;
}

ConstantHandleData& ResourceViewManager::GetHandle(const uint32_t& index)
{
	if (!handles.contains(index)) {
		assert(0);
	}
	return handles[index];
}

uint32_t ResourceViewManager::CreateCBV(const size_t& sizeInBytes)
{
	uint32_t index = CBVAllocate();

	Microsoft::WRL::ComPtr<ID3D12Resource>& resource = CBVResources[index];

	resource = CreateBufferResource(sizeInBytes);

	return index;
}

ID3D12Resource* ResourceViewManager::GetCBVResource(const uint32_t& index)
{
	if (CBVResources.contains(index)) {
		return CBVResources[index].Get();
	}
	return nullptr;
}

void ResourceViewManager::CBVRelease(const uint32_t& index)
{
	if (index == 0) {
		return;
	}
	if (CBVResources.contains(index)) {
		GetCBVResource(index)->Unmap(0, nullptr);
		CBVResources.erase(index);
		freeCBVIndexList.push_back(index);
	}
	return;
}

uint32_t ResourceViewManager::CreateMeshView(const uint32_t& vertices, const uint32_t& indices, const size_t& sizeInBytes, ID3D12Resource* vbvResource, ID3D12Resource* ibvResource,const uint32_t& updateIndex)
{
	uint32_t index;
	if (updateIndex) {
		index = updateIndex;
	}
	else {
		index = MeshViewAllocate();
	}

	MeshView& meshView = meshViews[index];

	//size_t sizeInBytes = sizeof(VertexData);

	meshView = CreateMeshViewResource(vertices,indices,sizeInBytes,vbvResource,ibvResource);

	return index;
}

MeshView* ResourceViewManager::GetMeshViewData(const uint32_t& index)
{
	if (!meshViews.contains(index)) {
		return nullptr;
	}
	return &meshViews[index];
}

void ResourceViewManager::CreateTextureResource(const uint32_t& index, const DirectX::TexMetadata& metadata)
{
	// 1.metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);// Textureの幅
	resourceDesc.Height = UINT(metadata.height);// Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);// mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);// 奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);// Textureの次元数。普段使っているのは2次元
	// 2.利用するHeapの設定 非常に特殊な運用。02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// 細かい設定を行う
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;// WriteBackポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;// プロセッサの近くに配置
	//heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;// 細かい設定を行う
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;// WriteBackポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;// プロセッサの近くに配置
	// 3.Resourceを生成する
	Microsoft::WRL::ComPtr < ID3D12Resource> resource = nullptr;
	HRESULT hr;hr = d3dDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties,// Heapの設定
		D3D12_HEAP_FLAG_NONE,// Heapの特殊な設定。特になし
		&resourceDesc,// Resourceの設定
		//D3D12_RESOURCE_STATE_GENERIC_READ,// 初回のResourceState。Textureは基本読むだけ
		D3D12_RESOURCE_STATE_COPY_DEST,// 初回のResourceState。Textureは基本読むだけ
		nullptr,// Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource));// 作成するResourceポインタのポインタ
	assert(SUCCEEDED(hr));

	handles[index].resource = resource;
}

void ResourceViewManager::UploadTextureDataEx(const uint32_t& index, const DirectX::ScratchImage& mipImages)
{
	std::vector<D3D12_SUBRESOURCE_DATA>subresources;
	DirectX::PrepareUpload(d3dDevice_->GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(handles[index].resource.Get(), 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr < ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);
	UpdateSubresources(d3dCommand_->GetCommand(CommandType::Copy).list.Get(), handles[index].resource.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	// Textureへの転送後は利用できるよう,D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = handles[index].resource.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	d3dCommand_->GetCommand(CommandType::Copy).list->ResourceBarrier(1, &barrier);
	uploadResources.push_back(intermediateResource);
}

void ResourceViewManager::CreateUAVResource(const uint32_t& index, const size_t& sizeInBytes)
{
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;// リソースのサイズ。今回はVector4を３頂点分
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	// バッファの場合ははこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	HRESULT hr;
	hr = d3dDevice_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_COMMON, nullptr,
		IID_PPV_ARGS(&handles[index].resource));
	assert(SUCCEEDED(hr));
}

void ResourceViewManager::CreateUAVforStructuredBuffer(const uint32_t& index, const UINT& numElements, const UINT& structuredByteStride)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.NumElements = numElements;
	uavDesc.Buffer.StructureByteStride = structuredByteStride;
	
	d3dDevice_->GetDevice()->CreateUnorderedAccessView(
		handles[index].resource.Get(),
		nullptr,
		&uavDesc,
		handles[index].CPUHandle
	);
}

void ResourceViewManager::UAVRelease(const uint32_t& index)
{
	if (index == 0) {
		return;// 0はImGuiで使うので解放しない
	}
	if (handles.contains(index)) {
		//handles[index].resource->Unmap(0, nullptr);
		handles.erase(index);
		freeSUVIndexList.push_back(index);
	}
	return;
}

void ResourceViewManager::CreateSRVResource(const uint32_t& index, const size_t& sizeInBytes)
{
	Microsoft::WRL::ComPtr<ID3D12Resource>& resource = handles[index].resource;

	resource = CreateBufferResource(sizeInBytes);
}

void ResourceViewManager::CreateSRVForTexture2D(const uint32_t& index, const DXGI_FORMAT& Format, const UINT& MipLevels)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};

	// metadataを基にSRVの設定
	srvDesc.Format = Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;// 2Dのテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(MipLevels);

	d3dDevice_->GetDevice()->CreateShaderResourceView(
		handles[index].resource.Get(), 
		&srvDesc,
		handles[index].CPUHandle
	);
}

void ResourceViewManager::CreateSRVforStructuredBuffer(const uint32_t& index, const UINT& numElements, const UINT& structuredByteStride)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = numElements;
	instancingSrvDesc.Buffer.StructureByteStride = structuredByteStride;

	d3dDevice_->GetDevice()->CreateShaderResourceView(
		handles[index].resource.Get(),
		&instancingSrvDesc,
		handles[index].CPUHandle
	);
}

void ResourceViewManager::SRVRelease(const uint32_t& index)
{
	if (index == 0) {
		return;// ImGuiのSRVは解放しない
	}
	if (handles.contains(index)) {
		handles[index].resource->Unmap(0, nullptr);
		handles.erase(index);
		freeSUVIndexList.push_back(index);
	}
	return;
}

void ResourceViewManager::CreateRenderTextureResource(const uint32_t& index, const uint32_t& width, const uint32_t& height, DXGI_FORMAT format, D3D12_RESOURCE_STATES state, const Color& clearColor)
{
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;// Textureの幅
	resourceDesc.Height = height;// Textureの高さ
	resourceDesc.MipLevels = 1;//mipmapの数
	resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
	resourceDesc.Format = format;// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;// RenderTargetとして利用可能にする

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.r;
	clearValue.Color[1] = clearColor.g;
	clearValue.Color[2] = clearColor.b;
	clearValue.Color[3] = clearColor.a;

	// Resourceの生成
	Microsoft::WRL::ComPtr < ID3D12Resource> resource = nullptr;
	HRESULT hr;hr = d3dDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		state,
		&clearValue,// Clear最適値。ClearRenderTargetをこの色でClearするようにする。最適化されているので高速である
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
	handles[index].resource = resource;

	CreateSRVForTexture2D(index, format, 1);
}


SpriteMeshData* ResourceViewManager::GetSpriteData(const uint32_t& index)
{
	if (!spriteContainer.contains(index)) {
		return nullptr;
	}
	return spriteContainer[index].get();
}

uint32_t ResourceViewManager::CreateSpriteData()
{
	uint32_t index = SpriteAllocate();
	spriteContainer[index] = std::make_unique<SpriteMeshData>();
	SpriteMeshData* data = spriteContainer[index].get();

	MeshGenerator::CreateSprite(*data, this);

	return index;
}

void ResourceViewManager::CreateOrthographicResource()
{
	orthographicCBVIndex = CreateCBV(sizeof(ConstantBuffer));
	GetCBVResource(orthographicCBVIndex)->Map(0, nullptr, reinterpret_cast<void**>(&orthographicConstBuffer));
}

void ResourceViewManager::CreateEffectNodeResource(EffectNode& effectNode)
{
	//// VertexBuffer
	//if (effectNode.draw.meshType == EffectMeshTypeSprite) {
	//	uint32_t vertices = 4;
	//	uint32_t indices = 6;
	//	// VBV
	//	effectNode.vertexUAVIndex = GetNewHandle();
	//	CreateUAVResource(effectNode.vertexUAVIndex, sizeof(VertexData) * vertices);
	//	CreateUAVforStructuredBuffer(
	//		effectNode.vertexUAVIndex,
	//		static_cast<UINT>(vertices),
	//		static_cast<UINT>(sizeof(VertexData))
	//	);
	//	// IBV
	//	effectNode.indexUAVIndex = GetNewHandle();
	//	CreateUAVResource(effectNode.indexUAVIndex, sizeof(uint32_t) * indices);
	//	CreateUAVforStructuredBuffer(
	//		effectNode.indexUAVIndex,
	//		static_cast<UINT>(indices),
	//		static_cast<UINT>(sizeof(uint32_t))
	//	);
	//}

	// EffectParticleResource
	effectNode.particle.particleUAVIndex = GetNewHandle();
	CreateUAVResource(effectNode.particle.particleUAVIndex, sizeof(ConstBufferDataEffectParticle) * effectNode.common.maxCount);
	CreateUAVforStructuredBuffer(
		effectNode.particle.particleUAVIndex,
		static_cast<UINT>(effectNode.common.maxCount),
		static_cast<UINT>(sizeof(ConstBufferDataEffectParticle))
	);

	// FreeListIndex
	effectNode.particle.freeListIndex.uavIndex = GetNewHandle();
	CreateUAVResource(effectNode.particle.freeListIndex.uavIndex, sizeof(ConstBufferDataFreeListIndex));
	CreateUAVforStructuredBuffer(
		effectNode.particle.freeListIndex.uavIndex,
		1,
		sizeof(ConstBufferDataFreeListIndex)
	);

	// FreeList
	effectNode.particle.freeList.uavIndex = GetNewHandle();
	CreateUAVResource(effectNode.particle.freeList.uavIndex, sizeof(ConstBufferDataFreeList) * effectNode.common.maxCount);
	CreateUAVforStructuredBuffer(
		effectNode.particle.freeList.uavIndex,
		static_cast<UINT>(effectNode.common.maxCount),
		static_cast<UINT>(sizeof(ConstBufferDataFreeList))
	);

	// EffectNodeResource
	effectNode.srvIndex = GetNewHandle();
	CreateSRVResource(effectNode.srvIndex, sizeof(ConstBufferDataEffectNode));
	CreateSRVforStructuredBuffer(
		effectNode.srvIndex,
		1,
		sizeof(ConstBufferDataEffectNode)
	);
	GetHandle(effectNode.srvIndex).resource->Map(0, nullptr, reinterpret_cast<void**>(&effectNode.constData));
}

void ResourceViewManager::RemakeEffectNodeResource(EffectNode& effectNode)
{
	// VertexBuffer
	if (effectNode.draw.meshType == EffectMeshTypeSprite) {
		uint32_t vertices = 4;
		uint32_t indices = 6;
		// VBV
		CreateUAVResource(effectNode.vertexUAVIndex, sizeof(VertexData) * vertices);
		CreateUAVforStructuredBuffer(
			effectNode.vertexUAVIndex,
			static_cast<UINT>(vertices),
			static_cast<UINT>(sizeof(VertexData))
		);
		// IBV
		CreateUAVResource(effectNode.indexUAVIndex, sizeof(uint32_t) * indices);
		CreateUAVforStructuredBuffer(
			effectNode.indexUAVIndex,
			static_cast<UINT>(indices),
			static_cast<UINT>(sizeof(uint32_t))
		);
	}

	// EffectParticleResource
	CreateUAVResource(effectNode.particle.particleUAVIndex, sizeof(ConstBufferDataParticle) * effectNode.common.maxCount);
	CreateUAVforStructuredBuffer(
		effectNode.particle.particleUAVIndex,
		static_cast<UINT>(effectNode.common.maxCount),
		static_cast<UINT>(sizeof(ConstBufferDataParticle))
	);

	//// FreeListIndex
	//CreateUAVResource(effectNode.particle.freeListIndex.uavIndex, sizeof(ConstBufferDataFreeListIndex));
	//CreateUAVforStructuredBuffer(
	//	effectNode.particle.freeListIndex.uavIndex,
	//	1,
	//	sizeof(ConstBufferDataFreeListIndex)
	//);

	// FreeList
	CreateUAVResource(effectNode.particle.freeList.uavIndex, sizeof(ConstBufferDataFreeList) * effectNode.common.maxCount);
	CreateUAVforStructuredBuffer(
		effectNode.particle.freeList.uavIndex,
		static_cast<UINT>(effectNode.common.maxCount),
		static_cast<UINT>(sizeof(ConstBufferDataFreeList))
	);

	//// EffectNodeResource
	//CreateSRVResource(effectNode.srvIndex, sizeof(ConstBufferDataEffectNode));
	//CreateSRVforStructuredBuffer(
	//	effectNode.srvIndex,
	//	1,
	//	sizeof(ConstBufferDataEffectNode)
	//);
}

void ResourceViewManager::CreateEffectNodesResource(EffectComponent& effectComponent)
{
	RemakeEffectNodesResource(effectComponent);
}

void ResourceViewManager::LoopEffectNodes(std::vector<EffectNode>& effectNodes, EffectComponent& effectComponent)
{
	effectNodes;
	effectComponent;
	/*for (auto& effectNode : effectNodes) {
		effectNode.instanceIndex = effectComponent.instanceCount;
		effectComponent.instanceCount++;
		LoopEffectNodes(effectNode.children, effectComponent);
	}*/
}

void ResourceViewManager::RemakeEffectNodesResource(EffectComponent& effectComponent)
{
	effectComponent;
	/*if (effectComponent.constData) {
		GetHandle(effectComponent.srvIndex).resource->Unmap(0, nullptr);
		effectComponent.instanceCount = 0;
		effectComponent.constData = nullptr;
	}
	else {
		effectComponent.srvIndex = GetNewHandle();
	}

	LoopEffectNodes(effectComponent.effectNodes, effectComponent);

	CreateSRVResource(effectComponent.srvIndex, sizeof(ConstBufferDataEffectNode) * effectComponent.instanceCount);
	CreateSRVforStructuredBuffer(
		effectComponent.srvIndex,
		static_cast<UINT>(effectComponent.instanceCount),
		static_cast<UINT>(sizeof(ConstBufferDataEffectNode))
	);
	GetHandle(effectComponent.srvIndex).resource->Map(0, nullptr, reinterpret_cast<void**>(&effectComponent.constData));*/
}

void ResourceViewManager::CreateLightResource()
{
	lightCBVIndex = CreateCBV(sizeof(PunctualLightData));
	GetCBVResource(lightCBVIndex)->Map(0, nullptr, reinterpret_cast<void**>(&lightConstData));

	lightConstData->ambientColor = Vector3(0.01f, 0.01f, 0.01f);

	for (int i = 0;i < kDirLightNum;i++) {
		lightConstData->dirLights[i].direction = Vector3(0.0f, -1.0f, 0.0f);
		lightConstData->dirLights[i].color = Vector3(1.0f, 1.0f, 1.0f);
		lightConstData->dirLights[i].intensity = 1.0f;
		lightConstData->dirLights[i].active = 0;
	}
	for (int i = 0;i < kPointLightNum;i++) {
		lightConstData->pointLights[i].position = Vector3(0.0f, 0.0f, 0.0f);
		lightConstData->pointLights[i].color = Vector3(1.0f, 1.0f, 1.0f);
		lightConstData->pointLights[i].intensity = 1.0f;
		lightConstData->pointLights[i].active = 0;
	}
	for (int i = 0;i < kSpotLightNum;i++) {
		lightConstData->spotLights[i].position = Vector3(0.0f, 0.0f, 0.0f);
		lightConstData->spotLights[i].direction = Vector3(0.0f, -1.0f, 0.0f);
		lightConstData->spotLights[i].color = Vector3(1.0f, 1.0f, 1.0f);
		lightConstData->spotLights[i].intensity = 1.0f;
		lightConstData->spotLights[i].active = 0;
	}
}

uint32_t ResourceViewManager::SUVAllocate()
{
	if (!freeSUVIndexList.empty()) {
		uint32_t index = freeSUVIndexList.back();
		freeSUVIndexList.pop_back();
		return index;
	}
	if (useSUVIndex_+useCBVIndex_ >= kMaxDescriptor) {
		assert(0);
	}
	// returnする番号を一旦記録する
	int index = useSUVIndex_;
	// 次回のため番号を1進める
	useSUVIndex_++;
	// 上で記録した番号をreturn
	return index;
}

D3D12_CPU_DESCRIPTOR_HANDLE ResourceViewManager::GetCPUDescriptorHandle(uint32_t& index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize_ * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE ResourceViewManager::GetGPUDescriptorHandle(uint32_t& index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize_ * index);
	return handleGPU;
}

uint32_t ResourceViewManager::CBVAllocate()
{
	if (!freeCBVIndexList.empty()) {
		uint32_t index = freeCBVIndexList.back();
		freeCBVIndexList.pop_back();
		return index;
	}
	if (useSUVIndex_ + useCBVIndex_ >= kMaxDescriptor) {
		assert(0);
	}
	// returnする番号を一旦記録する
	int index = useCBVIndex_;
	// 次回のため番号を1進める
	useCBVIndex_++;
	// 上で記録した番号をreturn
	return index;
}

Microsoft::WRL::ComPtr<ID3D12Resource> ResourceViewManager::CreateBufferResource(const size_t& sizeInBytes)
{
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;// リソースのサイズ。今回はVector4を３頂点分
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合ははこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr;
	hr = d3dDevice_->GetDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));
	return resource;
}

uint32_t ResourceViewManager::MeshViewAllocate()
{
	if (!freeMeshViewIndexList.empty()) {
		uint32_t index = freeMeshViewIndexList.back();
		freeMeshViewIndexList.pop_back();
		return index;
	}
	// returnする番号を一旦記録する
	int index = useMeshViewIndex_;
	// 次回のため番号を1進める
	useMeshViewIndex_++;
	// 上で記録した番号をreturn
	return index;
}

MeshView ResourceViewManager::CreateMeshViewResource(const uint32_t& vertices, const uint32_t& indices, const size_t& sizeInBytes, ID3D12Resource* vbvResource, ID3D12Resource* ibvResource)
{
	MeshView meshView;

	//size_t sizeInBytes = sizeof(VertexData);

	if (vbvResource == nullptr) {
		meshView.vbvData.resource = CreateBufferResource(sizeInBytes * static_cast<size_t>(vertices));
	}
	else {
		meshView.vbvData.resource = vbvResource;
	}
	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	meshView.vbvData.vbv.BufferLocation = meshView.vbvData.resource->GetGPUVirtualAddress();

	// 使用するリソースのサイズは頂点のサイズ
	meshView.vbvData.vbv.SizeInBytes = static_cast<UINT>(sizeInBytes * vertices);

	// 1頂点アタリのサイズ
	meshView.vbvData.vbv.StrideInBytes = static_cast<UINT>(sizeInBytes);

	if (indices > 0) {
		// インデックスバッファービューを作成する
		if (ibvResource == nullptr) {
			meshView.ibvData.resource = CreateBufferResource(sizeof(uint32_t) * static_cast<size_t>(indices));
		}
		else {
			meshView.ibvData.resource = ibvResource;
		}
		meshView.ibvData.ibv.BufferLocation = meshView.ibvData.resource->GetGPUVirtualAddress();
		meshView.ibvData.ibv.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * indices);
		meshView.ibvData.ibv.Format = DXGI_FORMAT_R32_UINT;
	}
	return meshView;
}

uint32_t ResourceViewManager::SpriteAllocate()
{
	if (!freeSpriteIndexList.empty()) {
		uint32_t index = freeSpriteIndexList.back();
		freeSpriteIndexList.pop_back();
		return index;
	}
	uint32_t index = useSpriteIndex_;
	useSpriteIndex_++;
	return index;
}

uint32_t ResourceViewManager::MeshesAllocate()
{
	if (!freeMeshesIndexList.empty()) {
		uint32_t index = freeMeshesIndexList.back();
		freeMeshesIndexList.pop_back();
		return index;
	}
	uint32_t index = useMeshesIndex_;
	useMeshesIndex_++;
	return index;
}

uint32_t ResourceViewManager::DirLightAllocate()
{
	uint32_t index;
	if (!freeDirLightIndexList.empty()) {
		index = freeDirLightIndexList.back();
		freeDirLightIndexList.pop_back();
	}
	else {
		if (useDirLightIndex >= kDirLightNum) {
			assert(0 && "DirLightの数が上限に達しています");
		}
		index = useDirLightIndex;
		useDirLightIndex++;
	}
	return index;
}

uint32_t ResourceViewManager::PointLightAllocate()
{
	uint32_t index;
	if (!freePointLightIndexList.empty()) {
		index = freePointLightIndexList.back();
		freePointLightIndexList.pop_back();
	}
	else {
		if (usePointLightIndex >= kPointLightNum) {
			assert(0 && "PointLightの数が上限に達しています");
		}
		index = usePointLightIndex;
		usePointLightIndex++;
	}
	return index;
}

uint32_t ResourceViewManager::SpotLightAllocate()
{
	uint32_t index;
	if (!freeSpotLightIndexList.empty()) {
		index = freeSpotLightIndexList.back();
		freeSpotLightIndexList.pop_back();
	}
	else {
		if (useSpotLightIndex >= kSpotLightNum) {
			assert(0 && "SpotLightの数が上限に達しています");
		}
		index = useSpotLightIndex;
		useSpotLightIndex++;
	}
	return index;
}

void ResourceViewManager::DirLightRelease(const uint32_t& index)
{
	lightConstData->dirLights[index].active = 0;
	freeDirLightIndexList.push_back(index);
}

void ResourceViewManager::PointLightRelease(const uint32_t& index)
{
	lightConstData->pointLights[index].active = 0;
	freePointLightIndexList.push_back(index);
}

void ResourceViewManager::SpotLightRelease(const uint32_t& index)
{
	lightConstData->spotLights[index].active = 0;
	freeSpotLightIndexList.push_back(index);
}

void ResourceViewManager::CreateMeshPattern()
{
	for (uint32_t index = 0; index < static_cast<uint32_t>(MeshPattern::CountPattern); index++) {
		if (MeshViewAllocate() != index) {
			//WriteLog("MeshViewAllocate Error");
			assert(0);
		}
		MeshesAllocate();
		// indexをMeshPattern型にキャストしてGeneratorMeshsに渡す
		meshContainer.push_back(
			std::make_unique<Meshes>(MeshGenerator::GeneratorMeshes(static_cast<MeshPattern>(index), this))
		);
	}
	MeshGenerator::CreateLineMesh(this);

}

void ResourceViewManager::CreateMeshViewDMP(const uint32_t& index, const  uint32_t& vertices, const uint32_t& indices, ID3D12Resource* vbvResource, ID3D12Resource* ibvResource)
{
	MeshView& meshView = meshViews[index];

	meshView = CreateMeshViewResource(vertices,indices,sizeof(VertexData),vbvResource,ibvResource);
}

uint32_t ResourceViewManager::CreateMeshResource(Meshes* meshes)
{
	uint32_t index = MeshesAllocate();
	{
	std::unique_ptr<Meshes> mesh = std::make_unique<Meshes>();
	meshContainer.push_back(std::move(mesh));
	}
	Meshes* meshesData = meshContainer[index].get();
	meshesData->meshData = meshes->meshData;
	meshesData->meshesName = meshes->meshesName;

	for (MeshData& meshData : meshesData->meshData) {
		ID3D12Resource* vbvResource = nullptr;
		ID3D12Resource* ibvResource = nullptr;
		if (meshData.srvVBVIndex != 0) {
			vbvResource = GetHandle(meshData.srvVBVIndex).resource.Get();
		}
		if (meshData.srvIBVIndex != 0) {
			ibvResource = GetHandle(meshData.srvIBVIndex).resource.Get();
		}
		meshData.meshViewIndex= CreateMeshView(
			meshData.size.vertices,
			meshData.size.indices,
			sizeof(VertexData),
			vbvResource,
			ibvResource
		);
	}

	return index;
}

void ResourceViewManager::ModelMeshMap(const uint32_t& index, const std::string& name)
{
	index;
	name;
	/*Meshes* meshes = meshContainer[index].get();

	meshViews[index].vbvData.resource->Map(
		0, nullptr, 
		reinterpret_cast<void**>(&meshes->meshData[name].vertexData)
	);

	meshViews[index].ibvData.resource->Map(
		0, nullptr,
		reinterpret_cast<void**>(&meshes->meshData[name].indexData)
	);*/

}

void ResourceViewManager::MeshDataCopy(const uint32_t& index, const std::string& name, const std::string& modelName)
{
	index;
	name;
	modelName;
	//Meshes* meshs = meshContainer[index].get();
	//ModelData* modelData = modelContainer[modelName].get();

	//// 頂点データをリソースにコピー
	//std::memcpy(meshs->meshData[name].vertexData,
	//	modelData->objects[name].vertices.data(),
	//	sizeof(VertexData) * modelData->objects[name].vertices.size()
	//);
	//// インデックスデータをリソースにコピー
	//std::memcpy(meshs->meshData[name].indexData,
	//	modelData->objects[name].indices.data(),
	//	sizeof(uint32_t) * modelData->objects[name].indices.size()
	//);
}

void ResourceViewManager::AddModel(const std::string& name)
{
	modelContainer[name] = std::make_unique<ModelData>();
}

ModelData* ResourceViewManager::GetModelData(const std::string& name)
{
	if (!modelContainer.contains(name)) {
		WriteLog("ModelData Error");
		return nullptr;
	}
	return modelContainer[name].get();
}
