#pragma once

#include "framework.h"

enum KeyState : int
{
	NONE	= 0,
	UP		= 1,
	DOWN	= 2,
	PRESSED = 4,
	RELEASED = 8,
};

struct InputManager
{
private:
	inline static KeyState mKeyState[316];
	inline static KeyState mMouseState[8];
	inline static glm::dvec2 mMousePosition;
	inline static glm::dvec2 mDeltaMousePosition;
	inline static GLFWwindow* mpWindow;

public:
	static void Init(GLFWwindow* pWindow);

	static void Update();
	
	static bool MouseButtonDown(unsigned int button);
	static bool MouseButtonUp(unsigned int button);
	static bool MouseButtonPressed(unsigned int button);
	static bool MouseButtonReleased(unsigned int button);
	
	static bool KeyDown(unsigned int key);
	static bool KeyUp(unsigned int key);
	static bool KeyPressed(unsigned int key);
	static bool KeyReleased(unsigned int key);

	static void SetMousePosition(glm::dvec2 pos);
	static double MousePositionX() { return mMousePosition.x; }
	static double MousePositionY() { return mMousePosition.y; }
	static double MouseDeltaX() { return mDeltaMousePosition.x; }
	static double MouseDeltaY() { return mDeltaMousePosition.y; }

};