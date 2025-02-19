#pragma once
#include<cstdint>
#include<vector>
#include"ConstantData/MeshData.h"
struct ResMeshlet
{
    uint32_t    VertexOffset;           //!< 頂点番号オフセット.
    uint32_t    VertexCount;            //!< 頂点数.
    uint32_t    PrimitiveOffset;        //!< プリミティブ番号オフセット.
    uint32_t    PrimitiveCount;         //!< プリミティブオフセット.
};

struct ResPrimitiveIndex
{
    uint32_t index0 : 10;   //!< 出力頂点番号0 (10bit).
    uint32_t index1 : 10;   //!< 出力頂点番号1 (10bit).
    uint32_t index2 : 10;   //!< 出力頂点番号2 (10bit).
    uint32_t reserved : 2;    //!< 予約領域.
};

///////////////////////////////////////////////////////////////////////////////
// ResMesh structure
///////////////////////////////////////////////////////////////////////////////
//struct ResMesh
//{
//    std::vector<VertexData>     vertices;     //!< 頂点データです.
//    std::vector<uint32_t>       indices;      //!< 頂点インデックスです.
//    uint32_t                    materialId;   //!< マテリアル番号です.
//
//    std::vector<ResMeshlet>         meshlets;               //!< メッシュレット.
//    std::vector<uint32_t>           uniqueVertexIndices;    //!< ユニーク頂点インデックス.
//    std::vector<ResPrimitiveIndex>  primitiveIndices;       //!< プリミティブインデックス.
//};