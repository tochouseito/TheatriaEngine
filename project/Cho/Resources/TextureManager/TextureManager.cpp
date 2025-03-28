#include "pch.h"
#include "TextureManager.h"
#include "Cho/Graphics/GraphicsEngine/GraphicsEngine.h"
#include "Cho/Resources/ResourceManager/ResourceManager.h"

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
            std::string fileName = entry.path().filename().string();

            // テクスチャファイルの形式をチェック（.dds, .png, .jpg など）
            if (fileName.ends_with(".dds") || fileName.ends_with(".png") || fileName.ends_with(".jpg"))
            {
                // 読み込み済みテクスチャを検索
                if (m_TextureNameContainer.contains(fileName))
                {
                    continue; // 既に読み込み済みならスキップ
                }
                // テクスチャデータを追加して書き込む
                Texture& texData = textureData[fileName];
                TextureData texData;
                texData.name = fileName;

                // 新たなハンドルを取得
                texData.rvIndex = rvManager_->GetNewHandle();

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
                assert(SUCCEEDED(hr));

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

                // リソース作成
                CommandContext* context = m_GraphicsEngine->GetCommandContext();
                m_GraphicsEngine->BeginCommandContext(context);
				BUFFER_TEXTURE_DESC desc = {};

                rvManager_->CreateTextureResource(texData.rvIndex, metadata);
                rvManager_->UploadTextureDataEx(texData.rvIndex, mipImages);

                d3dCommand_->Close(COPY, CommandType::Copy);
                d3dCommand_->Signal(COPY);

                texData.metadata = metadata;

                rvManager_->CreateSRVForTexture2D(texData.rvIndex, metadata.format, UINT(metadata.mipLevels));
                //}
            }
        }
    }
}
