#pragma once

#include "framework.h"

class Texture
{
	GLuint mTexture;
	glm::ivec2 mSize;
	int mChannelCount;
	bool mLoaded;
public:
	Texture();

	void Init(const char* path);

	GLuint Get() { return mTexture; }
	glm::ivec2 GetSize() { return mSize; }
};
