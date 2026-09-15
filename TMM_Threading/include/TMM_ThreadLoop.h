#pragma once

#include <TMM_Setup.h>
#include "TMM_Thread.h"

namespace TMM
{

	template<typename Arg>
	class ThreadLoop
	{
		Thread mThread;
		struct ThreadArgDesc
		{
			TMM::Callable<void, Arg*>* mpFunc;
			Arg* mpArg;
			uint64_t mLoopCount;
		} mArgDesc;
		bool mOwnArg;

		static void ProcThread(ThreadHandle* pHandle);

	public:

		ThreadLoop();
		ThreadLoop(Arg* pArg);

		virtual ~ThreadLoop();

		// <! return nullptr while the thread is running to avoid any overlap
		Arg* GetThreadArgument();

		// <! Dont Launch any proccess, only create the threads and pause them
		void Start(TMM::Callable<void, Arg*>* pFunc);

		// <! Launch the proccess <count> times
		void Proccess(int64_t count = 1);

		void WaitForEndOfProccess();
	};


}

#include "TMM_ThreadLoop.hpp"

