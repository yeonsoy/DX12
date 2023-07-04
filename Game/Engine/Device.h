﻿#pragma once

// 인력 사무소
// GPU와 직접적으로 핑퐁할 수 있는 부분
// 각종 객체를 생성하거나 하는 부분을 담당, DX의 이전 버전에서는 거의 모든 부분을 담당했었다.
class Device
{
public:
    void Init();

    ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
    ComPtr<ID3D12Device> GetDevice() { return _device; }

private:
    // COM(Component Object Model)
    // - DX의 프로그래밍 언어 독립성과 하위 호환성을 가능하게 하는 기술
    // - COM 객체(COM 인터페이스)를 사용. 세부사항은 우리한테 숨겨짐
    // - Com 객체를 받아주는 ComPtr 일종의 스마트 포인터
    ComPtr<ID3D12Debug>         _debugController;
    ComPtr<IDXGIFactory>        _dxgi; // 화면 관련 기능들
    ComPtr<ID3D12Device>        _device; // 각종 객체 생성
};

