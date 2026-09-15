//#pragma once
//
//#include <TMM_Setup.h>
//#include <TMM_OrderedArray.h>
//#include <TMM_Queue.h>
//#include "TMM_Thread.h"
//
//#include <iostream>
//
//namespace TMM
//{
//
//	struct ThreadFrame
//	{
//		unsigned int* mpFrameIndices	= nullptr;
//		unsigned int mIndicesCount		= 0;
//	};
//
//	class ThreadSynchronizer
//	{
//		// 3Frames : [
//		//	1 1 1
//		//	2 3 2
//		//      3
//		// ]
//		// 1 et 2, quand 2 fini lance 3 et quand 3 fini, relance 2 et 3
//
//		// Threads
//		ArrayOrdered<void*> mThreadArgs;
//		ArrayOrdered<Thread> mThreads;
//
//		// Sync
//		Queue<ThreadFrame> mThreadFrames;
//		ThreadFrame mCurrentFrame;
//		ThreadFrame mNextFrame;
//
//		// Params
//		bool mLoop = false;
//
//		template<typename T>
//		struct ThreadArg
//		{
//			TMM::Function<void, T*, unsigned int>* pFunc;
//			T* pData;
//			unsigned int dataCount;
//		};
//
//		template<typename T>
//		static DWORD ProcThreadLoop(ThreadContext<ThreadArg<T>>* pCtx)
//		{
//			ThreadArg<T>* pArg = pCtx->UnsafeData();
//
//			std::cout << "Proc Started !" << std::endl;
//
//			while (pCtx->TerminationStatus() == false)
//			{
//				pCtx->SafePause();
//				Sleep(0);
//				std::cout << "Pause status : " << pCtx->IsPaused() << std::endl;
//				pArg->pFunc->Call(pArg->pData, pArg->dataCount);
//			}
//
//			return pCtx->ExitWithStatus(ThreadStatusCode::SUCESS);
//		}
//
//	public:
//		ThreadSynchronizer();
//		virtual ~ThreadSynchronizer();
//
//		void SetLoop(bool active);
//
//		void Update();
//		unsigned int AddFrame(ThreadFrame frame);
//
//		template<typename T>
//		unsigned int AddThread(TMM::Function<void, T*, unsigned int>* pFunc, T* pData, unsigned int dataCount)
//		{
//			ThreadArg<T>* pArg = new ThreadArg<T>();
//			pArg->pData = pData;
//			pArg->dataCount = dataCount;
//			pArg->pFunc = pFunc;
//			mThreadArgs.Add(pArg);
//
//			mThreads.Add(Thread());
//			Thread& thread = mThreads[mThreads.Size() - 1];
//
//			auto pProc = MakeFunctionPtr(ProcThreadLoop<T>);
//			thread.Start<ThreadArg<T>>(pProc, pArg);
//
//			return mThreads.Size() - 1;
//		}
//	};
//}