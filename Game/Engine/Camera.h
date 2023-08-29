#pragma once
#include "Component.h"

enum class PROJECTION_TYPE
{
    PERSPECTIVE, // 원근 투영
    ORTHOGRAPHIC, // 직교 투영
};

class Camera : public Component
{
public:
    Camera();
    virtual ~Camera();

    virtual void FinalUpdate() override;
    void Render();

private:
    PROJECTION_TYPE _type = PROJECTION_TYPE::PERSPECTIVE;

    // 카메라가 찍는 정보와 연관
    float _near = 1.f;
    float _far = 1000.f;
    float _fov = XM_PI / 4.f; // field of view (찍는 영역)
    float _scale = 1.f; // 직교 투영에서 활용

    // 카메라가 여러 개 생길 수 있으므로 각기 정보를 갖고 있는다.
    Matrix _matView = {};
    Matrix _matProjection = {};

public:
    // TEMP
    static Matrix S_MatView;
    static Matrix S_MatProjection;
};

