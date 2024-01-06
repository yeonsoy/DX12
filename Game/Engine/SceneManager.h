#pragma once

class Scene;

enum
{
	MAX_LAYER = 32 // Unity에도 Layer가 32개 존재.
};

// Singleton Pattern을 사용하여 생성.
// 언제 어디서나 활용 가능. 유일하게 생성.
class SceneManager
{
    DECLARE_SINGLE(SceneManager);

public:
	void Update();
	void Render();
	void LoadScene(wstring sceneName);

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);

public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }

private:
	shared_ptr<Scene> LoadTestScene();

private:
	shared_ptr<Scene> _activeScene;

	array<wstring, MAX_LAYER> _layerNames;
	map<wstring, uint8> _layerIndex;
};

