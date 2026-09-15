#pragma once

#include <TMM_Setup.h>
#include "TMM_Thread.h"

#include <intrin.h>

namespace TMM
{

	template<typename Arg>
	class ThreadCast
	{
	public:

		struct PerformanceReport {
			uint64_t start_tick;
			uint64_t end_tick;
			uint64_t duration_ticks;
		};

	private:
		struct ThreadArgDesc
		{
			TMM::Callable<void, Arg*>* mpFunc;
			Arg* mpArg;
			PerformanceReport mReport;
		} mArgDesc;

		TMM::Thread mThread;
		bool mCasted;
		bool mOwnArg;

		static void ProcThread(ThreadHandle* pHandle);

	public:
		ThreadCast();
		ThreadCast(Arg* pArg);

		virtual ~ThreadCast();

		// <! return nullptr while the thread is running to avoid any overlap
		Arg* GetThreadArgument();

		void Cast(TMM::Callable<void, Arg*>* pFunc);

		bool HasEnded();

		// <! this block the current thread until the thread has finished
		void WaitForEnd();

		// <! return nullptr while the thread hasn't ended
		_NODISCARD PerformanceReport* GetReport();
	};



}

#include "TMM_ThreadCast.hpp"

