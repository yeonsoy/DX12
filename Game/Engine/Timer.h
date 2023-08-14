#pragma once

class Timer
{
public:
    void Init();
    void Update();

    uint32 GetFps() { return _fps; } // Frame Per Second, 평균적으로 몇 프레임에 실행되었는 지
    float GetDeltaTime() { return _deltaTime; } // 이전 프레임부터 현재 프레임까지 경과된 시간

private:
    uint64	_frequency = 0;
    uint64	_prevCount = 0;
    float	_deltaTime = 0.f;

private:
    uint32	_frameCount = 0;
    float	_frameTime = 0.f;
    uint32	_fps = 0;
};

