#pragma once
#include "Component.h"

class MonoBehaviour : public Component
{
public:
    MonoBehaviour();
    virtual ~MonoBehaviour();

private:
    virtual void FinalUpdate() sealed { } // MonoBehaviour를 상속 받아서 해당 함수를 override하려고 하면 sealed 키워드 때문에 불가능.
};
