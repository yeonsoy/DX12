#pragma once

// pragma: 전처리 단계
#ifdef _DEBUG
#pragma comment(lib, "Engine_debug.lib")
#else
#pragma comment(lib, "Engine.lib")
#endif

#include "EnginePch.h"