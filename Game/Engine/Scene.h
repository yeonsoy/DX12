#pragma once

// GameObect를 갖고 있고 각 시점에 맞춰서 실행.
class GameObject;

class Scene
{
public:
	void Awake();
	void Start();
	void Update();
	void LateUpdate();

	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

private:
	vector<shared_ptr<GameObject>> _gameObjects;
	// [ 0 ] WALL
	// [ 1 ] MONSTER
	// vector의 vector와 같은 경우로 구분해서 object를 생성하여 탐색에 용이하도록 생성.
};

