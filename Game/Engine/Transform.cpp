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

void Transform::PushData()
{
    TransformParams transformParams = {};
    transformParams.matWorld = _matWorld;
    transformParams.matView = Camera::S_MatView;
    transformParams.matProjection = Camera::S_MatProjection;
    transformParams.matWV = _matWorld * Camera::S_MatView;
    transformParams.matWVP = _matWorld * Camera::S_MatView * Camera::S_MatProjection;
	transformParams.matViewInv = Camera::S_MatView.Invert();

    CONST_BUFFER(CONSTANT_BUFFER_TYPE::TRANSFORM)->PushGraphicsData(&transformParams, sizeof(transformParams));
}

// Look 벡터를 바라보는 회전 행렬 구하기
void Transform::LookAt(const Vec3& dir)
{
	Vec3 front = dir;
	front.Normalize();

	Vec3 right = Vec3::Up.Cross(dir);
	if (right == Vec3::Zero)
		right = Vec3::Forward.Cross(dir);

	right.Normalize();

	Vec3 up = front.Cross(right);
	up.Normalize();

	Matrix matrix = XMMatrixIdentity();
	matrix.Right(right);
	matrix.Up(up);
	// 오른쪽 좌표계를 사용하기 때문에 front를 Backward에 넣어준다.
	matrix.Backward(front);

	_localRotation = DecomposeRotationMatrix(matrix);
}

bool Transform::CloseEnough(const float& a, const float& b, const float& epsilon)
{
	return (epsilon > std::abs(a - b));
}

Vec3 Transform::DecomposeRotationMatrix(const Matrix& rotation)
{
	Vec4 v[4];
	XMStoreFloat4(&v[0], rotation.Right());
	XMStoreFloat4(&v[1], rotation.Up());
	XMStoreFloat4(&v[2], rotation.Backward());
	XMStoreFloat4(&v[3], rotation.Translation());

	Vec3 ret;
	if (CloseEnough(v[0].z, -1.0f))
	{
		float x = 0;
		float y = XM_PI / 2;
		float z = x + atan2(v[1].x, v[2].x);
		ret = Vec3{ x, y, z };
	}
	else if (CloseEnough(v[0].z, 1.0f))
	{
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2(-v[1].x, -v[2].x);
		ret = Vec3{ x, y, z };
	}
	else
	{
		float y1 = -asin(v[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(v[1].z / cos(y1), v[2].z / cos(y1));
		float x2 = atan2f(v[1].z / cos(y2), v[2].z / cos(y2));

		float z1 = atan2f(v[0].y / cos(y1), v[0].x / cos(y1));
		float z2 = atan2f(v[0].y / cos(y2), v[0].x / cos(y2));

		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
		{
			ret = Vec3{ x1, y1, z1 };
		}
		else
		{
			ret = Vec3{ x2, y2, z2 };
		}
	}

	return ret;
}