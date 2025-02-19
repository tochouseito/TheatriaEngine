#pragma once
#include<cstdint>
#include<string>
struct MeshComponent final{
    uint32_t meshID = 0;
    std::string meshesName = "";
    

    void SetMeshID(uint32_t id) {
        meshID = id;
    }
};