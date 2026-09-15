#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() :
	mTexture(0),
	mSize(0, 0),
	mChannelCount(0),
	mLoaded(false)
{
}

void Texture::Init(const char* path)
{
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	unsigned char* pixels = stbi_load(path, &mSize.x, &mSize.y, &mChannelCount, 0);


	GLenum format = GL_RGBA;
	if (mChannelCount == 1) format = GL_R;
	else if (mChannelCount == 2) format = GL_RG;
	else if (mChannelCount == 3) format = GL_RGB;
	
	if (pixels)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, mSize.x, mSize.y, 0, format, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		mLoaded = true;
	}
	else {
		std::cerr << "Failed to Load Obama :(" << std::endl;
		mLoaded = false;
	}

	stbi_image_free(pixels);
}

