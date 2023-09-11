#pragma once
#include "Component.h"
#include "Object.h"

class Transform;
class MeshRenderer;
class Camera;
class Light;
class MonoBehaviour;

// Component를 관리하는 조립체
class GameObject : public Object, public enable_shared_from_this<GameObject>
{
public:
    GameObject();
    virtual ~GameObject();

    void Awake();
    void Start();
    void Update();
    void LateUpdate();
    void FinalUpdate();

    shared_ptr<Component> GetFixedComponent(COMPONENT_TYPE type);

    shared_ptr<Transform> GetTransform();
    shared_ptr<MeshRenderer> GetMeshRenderer();
    shared_ptr<Camera> GetCamera();
    shared_ptr<Light> GetLight();

    void AddComponent(shared_ptr<Component> component);

private:
    array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
    // Script들은 개수가 명확하지 않으므로 따로 관리
    vector<shared_ptr<MonoBehaviour>> _scripts;
};
