﻿#pragma once
#include "Component.h"
#include "Frustum.h"

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

    void SetProjectionType(PROJECTION_TYPE type) { _type = type; }
    PROJECTION_TYPE GetProjectionType() { return _type; }

    void SetCullingMaskLayerOnOff(uint8 layer, bool on)
    {
        if (on)
            _cullingMask |= (1 << layer);
        else
            _cullingMask &= ~(1 << layer);
    }

    void SortGameObject();
    void SortShadowObject();

    void Render_Deferred();
    void Render_Forward();
    void Render_Shadow();

    // 모든 대상을 컬링 (아무 것도 찍지 않는다)
    void SetCullingMaskAll() { SetCullingMask(UINT32_MAX); }
    void SetCullingMask(uint32 mask) { _cullingMask = mask; }
    bool IsCulled(uint8 layer) { return (_cullingMask & (1 << layer)) != 0; }

    void SetNear(float value) { _near = value; }
    void SetFar(float value) { _far = value; }
    void SetFOV(float value) { _fov = value; }
    void SetScale(float value) { _scale = value; }
    void SetWidth(float value) { _width = value; }
    void SetHeight(float value) { _height = value; }

    Matrix& GetViewMatrix() { return _matView; }
    Matrix& GetProjectionMatrix() { return _matProjection; }

private:
    PROJECTION_TYPE _type = PROJECTION_TYPE::PERSPECTIVE;

    // 카메라가 찍는 정보와 연관
    float _near = 1.f;
    float _far = 1000.f;
    float _fov = XM_PI / 4.f; // field of view (찍는 영역)
    float _scale = 1.f; // 직교 투영에서 활용
    float _width = 0.f;
    float _height = 0.f;

    // 카메라가 여러 개 생길 수 있으므로 각기 정보를 갖고 있는다.
    Matrix _matView = {};
    Matrix _matProjection = {};

    Frustum _frustum;
    // 특정 UI만 보여주고 싶으면 사용
    // 1인 경우 컬링 대상.
    uint32 _cullingMask = 0; // 32bit로 컬링 여부 표현

private:
    vector<shared_ptr<GameObject>>	_vecDeferred;
    vector<shared_ptr<GameObject>>	_vecForward;
    vector<shared_ptr<GameObject>>	_vecParticle;
    vector<shared_ptr<GameObject>>	_vecShadow;

public:
    // TEMP
    static Matrix S_MatView;
    static Matrix S_MatProjection;
};

