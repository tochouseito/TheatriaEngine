#include "pch.h"
#include "VertexBuffer.h"

void VertexBuffer::CreateVertexResource(ID3D12Device8* device, const UINT& numElements, const UINT& structuredByteStride)
{
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = numElements * structuredByteStride;// リソースのサイズ
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合ははこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	ComPtr<ID3D12Resource> pResource = GetResource();
	m_ElementCount = numElements;
	m_ElementSize = structuredByteStride;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pResource)
	);
	ChoAssertLog("Failed to create gpu resource.", hr, __FILE__, __LINE__);
	// VertexBufferViewの作成
	// Resourceの先頭のアドレスから使う
	m_VertexBufferView.BufferLocation = pResource->GetGPUVirtualAddress();
	// 使用するResourceのsizeは頂点のsize
	m_VertexBufferView.SizeInBytes = structuredByteStride * numElements;
	// 1頂点のサイズ
	m_VertexBufferView.StrideInBytes = structuredByteStride;
}

void VertexBuffer::CreateIndexResource(ID3D12Device8* device, const UINT& numElements, const UINT& structuredByteStride)
{
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = numElements * structuredByteStride;// リソースのサイズ
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合ははこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// 実際にリソースを作る
	ComPtr<ID3D12Resource> pResource = m_IndexBuffer.GetResource();// IndexBufferのResourceに対して作成する
	m_IndexBuffer.SetElementCount(numElements);
	m_IndexBuffer.SetElementSize(structuredByteStride);
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pResource)
	);
	ChoAssertLog("Failed to create gpu resource.", hr, __FILE__, __LINE__);
	// IndexBufferViewの作成
	// Resourceの先頭のアドレスから使う
	m_IndexBufferView.BufferLocation = pResource->GetGPUVirtualAddress();
	// 使用するResourceのsizeは頂点のsize
	m_IndexBufferView.SizeInBytes = structuredByteStride * numElements;
	// 1頂点のサイズ
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
}
