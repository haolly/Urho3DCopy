//
// Created by liuhao1 on 2017/12/20.
//

#ifndef URHO3DCOPY_SORT_H
#define URHO3DCOPY_SORT_H

#include "VectorBase.h"

namespace Urho3D
{

	static const int QUICKSORT_THRESHOLD = 16;

// Based on Comparison of several sorting algorithms by Juha Nieminen
// http://warp.povusers.org/SortComparison/
	template <class T> void InsertionSort(RandomAccessIterator<T> begin, RandomAccessIterator<T> end)
	{
		// note, 先假设第一个元素已经排好序，然后从第二个元素开始，把剩下的插入到已经排好序的列表里面
		for(RandomAccessIterator<T> i = begin + 1; i< end; ++i)
		{
			T temp = *i;
			RandomAccessIterator<T> jj = i;
			// find the correct position to insert
			while(jj > begin && temp < *(jj - 1))
			{
				*jj = *(jj - 1);
				--jj;
			}
			*jj = temp;
		}

	}

	template <class T, class U>
	void InsertionSort(RandomAccessIterator<T> begin, RandomAccessIterator<T> end, U compare)
	{
		for(RandomAccessIterator<T> i = begin + 1; i < end; ++i)
		{
			T temp = *i;
			RandomAccessIterator<T> jj = i;
			while(jj > begin && compare(temp, *(jj -1)))
			{
				*jj = *(jj -1);
				--jj;
			}
			*jj = temp;
		}
	};

	// Perform quick sort initial pass on an array using a compare function. Does not sort fully
	template <class T>
	void InitialQuickSort(RandomAccessIterator<T> begin, RandomAccessIterator<T> end)
	{
		while(end - begin > QUICKSORT_THRESHOLD)
		{
			RandomAccessIterator<T> pivot = begin + ((end - begin) / 2);
			if((*begin < *pivot) && (*(end -1) < * begin))
				pivot = begin;
			else if((*(end -1)< *pivot) && (*begin < *(end -1)))
				pivot = end -1;

			RandomAccessIterator<T> i = begin -1;
			RandomAccessIterator<T> jj = end;
			T pivotValue = *pivot;
			for(;;)
			{
				while((pivotValue < *(--jj)));
				while((*(++i) < pivotValue));

				if(i < jj)
					Swap(*i, *jj);
				else
					break;
			}

			InitialQuickSort(begin, jj + 1);
			begin = jj + 1;
		}
	};

	// Perform quick sort initial pass on an array using a compare function. Does not sort fully
	template <class T, class U>
	void InitialQuickSort(RandomAccessIterator<T> begin, RandomAccessIterator<T> end, U compare)
	{
		while(end - begin > QUICKSORT_THRESHOLD)
		{
			RandomAccessIterator<T> pivot = begin + ((end - begin) / 2);
			if(compare(*begin, *pivot) && compare(*(end -1) , * begin))
				pivot = begin;
			else if(compare(*(end -1), *pivot) && compare(*begin, *(end -1)))
				pivot = end -1;

			RandomAccessIterator<T> i = begin -1;
			RandomAccessIterator<T> jj = end;
			T pivotValue = *pivot;
			for(;;)
			{
				while(compare(pivotValue, *(--jj)));
				while(compare(*(++i), pivotValue));

				if(i < jj)
					Swap(*i, *jj);
				else
					break;
			}

			InitialQuickSort(begin, jj + 1, compare);
			begin = jj + 1;
		}
	};

	template <class T>
	void Sort(RandomAccessIterator<T> begin, RandomAccessIterator<T> end)
	{
		InitialQuickSort(begin, end);
		InsertionSort(begin, end);
	}

	template <class T, class U>
	void Sort(RandomAccessIterator<T> begin, RandomAccessIterator<T> end, U compare)
	{
		InitialQuickSort(begin, end, compare);
		InsertionSort(begin, end, compare);
	};
}

#endif //URHO3DCOPY_SORT_H
