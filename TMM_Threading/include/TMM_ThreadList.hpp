#include "TMM_ThreadList.h"

namespace TMM
{

	template<typename Resource_t>
	inline ThreadListWorkerCtx<Resource_t>::ThreadListWorkerCtx()
	{

	}

	template<typename Resource_t>
	inline ThreadListWorkerCtx<Resource_t>::~ThreadListWorkerCtx()
	{
		delete mpDelimiter;
	}

	template<typename Resource_t>
	inline void ThreadListWorkerCtx<Resource_t>::Init(ThreadListResource<Resource_t>* pResource, ThreadListDelimiter* pDelimiter, TMM::Callable<void, Resource_t&, uint64_t, const Resource_t*, uint64_t>* pFunc)
	{
		mpResource = pResource;
		mpDelimiter = pDelimiter;
		mpFunc = pFunc;
		Reset();
	}

	template<typename Resource_t>
	inline void ThreadListWorkerCtx<Resource_t>::Reset()
	{
		mpDelimiter->Reset(mCurrent);
	}

	template<typename Resource_t>
	inline bool ThreadListWorkerCtx<Resource_t>::Step()
	{
		if (mpDelimiter->HasEnded(mCurrent, mpResource->dataSize))
		{
			return false; // Skip if ended
		}
		
		mpFunc->Call(mpResource->pData[mCurrent], mCurrent, mpResource->pData, mpResource->dataSize);
		return mpDelimiter->Next(mCurrent, mCurrent, mpResource->dataSize);
	}

	template<typename Resource_t>
	inline void ThreadList<Resource_t>::WorkerProc(ThreadHandle* pHandle)
	{
		ThreadListWorkerCtx<Resource_t>* pCtx = (ThreadListWorkerCtx<Resource_t>*)pHandle->GetData();

		pHandle->Pause();
		while (pHandle->Synchronize() == false)
		{
			pCtx->Reset();
			//std::cout << "Launching... " << pCtx->GetLayer() << std::endl;
			while (pCtx->Step()) { } // execute Step until return false (eq.no more data to process)
			pHandle->Pause();
		}

		pHandle->Exit();
	}

	template<typename Resource_t>
	inline ThreadList<Resource_t>::ThreadList() : 
		mpFunc(nullptr),
		mpWorkers(nullptr),
		mpWorkersCtx(nullptr)
	{

	}

	template<typename Resource_t>
	inline ThreadList<Resource_t>::~ThreadList()
	{
		for (int i = 0; i < mThreadCount; ++i)
		{
			mpWorkers[i].GetGateWay()->Terminate();
			mpWorkers[i].GetGateWay()->Resume();
		}
		delete mpFunc;
		delete[] mpWorkers;
		delete[] mpWorkersCtx;
	}

	template<typename Resource_t>
	inline void ThreadList<Resource_t>::Init(ThreadListDescriptor<Resource_t>* pDesc)
	{
		mThreadCount = pDesc->threadCount;
		mResource = pDesc->resource;

		mpFunc = pDesc->pFunc;
		mpWorkersCtx = new ThreadListWorkerCtx<Resource_t>[mThreadCount];
		mpWorkers = new Thread[mThreadCount];
		for (uint64_t i = 0; i < mThreadCount; ++i)
		{
			ThreadListWorkerCtx<Resource_t>& workerCtx = mpWorkersCtx[i];
			workerCtx.Init(&mResource, pDesc->pThreadDelimiters[i], mpFunc);
		}
	}

	template<typename Resource_t>
	inline void ThreadList<Resource_t>::Start()
	{
		for (uint64_t i = 0; i < mThreadCount; ++i)
		{
			mpWorkers[i].Start(TMM::MakeFunctionPtr(WorkerProc), mpWorkersCtx + i, 1);
		}
		WaitForAllEnd();
	}

	template<typename Resource_t>
	inline void ThreadList<Resource_t>::ProcessAllLayers()
	{
		for (uint64_t i = 0; i < mThreadCount; ++i)
		{
			mpWorkers[i].GetGateWay()->Resume();
		}
	}

	template<typename Resource_t>
	inline void ThreadList<Resource_t>::ProcessLayer(uint64_t layer)
	{
		for (uint64_t i = 0; i < mThreadCount; ++i)
		{
			uint64_t current = mpWorkersCtx[i].GetLayer();
			if (current != layer) continue;
			mpWorkers[i].GetGateWay()->Resume();
		}
	}

	template<typename Resource_t>
	inline void ThreadList<Resource_t>::UnsafeSetResource(Resource_t* pData, uint64_t datasize)
	{
		mResource.pData = pData;
		mResource.dataSize = datasize;
	}

	template<typename Resource_t>
	inline ThreadListDelimiter* ThreadList<Resource_t>::UnsafeDelimiter(unsigned int index)
	{
		return mpWorkersCtx[index].GetDelimiter();
	}

	template<typename Resource_t>
	inline void ThreadList<Resource_t>::WaitForAllEnd()
	{
		for (uint64_t i = 0; i < mThreadCount; ++i)
		{
			mpWorkers[i].GetGateWay()->WaitForPauseNotification();
		}
	}

}