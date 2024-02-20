#pragma once

#include"iterator.h" // 使用了提取迭代器类型的辅助函数

namespace esgstl {
	template <class RandomIter,class Distance,class T>
	void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value) {
		// holeIndex 减 1 再 除 2是因为左节点下标直接除2是父节点下标，减1除2还是父节点下标
		// 但右节点直接除2将是父节点下标加1，而减1除而则会变为父节点下标，因此减1除2能稳定获得父节点下标
		auto parentIndex = (holeIndex - 1) / 2;
		while (holeIndex != topIndex && *(first + parentIndex) < value) {
			*(first + holeIndex) = *(first + parentIndex);
			holeIndex = parentIndex;
			parentIndex = (holeIndex - 1) / 2;
		}
		*(first + holeIndex) = value;
	}

	template <class RandomIter, class Distance>
	void push_heap_d(RandomIter first, RandomIter last, Distance*) {
		esgstl::push_heap_aux(first, last - first - 1, static_cast<Distance>(0), *(last - 1));
	}
	
	template <class RandomIter>
	void push_heap(RandomIter first, RandomIter last) {
		esgstl::push_heap_d(first, last, esgstl::distance_type(first));
	}

	// 使用给定的比较仿函数进行元素比较
	template <class RandomIter, class Distance, class T, class Compare>
	void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value, Compare comp) {
		auto parentIndex = (holeIndex - 1) / 2;
		while (holeIndex != topIndex && comp(*(first + parentIndex),value)) {
			*(first + holeIndex) = *(first + parentIndex);
			holeIndex = parentIndex;
			parentIndex = (holeIndex - 1) / 2;
		}
		*(first + holeIndex) = value;
	}

	template <class RandomIter, class Distance, class Compare>
	void push_heap_d(RandomIter first, RandomIter last, Distance*, Compare comp) {
		esgstl::push_heap_aux(first, last - first - 1, static_cast<Distance>(0), *(last - 1),comp);
	}

	template <class RandomIter, class Compare>
	void push_heap(RandomIter first, RandomIter last, Compare comp) {
		esgstl::push_heap_d(first, last, esgstl::distance_type(first),comp);
	}

	template <class RandomIter, class Distance, class T>
	void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value) {
		// 下溯
		// 记录树顶节点下标
		Distance topIndex = holeIndex;
		// 右子节点的下标
		Distance rightIndex = 2 * holeIndex + 2;
		while (rightIndex < len) {
			// 若左子节点的值要大于右子节点 
			if (*(first + rightIndex) < *(first + rightIndex - 1)) rightIndex = rightIndex - 1;
			// 令洞值等于较大节点的值
			*(first + holeIndex) = *(first + rightIndex);
			// 继续下溯
			holeIndex = rightIndex;
			rightIndex = 2 * holeIndex + 2;
		}
		// 若最后遍历到的子树只有左节点没有右节点
		if (rightIndex == len) {
			*(first + holeIndex) = *(first + (rightIndex - 1));
			holeIndex = rightIndex - 1;
		}
		// 上溯
		push_heap_aux(first, holeIndex, topIndex, value);
	}

	template <class RandomIter, class Distance, class T>
	void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance*) {
		*result = *first;
		esgstl::adjust_heap(first, static_cast<Distance>(0), last - first, value);
	}

	template <class RandomIter>
	void pop_heap(RandomIter first, RandomIter last) {
		pop_heap_aux(first, last - 1, last - 1, *(last - 1), esgstl::distance_type(first));
	}

	// 自定义比较器
	template <class RandomIter, class Distance, class T, class Compare>
	void adjust_heap(RandomIter first, Distance holeIndex, Distance len, T value, Compare comp) {
		Distance topIndex = holeIndex;
		Distance rightIndex = 2 * holeIndex + 2;
		while (rightIndex < len) {
			if (comp(*(first + rightIndex), *(first + rightIndex - 1))) rightIndex = rightIndex - 1;
			*(first + holeIndex) = *(first + rightIndex);
			holeIndex = rightIndex;
			rightIndex = 2 * holeIndex + 2;
		}
		if (rightIndex == len) {
			*(first + holeIndex) = *(first + (rightIndex - 1));
			holeIndex = rightIndex - 1;
		}
		push_heap_aux(first, holeIndex, topIndex, value,comp);
	}

	template <class RandomIter, class Distance, class T, class Compare>
	void pop_heap_aux(RandomIter first, RandomIter last, RandomIter result, T value, Distance*, Compare comp) {
		*result = *first;
		esgstl::adjust_heap(first, static_cast<Distance>(0), last - first, value, comp);
	}

	template <class RandomIter, class Compare>
	void pop_heap(RandomIter first, RandomIter last, Compare comp) {
		pop_heap_aux(first, last - 1, last - 1, *(last - 1), esgstl::distance_type(first), comp);
	}

	template <class RandomIter>
	void sort_heap(RandomIter first, RandomIter last) {
		// 不断pop元素，直到只剩一个元素
		while (last - first > 1)
			pop_heap(first, last--);
	}

	template <class RandomIter, class Compare>
	void sort_heap(RandomIter first, RandomIter last, Compare comp) {
		// 不断pop元素，直到只剩一个元素
		while (last - first > 1)
			pop_heap(first, last--,comp);
	}

	template <class RandomIter, class Distance>
	void make_heap_aux(RandomIter first, RandomIter last, Distance*) {
		if (last - first < 2) return;
		Distance len = last - first;
		Distance holeIndex = (len - 2) / 2;
		while (true) {
			adjust_heap(first, holeIndex, len, *(first + holeIndex));
			// 采用if的break方式是因为holeIndex实际要减为负数才会终止循环
			// 但distance是一个无符号数，不好判断
			if (holeIndex == 0) return;
			--holeIndex;
		}
	}

	template <class RandomIter>
	void make_heap(RandomIter first, RandomIter last) {
		make_heap_aux(first, last, esgstl::distance_type(first));
	}

	template <class RandomIter, class Distance, class Compare>
	void make_heap_aux(RandomIter first, RandomIter last, Distance*, Compare comp) {
		if (last - first < 2) return;
		Distance len = last - first;
		Distance holeIndex = (len - 2) / 2;
		while (true) {
			adjust_heap(first, holeIndex, len, *(first + holeIndex),comp);
			// 采用if的break方式是因为holeIndex实际要减为负数才会终止循环
			// 但distance是一个无符号数，不好判断
			if (holeIndex == 0) return;
			--holeIndex;
		}
	}

	template <class RandomIter, class Compare>
	void make_heap(RandomIter first, RandomIter last, Compare comp) {
		make_heap_aux(first, last, esgstl::distance_type(first),comp);
	}

}