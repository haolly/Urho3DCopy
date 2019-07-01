//
// Created by liuhao1 on 2018/1/29.
//

#include "WorkQueue.h"
#include "Thread.h"
#include "CoreEvent.h"
#include "../Container/List.h"
#include "Timer.h"

namespace Urho3D
{
	class WorkerThread : public Thread, public RefCounted
	{
	public:
		WorkerThread(WorkQueue* owner, unsigned index) :
				owner_(owner),
				index_(index)
		{
		}

		virtual void ThreadFunction() override
		{
			InitFPU();
			owner_->ProcessItem(index_);
		}

		unsigned GetIndex() { return index_; }
	private:
		WorkQueue* owner_;
		// Thread index
		unsigned index_;
	};

	WorkQueue::WorkQueue(Context *context) :
			Object(context),
			shutDown_(false),
			pausing_(false),
			paused_(false),
			completing_(false),
			tolerance_(10),
			lastSize_(0),
			maxNonThreadedWorkMs_(5)
	{
		SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(WorkQueue, HandleBeginFrame));
	}

	WorkQueue::~WorkQueue()
	{
		shutDown_ = true;
		Resume();

		for(unsigned i=0; i< threads_.Size(); ++i)
			threads_[i]->Stop();
	}

	void WorkQueue::CreateThread(unsigned numThreads)
	{
		/// Other subsystems may initialize themselves according to the number of threads.
		// Therefore allow creating the threads only once, after which the amount is fixed
		if(!threads_.Empty())
			return;

		Pause();

		for(unsigned i=0; i< numThreads; ++i)
		{
			SharedPtr<WorkerThread> thread(new WorkerThread(this, i+1));
			thread->Run();
			threads_.Push(thread);
		}
	}

	SharedPtr<WorkItem> WorkQueue::GetFreeItem()
	{
		if(poolItems_.Size() > 0)
		{
			SharedPtr<WorkItem> item = poolItems_.Front();
			poolItems_.Pop();
			return item;
		}
		else
		{
			SharedPtr<WorkItem> item(new WorkItem());
			item->pooled_ = true;
			return item;
		}
	}

	// todo, only called in main thread ??
	void WorkQueue::AddWorkItem(SharedPtr<WorkItem> item)
	{
		if(!item)
		{
			URHO3D_LOGERROR("Null work item submitted to the work queue");
			return;
		}

		assert(!workItems_.Contains(item));

		workItems_.Push(item);
		item->completed_ = false;

		// Make sure the worker threads list is safe to modify, so acquire the mutex
		if(threads_.Size() && !paused_)
			queueMutex_.Acquire();

		if(queue_.Empty())
			queue_.Push((WorkItem*)(item));
		else
		{
			bool inserted = false;

			for(auto it = queue_.Begin(); it != queue_.End(); ++it)
			{
				if((*it)->priority_ <= item->priority_)
				{
					queue_.Insert(it, (WorkItem*)item);
					inserted = true;
					break;
				}
			}

			if(!inserted)
				queue_.Push((WorkItem*)item);
		}

		if(threads_.Size())
		{
			queueMutex_.Release();
			paused_ = false;
		}
	}

	bool WorkQueue::RemoveWorkItem(SharedPtr<WorkItem> item)
	{
		if(!item)
			return false;

		MutexLock lock(queueMutex_);

		// Can only remove successfully if the item was not yet taken by threads for execution
		auto it = queue_.Find((WorkItem*)item);
		if(it != queue_.End())
		{
			auto itList = workItems_.Find(item);
			if(itList != workItems_.End())
			{
				queue_.Erase(it);
				ReturnToPool(item);
				workItems_.Erase(itList);
				return true;
			}
			else
			{
				//todo not find ?
				// How could this happen?
			}
		}
		return false;
	}

	unsigned WorkQueue::RemoveWorkItem(const Vector<SharedPtr<WorkItem>> &items)
	{
		MutexLock lock(queueMutex_);
		unsigned removed = 0;

		for(auto&& it = items.Begin(); it != items.End(); ++it)
		{
			auto&& itFoundQueu = queue_.Find(it->Get());
			if(itFoundQueu != queue_.End())
			{
				auto&& itList = workItems_.Find(*it);
				if(itList != workItems_.End())
				{
					queue_.Erase(itFoundQueu);
					ReturnToPool(*itList);
					workItems_.Erase(itList);
					++removed;
				}
			}
		}
		return removed;
	}

	void WorkQueue::Pause()
	{
		if(!paused_)
		{
			pausing_ = true;

			// Note, when paused, acquire the mutex, so worker thread can not get it
			queueMutex_.Acquire();
			paused_ = true;
			pausing_ = false;
		}
	}

	void WorkQueue::Resume()
	{
		if(paused_)
		{
			queueMutex_.Release();
			paused_ = false;
		}
	}

	//todo, Only called in Main Thread ??
	void WorkQueue::Complete(unsigned priority)
	{
		completing_ = true;

		if(threads_.Size())
		{
			Resume();
			while(!queue_.Empty())
			{
				queueMutex_.Acquire();
				if(!queue_.Empty() && queue_.Front()->priority_ >= priority)
				{
					WorkItem* item = queue_.Front();
					queue_.PopFront();
					queueMutex_.Release();
					item->workFunction_(item, 0);
				}
				else
				{
					queueMutex_.Release();
					break;
				}
			}

			// Wait for threaded work to complete
			while(!IsCompleted(priority))
			{
			}

			// If no work at all remaining, pause worker threads by leaving the mutex locked
			if(queue_.Empty())
				Pause();
		}
		else
		{
			// No worker threads: ensure all high-priority items are completed in the main thread
			while(!queue_.Empty() && queue_.Front()->priority_ >= priority)
			{
				WorkItem* item = queue_.Front();
				queue_.PopFront();
				item->workFunction_(item, 0);
				item->completed_ = true;
			}
		}

		PurgeCompleted(priority);
		PurgePool();
	}

	bool WorkQueue::IsCompleted(unsigned priority) const
	{
		for(auto it = workItems_.Begin(); it != workItems_.End(); ++it)
		{
			if((*it)->priority_ >= priority && (!(*it)->completed_))
				return false;
		}
		return true;
	}

	void WorkQueue::ProcessItem(unsigned threadIndex)
	{
		bool wasActive = false;
		for(;;)
		{
			if(shutDown_)
				return;
			if(pausing_ && !wasActive)
				Time::Sleep(0);
			else
			{
				queueMutex_.Acquire();
				if(!queue_.Empty())
				{
					wasActive = true;

					WorkItem* item = queue_.Front();
					queue_.PopFront();
					queueMutex_.Release();
					item->workFunction_(item, threadIndex);
					item->completed_ = true;
				}
				else
				{
					wasActive = false;

					queueMutex_.Release();
					Time::Sleep(0);
				}
			}
		}
	}

	void WorkQueue::PurgeCompleted(unsigned priority)
	{
		for(auto it = workItems_.Begin(); it != workItems_.End(); )
		{
			if((*it)->completed_ && (*it)->priority_ >= priority)
			{
				if((*it)->sendEvent_)
				{
					using namespace WorkItemCompleted;

					VariantMap& eventData = GetEventDataMap();
					eventData[P_ITEM] = it->Get();
					SendEvent(E_WORKITEMCOMPLETED, eventData);
				}

				ReturnToPool(*it);
				it = workItems_.Erase(it);
			}
			else
				++it;
		}
	}

	void WorkQueue::PurgePool()
	{
		unsigned currentSize = poolItems_.Size();
		int difference = lastSize_ - currentSize;

		for(unsigned i=0; poolItems_.Size() > 0 && difference > tolerance_ && i < (unsigned)difference; i++)
			poolItems_.PopFront();

		lastSize_ = currentSize;
	}

	void WorkQueue::ReturnToPool(SharedPtr<WorkItem> &item)
	{
		if(item->pooled_)
		{
			item->start_ = nullptr;
			item->end_ = nullptr;
			item->aux_ = nullptr;
			item->workFunction_ = nullptr;
			item->priority_ = M_MAX_UNSIGNED;
			item->sendEvent_ = false;
			item->completed_ = false;

			poolItems_.Push(item);
		}
	}

	void WorkQueue::HandleBeginFrame(StringHash eventType, VariantMap &eventData)
	{
		// If no worker threads
		if(threads_.Empty() && !queue_.Empty())
		{
			HiresTimer timer;

			while((!queue_.Empty()) && timer.GetUSec(false) < maxNonThreadedWorkMs_ * 1000)
			{
				WorkItem* item = queue_.Front();
				queue_.PopFront();
				item->workFunction_(item, 0);
				item->completed_ = true;
			}
		}

		PurgeCompleted(0);
		PurgePool();
	}
}