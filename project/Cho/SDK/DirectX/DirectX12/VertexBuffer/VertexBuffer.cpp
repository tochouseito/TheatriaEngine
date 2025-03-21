#include "pch.h"
#include "VertexBuffer.h"

void VertexBuffer::UnMap()
{
	GpuBuffer::UnMap();
	m_IndexBuffer.UnMap();
}

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
	Create(device, heapProperties, D3D12_HEAP_FLAG_NONE, resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ);
	m_ElementCount = numElements;
	m_ElementSize = structuredByteStride;
	// VertexBufferViewの作成
	// Resourceの先頭のアドレスから使う
	m_VertexBufferView.BufferLocation = GetResource()->GetGPUVirtualAddress();
	// 使用するResourceのサイズは頂点のサイズ
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
	// IndexBufferのResourceに対して作成する
	m_IndexBuffer.Create(
		device, heapProperties, D3D12_HEAP_FLAG_NONE,
		resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ
	);
	m_IndexBuffer.SetElementCount(numElements);
	m_IndexBuffer.SetElementSize(structuredByteStride);
	// IndexBufferViewの作成
	// Resourceの先頭のアドレスから使う
	m_IndexBufferView.BufferLocation = m_IndexBuffer.GetResource()->GetGPUVirtualAddress();
	// 使用するResourceのサイズは頂点のサイズ
	m_IndexBufferView.SizeInBytes = structuredByteStride * numElements;
	// 1頂点のサイズ
	m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
}
