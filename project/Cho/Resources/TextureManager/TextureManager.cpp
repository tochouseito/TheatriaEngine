#include "pch.h"
#include "TextureManager.h"
#include "Cho/Resources/ResourceManager/ResourceManager.h"
#include "Cho/Graphics/GraphicsEngine/GraphicsEngine.h"

void TextureManager::LoadEngineTexture()
{
    // テクスチャデータの読み込み
    std::string path = "Cho/Resources/Texture";
    // ファイルパスのリストを取得
    for (const auto& entry : fs::directory_iterator(path))
    {
        // ファイルかどうかを確認し、DDS または WIC 画像フォーマットかどうかチェック
        if (fs::is_regular_file(entry.path()))
        {
            std::string filePath = entry.path().string();

            // ファイル名部分のみ取得（ディレクトリパスを除去）
            std::wstring fileName = entry.path().filename().wstring();

            // テクスチャファイルの形式をチェック（.dds, .png, .jpg など）
            if (fileName.ends_with(L".dds") || fileName.ends_with(L".png") || fileName.ends_with(L".jpg"))
            {
                // 読み込み済みテクスチャを検索
                if (m_TextureNameContainer.contains(fileName))
                {
                    continue; // 既に読み込み済みならスキップ
                }
                HRESULT hr;
                // テクスチャファイルを読んでプログラムで扱えるようにする
                DirectX::ScratchImage image{};
                std::wstring filePathW = ConvertString(filePath);

                if (filePathW.ends_with(L".dds"))
                {
                    // .dds形式のファイルを読み込む
                    hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
                } else
                {
                    // 他のフォーマット (.png, .jpg など) を読み込む
                    hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
                }
				ChoAssertLog("Failed to load texture", hr, __FILE__, __LINE__);

                // ミップマップの生成
                DirectX::ScratchImage mipImages{};

                // 圧縮フォーマットかどうか調べる
                if (DirectX::IsCompressed(image.GetMetadata().format))
                {
                    // 圧縮フォーマットならそのまま使うのでmoveする
                    mipImages = std::move(image);
                } else
                {
                    // テクスチャが小さい場合はミップマップ生成をスキップ
                    if (image.GetMetadata().width > 1 && image.GetMetadata().height > 1)
                    {
                        hr = DirectX::GenerateMipMaps(
                            image.GetImages(),
                            image.GetImageCount(),
                            image.GetMetadata(),
                            DirectX::TEX_FILTER_SRGB,
                            0,
                            mipImages
                        );
                    } else
                    {
                        // 小さいテクスチャの場合はミップマップを生成せず、そのまま移動
                        mipImages = std::move(image);
                    }
                }
                assert(SUCCEEDED(hr));

                const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

                // Resource作成
                BUFFER_TEXTURE_DESC desc = {};
                desc.width = static_cast<UINT>(metadata.width);// Textureの幅
                desc.height = static_cast<UINT>(metadata.height);// Textureの高さ
                desc.mipLevels = static_cast<UINT16>(metadata.mipLevels);// mipmapの数
                desc.arraySize = static_cast<UINT16>(metadata.arraySize);// 奥行き or 配列Textureの配列数
                desc.format = metadata.format;// Textureのフォーマット
                desc.dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);// Textureの次元
                desc.state = D3D12_RESOURCE_STATE_COPY_DEST;// 読むだけなのでCOPY_DEST
                TextureData texData;
                texData.name = fileName;// テクスチャ名
                texData.bufferIndex = m_ResourceManager->CreateTextureBuffer(desc);// テクスチャバッファの作成
                texData.metadata = metadata;// メタデータ
                // テクスチャデータをコピー
                CommandContext* context = m_GraphicsEngine->GetCommandContext();
                UploadTextureDataEx(
                    context,
                    m_ResourceManager->GetBufferManager()->GetTextureBuffer(texData.bufferIndex)->GetResource(),
                    mipImages
                );
                // テクスチャデータを追加
                m_TextureNameContainer[fileName] = static_cast<uint32_t>(m_Textures.push_back(std::move(texData)));
            }
        }
    }
}

void TextureManager::UploadTextureDataEx(CommandContext* context, ID3D12Resource* resource, const DirectX::ScratchImage& mipImages)
{
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(m_Device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    UINT64 intermediateSize = GetRequiredIntermediateSize(resource, 0, static_cast<UINT>(subresources.size()));
    // Resourceの各種設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;// UploadHeapを使う
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Width = intermediateSize;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    ComPtr<ID3D12Resource> intermediate = nullptr;
    m_ResourceManager->CreateGPUResource(intermediate, heapProperties, resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
    // メモリにデータをコピー
    m_GraphicsEngine->BeginCommandContext(context);
    UpdateSubresources(
        context->GetCommandList(),
        resource,
        intermediate.Get(),
        0, 0,
        static_cast<UINT>(subresources.size()),
        subresources.data()
    );
    // Textureへの転送後は利用できるよう,
    // D3D12_RESOURCE_STATE_COPY_DESTから
    // D3D12_RESOURCE_STATE_GENERIC_READへ
    // ResourceStateを変更する
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = resource;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    context->ResourceBarrier(1, &barrier);
    m_GraphicsEngine->EndCommandContext(context, QueueType::Copy);
    m_GraphicsEngine->WaitForGPU(QueueType::Copy);
}

// ダミーテクスチャバッファを取得
TextureBuffer* TextureManager::GetDummyTextureBuffer()
{
    std::wstring fileName = L"Dummy202411181622.png";
    if (!m_TextureNameContainer.contains(fileName))
    {
        ChoAssertLog("DummyTexture is not found", false, __FILE__, __LINE__);
    }
    uint32_t index = m_TextureNameContainer[fileName];
    return m_ResourceManager->GetBufferManager()->GetTextureBuffer(m_Textures[index].bufferIndex);
}
