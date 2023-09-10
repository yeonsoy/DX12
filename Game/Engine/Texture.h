#pragma once
#include "Object.h"

class Texture : public Object
{
public:
    Texture();
    virtual ~Texture();

    void Init(const wstring& path);

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() { return _srvHandle; }

public:
    void CreateTexture(const wstring& path);
    void CreateView();

private:
    ScratchImage                    _image;
    ComPtr<ID3D12Resource>          _tex2D;

    ComPtr<ID3D12DescriptorHeap>    _srvHeap; // Constant Buffer와 유사
    D3D12_CPU_DESCRIPTOR_HANDLE     _srvHandle = {}; // 한 번만 로딩하고 계속 사용하므로 Handle 사용
};

