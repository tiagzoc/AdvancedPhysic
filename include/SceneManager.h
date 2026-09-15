#pragma once

#include "framework.h"
#include "Scene.h"

class SceneManager
{
	GLFWwindow* mpWindow;
	std::vector<Scene*> mpScenes;
	int64_t mActiveScene = -1;

public:
	SceneManager(GLFWwindow* pWindow);
	virtual ~SceneManager();

	template<typename Scene_t>
	int64_t AddScene()
	{
		Scene_t* pScene = new Scene_t(mpWindow);
		pScene->Init();
		mpScenes.push_back( pScene );
		return mpScenes.size() - 1;
	}
	
	void SetActiveScene(int64_t index);
	void SetInactive();
	int64_t GetActiveScene() { return mActiveScene; }

	void Update();
};
