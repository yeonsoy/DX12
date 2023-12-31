﻿#include "pch.h"
#include "RootSignature.h"
#include "Engine.h"

void RootSignature::Init()
{
    CreateSamplerDesc();
    CreateRootSignature();
}

void RootSignature::CreateRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE ranges[] =
    {
        CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_REGISTER_COUNT - 1, 1), // b1~b4
        CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_REGISTER_COUNT, 0), // t0~t4
    };

    CD3DX12_ROOT_PARAMETER param[2];
    // param[0].InitAsConstantBufferView(0); // 0번 -> b0 -> CBV 
    // 만약 이름을 겹치게 만들더라도 Register Space 인자를 다르게 보내주면 구분이 가능하다.
    // 마지막 인자의 D3D12_SHADER_VISIBILITY_ALL은 모든 렌더링 파이프라인 단계에서 활용할 수 있도록 하는 것이다.
    // param[1].InitAsConstantBufferView(1); // 1번 -> b1 -> CBV
    param[0].InitAsConstantBufferView(static_cast<uint32>(CBV_REGISTER::b0));
    param[1].InitAsDescriptorTable(_countof(ranges), ranges);

    D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(_countof(param), param, 1, &_samplerDesc);
    sigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT; // 입력 조립기 단계

    // D3D12_DEFAULT : 가장 기본 상태의 서명이 들어간다.
    // D3D12_ROOT_SIGNATURE_DESC sigDesc = CD3DX12_ROOT_SIGNATURE_DESC(D3D12_DEFAULT);

    ComPtr<ID3DBlob> blobSignature;
    ComPtr<ID3DBlob> blobError;
    ::D3D12SerializeRootSignature(&sigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blobSignature, &blobError);
    DEVICE->CreateRootSignature(0, blobSignature->GetBufferPointer(), blobSignature->GetBufferSize(), IID_PPV_ARGS(&_signature));
}

void RootSignature::CreateSamplerDesc()
{
    _samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);
}