#pragma once

enum class OBJECT_TYPE : uint8
{
    NONE,
    GAMEOBJECT, // Unity: Prefab, Unreal: Blueprint
    COMPONENT,
    MATERIAL,
    MESH,
    SHADER,
    TEXTURE,

    END
};

enum
{
    OBJECT_TYPE_COUNT = static_cast<uint8>(OBJECT_TYPE::END)
};

class Object
{
public:
    Object(OBJECT_TYPE type);
    virtual ~Object();

    OBJECT_TYPE GetType() { return _objectType; }

    void SetName(const wstring& name) { _name = name; }
    const wstring& GetName() { return _name; }

    // Object를 복사하는 역할
    // TODO : Instantiate

protected:
    // Resource는 파일에 저장하고 불러올 수 있어야 한다.
    friend class Resources;
    virtual void Load(const wstring& path) { }
    virtual void Save(const wstring& path) { }

protected:
    OBJECT_TYPE _objectType = OBJECT_TYPE::NONE;
    wstring _name;
};
