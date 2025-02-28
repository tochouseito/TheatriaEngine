#include "pch.h"
#include "ResourceManager.h"

ResourceManager::ResourceManager(ID3D12Device8* device)
{
	CreatePool(device);// デバイスを受け取り、プールを作成する
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

void ResourceManager::CreateSUVDescriptorPool(ID3D12Device8* device)
{
	m_SUVDescriptorPool = std::make_unique<SUVDescriptorPool>(device, 1024);
}

void ResourceManager::CreateRTVDescriptorPool(ID3D12Device8* device)
{
	m_RTVDescriptorPool = std::make_unique<RTVDescriptorPool>(device, 16);
}

void ResourceManager::CreateDSVDescriptorPool(ID3D12Device8* device)
{
	m_DSVDescriptorPool = std::make_unique<DSVDescriptorPool>(device, 1);
}

void ResourceManager::CreatePool(ID3D12Device8* device)
{
	CreateSUVDescriptorPool(device);
	CreateRTVDescriptorPool(device);
	CreateDSVDescriptorPool(device);
}
