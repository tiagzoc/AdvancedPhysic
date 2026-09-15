//#include "TMM_ThreadSynchronizer.h"
//
//namespace TMM
//{
//	ThreadSynchronizer::ThreadSynchronizer()
//	{
//
//	}
//
//	ThreadSynchronizer::~ThreadSynchronizer()
//	{
//
//	}
//
//	void ThreadSynchronizer::SetLoop(bool active)
//	{
//		mLoop = active;
//	}
//
//	unsigned int ThreadSynchronizer::AddFrame(ThreadFrame frame)
//	{
//		mThreadFrames.Enqueue(frame);
//		return mThreadFrames.Size() - 1;
//	}
//
//
//	void ThreadSynchronizer::Update()
//	{
//		bool CAN_GO_NEXT_FRAME = true;
//		if (mCurrentFrame.mIndicesCount == 0) goto SKIP_CHECK;
//
//		TMM_ITER(i, mCurrentFrame.mIndicesCount)
//		{
//			bool THREAD_RUNNING = mThreads[mCurrentFrame.mpFrameIndices[i]].IsPaused() == false;
//			if (THREAD_RUNNING)
//			{
//				bool CONTINUE_NEXT_FRAME = false;
//				TMM_ITER(j, mNextFrame.mIndicesCount)
//				{
//					if (mNextFrame.mpFrameIndices[j] == mCurrentFrame.mpFrameIndices[i])
//					{
//						CONTINUE_NEXT_FRAME = true;
//						break;
//					}
//				}
//				if (CONTINUE_NEXT_FRAME == false) CAN_GO_NEXT_FRAME = false;
//			}
//		}
//
//	SKIP_CHECK:
//		if (CAN_GO_NEXT_FRAME)
//		{
//			// Update current frame
//			if (mCurrentFrame.mpFrameIndices != nullptr && mLoop == false) delete[] mCurrentFrame.mpFrameIndices;
//			if (mLoop) mThreadFrames.Enqueue(mCurrentFrame);
//			mCurrentFrame = mNextFrame;
//
//			if (mCurrentFrame.mpFrameIndices != nullptr)
//			{
//				TMM_ITER(i, mCurrentFrame.mIndicesCount)
//				{
//					unsigned int index = mCurrentFrame.mpFrameIndices[i];
//					Thread& thread = mThreads[index];
//					if (mThreads[index].IsPaused())
//					{
//						mThreads[index].Resume();
//					}
//				}
//			}
//
//			// Update next frame
//			if (mThreadFrames.TryDequeue(&mNextFrame) == false) {
//				mNextFrame.mpFrameIndices = nullptr;
//				mNextFrame.mIndicesCount = 0;
//			}
//		}
//	}
//}
