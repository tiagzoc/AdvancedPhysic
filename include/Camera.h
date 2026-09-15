#pragma once


#include "framework.h"
#include "Transform.h"

struct Camera
{
	glm::mat4 Proj;
	
	Transform transform;
	
	Camera();
	glm::mat4 GetView();

};