#pragma once
#include "Object.h"

enum class COMPONENT_TYPE : uint8
{
    TRANSFORM,
    MESH_RENDERER,
    CAMERA,
    LIGHT,
    PARTICLE_SYSTEM,
    TERRAIN,
    COLLIDER,
    // ...
    MONO_BEHAVIOUR, // 반드시 마지막이어야 한다.
    END,
};

// MonoBeviour 제외한 개수 (MonoBeviour는 여러 개를 만들 수 있기 때문에 별도로 생각)
enum
{
    FIXED_COMPONENT_COUNT = static_cast<uint8>(COMPONENT_TYPE::END) - 1
};

class GameObject;
class Transform;

class Component: public Object
{
public:
    Component(COMPONENT_TYPE type);
    // 부모 클래스로 활용될 예정이므로 Memory Leak 예방
    virtual ~Component();

public:
    // 생명주기 관련 함수
    virtual void Awake() { }
    virtual void Start() { }
    virtual void Update() { }
    virtual void LateUpdate() { }
    virtual void FinalUpdate() { }

public:
    COMPONENT_TYPE GetType() { return _type; }
    bool IsValid() { return _gameObject.expired() == false; }

    shared_ptr<GameObject> GetGameObject();
    shared_ptr<Transform> GetTransform();

private:
    friend class GameObject;
    void SetGameObject(shared_ptr<GameObject> gameObject) { _gameObject = gameObject; }

protected:
    COMPONENT_TYPE _type;
    // weak_ptr인 이유는 gameObject에서도 Component를 갖고있을 것이므로 서로 가리키는 형태가 되기 때문에 순환 구조가 되지 않도록 weak_ptr로 사용.
    // shared_ptr로 사용하는 경우에는 GameObject가 Destroy될 때 명시적으로 Pointer를 끊어주는 방법도 가능하다.
    weak_ptr<GameObject> _gameObject;
};
