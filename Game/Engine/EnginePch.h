#pragma once

// std::byte 사용하지 않음
#define _HAS_STD_BYTE 0

// 각종 include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
using namespace std;

#include <filesystem>
namespace fs = std::filesystem;

#include "d3dx12.h"
#include "SimpleMath.h"
#include <d3d12.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace Microsoft::WRL;

#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>

// 각종 lib
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex\\DirectXTex_debug.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

// 각종 typedef
// 별도의 별칭을 정의한다.
using int8      = __int8;
using int16     = __int16;
using int32     = __int32;
using int64     = __int64;
using uint8     = unsigned __int8;
using uint16    = unsigned __int16;
using uint32    = unsigned __int32;
using uint64    = unsigned __int64;
using Vec2      = DirectX::SimpleMath::Vector2;
using Vec3      = DirectX::SimpleMath::Vector3;
using Vec4      = DirectX::SimpleMath::Vector4;
using Matrix    = DirectX::SimpleMath::Matrix; 
// XMFLOAT2, XMMATRIX 등 : 특수 하드웨어 사용. SIMD (Single Instruction Multiple Data)
// SimpleMath는 오른손 좌표계를 기준으로 사용. 부호가 뒤바뀌는 문제에 주의.

enum class CBV_REGISTER : uint8
{
    b0,
    b1,
    b2,
    b3,
    b4,

    END
};

// 겹치지 않는 정보로 매핑해주어야 한다.
enum class SRV_REGISTER : uint8
{
    t0 = static_cast<uint8>(CBV_REGISTER::END),
    t1,
    t2,
    t3,
    t4,

    END
};

enum
{
    SWAP_CHAIN_BUFFER_COUNT = 2,
    CBV_REGISTER_COUNT = CBV_REGISTER::END,
    SRV_REGISTER_COUNT = static_cast<uint8>(static_cast<uint8>(SRV_REGISTER::END) - CBV_REGISTER_COUNT),
    REGISTER_COUNT = CBV_REGISTER_COUNT + SRV_REGISTER_COUNT, // 총 Register 개수
};

struct WindowInfo
{
    HWND    hwnd;       // 출력 윈도우
    int32   width;      // 너비
    int32   height;     // 높이
    bool    windowed;   // 창모드 or 전체화면
};

struct Vertex
{
    Vertex() {}

    Vertex(Vec3 p, Vec2 u, Vec3 n, Vec3 t)
        : pos(p), uv(u), normal(n), tangent(t)
    {
    }

    Vec3 pos;
    Vec2 uv;
    Vec3 normal;
    Vec3 tangent;
};

// type: Class Type
// private에 생성자를 넣어서 외부에서 생성하려고 하면 보호 수준 문제로 생성 불가.
// GetInstance 함수는 내부의 변수의 주소를 반환하는데 stack memory의 주소를 반환하는 위험성이 있지 않을까?
// static 키워드를 붙이는 경우 stack이 아닌 data 영역 (정적/전역) 쪽에 올라가므로 문제가 없다.

#define DECLARE_SINGLE(type)		\
private:							\
	type() {}						\
	~type() {}						\
public:								\
	static type* GetInstance()		\
	{								\
		static type instance;		\
		return &instance;			\
	}								\


#define GET_SINGLE(type)	type::GetInstance()

// extern은 다른 코드는 GEngine 자체의 존재를 모르기 때문에 미리 선언해주는 개념.
// EnginePch.h를 사용하는 모든 곳에서 사용할 수 있음.

// class Engine - 전방 선언
// extern unique_ptr<Engine> GEngine;
// 아래처럼 한줄로 전방 선언과 함께 선언한다.

#define DEVICE              GEngine->GetDevice()->GetDevice()
#define CMD_LIST            GEngine->GetCmdQueue()->GetCmdList()
#define RESOURCE_CMD_LIST   GEngine->GetCmdQueue()->GetResourceCmdList()
#define ROOT_SIGNATURE      GEngine->GetRootSignature()->GetSignature()

#define INPUT               GET_SINGLE(Input)
#define DELTA_TIME          GET_SINGLE(Timer)->GetDeltaTime()

#define CONST_BUFFER(type)  GEngine->GetConstantBuffer(type)


struct TransformParams
{
    Matrix matWVP;
};

extern unique_ptr<class Engine> GEngine;