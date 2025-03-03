#include "pch.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager(ID3D12Device8* device)
{
	CreateHeap(device);// デバイスを受け取り、プールを作成する

	m_BufferManager = std::make_unique<BufferManager>(device, this);// バッファマネージャの生成
	m_Device = device;// デバイスの設定

	Initialize();// 初期化
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::Initialize()
{
	
}

void ResourceManager::Finalize()
{
}

void ResourceManager::Update()
{
}

void ResourceManager::Release()
{
}

void ResourceManager::CreateSUVDescriptorHeap(ID3D12Device8* device)
{
	m_SUVDescriptorHeap = std::make_unique<SUVDescriptorHeap>(device, 1024);
}

void ResourceManager::CreateRTVDescriptorHeap(ID3D12Device8* device)
{
	m_RTVDescriptorHeap = std::make_unique<RTVDescriptorHeap>(device, 16);
}

void ResourceManager::CreateDSVDescriptorHeap(ID3D12Device8* device)
{
	m_DSVDescriptorHeap = std::make_unique<DSVDescriptorHeap>(device, 1);
}

void ResourceManager::CreateHeap(ID3D12Device8* device)
{
	CreateSUVDescriptorHeap(device);
	CreateRTVDescriptorHeap(device);
	CreateDSVDescriptorHeap(device);
}
