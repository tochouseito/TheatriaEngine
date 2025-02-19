#include "PrecompiledHeader.h"
#include "SpriteSystem.h"
#include"SystemState/SystemState.h"
#include"D3D12/ResourceViewManager/ResourceViewManager.h"
#include"Load/TextureLoader/TextureLoader.h"

void SpriteSystem::Initialize(EntityManager& entityManager, ComponentManager& componentManager)
{
    for (Entity entity : entityManager.GetActiveEntities()) {
		if (componentManager.GetComponent<SpriteComponent>(entity)) {
			SpriteComponent& sprite = componentManager.GetComponent<SpriteComponent>(entity).value();
			UpdateMatrix(sprite);
		}
    }
}

void SpriteSystem::Update(EntityManager& entityManager, ComponentManager& componentManager)
{
    for (Entity entity : entityManager.GetActiveEntities()) {
        if (componentManager.GetComponent<SpriteComponent>(entity)) {
            SpriteComponent& sprite = componentManager.GetComponent<SpriteComponent>(entity).value();
            UpdateMatrix(sprite);
        }
    }
}

void SpriteSystem::UpdateMatrix(SpriteComponent& comp)
{
    // テクスチャが変更されていたら
    if (comp.material.textureID != comp.material.preTexID) {
        Vector2 size = texLoader_->GetTextureSize(comp.material.textureID);
        comp.textureSize = size;
        comp.size = size;
    }

    float left = 0.0f - comp.anchorPoint.x;
    float right = comp.size.x - comp.anchorPoint.x;
    float top = 0.0f - comp.anchorPoint.y;
    float bottom = comp.size.y - comp.anchorPoint.y;

    float tex_left = comp.textureLeftTop.x / comp.size.x;
    float tex_right = (comp.textureLeftTop.x + comp.size.x) / comp.size.x;
    float tex_top = comp.textureLeftTop.y / comp.size.y;
    float tex_bottom = (comp.textureLeftTop.y + comp.size.y) / comp.size.y;

    SetVertexData(comp, left, right, top, bottom, tex_left, tex_right, tex_top, tex_bottom);

    //comp.scale = comp.size;

    Matrix4 worldMatrixSprite = ChoMath::MakeAffineMatrix(Vector3(comp.scale.x, comp.scale.y, 1.0f), Vector3(0.0f, 0.0f, comp.rotation), Vector3(comp.position.x, comp.position.y, 0.0f));

    Matrix4 viewMatrixSprite = ChoMath::MakeIdentity4x4();

    Matrix4 projectionMatrixSprite = ChoMath::MakeOrthographicMatrix(0.0f, 0.0f,
        static_cast<float>(WindowWidth()), static_cast<float>(WindowHeight()),
        0.0f, 100.0f
    );

    Matrix4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));

    comp.matWorld = worldViewProjectionMatrixSprite;

    comp.material.matUV = MakeAffineMatrix(Vector3(comp.uvScale.x, comp.uvScale.y, 1.0f), Vector3(0.0f, 0.0f, comp.uvRot), Vector3(comp.uvPos.x, comp.uvPos.y, 0.0f));



    // 次のフレーム用にテクスチャIDを保存
    comp.material.preTexID = comp.material.textureID;

    // 行列の転送
    TransferMatrix(comp);
}

void SpriteSystem::TransferMatrix(SpriteComponent& comp)
{
    comp.constData->matWorld = comp.matWorld;

    // マテリアル
    if (comp.material.constData) {
        comp.material.constData->color = comp.material.color;
        comp.material.constData->matUV = comp.material.matUV;
    }
}

void SpriteSystem::SetVertexData(SpriteComponent& comp, const float& left, const float& right, const float& top, const float& bottom, const float& tex_left, const float& tex_right, const float& tex_top, const float& tex_bottom)
{
   SpriteMeshData* data= rvManager_->GetSpriteData(comp.spriteIndex);

   if (!data) {
       return;
   }
   
   data->vertexData[0].position = { left,bottom,0.0f,1.0f};// 左下
   data->vertexData[0].texcoord = { tex_left,tex_bottom };
   
   data->vertexData[1].position = { left,top};// 左上
   data->vertexData[1].texcoord = { tex_left,tex_top };
   
   data->vertexData[2].position = { right,bottom,0.0f,1.0f };// 右下
   data->vertexData[2].texcoord = { tex_right,tex_bottom };
   
   data->vertexData[3].position = { right,top,0.0f,1.0f };// 左上
   data->vertexData[3].texcoord = { tex_right,tex_top };
   
}
