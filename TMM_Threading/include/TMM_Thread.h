#pragma once

// REQUIRED internal include
#include <TMM_Setup.h>
#include <TMM_Functional.h>

#include <Windows.h>

namespace TMM 
{
	class Thread;
	class ThreadGateWay;
	class ThreadHandle;

	class ThreadHandle {
		void* mpData;
		uint64_t mDataSize;
		bool mShouldPause;
		bool mShouldTerminate;
		bool mIsPaused;
		bool mHasTerminated;
		uint64_t mTerminationCode;
		CRITICAL_SECTION mCS;
		HANDLE mPauseEvent;

		ThreadGateWay* mpGateWay;

		friend class ThreadGateWay;

		void SetGateWay(ThreadGateWay* pGateWay);
	public:
		ThreadHandle(void* pData, uint64_t size);
		~ThreadHandle();

		void* GetData();
		uint64_t GetDataSize();

		bool IsPaused();
		bool HasTerminated();
		bool ShouldPause();
		bool ShouldTerminate();
		uint64_t GetExitCode();

		void Pause();
		void Resume();
		void Terminate();

		// Update the status of the Thread to match desired status
		bool Synchronize();

		// To call When exiting a Thread
		void Exit(uint64_t code = 0);
	};

	class ThreadGateWay {
		ThreadHandle* mpHandle;
		HANDLE mThreadTerminateWait;

		HANDLE mPauseNotification;

		CRITICAL_SECTION mCS;

		friend class ThreadHandle;

		void StopWaitThreadTermination();
		HANDLE GetPauseNotification();
	public:
		ThreadGateWay(ThreadHandle* pHandle);
		virtual ~ThreadGateWay();

		bool IsPaused();
		bool Exited(uint64_t* pCode = nullptr);
		void Pause();
		void Resume();
		void Terminate();
		void TerminateWait();

		void WaitForPauseNotification();
	};

	class Thread {
		ThreadGateWay* mpGateWay;
		struct ThreadCtx {
			TMM::Callable<void, ThreadHandle*>* pFunction;
			ThreadHandle* pHandle;
		} mCtx;

		HANDLE mHandle;
		DWORD mID;

		bool mIsDestroyed;

		static DWORD ProcThread(ThreadCtx* pCtx);

	public:

		static uint16_t GetMaxThreadCount();

		Thread();
		virtual ~Thread();
		void Start(TMM::Callable<void, ThreadHandle*>* pFunc, void* pData, uint64_t size);
		//<! This block the current thread and can take a while
		void End();
		ThreadGateWay* GetGateWay();
	};

}
