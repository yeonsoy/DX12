#include "pch.h"
#include "Transform.h"
#include "Engine.h"
#include "Camera.h"

Transform::Transform() : Component(COMPONENT_TYPE::TRANSFORM)
{

}

Transform::~Transform()
{

}

void Transform::FinalUpdate()
{
    // WVP 중에서 VP의 경우에는 카메라가 고정이고 1개만 있다고 가정하면 VP는 변하지 않는다.
    // 그러나 World는 물체마다 제각각이므로 각각의 Transform Component에서 계산을 해주어야 한다.
    Matrix matScale = Matrix::CreateScale(_localScale);
    // 이후에는 Quaternion으로 변경해주어야 한다.
    Matrix matRotation = Matrix::CreateRotationX(_localRotation.x);
    matRotation *= Matrix::CreateRotationY(_localRotation.y);
    matRotation *= Matrix::CreateRotationZ(_localRotation.z);
    Matrix matTranslation = Matrix::CreateTranslation(_localPosition);

    _matLocal = matScale * matRotation * matTranslation; // SRT : 부모가 없는 경우 World 행렬을 의미
    _matWorld = _matLocal;

    shared_ptr<Transform> parent = GetParent().lock(); // 부모가 있는 경우
    if (parent != nullptr)
    {
        _matWorld *= parent->GetLocalToWorldMatrix(); // 부모 객체의 WorldMatrix를 추가적으로 곱해야 한다.
    }
}

void Transform::PushGraphicsData()
{
    TransformParams transformParams = {};
    transformParams.matWorld = _matWorld;
    transformParams.matView = Camera::S_MatView;
    transformParams.matProjection = Camera::S_MatProjection;
    transformParams.matWV = _matWorld * Camera::S_MatView;
    transformParams.matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection;

    CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transformParams, sizeof(transformParams));
}
