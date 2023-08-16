#pragma once
#include "Component.h"

class Transform;
class MeshRenderer;
class MonoBehaviour;

// Component를 관리하는 조립체
class GameObject : public enable_shared_from_this<GameObject>
{
public:
    GameObject();
    virtual ~GameObject();

    void Init();

    void Awake();
    void Start();
    void Update();
    void LateUpdate();

    shared_ptr<Transform> GetTransform();

    void AddComponent(shared_ptr<Component> component);

private:
    array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> _components;
    // Script들은 개수가 명확하지 않으므로 따로 관리
    vector<shared_ptr<MonoBehaviour>> _scripts;
};
