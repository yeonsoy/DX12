﻿#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"

#include "TestCameraScript.h"
#include "Resources.h"

void SceneManager::Update()
{
    if (_activeScene == nullptr)
        return;

    _activeScene->Update();
    _activeScene->LateUpdate();
    _activeScene->FinalUpdate();
}

// TEMP
void SceneManager::Render()
{
    if (_activeScene)
        _activeScene->Render();
}

void SceneManager::LoadScene(wstring sceneName)
{
    // TODO : 기존 Scene 정리
    // TODO : 파일에서 Scene 정보 로드

    _activeScene = LoadTestScene();

    _activeScene->Awake();
    _activeScene->Start();
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
    shared_ptr<Scene> scene = make_shared<Scene>();

#pragma region Camera
    shared_ptr<GameObject> camera = make_shared<GameObject>();
    camera->AddComponent(make_shared<Transform>());
    camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45도
    camera->AddComponent(make_shared<TestCameraScript>());
    camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
    scene->AddGameObject(camera);
#pragma endregion

#pragma region Cube
	{
		shared_ptr<GameObject> sphere = make_shared<GameObject>();
		sphere->AddComponent(make_shared<Transform>());
		sphere->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		sphere->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 150.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadCubeMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Shader> shader = make_shared<Shader>();
			shared_ptr<Texture> texture = make_shared<Texture>();
			shared_ptr<Texture> texture2 = make_shared<Texture>();
			shader->Init(L"..\\Resources\\Shader\\default.hlsli");
			texture->Init(L"..\\Resources\\Texture\\Stone_Floor.jpg");
			texture2->Init(L"..\\Resources\\Texture\\Stone_Floor_Normal.jpg");
			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			material->SetTexture(1, texture2);
			meshRenderer->SetMaterial(material);
		}
		sphere->AddComponent(meshRenderer);
		scene->AddGameObject(sphere);
	}
#pragma endregion

#pragma region White Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(1.f, 0.f, 1.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(0.5f, 0.5f, 0.5f));
		light->GetLight()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));

		scene->AddGameObject(light);
	}

#pragma endregion

    return scene;
}