#include "TMM_ThreadCast.h"
#include "TMM_ThreadLoop.h"

namespace TMM
{
	template<typename Arg>
	inline void ThreadCast<Arg>::ProcThread(ThreadHandle* pHandle)
	{
		ThreadArgDesc* pDesc = (ThreadArgDesc*)pHandle->GetData();
		pDesc->mpFunc->Call(pDesc->mpArg);
		pDesc->mReport.end_tick = __rdtsc();
		pDesc->mReport.duration_ticks = pDesc->mReport.end_tick - pDesc->mReport.start_tick;
		pHandle->Exit(0);
	}

	template<typename Arg>
	inline ThreadCast<Arg>::ThreadCast() :
		mArgDesc(
			nullptr,
			new Arg()
		),
		mThread(),
		mOwnArg(true),
		mCasted(false)
	{ }

	template<typename Arg>
	inline ThreadCast<Arg>::ThreadCast(Arg* pArg) :
		mArgDesc(
			nullptr,
			pArg
		),
		mThread(),
		mOwnArg(false),
		mCasted(false)
	{ }

	template<typename Arg>
	inline ThreadCast<Arg>::~ThreadCast()
	{
		if (mOwnArg) delete mArgDesc.mpArg;
		delete mArgDesc.mpFunc;
	}

	template<typename Arg>
	inline Arg* ThreadCast<Arg>::GetThreadArgument()
	{
		if (mCasted) return nullptr;
		return mArgDesc.mpArg;
	}



	template<typename Arg>
	inline void ThreadCast<Arg>::Cast(TMM::Callable<void, Arg*>* pFunc)
	{
		mArgDesc.mpFunc = pFunc;
		if (mArgDesc.mpFunc == nullptr) return;

		mCasted = true;
		mArgDesc.mReport.start_tick = __rdtsc();
		mThread.Start(
			TMM::MakeFunctionPtr(ProcThread),
			&mArgDesc,
			1
		);
	}


	template<typename Arg>
	inline bool ThreadCast<Arg>::HasEnded()
	{
		mCasted = mThread.GetGateWay()->Exited();
		return mCasted;
	}



	template<typename Arg>
	inline void ThreadCast<Arg>::WaitForEnd()
	{
		mThread.GetGateWay()->TerminateWait();
		mCasted = false;
	}

	template<typename Arg>
	inline ThreadCast<Arg>::PerformanceReport* ThreadCast<Arg>::GetReport()
	{
		if (mCasted == true) return nullptr;
		return &mArgDesc.mReport;
	}
}