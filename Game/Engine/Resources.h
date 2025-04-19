#pragma once

#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class Resources
{
    DECLARE_SINGLE(Resources);

public:
    void Init();

    // template는 헤더 파일에 전부 만드는 경우가 많다.
    // 그러다보니 ini 파일로 따로 빼는 경우도 많다.
    template<typename T>
    shared_ptr<T> Load(const wstring& key, const wstring& path);

    template<typename T>
    bool Add(const wstring& key, shared_ptr<T> object);

    template<typename T>
    shared_ptr<T> Get(const wstring& Key);

    template<typename T>
    OBJECT_TYPE GetObjectType();

    shared_ptr<Mesh> LoadPointMesh();
    shared_ptr<Mesh> LoadRectangleMesh();
    shared_ptr<Mesh> LoadCubeMesh();
    shared_ptr<Mesh> LoadSphereMesh();
    shared_ptr<Mesh> LoadTerrainMesh(int32 sizeX = 15, int32 sizeZ = 15);

    shared_ptr<Texture> CreateTexture(const wstring& name, DXGI_FORMAT format, uint32 width, uint32 height,
        const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
        D3D12_RESOURCE_FLAGS resFlags = D3D12_RESOURCE_FLAG_NONE, Vec4 clearColor = Vec4());

    shared_ptr<Texture> CreateTextureFromResource(const wstring& name, ComPtr<ID3D12Resource> tex2D);

private:
    void CreateDefaultShader();
    void CreateDefaultMaterial();

private:
    // Resources 자체는 배열이지만 각 타입에 따른 Map으로 이루어진 배열 생성.
    using KeyObjMap = std::map<wstring/*key*/, shared_ptr<Object>>;
    array<KeyObjMap, OBJECT_TYPE_COUNT> _resources;
};

template<typename T>
inline shared_ptr<T> Resources::Load(const wstring& key, const wstring& path)
{
    OBJECT_TYPE objectType = GetObjectType<T>();
    KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

    auto findIt = keyObjMap.find(key);
    if (findIt != keyObjMap.end())
        return static_pointer_cast<T>(findIt->second);

    shared_ptr<T> object = make_shared<T>();
    object->Load(path); // 리소스 파일의 경로를 읽어온다.
    keyObjMap[key] = object;

    return object;
}

template<typename T>
bool Resources::Add(const wstring& key, shared_ptr<T> object)
{
    OBJECT_TYPE objectType = GetObjectType<T>();
    KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

    auto findIt = keyObjMap.find(key);
    if (findIt != keyObjMap.end())
        return false;

    keyObjMap[key] = object;

    return true;
}

template<typename T>
shared_ptr<T> Resources::Get(const wstring& key)
{
    OBJECT_TYPE objectType = GetObjectType<T>();
    // 원본 데이터(&)를 가져온다.
    KeyObjMap& keyObjMap = _resources[static_cast<uint8>(objectType)];

    auto findIt = keyObjMap.find(key);
    if (findIt != keyObjMap.end())
        return static_pointer_cast<T>(findIt->second);

    return nullptr;
}

template<typename T>
// class type이 주어졌을 때 complie 타임에 타입을 반환해준다.
// 객체를 만들기 전에 class type만 보고 타입을 알아오기 위한 함수.
inline OBJECT_TYPE Resources::GetObjectType()
{
    if (std::is_same_v<T, GameObject>)
        return OBJECT_TYPE::GAMEOBJECT;
    else if (std::is_same_v<T, Material>)
        return OBJECT_TYPE::MATERIAL;
    else if (std::is_same_v<T, Mesh>)
        return OBJECT_TYPE::MESH;
    else if (std::is_same_v<T, Shader>)
        return OBJECT_TYPE::SHADER;
    else if (std::is_same_v<T, Texture>)
        return OBJECT_TYPE::TEXTURE;
    else if (std::is_convertible_v<T, Component>)
        return OBJECT_TYPE::COMPONENT;
    else
        return OBJECT_TYPE::NONE;
}
