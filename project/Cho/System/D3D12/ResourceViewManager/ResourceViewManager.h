#pragma once

#include<d3d12.h>
#include<d3dx12.h>
#include<DirectXTex.h>
#include<wrl.h>
#include<cstdint>
#include<unordered_map>
#include<array>
#include<memory>
#include"ECS/EntityManager/EntityManager.h"
#include"Color.h"
#include"ConstantData/ModelData.h"
#include"ConstantData/LineData.h"
#include"ECS/ComponentManager/Components/Components.h"

// 線分の最大数
static const int32_t kMaxLineCount = 4096*2;
// 線分の頂点数
static const UINT kVertexCountLine = 2;
// 線分のインデックス数
static const UINT kIndexCountLine = 0;

// 3D変換行列
struct ConstantBuffer {
	Matrix4 mat;
};

// ディスクリプタハンドル定数データ
struct ConstantHandleData {
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;
};
struct VBVData {
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_VERTEX_BUFFER_VIEW vbv{};
};
struct IBVData {
	Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	D3D12_INDEX_BUFFER_VIEW ibv{};
};
struct MeshView {
	VBVData vbvData;
	IBVData ibvData;
};
class D3DDevice;
class D3DCommand;
class ResourceViewManager
{
public:// メンバ関数

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(D3DDevice* d3dDevice, D3DCommand* d3dCommand);

	/// <summary>
	/// ディスクリプタヒープをセット
	/// </summary>
	/// <param name="commandList"></param>
	void SetDescriptorHeap(ID3D12GraphicsCommandList6* commandList);

	ID3D12DescriptorHeap* GetDescriptorHeap()const { return descriptorHeap_.Get(); }

	void Reset();

	uint32_t GetNowIndex()const { return useSUVIndex_ + useCBVIndex_; }
	static uint32_t GetMaxIndex() { return kMaxDescriptor; }

	/// <summary>
	/// 新しいハンドルを入手する
	/// </summary>
	/// <returns></returns>
	uint32_t GetNewHandle();

	// ハンドルを取得
	ConstantHandleData& GetHandle(const uint32_t& index);

	uint32_t CreateCBV(const size_t& sizeInBytes);

	ID3D12Resource* GetCBVResource(const uint32_t& index);

	void CBVRelease(const uint32_t& index);

	uint32_t CreateMeshView(
		const uint32_t& vertices,
		const uint32_t& indices,
		const size_t& sizeInBytes,
		ID3D12Resource* vbvResource=nullptr,
		ID3D12Resource* ibvResource = nullptr,
		const uint32_t& updateIndex = 0
	);

	MeshView* GetMeshViewData(const uint32_t& index);

	void CreateTextureResource(const uint32_t& index,const DirectX::TexMetadata& metadata);

	void UploadTextureDataEx(const uint32_t& index,const DirectX::ScratchImage& mipImages);

	void CreateUAVResource(const uint32_t& index, const size_t& sizeInBytes);

	void CreateUAVforStructuredBuffer(const uint32_t& index,const UINT& numElements, const UINT& structuredByteStride);

	void UAVRelease(const uint32_t& index);

	void CreateSRVResource(const uint32_t& index, const size_t& sizeInBytes);

	void CreateSRVForTexture2D(const uint32_t& index, const DXGI_FORMAT& Format, const UINT& MipLevels);

	// SRV生成(StructuredBuffer用)
	void CreateSRVforStructuredBuffer(const uint32_t& index, const UINT& numElements, const UINT& structuredByteStride);

	void SRVRelease(const uint32_t& index);

	void CreateRenderTextureResource(
		const uint32_t& index,
		const uint32_t& width,
		const uint32_t& height,
		DXGI_FORMAT format,
		D3D12_RESOURCE_STATES state,
		const Color& clearColor
	);

	// デフォルトメッシュパターン生成
	void CreateMeshPattern();

	// デフォルトメッシュ用の専用関数
	void CreateMeshViewDMP(const uint32_t& index, const uint32_t& vertices, const uint32_t& indices, ID3D12Resource* vbvResource = nullptr, ID3D12Resource* ibvResource=nullptr);

	Meshes* GetMesh(const uint32_t& index)const { return meshContainer[index].get(); }

	uint32_t CreateMeshResource(Meshes* meshes);

	void ModelMeshMap(const uint32_t& index, const std::string& name);

	void MeshDataCopy(const uint32_t& index, const std::string& name, const std::string& modelName);

	void AddModel(const std::string& name);

	ModelData* GetModelData(const std::string& name);

	const std::vector<std::unique_ptr<Meshes>>& GetMeshes() { return meshContainer; }

	const std::unordered_map<std::string, std::unique_ptr<ModelData>>& GetModels() { return modelContainer; }

	const std::unordered_map<uint32_t, std::unique_ptr<SpriteMeshData>>& GetSprites() { return spriteContainer; }

	SpriteMeshData* GetSpriteData(const uint32_t& index);

	uint32_t CreateSpriteData();

	LineVertexData* GetLineData() { return &lineData; }
	void IndexLineAllocate(const uint32_t& value = 0) {
		if (value == 0) {
			indexLine_++;
		}
		else {
			indexLine_ = value;

		}
	}
	uint32_t GetIndexLine() { return indexLine_; }
	uint32_t GetOrthographicCBVIndex() { return orthographicCBVIndex; }
	void SetOrthographicCBVIndex(const uint32_t& index) { orthographicCBVIndex = index; }
	ConstantBuffer* GetOrthographicConstBuffer() { return orthographicConstBuffer; }
	void SetOrthographicConstBuffer(const Matrix4& buffer) { orthographicConstBuffer->mat = buffer; }
	void CreateOrthographicResource();

	// EffectNodeのリソース作成
	void CreateEffectNodeResource(EffectNode& effectNode);
	void RemakeEffectNodeResource(EffectNode& effectNode);
	void CreateEffectNodesResource(EffectComponent& effectComponent);
	void LoopEffectNodes(std::vector<EffectNode>& effectNodes, EffectComponent& effectComponent);
	void RemakeEffectNodesResource(EffectComponent& effectComponent);

	// ライトリソース作成
	void CreateLightResource();
	// 各ライトのアロケータ
	uint32_t DirLightAllocate();
	uint32_t PointLightAllocate();
	uint32_t SpotLightAllocate();

	// 各ライトのリリース
	void DirLightRelease(const uint32_t& index);
	void PointLightRelease(const uint32_t& index);
	void SpotLightRelease(const uint32_t& index);
	
public:// Getter、Setter

	// ライトリソースのインデックスを取得
	uint32_t GetLightCBVIndex()const { return lightCBVIndex; }
	// ライトリソースを取得
	PunctualLightData* GetLightConstData()const { return lightConstData; }

private:

	uint32_t SUVAllocate();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t& index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t& index);

	uint32_t CBVAllocate();

	// CBVリソース作成
	Microsoft::WRL::ComPtr < ID3D12Resource> CreateBufferResource(const size_t& sizeInBytes);

	uint32_t MeshViewAllocate();

	// VBVリソース作成
	MeshView CreateMeshViewResource(const uint32_t& vertices, const uint32_t& indices, const size_t& sizeInBytes, ID3D12Resource* vbvResource=nullptr, ID3D12Resource* ibvResource=nullptr);

	uint32_t SpriteAllocate();

	uint32_t MeshesAllocate();

private:// メンバ変数

	D3DDevice* d3dDevice_ = nullptr;
	D3DCommand* d3dCommand_ = nullptr;

	// 最大ディスクリプタ数
	static const uint32_t kMaxDescriptor = 1024;

	// ヒープタイプ
	static const D3D12_DESCRIPTOR_HEAP_TYPE HEAP_TYPE =
		D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	// デスクリプタサイズ
	uint32_t descriptorSize_ = 0;

	// デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	// アップロードリソース
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> uploadResources;

	// 次に使用するSRV,UAVインデックス。先頭はImGuiで使うので1から
	uint32_t useSUVIndex_ = 1;
	std::vector<uint32_t> freeSUVIndexList;
	// SRV,UAVコンテナ
	std::unordered_map<uint32_t, ConstantHandleData> handles;

	// 次に使用するCBVインデックス
	uint32_t useCBVIndex_ = 0;
	std::vector<uint32_t> freeCBVIndexList;
	// CBVコンテナ
	std::unordered_map<uint32_t, Microsoft::WRL::ComPtr<ID3D12Resource>> CBVResources;

	// 次に使用するMeshViewインデックス
	uint32_t useMeshViewIndex_ = 0;
	std::vector<uint32_t> freeMeshViewIndexList;
	// MeshViewコンテナ
	std::unordered_map<uint32_t, MeshView> meshViews;

	// 次に使用するメッシュインデックス
	uint32_t useMeshesIndex_ = 0;
	std::vector<uint32_t> freeMeshesIndexList;
	// メッシュコンテナ
	std::vector<std::unique_ptr<Meshes>> meshContainer;

	// Modelコンテナ
	std::unordered_map<std::string, std::unique_ptr<ModelData>> modelContainer;

	// 次に使用するスプライトインデックス
	uint32_t useSpriteIndex_ = 0;
	std::vector<uint32_t> freeSpriteIndexList;
	// スプライトコンテナ
	std::unordered_map<uint32_t, std::unique_ptr<SpriteMeshData>> spriteContainer;

	// ライトリソース
	PunctualLightData* lightConstData = nullptr;
	uint32_t lightCBVIndex = 0;
	// 各ライトの次に使用するインデックス
	uint32_t useDirLightIndex = 0;
	uint32_t usePointLightIndex = 0;
	uint32_t useSpotLightIndex = 0;

	// 各ライトのフリーリスト
	std::vector<uint32_t> freeDirLightIndexList;
	std::vector<uint32_t> freePointLightIndexList;
	std::vector<uint32_t> freeSpotLightIndexList;

	LineVertexData lineData;
	uint32_t indexLine_ = 0;
	uint32_t orthographicCBVIndex = 0;
	ConstantBuffer* orthographicConstBuffer = nullptr;
	//std::vector<LineVertex> lineVertex;
};

