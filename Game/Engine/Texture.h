﻿#pragma once
#include "Object.h"

class Texture : public Object
{
public:
    Texture();
    virtual ~Texture();

    // 파일에서 Texture를 생성할 때 활용
    virtual void Load(const wstring& path) override;

public:
    // 처음부터 Texture를 생성
    void Create(DXGI_FORMAT format, uint32 width, uint32 height,
        const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
        D3D12_RESOURCE_FLAGS resFlags, Vec4 clearColor = Vec4());

    // Resource로 부터 Texture를 생성 (EX. SwapChain)
    void CreateFromResource(ComPtr<ID3D12Resource> tex2D);

    float GetWidth() { return static_cast<float>(_desc.Width); }
    float GetHeight() { return static_cast<float>(_desc.Height); }

public:
    ComPtr<ID3D12Resource> GetTex2D() { return _tex2D; }
    ComPtr<ID3D12DescriptorHeap> GetSRV() { return _srvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetRTV() { return _rtvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetDSV() { return _dsvHeap; }
    ComPtr<ID3D12DescriptorHeap> GetUAV() { return _uavHeap; }

    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVHandle() { return _srvHeapBegin; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetUAVHandle() { return _uavHeapBegin; }

private:
    ScratchImage                    _image;
    D3D12_RESOURCE_DESC				_desc;
    ComPtr<ID3D12Resource>          _tex2D;

    ComPtr<ID3D12DescriptorHeap>    _srvHeap; // Constant Buffer와 유사
    ComPtr<ID3D12DescriptorHeap>	_rtvHeap;
    ComPtr<ID3D12DescriptorHeap>	_dsvHeap;
    ComPtr<ID3D12DescriptorHeap>	_uavHeap;
    
private:
    D3D12_CPU_DESCRIPTOR_HANDLE     _srvHeapBegin = {}; // 한 번만 로딩하고 계속 사용하므로 Handle 사용
    D3D12_CPU_DESCRIPTOR_HANDLE     _uavHeapBegin = {}; 
};

