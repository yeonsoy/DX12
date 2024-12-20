﻿#pragma once

// [계약서 / 결재]

// CPU [        ]    GPU [        ]
// 외주를 맡기고 싶을 때 RAM에 있는 데이터를 GPU를 넘겨줘야 한다.
// 외주할 일이 어떤 것인지, 무엇을 대상으로 가공해야할 지도 알려줘야 한다.

class RootSignature
{
public:
    void Init();

    ComPtr<ID3D12RootSignature>	GetGraphicsRootSignature() { return _graphicsRootSignature; }
    ComPtr<ID3D12RootSignature>	GetComputeRootSignature() { return _computeRootSignature; }

private:
    void CreateGraphicsRootSignature();
    void CreateComputeRootSignature();

private:
    // ComPtr로 되어있는 거의 대부분의 모든 부분들은 GPU에게 요청을 하는 것이다.
    // 내부에서 실질적으로 객체가 만들어진다.
    D3D12_STATIC_SAMPLER_DESC _samplerDesc;

    ComPtr<ID3D12RootSignature>	_graphicsRootSignature;
    ComPtr<ID3D12RootSignature>	_computeRootSignature;
};

