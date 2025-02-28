#pragma once

/*--------------------------------------------
リソースマネージャクラス
--------------------------------------------*/

#include "SDK/DirectX/DirectX12/DescriptorHeap/DescriptorHeap.h"

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
private:// methods
	// SUVディスクリプタプールの生成
	void CreateSUVDescriptorHeap(ID3D12Device8* device);
	// RTVディスクリプタプールの生成
	void CreateRTVDescriptorHeap(ID3D12Device8* device);
	// DSVディスクリプタプールの生成
	void CreateDSVDescriptorHeap(ID3D12Device8* device);

	// Pool生成
	void CreateHeap(ID3D12Device8* device);
private:// members
	// SUVディスクリプタプール
	std::unique_ptr<DescriptorHeap> m_SUVDescriptorHeap = nullptr;
	// RTVディスクリプタプール
	std::unique_ptr<DescriptorHeap> m_RTVDescriptorHeap = nullptr;
	// DSVディスクリプタプール
	std::unique_ptr<DescriptorHeap> m_DSVDescriptorHeap = nullptr;
};

