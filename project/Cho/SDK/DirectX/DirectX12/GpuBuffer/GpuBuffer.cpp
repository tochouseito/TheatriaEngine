#include "pch.h"
#include "GpuBuffer.h"

void StructuredBuffer::CreateResource(ID3D12Device* device, const size_t& sizeInBytes)
{
	// リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = static_cast<UINT64>(sizeInBytes);// リソースのサイズ。今回はVector4を３頂点分
	// バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	// バッファの場合ははこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	GpuBuffer::CreateResource(device, heapProperties, D3D12_HEAP_FLAG_NONE, resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
}

void GpuBuffer::CreateBuffer(ID3D12Device* device, D3D12_HEAP_PROPERTIES& heapProperties, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_STATES InitialState, D3D12_RESOURCE_FLAGS resourceFlags, const UINT& numElementes, const UINT& structureByteStride)
{
	// バッファのサイズを取得
	m_BufferSize = static_cast<UINT64>(numElementes * structureByteStride);
	// 要素数を取得
	m_NumElementes = numElementes;
	// 要素のサイズを取得
	m_StructureByteStride = structureByteStride;
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = m_BufferSize;// リソースのサイズ
	// バッファリソースの設定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = resourceFlags;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	GpuResource::CreateResource(device, heapProperties, heapFlags, resourceDesc, InitialState, nullptr);
}
