#pragma once

/*--------------------------------------------
リソースマネージャクラス
--------------------------------------------*/

#include "SDK/DirectX/DirectX12/DescriptorHeap/DescriptorHeap.h"
#include "SDK/DirectX/DirectX12/BufferManager/BufferManager.h"
#include "Resources/ModelManager/ModelManager.h"
#include "Resources/TextureManager/TextureManager.h"
#include <optional>

class SwapChain;

class ResourceManager
{
	friend class GraphicsEngine;
public:
	// コンストラクタ
	ResourceManager(ID3D12Device8* device);
	// デストラクタ
	~ResourceManager();
	// 初期化
	void Initialize();
	// 終了処理
	void Finalize();
	// 更新
	void Update();
	// 解放
	void Release();

	void CreateSwapChain(SwapChain* swapChain);

	// BufferMethod
	uint32_t CreateColorBuffer(BUFFER_COLOR_DESC& desc);
	uint32_t CreateDepthBuffer(BUFFER_DEPTH_DESC& desc);
	uint32_t CreateVertexBuffer(BUFFER_VERTEX_DESC& desc);

	// RemakeMethod
	void RemakeColorBuffer(const uint32_t& index, BUFFER_COLOR_DESC& desc);
	void RemakeDepthBuffer(const uint32_t& index, BUFFER_DEPTH_DESC& desc);
	void RemakeVertexBuffer(const uint32_t& index, BUFFER_VERTEX_DESC& desc);

	// ReleaseMethod
	void ReleaseColorBuffer(const uint32_t& index);
	void ReleaseDepthBuffer(const uint32_t& index);
	void ReleaseVertexBuffer(const uint32_t& index);

	//Getters
	DescriptorHeap* GetSUVDHeap() const { return m_SUVDescriptorHeap.get(); }
	DescriptorHeap* GetRTVDHeap() const { return m_RTVDescriptorHeap.get(); }
	DescriptorHeap* GetDSVDHeap() const { return m_DSVDescriptorHeap.get(); }
	BufferManager* GetBufferManager() const { return m_BufferManager.get(); }
private:
	// SUVディスクリプタヒープの生成
	void CreateSUVDescriptorHeap(ID3D12Device8* device);
	// RTVディスクリプタヒープの生成
	void CreateRTVDescriptorHeap(ID3D12Device8* device);
	// DSVディスクリプタヒープの生成
	void CreateDSVDescriptorHeap(ID3D12Device8* device);
	// Heap生成
	void CreateHeap(ID3D12Device8* device);
	// CPUHandleの取得
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(const uint32_t& index,D3D12_DESCRIPTOR_HEAP_TYPE type);

	// Device
	ID3D12Device8* m_Device = nullptr;
	// BufferManager
	std::unique_ptr<BufferManager> m_BufferManager = nullptr;
	// SUVディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_SUVDescriptorHeap = nullptr;
	// RTVディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_RTVDescriptorHeap = nullptr;
	// DSVディスクリプタヒープ
	std::unique_ptr<DescriptorHeap> m_DSVDescriptorHeap = nullptr;
	// テクスチャマネージャ
	std::unique_ptr<TextureManager> m_TextureManager = nullptr;
	// モデルマネージャ
	std::unique_ptr<ModelManager> m_ModelManager = nullptr;
};

