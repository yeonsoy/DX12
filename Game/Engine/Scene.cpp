#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"

void Scene::Awake()
{
    // shared_ptr 특성상 reference로 받지 않는 경우에는 복사가 일어나서 reference count가 1개씩 올라간다.
    for (const shared_ptr<GameObject>& gameObject : _gameObjects)
    {
        gameObject->Awake();
    }
}

void Scene::Start()
{
    for (const shared_ptr<GameObject>& gameObject : _gameObjects)
    {
        gameObject->Start();
    }
}

void Scene::Update()
{
    for (const shared_ptr<GameObject>& gameObject : _gameObjects)
    {
        gameObject->Update();
    }
}

void Scene::LateUpdate()
{
    for (const shared_ptr<GameObject>& gameObject : _gameObjects)
    {
        gameObject->LateUpdate();
    }
}

void Scene::FinalUpdate()
{
    for (const shared_ptr<GameObject>& gameObject : _gameObjects)
    {
        gameObject->FinalUpdate();
    }
}

void Scene::Render()
{
    PushLightData();

    for (auto& gameObject : _gameObjects)
    {
        if (gameObject->GetCamera() == nullptr)
            continue;

        gameObject->GetCamera()->Render();
    }
}

void Scene::PushLightData()
{
    LightParams lightParams = {};

    for (auto& gameObject : _gameObjects)
    {
        if (gameObject->GetLight() == nullptr)
            continue;

        const LightInfo& lightInfo = gameObject->GetLight()->GetLightInfo();

        lightParams.lights[lightParams.lightCount] = lightInfo;
        lightParams.lightCount++;
    }

    CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
    _gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
    auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
    if (findIt != _gameObjects.end())
    {
        _gameObjects.erase(findIt);
    }
}