#include "pch.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Transform.h"

SphereCollider::SphereCollider() : BaseCollider(ColliderType::Sphere)
{

}

SphereCollider::~SphereCollider()
{

}

void SphereCollider::FinalUpdate()
{
    // 물체의 Position을 Center로 취급하여 계산
    _boundingSphere.Center = GetGameObject()->GetTransform()->GetWorldPosition();

    Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
    _boundingSphere.Radius = _radius * max(max(scale.x, scale.y), scale.z);
}

bool SphereCollider::Intersects(Vec4 rayOrigin, Vec4 rayDir, OUT float& distance)
{
    return _boundingSphere.Intersects(rayOrigin, rayDir, OUT distance);
}
