#include "PrecompiledHeader.h"
#include "TextureLoader.h"

#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"D3D12/D3DCommand/D3DCommand.h"

// Utility
#include"ConvertString/ConvertString.h"

void TextureLoader::Initialize(D3DDevice* d3dDevice, D3DCommand* d3dCommand, ResourceViewManager* rvManager)
{
	d3dDevice_ = d3dDevice;
	d3dCommand_ = d3dCommand;
	rvManager_ = rvManager;
}

void TextureLoader::Load(const std::string& filePath)
{
	// 後でフォルダの中のもの全て別のスレッドで読み込む
	//const std::string& filePath = "Cho\\Resources\\Texture\\monsterBall.png";
    
    std::string fullFilePath;

    if (SystemState::GetInstance().IsEngineTest()) {
        fullFilePath = "EngineTest/Assets/Models/" + filePath;
    }
    else {
        fullFilePath = "Game/Assets/Models/" + filePath;
    }

    std::string fileName = fs::path(fullFilePath).filename().string();

    // 読み込み済みテクスチャを検索
    if (textureData.contains(fileName)) {
        return;
    }

	if (fs::is_regular_file(fullFilePath) == false)
	{
        assert(0);
	}

	// テクスチャデータを追加して書き込む
	Texture& texData = textureData[fileName];

	// 新たなハンドルを取得
	texData.rvIndex = rvManager_->GetNewHandle();

	HRESULT hr;

	// テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(fullFilePath);
	if (filePathW.ends_with(L".dds")) {/*.ddsで終わっていたらddsとみなす。より安全な方法はいくらでもあるがいまのとここれ*/
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	} else {
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
	assert(SUCCEEDED(hr));

	// ミップマップの生成	
	DirectX::ScratchImage mipImages{};

	/*圧縮フォーマットかどうか調べる*/
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		/*圧縮フォーマットならそのまま使うのでmoveする*/
		mipImages = std::move(image);
	} else {
		// テクスチャが小さい場合はミップマップ生成をスキップ
		if (image.GetMetadata().width > 1 && image.GetMetadata().height > 1) {
			hr = DirectX::GenerateMipMaps(
				image.GetImages(),
				image.GetImageCount(),
				image.GetMetadata(),
				DirectX::TEX_FILTER_SRGB,
				0,
				mipImages
			);
		} else {
			// 小さいテクスチャの場合はミップマップを生成せず、そのまま移動
			mipImages = std::move(image);
		}
	}
	assert(SUCCEEDED(hr));

	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	// リソース作成
	d3dCommand_->Reset(CommandType::Copy);

	rvManager_->CreateTextureResource(texData.rvIndex, metadata);
	rvManager_->UploadTextureDataEx(texData.rvIndex, mipImages);

	d3dCommand_->Close(COPY,CommandType::Copy);
	d3dCommand_->Signal(COPY);

	texData.metadata = metadata;

	rvManager_->CreateSRVForTexture2D(texData.rvIndex, metadata.format, UINT(metadata.mipLevels));
}

void TextureLoader::FirstResourceLoad(const std::string& directoryPath)
{
    // directoryPath をフルパスに変換
    //std::string fullDirectoryPath = fs::absolute(directoryPath).string();

    // ファイルパスのリストを取得
    for (const auto& entry : fs::directory_iterator(directoryPath))
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
                //{
                  //  std::unique_lock<std::mutex> lock(textureMutex);
                    // 読み込み済みテクスチャを検索
                    if (textureData.contains(fileName))
                    {
                        continue; // 既に読み込み済みならスキップ
                    }


                    // テクスチャデータを追加して書き込む
                    Texture& texData = textureData[fileName];

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
                    d3dCommand_->Reset(CommandType::Copy);

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