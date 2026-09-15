#include "TMM_String.h"
#include "TMM_String.h"
#include "TMM_StringBuilder.h"

namespace TMM
{

	String::String(uint64_t size)
	{
		mpStr = new char[size];
		mStrSize = size;
		mOwner = true;
	}

	String::String(const char* pchars)
	{
		uint64_t size = strlen(pchars) + 1;
		mpStr = new char[size];
		memcpy(mpStr, pchars, size);
		mStrSize = size;
		mOwner = true;
	}

	String::String(char* pchars, uint64_t size)
	{
		mpStr = new char[size];
		memcpy(mpStr, pchars, size);
		mStrSize = size;
		mOwner = true;
	}

	String::String(const String& other)
	{
		mpStr = new char[other.size()];
		memcpy(mpStr, other.data(), other.size());
		mStrSize = other.size();
		mOwner = true;
	}

	String::~String()
	{
		Destroy();
	}

	void String::Destroy()
	{
		if (IsOwner() && mpStr != nullptr)
		{
			delete[] mpStr;
		}
		mpStr = nullptr;
		mStrSize = 0;
		mOwner = false;
	}

	void String::Move(String& other)
	{
		other.Destroy();
		other.mpStr = mpStr;
		other.mStrSize;
		other.mOwner = mOwner;
		mOwner = false;
	}

	void String::Move(char* pChars, uint64_t size)
	{
		Destroy();
		mpStr = pChars;
		mStrSize = size;
		mOwner = true;
	}

	String String::operator+(const String& other)
	{
		char* pBuffer = new char[mStrSize + other.mStrSize];
		memcpy(pBuffer, mpStr, mStrSize);
		memcpy(pBuffer + mStrSize, other.mpStr, other.mStrSize);
		String str{ mStrSize + other.mStrSize };
		str.mpStr = pBuffer;
		str.mOwner = true;
		return str;
	}

	String String::operator+(const char* other)
	{
		uint64_t size = strlen(other) + 1;
		char* pBuffer = new char[mStrSize + size];
		memcpy(pBuffer, mpStr, mStrSize);
		memcpy(pBuffer + mStrSize, other, size);
		String str{ mStrSize + size };
		str.mpStr = pBuffer;
		str.mOwner = true;
		return str;
	}

	void String::operator=(const char* other)
	{
		Destroy();
		uint64_t size = strlen(other);
		mpStr = new char[size];
		memcpy(mpStr, other, size);
		mStrSize = size;
		mOwner = true;
	}

	void String::operator=(const String& other)
	{
		Destroy();
		mpStr = new char[other.size()];
		memcpy(mpStr, other.data(), other.size());
		mStrSize = other.size();
		mOwner = true;
	}

	char& String::operator[](uint64_t index)
	{
		return mpStr[index];
	}


}