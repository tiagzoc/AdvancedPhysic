#include "TMM_Thread.h"

namespace TMM 
{


	void ThreadHandle::SetGateWay(ThreadGateWay* pGateWay)
	{
		mpGateWay = pGateWay;
	}

	ThreadHandle::ThreadHandle(void* pData, uint64_t size) :
		mpData(pData),
		mDataSize(size),
		mShouldPause(false),
		mShouldTerminate(false),
		mIsPaused(false),
		mHasTerminated(false)
	{
		InitializeCriticalSection(&mCS);

		mPauseEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	}

	ThreadHandle::~ThreadHandle()
	{
		DeleteCriticalSection(&mCS);
	}

	void* ThreadHandle::GetData()
	{
		void* r;
		EnterCriticalSection(&mCS);
		r = mpData;
		LeaveCriticalSection(&mCS);
		return r;
	}

	uint64_t ThreadHandle::GetDataSize()
	{
		uint64_t r;
		EnterCriticalSection(&mCS);
		r = mDataSize;
		LeaveCriticalSection(&mCS);
		return r;
	}

	bool ThreadHandle::IsPaused()
	{
		bool r;
		EnterCriticalSection(&mCS);
		r = mIsPaused;
		LeaveCriticalSection(&mCS);
		return r;
	}

	bool ThreadHandle::HasTerminated()
	{
		bool r;
		EnterCriticalSection(&mCS);
		r = mHasTerminated;
		LeaveCriticalSection(&mCS);
		return r;
	}

	bool ThreadHandle::ShouldPause()
	{
		bool r;
		EnterCriticalSection(&mCS);
		r = mShouldPause;
		LeaveCriticalSection(&mCS);
		return r;
	}

	bool ThreadHandle::ShouldTerminate()
	{
		bool r;
		EnterCriticalSection(&mCS);
		r = mShouldTerminate;
		LeaveCriticalSection(&mCS);
		return r;
	}

	uint64_t ThreadHandle::GetExitCode()
	{
		uint64_t code;
		EnterCriticalSection(&mCS);
		code = mTerminationCode;
		LeaveCriticalSection(&mCS);
		return code;
	}

	void ThreadHandle::Pause()
	{
		EnterCriticalSection(&mCS);
		mShouldPause = true;
		LeaveCriticalSection(&mCS);
	}

	void ThreadHandle::Resume()
	{
		EnterCriticalSection(&mCS);
		if (IsPaused())
		{
			SetEvent(mPauseEvent);
		}
		ResetEvent(mpGateWay->GetPauseNotification());
		mShouldPause = false;
		LeaveCriticalSection(&mCS);
	}

	void ThreadHandle::Terminate()
	{
		EnterCriticalSection(&mCS);
		mShouldTerminate = true;
		LeaveCriticalSection(&mCS);
	}



	bool ThreadHandle::Synchronize()
	{
		if (ShouldTerminate() || HasTerminated()) return true;

		if (ShouldPause()) {
			EnterCriticalSection(&mCS);
			mShouldPause = false;
			mIsPaused = true;
			LeaveCriticalSection(&mCS);
			SetEvent(mpGateWay->GetPauseNotification());
			WaitForSingleObject(mPauseEvent, INFINITE);
		}
		return ShouldTerminate();
	}

	void ThreadHandle::Exit(uint64_t code)
	{
		EnterCriticalSection(&mCS);
		mHasTerminated = true;
		mShouldTerminate = false;
		mTerminationCode = code;
		mpGateWay->StopWaitThreadTermination();
		LeaveCriticalSection(&mCS);
	}


	void ThreadGateWay::StopWaitThreadTermination()
	{
		SetEvent(mThreadTerminateWait);
	}

	ThreadGateWay::ThreadGateWay(ThreadHandle* pHandle) :
		mpHandle(pHandle)
	{
		mThreadTerminateWait = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		mPauseNotification = CreateEvent(nullptr, TRUE, FALSE, nullptr);
		pHandle->SetGateWay(this);
		InitializeCriticalSection(&mCS);
	}

	ThreadGateWay::~ThreadGateWay()
	{
		DeleteCriticalSection(&mCS);
	}

	bool ThreadGateWay::IsPaused() { return mpHandle->IsPaused(); }
	bool ThreadGateWay::Exited(uint64_t* pCode)
	{
		bool Terminated = mpHandle->HasTerminated();
		if (pCode != nullptr) *pCode = mpHandle->GetExitCode();
		return Terminated;
	}
	void ThreadGateWay::Pause() { mpHandle->Pause(); }
	void ThreadGateWay::Resume() { mpHandle->Resume();  }
	void ThreadGateWay::Terminate() { mpHandle->Terminate(); }
	void ThreadGateWay::TerminateWait() { 
		mpHandle->Terminate(); 
		WaitForSingleObject(mThreadTerminateWait, INFINITE);
	}

	void ThreadGateWay::WaitForPauseNotification()
	{
		WaitForSingleObject(mPauseNotification, INFINITE);
	}

	HANDLE ThreadGateWay::GetPauseNotification()
	{
		HANDLE r;
		EnterCriticalSection(&mCS);
		r = mPauseNotification;
		LeaveCriticalSection(&mCS);
		return r;
	}

	DWORD Thread::ProcThread(ThreadCtx* pCtx)
	{
		pCtx->pFunction->Call(pCtx->pHandle);
		return 0;
	}

	uint16_t Thread::GetMaxThreadCount()
	{
		return static_cast<uint16_t>(GetMaximumProcessorCount(ALL_PROCESSOR_GROUPS));
	}

	Thread::Thread() :
		mCtx(nullptr, nullptr),
		mpGateWay(nullptr),
		mHandle(nullptr),
		mID(0),
		mIsDestroyed(true)
	{

	}

	Thread::~Thread()
	{
		End();
	}

	void Thread::Start(TMM::Callable<void, ThreadHandle*>* pFunc, void* pData, uint64_t size)
	{
		mCtx.pHandle = new ThreadHandle(pData, size);
		mCtx.pFunction = pFunc;
		mpGateWay = new ThreadGateWay(mCtx.pHandle);

		mHandle = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)ProcThread,
			&mCtx,
			0,
			&mID
		);
		mIsDestroyed = false;
	}

	void Thread::End()
	{
		if (mIsDestroyed) return;

		if(mpGateWay->Exited(nullptr) == false)
			mpGateWay->TerminateWait();

		delete mpGateWay;
		delete mCtx.pFunction;
		delete mCtx.pHandle;

		mIsDestroyed = true;
	}

	ThreadGateWay* Thread::GetGateWay()
	{
		return mpGateWay;
	}
}