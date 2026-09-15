#pragma once

#include "framework.h"

template<typename T>
struct Node {
	T node;
	Node<T>* pNext	= nullptr;
	Node<T>* pPrev	= nullptr;
};

template<typename T>
class SafeQueue
{
	uint64_t mSize;
	Node<T>* mpFirst;
	Node<T>* mpLast;
	CRITICAL_SECTION mCS;

public:
	SafeQueue()
	{
		mSize = 0;
		mpFirst = nullptr;
		mpLast = nullptr;
		InitializeCriticalSection(&mCS);
	}

	~SafeQueue() {
		Node<T>* pNode;
		Node<T>* pNext;
		EnterCriticalSection(&mCS);
		pNode = mpFirst;
		while (pNode != nullptr)
		{
			pNext = pNode->pNext;
			delete pNode;
			pNode = pNext;
		}
		LeaveCriticalSection(&mCS);
		DeleteCriticalSection(&mCS);
	}

	void Push(const T& v)
	{
		Node<T>* pNode;
		EnterCriticalSection(&mCS);
		mSize++;
		if (mpFirst == nullptr)
		{
			mpFirst = new Node<T>();
			mpLast = mpFirst;
			mpFirst->node = v;
			LeaveCriticalSection(&mCS);
			return;
		}

		pNode = new Node<T>();
		pNode->node = v;
		pNode->pPrev = mpLast;
		mpLast->pNext = pNode;
		mpLast = pNode;
		LeaveCriticalSection(&mCS);
	}

	void Pop() {
		Node<T>* pNode;
		EnterCriticalSection(&mCS);
		mSize--;
		pNode = mpLast->pPrev;
		delete mpLast;
		if (pNode == nullptr) {
			mpFirst = nullptr;
			mpLast = nullptr;
			LeaveCriticalSection(&mCS);
			return;
		}
		mpLast = pNode;
		LeaveCriticalSection(&mCS);
	}

	T Get() {
		T v;
		EnterCriticalSection(&mCS);
		v = mpLast->node;
		LeaveCriticalSection(&mCS);
		return v;
	}

	uint64_t Size()
	{
		uint64_t s;
		EnterCriticalSection(&mCS);
		s = mSize;
		LeaveCriticalSection(&mCS);
		return s;
	}
};
