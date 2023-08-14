#include "pch.h"
#include "Timer.h"

void Timer::Init()
{
    // GetTickCount64 함수를 통해 Tick을 가져와서 시간 계산 가능
    // 하지만 정밀도가 떨어져서 QueryPerformanceFrequency를 사용.
    // 어떤 시간을 기준으로 상대 시간을 계산할 수 있으면 된다.
    ::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
    ::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount)); // CPU 클럭
}

void Timer::Update()
{
    uint64 currentCount;
    ::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

    // 단위는 초 (seconds)
    _deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
    _prevCount = currentCount;

    _frameCount++;
    // 누적 시간 계산
    _frameTime += _deltaTime;

    // 매 초마다 계산
    if (_frameTime > 1.f)
    {
        _fps = static_cast<uint32>(_frameCount / _frameTime);

        _frameTime = 0.f;
        _frameCount = 0;
    }
}