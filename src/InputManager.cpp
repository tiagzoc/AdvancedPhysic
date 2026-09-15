#include "InputManager.h"
#include "Scene.h"



void InputManager::Init(GLFWwindow* pWindow)
{
	mpWindow = pWindow;
	for (int i = 0; i < 316; ++i)
	{
		mKeyState[i] = KeyState::UP;
	}
	glfwGetCursorPos(pWindow, &mMousePosition.x, &mMousePosition.y);
	mDeltaMousePosition = { 0, 0 };
}


void InputManager::Update()
{
	for (int i = 0; i < 316; ++i)
	{
		bool down = glfwGetKey(mpWindow, 32 + i) == GLFW_PRESS;
		bool pressed = down && !(mKeyState[i] & KeyState::DOWN);
		bool released = !down && !(mKeyState[i] & KeyState::UP);
		
		KeyState a = down ? KeyState::DOWN : KeyState::UP;
		KeyState b = pressed ? KeyState::PRESSED : KeyState::NONE;
		KeyState c = released ? KeyState::RELEASED : KeyState::NONE;

		mKeyState[i] = (KeyState)(a | b | c);
	}

	for (int i = 0; i < 8; ++i)
	{
		bool down = glfwGetMouseButton(mpWindow, i) == GLFW_PRESS;
		bool pressed = down && !(mMouseState[i] & KeyState::DOWN);
		bool released = !down && !(mMouseState[i] & KeyState::UP);

		KeyState a = down ? KeyState::DOWN : KeyState::UP;
		KeyState b = pressed ? KeyState::PRESSED : KeyState::NONE;
		KeyState c = released ? KeyState::RELEASED : KeyState::NONE;

		mMouseState[i] = (KeyState)(a | b | c);
	}

	double X, Y;
	glfwGetCursorPos(mpWindow, &X, &Y);
	mDeltaMousePosition = { X - mMousePosition.x, Y - mMousePosition.y };
	mMousePosition = { X, Y };
}


bool InputManager::MouseButtonDown(unsigned int button)
{
	return mMouseState[button] & KeyState::DOWN;
}

bool InputManager::MouseButtonUp(unsigned int button)
{
	return mMouseState[button] & KeyState::UP;
}

bool InputManager::MouseButtonPressed(unsigned int button)
{
	return mMouseState[button] & KeyState::PRESSED;
}

bool InputManager::MouseButtonReleased(unsigned int button)
{
	return mMouseState[button] & KeyState::RELEASED;
}

bool InputManager::KeyDown(unsigned int key)
{
	return mKeyState[key - 32] & KeyState::DOWN;
}

bool InputManager::KeyUp(unsigned int key)
{
	return mKeyState[key - 32] & KeyState::UP;
}

bool InputManager::KeyPressed(unsigned int key)
{
	return mKeyState[key - 32] & KeyState::PRESSED;
}

bool InputManager::KeyReleased(unsigned int key)
{
	return mKeyState[key - 32] & KeyState::RELEASED;
}

void InputManager::SetMousePosition(glm::dvec2 pos)
{
	glfwSetCursorPos(mpWindow, pos.x, pos.y);
	mMousePosition = { pos.x, pos.y };
}
