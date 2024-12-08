﻿#include "pch.h"
#include "Scene.h"
#include "GameObject.h"
#include "Camera.h"
#include "Engine.h"
#include "ConstantBuffer.h"
#include "Light.h"
#include "Engine.h"
#include "Resources.h"

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

    // CommandQueue의 RenderBegin에 있던 부분 이전.
    // SwapChain Group 초기화
    int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
    GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->ClearRenderTargetView(backIndex);
    // Deferred Group 초기화
    GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();
    // Deferred Group 초기화
    GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->ClearRenderTargetView();
    // Lighting Group 초기화
    GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->ClearRenderTargetView();

    // Deferred OMSet
    GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

    // Main Camera만 Deferred 후 Forward / 다른 Camera는 Forward
    shared_ptr<Camera> mainCamera = _cameras[0];
    mainCamera->SortGameObject();
    mainCamera->Render_Deferred();
    GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->WaitTargetToResource();

    RenderLights();
    GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->WaitTargetToResource();

    RenderFinal();

    mainCamera->Render_Forward();

    for (auto& camera : _cameras)
    {
        if (camera == mainCamera)
            continue;

        camera->SortGameObject();
        camera->Render_Forward();
    }
}

void Scene::RenderLights()
{
    GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

    // 광원을 그린다.
    for (auto& light : _lights)
    {
        light->Render();
    }
}

void Scene::RenderFinal()
{
    // Swapchain OMSet
    int8 backIndex = GEngine->GetSwapChain()->GetBackBufferIndex();
    GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets(1, backIndex);

    GET_SINGLE(Resources)->Get<Material>(L"Final")->PushGraphicsData();
    GET_SINGLE(Resources)->Get<Mesh>(L"Rectangle")->Render();
}

void Scene::PushLightData()
{
    LightParams lightParams = {};

    for (auto& light : _lights)
    {
        const LightInfo& lightInfo = light->GetLightInfo();

        light->SetLightIndex(lightParams.lightCount);

        lightParams.lights[lightParams.lightCount] = lightInfo;
        lightParams.lightCount++;
    }

    CONST_BUFFER(CONSTANT_BUFFER_TYPE::GLOBAL)->SetGraphicsGlobalData(&lightParams, sizeof(lightParams));
}

void Scene::AddGameObject(shared_ptr<GameObject> gameObject)
{
    if (gameObject->GetCamera() != nullptr)
    {
        _cameras.push_back(gameObject->GetCamera());
    }
    else if (gameObject->GetLight() != nullptr)
    {
        _lights.push_back(gameObject->GetLight());
    }

    _gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
    if (gameObject->GetCamera())
    {
        auto findIt = std::find(_cameras.begin(), _cameras.end(), gameObject->GetCamera());
        if (findIt != _cameras.end())
            _cameras.erase(findIt);
    }
    else if (gameObject->GetLight())
    {
        auto findIt = std::find(_lights.begin(), _lights.end(), gameObject->GetLight());
        if (findIt != _lights.end())
            _lights.erase(findIt);
    }

    auto findIt = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);
    if (findIt != _gameObjects.end())
        _gameObjects.erase(findIt);
}