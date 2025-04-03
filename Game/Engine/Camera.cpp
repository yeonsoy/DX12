#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Engine.h"
#include "Material.h"
#include "Shader.h"
#include "ParticleSystem.h"

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

    _frustum.FinalUpdate();
}

void Camera::Render()
{
    // 카메라가 여러 대 생길 것이므로 Update 시가 아니라 Render 시 자신의 Matrix로 교체.
    S_MatView = _matView;
    S_MatProjection = _matProjection;

    shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();

    // TODO : Layer 구분
    const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

    for (auto& gameObject : gameObjects)
    {
        if (gameObject->GetMeshRenderer() == nullptr)
            continue;

        if (IsCulled(gameObject->GetLayerIndex()))
            continue;

        if (gameObject->GetCheckFrustum())
        {
            if (_frustum.ContainsSphere(
                gameObject->GetTransform()->GetWorldPosition(),
                gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
            {
                continue;
            }
        }

        gameObject->GetMeshRenderer()->Render();
    }
}

void Camera::SortGameObject()
{
	shared_ptr<Scene> scene = GET_SINGLE(SceneManager)->GetActiveScene();
	const vector<shared_ptr<GameObject>>& gameObjects = scene->GetGameObjects();

	_vecForward.clear();
	_vecDeferred.clear();
	_vecParticle.clear();

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetMeshRenderer() == nullptr && gameObject->GetParticleSystem() == nullptr)
			continue;

		if (IsCulled(gameObject->GetLayerIndex()))
			continue;

		if (gameObject->GetCheckFrustum())
		{
			if (_frustum.ContainsSphere(
				gameObject->GetTransform()->GetWorldPosition(),
				gameObject->GetTransform()->GetBoundingSphereRadius()) == false)
			{
				continue;
			}
		}

		if (gameObject->GetMeshRenderer())
		{
			SHADER_TYPE shaderType = gameObject->GetMeshRenderer()->GetMaterial()->GetShader()->GetShaderType();
			switch (shaderType)
			{
			case SHADER_TYPE::DEFERRED:
				_vecDeferred.push_back(gameObject);
				break;
			case SHADER_TYPE::FORWARD:
				_vecForward.push_back(gameObject);
				break;
			}
		}
		else
		{
			_vecParticle.push_back(gameObject);
		}
	}
}

void Camera::Render_Deferred()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	for (auto& gameObject : _vecDeferred)
	{
		gameObject->GetMeshRenderer()->Render();
	}
}

void Camera::Render_Forward()
{
	S_MatView = _matView;
	S_MatProjection = _matProjection;

	for (auto& gameObject : _vecForward)
	{
		gameObject->GetMeshRenderer()->Render();
	}

	for (auto& gameObject : _vecParticle)
	{
		gameObject->GetParticleSystem()->Render();
	}
}