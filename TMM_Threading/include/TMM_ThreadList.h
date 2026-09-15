#pragma once
// REQUIRED internal include
#include <TMM_Setup.h>
#include <TMM_Function.h>
#include "TMM_Thread.h"

// TODO : remove
#include <iostream>

namespace TMM
{

	struct ThreadListDelimiter
	{
		uint64_t layer;
		ThreadListDelimiter(uint64_t _layer = 0) : layer(_layer) { }
		virtual bool Next(uint64_t current, uint64_t& dest, uint64_t resource_size) = 0;
		virtual void Reset(uint64_t& dest) = 0;
		virtual bool HasEnded(uint64_t current, uint64_t resource_size) = 0;
	};

	struct ThreadListDelimiter1D : public ThreadListDelimiter
	{
		// Start |	  offset    |	 offset    | ...
		// Start | stride | ... | stride | ... | ...
		//         ^^^^^^^        ^^^^^^^
		//		   Working        Working

		uint64_t start;
		uint64_t stride;
		uint64_t offset;
		uint64_t end;

		ThreadListDelimiter1D(
			uint64_t _start,
			uint64_t _stride,
			uint64_t _offset,
			uint64_t _end,
			uint64_t _layer = 0
		) :
			ThreadListDelimiter(_layer),
			start(_start),
			stride(_stride),
			offset(_offset),
			end(_end)
		{}

		bool Next(uint64_t current, uint64_t& dest, uint64_t resource_size) override
		{
			current++;
			if ((current - start) % offset >= stride)
			{
				current += offset - stride;
			}
			dest = current;
			if (current >= end || current >= resource_size) return false;
			return true;
		}

		void Reset(uint64_t& dest) override
		{
			dest = start;
		}

		bool HasEnded(uint64_t current, uint64_t resource_size) override
		{
			if (stride == 0 || current >= end || current >= resource_size) return true;
			return false;
		}
	};

	template<typename Resource_t>
	struct ThreadListResource
	{
		Resource_t* pData;
		uint64_t dataSize;
	};

	template<typename Resource_t>
	class ThreadListWorkerCtx
	{
		ThreadListResource<Resource_t>* mpResource;
		ThreadListDelimiter* mpDelimiter;
		TMM::Callable<void, Resource_t&, uint64_t, const Resource_t*, uint64_t>* mpFunc;

		uint64_t mCurrent;

	public:
		ThreadListWorkerCtx();
		virtual ~ThreadListWorkerCtx();

		void Init(
			ThreadListResource<Resource_t>* pResource, 
			ThreadListDelimiter* pDelimiter,
			TMM::Callable<void, Resource_t&, uint64_t, const Resource_t*, uint64_t>* pFunc
		);

		void Reset();

		uint64_t GetLayer() const { return mpDelimiter->layer; }
		ThreadListDelimiter* GetDelimiter() { return mpDelimiter; }

		bool Step();
	};

	template<typename Resource_t>
	struct ThreadListDescriptor
	{
		// <! void f(Resource_t& edited_data, uint64_t index_edited, const Resource_t* unsafe_resource, uint64_t resouce_size)
		TMM::Callable<void, Resource_t&, uint64_t, const Resource_t*, uint64_t>* pFunc;
		ThreadListDelimiter** pThreadDelimiters;
		ThreadListResource<Resource_t> resource;
		uint16_t threadCount;
		~ThreadListDescriptor() { delete[] pThreadDelimiters; }
	};

	template<typename Resource_t>
	class ThreadList {
		TMM::Callable<void, Resource_t&, uint64_t, const Resource_t*, uint64_t>* mpFunc;
		ThreadListResource<Resource_t> mResource;
		ThreadListWorkerCtx<Resource_t>* mpWorkersCtx;
		uint16_t mThreadCount;
		Thread* mpWorkers;
		uint64_t mLayerCount;

		static void WorkerProc(ThreadHandle* pHandle);

	public:
		ThreadList();
		virtual ~ThreadList();

		void Init(ThreadListDescriptor<Resource_t>* pDesc);

		void Start();

		void ProcessAllLayers();
		void ProcessLayer(uint64_t layer);

		void UnsafeSetResource(Resource_t* pData, uint64_t datasize);
		ThreadListDelimiter* UnsafeDelimiter(unsigned int index);
		uint16_t GetThreadCount() { return mThreadCount; }

		void WaitForAllEnd();
	};



}

#include "TMM_ThreadList.hpp"