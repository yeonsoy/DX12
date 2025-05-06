#pragma once
#include "Component.h"

class Mesh;
class Material;

// [Mesh ID][Material ID]
// [32][32]
union InstanceID
{
    struct
    {
        uint32 meshID;
        uint32 materialID;
    };
    uint64 id;
};

class MeshRenderer : public Component
{
public:
    MeshRenderer();
    virtual ~MeshRenderer();

    shared_ptr<Material> GetMaterial(uint32 idx = 0) { return _materials[idx]; }

    void SetMesh(shared_ptr<Mesh> mesh) { _mesh = mesh; }
    void SetMaterial(shared_ptr<Material> material, uint32 idx = 0);

    void Render();
    void Render(shared_ptr<class InstancingBuffer>& buffer);
    // Shadow의 경우 일단 Instancing을 고려하지 않고 생성
    void RenderShadow();

    uint64 GetInstanceID();

private:
    shared_ptr<Mesh> _mesh;
    vector<shared_ptr<Material>> _materials;
};
