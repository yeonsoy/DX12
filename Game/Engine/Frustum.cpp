#include "pch.h"
#include "Frustum.h"
#include "Camera.h"

void Frustum::FinalUpdate()
{
    // Projection > View > World 좌표로 역산.
    // 역행렬 사용.
    Matrix matViewInv = Camera::S_MatView.Invert();
    Matrix matProjectionInv = Camera::S_MatProjection.Invert();
    Matrix matInv = matProjectionInv * matViewInv;

    // Proejction 좌표계에서의 정점 좌표를 변환
    vector<Vec3> worldPos =
    {
        // 투영좌표계에서 View 좌표계를 넘어가기 이전, 
        // 동차클립좌표계를 거치기 때문에 Coord 함수에서 w를 1로 설정.
        ::XMVector3TransformCoord(Vec3(-1.f, 1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, 1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, -1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(-1.f, -1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(-1.f, 1.f, 1.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, 1.f, 1.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, -1.f, 1.f), matInv),
        ::XMVector3TransformCoord(Vec3(-1.f, -1.f, 1.f), matInv)
    };

    // 두 점을 이용해서 Vector를 만들고, Normal Vector를 만든다.
    // Normal Vector를 이용해서 하나의 점과 내적을 하고 d 값을 구한다.
    _planes[PLANE_FRONT] = ::XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]); // CW
    _planes[PLANE_BACK] = ::XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]); // CCW
    _planes[PLANE_UP] = ::XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]); // CW
    _planes[PLANE_DOWN] = ::XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]); // CCW
    _planes[PLANE_LEFT] = ::XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]); // CW
    _planes[PLANE_RIGHT] = ::XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]); // CCW
}

// Frustum 내부에 점이 있는지 판별
bool Frustum::ContainsSphere(const Vec3& pos, float radius)
{
    // BoundingFrustum: DX12에서 기본 제공해주는 Frustum
    // 수학적 연산을 테스팅하기 위해 당장은 직접 구현하여 사용.
    for (const Vec4& plane : _planes)
    {
        // n = (a, b, c)
        Vec3 normal = Vec3(plane.x, plane.y, plane.z);

        // ax + by + cz + d > radius
        if (normal.Dot(pos) + plane.w > radius)
            return false;
    }

    return true;
}
