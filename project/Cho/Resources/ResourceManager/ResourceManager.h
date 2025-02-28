#pragma once

/*--------------------------------------------
リソースマネージャクラス
--------------------------------------------*/

#include "SDK/DirectX/DirectX12/DescriptorPool/DescriptorPool.h"

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
	void CreateSUVDescriptorPool(ID3D12Device8* device);
	// RTVディスクリプタプールの生成
	void CreateRTVDescriptorPool(ID3D12Device8* device);
	// DSVディスクリプタプールの生成
	void CreateDSVDescriptorPool(ID3D12Device8* device);

	// Pool生成
	void CreatePool(ID3D12Device8* device);
private:// members
	// SUVディスクリプタプール
	std::unique_ptr<SUVDescriptorPool> m_SUVDescriptorPool = nullptr;
	// RTVディスクリプタプール
	std::unique_ptr<RTVDescriptorPool> m_RTVDescriptorPool = nullptr;
	// DSVディスクリプタプール
	std::unique_ptr<DSVDescriptorPool> m_DSVDescriptorPool = nullptr;
};

