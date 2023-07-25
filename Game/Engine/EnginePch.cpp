#include "pch.h"
#include "EnginePch.h"
#include "Engine.h"

// Engine을 자유롭게 사용하도록 선언한다.
// Singleton을 사용할 수도 있지만 강의에서는 전역으로 선언.
unique_ptr<Engine> GEngine = make_unique<Engine>();