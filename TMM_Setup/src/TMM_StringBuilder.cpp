#include "TMM_StringBuilder.h"

namespace TMM
{

	void StringBuilder::AddFirst(const char* pChars, uint64_t size)
	{
		StringNode* pNode = CreateFirst();
		pNode->pBuffer = pChars;
		pNode->bufferSize = size;
		mTotalSize += size;
	}

	void StringBuilder::AddFirst(char c)
	{
		StringNode* pNode = CreateFirst();
		pNode->mChar = c;
		pNode->bufferSize = 0;
		mTotalSize++;
	}

	void StringBuilder::AddFirst(const String& str)
	{
		AddFirst(str.data(), str.size());
	}

	void StringBuilder::AddLast(const char* pChars, uint64_t size)
	{
		StringNode* pNode = CreateLast();
		pNode->pBuffer = pChars;
		pNode->bufferSize = size;
		mTotalSize += size;
	}

	void StringBuilder::AddLast(char c)
	{
		StringNode* pNode = CreateLast();
		pNode->mChar = c;
		pNode->bufferSize = 0;
		mTotalSize++;
	}

	void StringBuilder::AddLast(const String& str)
	{
		AddLast(str.data(), str.size());
	}

	String StringBuilder::Build()
	{
		if (mTotalSize == 0) return String();
		
		char* pBuffer = new char[mTotalSize];
		uint64_t offset = 0;
		StringNode* pCurrent = mpFirst;

		while (pCurrent != nullptr)
		{
			if (pCurrent->bufferSize == 0)
			{
				*(pBuffer + offset) = pCurrent->mChar;
				++offset;
			}
			else
			{
				memcpy(pBuffer + offset, pCurrent->pBuffer, pCurrent->bufferSize);
				offset += pCurrent->bufferSize;
			}

			pCurrent = pCurrent->pNext;
		}

		String out{};
		out.Move(pBuffer, mTotalSize);
		return out;
	}

}