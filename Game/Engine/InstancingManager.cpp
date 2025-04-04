#include "pch.h"
#include "InstancingManager.h"
#include "InstancingBuffer.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"

void InstancingManager::Render(vector<shared_ptr<GameObject>>& gameObjects)
{
    map<uint64, vector<shared_ptr<GameObject>>> cache;

    for (shared_ptr<GameObject>& gameObject : gameObjects)
    {
        // 동일 Mesh와 동일 Material을 사용하면 Instancing 사용
        const uint64 instanceId = gameObject->GetMeshRenderer()->GetInstanceID();
        cache[instanceId].push_back(gameObject);
    }

    for (auto& pair : cache)
    {
        const vector<shared_ptr<GameObject>>& vec = pair.second;

        // 일반적인 Render
        if (vec.size() == 1)
        {
            vec[0]->GetMeshRenderer()->Render();
        }
        // Instancing이 적용된 Render
        else
        {
            const uint64 instanceId = pair.first;

            for (const shared_ptr<GameObject>& gameObject : vec)
            {
                InstancingParams params;
                params.matWorld = gameObject->GetTransform()->GetLocalToWorldMatrix();
                params.matWV = params.matWorld * Camera::S_MatView;
                params.matWVP = params.matWorld * Camera::S_MatView * Camera::S_MatProjection;

                AddParam(instanceId, params);
            }

            shared_ptr<InstancingBuffer>& buffer = _buffers[instanceId];
            vec[0]->GetMeshRenderer()->Render(buffer);
        }
    }
}

// 매 Frame 작동을 위해 Clear 사용
void InstancingManager::ClearBuffer()
{
    for (auto& pair : _buffers)
    {
        shared_ptr<InstancingBuffer>& buffer = pair.second;
        buffer->Clear();
    }
}

void InstancingManager::AddParam(uint64 instanceId, InstancingParams& data)
{
    if (_buffers.find(instanceId) == _buffers.end())
        _buffers[instanceId] = make_shared<InstancingBuffer>();

    _buffers[instanceId]->AddData(data);
}