#pragma once

class Scene;

// Singleton Pattern을 사용하여 생성.
// 언제 어디서나 활용 가능. 유일하게 생성.
class SceneManager
{
    DECLARE_SINGLE(SceneManager);

public:
	void Update();
	void LoadScene(wstring sceneName);

public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }

private:
	shared_ptr<Scene> LoadTestScene();

private:
	shared_ptr<Scene> _activeScene;
};

