#include "TMM_ThreadLoop.h"

namespace TMM
{
	template<typename Arg>
	inline void ThreadLoop<Arg>::ProcThread(TMM::ThreadHandle* pHandle)
	{
		ThreadArgDesc* pDesc = (ThreadArgDesc*)pHandle->GetData();

		pHandle->Pause();
		while (pHandle->Synchronize() == false)
		{
			while (InterlockedExchange(&pDesc->mLoopCount, pDesc->mLoopCount) > 0)
			{
				pDesc->mpFunc->Call(pDesc->mpArg);
				InterlockedDecrement(&pDesc->mLoopCount);
			}
			pHandle->Pause();
		}
		pHandle->Exit(0);
	}

	template<typename Arg>
	inline ThreadLoop<Arg>::ThreadLoop() :
		mThread(),
		mArgDesc(
			nullptr,
			new Arg(),
			0
		),
		mOwnArg(true)
	{

	}

	template<typename Arg>
	inline ThreadLoop<Arg>::ThreadLoop(Arg* pArg) :
		mThread(),
		mArgDesc(
			nullptr,
			pArg,
			0
		),
		mOwnArg(false)
	{

	}


	template<typename Arg>
	inline ThreadLoop<Arg>::~ThreadLoop()
	{
		mThread.GetGateWay()->Terminate();
		mThread.GetGateWay()->Resume();

		if (mOwnArg) delete mArgDesc.mpArg;
		delete mArgDesc.mpFunc;
	}

	template<typename Arg>
	inline Arg* ThreadLoop<Arg>::GetThreadArgument()
	{
		if (mThread.GetGateWay()->IsPaused() == false) return nullptr;
		return mArgDesc.mpArg;
	}

	template<typename Arg>
	inline void ThreadLoop<Arg>::Start(TMM::Callable<void, Arg*>* pFunc)
	{
		mArgDesc.mpFunc = pFunc;
		if (mArgDesc.mpFunc == nullptr) return;

		mThread.Start(
			TMM::MakeFunctionPtr(ProcThread),
			&mArgDesc,
			1
		);
		mThread.GetGateWay()->WaitForPauseNotification();
	}


	template<typename Arg>
	inline void ThreadLoop<Arg>::Proccess(int64_t count)
	{
		InterlockedAdd64((LONG64*)&mArgDesc.mLoopCount, count);
		mThread.GetGateWay()->Resume();
	}

	template<typename Arg>
	inline void ThreadLoop<Arg>::WaitForEndOfProccess()
	{
		mThread.GetGateWay()->WaitForPauseNotification();
	}
}