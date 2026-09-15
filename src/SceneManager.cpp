#include "SceneManager.h"

SceneManager::SceneManager(GLFWwindow* pWindow) :
	mpWindow(pWindow),
	mActiveScene(-1)
{

}

SceneManager::~SceneManager()
{
	for (auto pScene : mpScenes)
	{
		delete pScene;
	}
}

void SceneManager::SetActiveScene(int64_t index)
{
	if (index >= mpScenes.size()) return;

	if (mActiveScene != index)
	{
		mpScenes[index]->Activate();
	}
	mActiveScene = index;
}

void SceneManager::SetInactive()
{
	mActiveScene = -1;
}

void SceneManager::Update()
{
	if (mActiveScene < 0)
	{
		glfwPollEvents();
		return;
	}
	mpScenes[mActiveScene]->Frame();
}
