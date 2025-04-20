#include "pch.h"
#include "Input.h"
#include "Engine.h"

void Input::Init(HWND hwnd)
{
    _hwnd = hwnd;
    // 입력을 받지 않은 상태(NONE)으로 초기화
    _states.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
}

void Input::Update()
{
    HWND hwnd = ::GetActiveWindow();
    if (_hwnd != hwnd) // 현재 사용하는 Window인지 확인
    {
        for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
            _states[key] = KEY_STATE::NONE;

        return;
    }

    BYTE asciiKeys[KEY_TYPE_COUNT] = {};
    // 256개의 BYTE Array를 받아서 각 Virtual Key의 State를 받아온다.
    // high-order byte가 1이면(=1000 0000) Key가 눌린 상태이다.
    if (::GetKeyboardState(asciiKeys) == false)
        return;

    for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
    {
        // 키가 눌려 있으면 true
        if (asciiKeys[key] & 0x80)
        {
            KEY_STATE& state = _states[key];

            // 이전 프레임에 키를 누른 상태라면 PRESS
            if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
                state = KEY_STATE::PRESS;
            else
                state = KEY_STATE::DOWN;
        }
        else
        {
            KEY_STATE& state = _states[key];

            // 이전 프레임에 키를 누른 상태라면 UP
            if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
                state = KEY_STATE::UP;
            else
                state = KEY_STATE::NONE;
        }
    }

    ::GetCursorPos(&_mousePos);
    ::ScreenToClient(GEngine->GetWindow().hwnd, &_mousePos);
}