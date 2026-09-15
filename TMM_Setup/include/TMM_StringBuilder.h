#pragma once

#include "TMM_String.h"

namespace TMM
{

	class StringBuilder
	{
		struct StringNode
		{
			union {
				const char* pBuffer = nullptr;
				char mChar;
			};
			uint64_t bufferSize	= 0;
			StringNode* pNext	= nullptr;
			virtual ~StringNode() { delete pNext; }
		};

		StringNode* mpFirst	= nullptr;
		StringNode* mpLast	= nullptr;
		uint64_t mTotalSize = 0;

		StringNode* CreateRoot() {
			StringNode* pNode = new StringNode();
			mpFirst = pNode;
			mpLast = pNode;
			return pNode;
		}

		StringNode* CreateFirst() {
			StringNode* pNode;
			if (mpFirst == nullptr)
			{
				pNode = CreateRoot();
			}
			else {
				pNode = new StringNode();
				pNode->pNext = mpFirst;
				mpFirst = pNode;
			}
			return pNode;
		}

		StringNode* CreateLast() {
			StringNode* pNode;
			if (mpLast == nullptr)
			{
				pNode = CreateRoot();
			}
			else {
				pNode = new StringNode();
				mpLast->pNext = pNode;
				mpLast = pNode;
			}
			return pNode;
		}

	public:
		StringBuilder() = default;
		virtual ~StringBuilder() { delete mpFirst; };

		void AddFirst(const char* pChars, uint64_t size);
		void AddFirst(char c);
		void AddFirst(const String& str);

		void AddLast(const char* pChars, uint64_t size);
		void AddLast(char c);
		void AddLast(const String& str);

		String Build();
	};

}
