#pragma once

#include "framework.h"

static glm::mat4x4 Translate(const glm::vec3& pos) {
	return glm::mat4x4(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		pos.x, pos.y, pos.z, 1
	);
}

static glm::mat4x4 Scale(const glm::vec3& scale) {
	return glm::mat4x4(
		scale.x,	0,			0,			0,
		0,			scale.y,	0,			0,
		0,			0,			scale.z,	0,
		0,			0,			0,			1
	);
}

static glm::mat4x4 RotateYawPitchRoll(const glm::vec3& YawPitchRoll) {
	glm::quat q = glm::angleAxis(YawPitchRoll.x, glm::vec3{ 0.0f, 1.0f, 0.0f });

	glm::vec3 right		= { 1.f, 0.f, 0.0f };
	glm::vec3 forward	= { 0.f, 0.f, 1.0f };

	right = q * right;
	q = glm::angleAxis(YawPitchRoll.y, right) * q;

	forward = q * forward;
	q = glm::angleAxis(YawPitchRoll.z, forward) * q;

	return glm::mat4_cast(q);
}
