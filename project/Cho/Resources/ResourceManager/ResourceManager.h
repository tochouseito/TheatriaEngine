#pragma once

/*--------------------------------------------
リソースマネージャクラス
--------------------------------------------*/

#include "SDK/DirectX/DirectX12/DescriptorHeap/DescriptorHeap.h"
#include "SDK/DirectX/DirectX12/BufferManager/BufferManager.h"

class ResourceManager
{
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

	// Getters
	DescriptorHeap* GetSUVDHeap() const { return m_SUVDescriptorHeap.get(); }
	DescriptorHeap* GetRTVDHeap() const { return m_RTVDescriptorHeap.get(); }
	DescriptorHeap* GetDSVDHeap() const { return m_DSVDescriptorHeap.get(); }
private:// methods
	// SUVディスクリプタヒープの生成
	void CreateSUVDescriptorHeap(ID3D12Device8* device);
	// RTVディスクリプタヒープの生成
	void CreateRTVDescriptorHeap(ID3D12Device8* device);
	// DSVディスクリプタヒープの生成
	void CreateDSVDescriptorHeap(ID3D12Device8* device);

	// Pool生成
	void CreateHeap(ID3D12Device8* device);
private:// members
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
};

