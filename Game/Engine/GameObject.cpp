#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "MonoBehaviour.h"

GameObject::GameObject()
{

}

GameObject::~GameObject()
{

}

void GameObject::Init()
{
    // Unity Style로 모든 객체가 Transform을 갖고 있도록 설정.
    AddComponent(make_shared<Transform>());
}

void GameObject::Awake()
{
    for (shared_ptr<Component>& component : _components)
    {
        // Null Check
        if (component)
            component->Awake();
    }

    for (shared_ptr<MonoBehaviour>& script : _scripts)
    {
        script->Awake();
    }
}

void GameObject::Start()
{
    for (shared_ptr<Component>& component : _components)
    {
        if (component)
            component->Start();
    }

    for (shared_ptr<MonoBehaviour>& script : _scripts)
    {
        script->Start();
    }
}

void GameObject::Update()
{
    for (shared_ptr<Component>& component : _components)
    {
        if (component)
            component->Update();
    }

    for (shared_ptr<MonoBehaviour>& script : _scripts)
    {
        script->Update();
    }
}

void GameObject::LateUpdate()
{
    for (shared_ptr<Component>& component : _components)
    {
        if (component)
            component->LateUpdate();
    }

    for (shared_ptr<MonoBehaviour>& script : _scripts)
    {
        script->LateUpdate();
    }
}

shared_ptr<Transform> GameObject::GetTransform()
{
    uint8 index = static_cast<uint8>(COMPONENT_TYPE::TRANSFORM);
    return static_pointer_cast<Transform>(_components[index]);
}

void GameObject::AddComponent(shared_ptr<Component> component)
{
    // enable_shared_from_this를 하지 않는 경우 shared_from_this를 사용할 수 없다.
    // this는 스마트포인터가 아니라 넘겨줄 수 없다.
    // make_shared<GameObject>(this) 같은 방식으로 만들 경우엔 문제 발생.
    // enable_shared_from_this는 자기 자신의 weak_ptr를 들고있도록 만든다.
    // make_shared<GameObject>(weak_ptr)을 넘겨준다.
    component->SetGameObject(shared_from_this());

    uint8 index = static_cast<uint8>(component->GetType());
    if (index < FIXED_COMPONENT_COUNT)
    {
        _components[index] = component;
    }
    else
    {
        // 스마트 포인터끼리 캐스팅할 때 Dynamic 캐스팅을 쓰고 싶을 때 사용
        _scripts.push_back(dynamic_pointer_cast<MonoBehaviour>(component));
    }
}
