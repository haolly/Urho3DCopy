//
// Created by liuhao1 on 2018/1/29.
//

#ifndef URHO3DCOPY_WORKQUEUE_H
#define URHO3DCOPY_WORKQUEUE_H

#include "../Container/RefCounted.h"
#include "Context.h"

namespace Urho3D
{
	class WorkerThread;

	struct WorkItem : public RefCounted
	{
		friend class WorkQueue;

	public:
		WorkItem() :
				priority_(0),
				sendEvent_(false),
				completed_(false),
				pooled_(false)
		{
		}

		//Worker function. Called with the work item and thread index (0 = main thread) as parameters
		void (*workFunction_)(const WorkItem*, unsigned threadIndex);
		// Data start pointer
		void* start_;
		// Data end pointer
		void* end_;
		// Auxiliary data pointer
		void* aux_;
		unsigned priority_;
		bool sendEvent_;
		volatile bool completed_;
	private:
		bool pooled_;
	};


	// Manages multiply threads
	class WorkQueue : public Object
	{
		URHO3D_OBJECT(WorkQueue, Object);
		friend class WorkerThread;
	public:
		WorkQueue(Context* context);
		virtual ~WorkQueue() override;

		void CreateThread(unsigned numThreads);
		SharedPtr<WorkItem> GetFreeItem();
		void AddWorkItem(SharedPtr<WorkItem> item);
		bool RemoveWorkItem(SharedPtr<WorkItem> item);
		unsigned RemoveWorkItem(const Vector<SharedPtr<WorkItem>>& items);
		void Pause();
		void Resume();
		// Finish all queued which has at least the specified priority.
		// Main thread will also execute priority work.
		// Pause worker thread if no more work remains.
		void Complete(unsigned priority);

		void SetTolerance(int tolerance)
		{
			tolerance_ = tolerance;
		}

		void SetNonThreadedWorkMs(int ms)
		{
			maxNonThreadedWorkMs_ = Max(ms, 1);
		}

		unsigned GetNumThreads() const { return threads_.Size(); }

		// Return whether all work with at least the specified priority is finished
		bool IsCompleted(unsigned priority) const;

		bool IsCompleting() const
		{
			return completing_;
		}

		int GetTolerance() const
		{
			return tolerance_;
		}

		int GetNonThreadedWorkMs() const
		{
			return maxNonThreadedWorkMs_;
		}

	private:
		void ProcessItem(unsigned threadIndex);
		void PurgeCompleted(unsigned priority);
		void PurgePool();
		void ReturnToPool(SharedPtr<WorkItem>& item);
		void HandleBeginFrame(StringHash eventType, VariantMap& eventData);

		Vector<SharedPtr<WorkerThread>> threads_;
		List<SharedPtr<WorkItem>> poolItems_;
		List<SharedPtr<WorkItem>> workItems_;
		// Work item prioritized queue for worker threads. Pointers are guaranteed to be valid (point to workItem)
		// todo, queue_ is accessed by main thread and worker thread ? so why not be volatile?
		List<WorkItem*> queue_;
		Mutex queueMutex_;
		volatile bool shutDown_;
		// Indicates the worker threads should not contend for the queue mutex.
		volatile bool pausing_;
		// todo, why this field does not need to be volatile?
		bool paused_;
		// Completing work in the main thread flag
		bool completing_;
		// Tolerance for the shared pool before it begins to deallocate
		int tolerance_;
		// Last size of the shared pool
		unsigned lastSize_;
		// Maximum milliseconds per frame to spend on low-priority work, WHEN there are no worker threads !!
		int maxNonThreadedWorkMs_;
	};

}


#endif //URHO3DCOPY_WORKQUEUE_H
