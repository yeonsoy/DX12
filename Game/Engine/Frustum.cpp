#include "pch.h"
#include "Frustum.h"
#include "Camera.h"

void Frustum::FinalUpdate()
{
    // Projection > View > World ��ǥ�� ����.
    // ����� ���.
    Matrix matViewInv = Camera::S_MatView.Invert();
    Matrix matProjectionInv = Camera::S_MatProjection.Invert();
    Matrix matInv = matProjectionInv * matViewInv;

    // Proejction ��ǥ�迡���� ���� ��ǥ�� ��ȯ
    vector<Vec3> worldPos =
    {
        // ������ǥ�迡�� View ��ǥ�踦 �Ѿ�� ����, 
        // ����Ŭ����ǥ�踦 ��ġ�� ������ Coord �Լ����� w�� 1�� ����.
        ::XMVector3TransformCoord(Vec3(-1.f, 1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, 1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, -1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(-1.f, -1.f, 0.f), matInv),
        ::XMVector3TransformCoord(Vec3(-1.f, 1.f, 1.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, 1.f, 1.f), matInv),
        ::XMVector3TransformCoord(Vec3(1.f, -1.f, 1.f), matInv),
        ::XMVector3TransformCoord(Vec3(-1.f, -1.f, 1.f), matInv)
    };

    // �� ���� �̿��ؼ� Vector�� �����, Normal Vector�� �����.
    // Normal Vector�� �̿��ؼ� �ϳ��� ���� ������ �ϰ� d ���� ���Ѵ�.
    _planes[PLANE_FRONT] = ::XMPlaneFromPoints(worldPos[0], worldPos[1], worldPos[2]); // CW
    _planes[PLANE_BACK] = ::XMPlaneFromPoints(worldPos[4], worldPos[7], worldPos[5]); // CCW
    _planes[PLANE_UP] = ::XMPlaneFromPoints(worldPos[4], worldPos[5], worldPos[1]); // CW
    _planes[PLANE_DOWN] = ::XMPlaneFromPoints(worldPos[7], worldPos[3], worldPos[6]); // CCW
    _planes[PLANE_LEFT] = ::XMPlaneFromPoints(worldPos[4], worldPos[0], worldPos[7]); // CW
    _planes[PLANE_RIGHT] = ::XMPlaneFromPoints(worldPos[5], worldPos[6], worldPos[1]); // CCW
}

// Frustum ���ο� ���� �ִ��� �Ǻ�
bool Frustum::ContainsSphere(const Vec3& pos, float radius)
{
    // BoundingFrustum: DX12���� �⺻ �������ִ� Frustum
    // ������ ������ �׽����ϱ� ���� ������ ���� �����Ͽ� ���.
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
