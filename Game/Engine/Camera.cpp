﻿#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"

Matrix Camera::S_MatView;
Matrix Camera::S_MatProjection;

Camera::Camera() : Component(COMPONENT_TYPE::CAMERA)
{
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
    // 실질적으로 행렬을 계산해주는 부분
    _matView = GetTransform()->GetLocalToWorldMatrix().Invert(); // World 행렬에 역행렬을 구하게 되면 View 행렬.

    float width = static_cast<float>(GEngine->GetWindow().width);
    float height = static_cast<float>(GEngine->GetWindow().height);

    if (_type == PROJECTION_TYPE::PERSPECTIVE)
        // Left-Handed 좌표계 사용을 위해 DX 함수 사용
        _matProjection = ::XMMatrixPerspectiveFovLH(_fov, width / height, _near, _far);
    else
        // 직교 투영: 원근법을 적용시키지 않고 물체의 크기대로 보여짐
        _matProjection = ::XMMatrixOrthographicLH(width * _scale, height * _scale, _near, _far);

    S_MatView = _matView;
    S_MatProjection = _matProjection;
}

void Camera::Render()
{
    shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

    // TODO : Layer 구분
    const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

    for (auto& gameObject : gameObjects)
    {
        if (gameObject->GetMeshRenderer() == nullptr)
            continue;

        gameObject->GetMeshRenderer()->Render();
    }
}