#pragma once

#include"iterator.h" // ʹ������ȡ���������͵ĸ�������

namespace esgstl {
	template <class RandomIter,class Distance,class T>
	void push_heap_aux(RandomIter first, Distance holeIndex, Distance topIndex, T value) {
		// holeIndex �� 1 �� �� 2����Ϊ��ڵ��±�ֱ�ӳ�2�Ǹ��ڵ��±꣬��1��2���Ǹ��ڵ��±�
		// ���ҽڵ�ֱ�ӳ�2���Ǹ��ڵ��±��1������1��������Ϊ���ڵ��±꣬��˼�1��2���ȶ���ø��ڵ��±�
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

	// ʹ�ø����ıȽϷº�������Ԫ�رȽ�
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
		// ����
		// ��¼�����ڵ��±�
		Distance topIndex = holeIndex;
		// ���ӽڵ���±�
		Distance rightIndex = 2 * holeIndex + 2;
		while (rightIndex < len) {
			// �����ӽڵ��ֵҪ�������ӽڵ� 
			if (*(first + rightIndex) < *(first + rightIndex - 1)) rightIndex = rightIndex - 1;
			// �ֵ���ڽϴ�ڵ��ֵ
			*(first + holeIndex) = *(first + rightIndex);
			// ��������
			holeIndex = rightIndex;
			rightIndex = 2 * holeIndex + 2;
		}
		// ����������������ֻ����ڵ�û���ҽڵ�
		if (rightIndex == len) {
			*(first + holeIndex) = *(first + (rightIndex - 1));
			holeIndex = rightIndex - 1;
		}
		// ����
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

	// �Զ���Ƚ���
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
		// ����popԪ�أ�ֱ��ֻʣһ��Ԫ��
		while (last - first > 1)
			pop_heap(first, last--);
	}

	template <class RandomIter, class Compare>
	void sort_heap(RandomIter first, RandomIter last, Compare comp) {
		// ����popԪ�أ�ֱ��ֻʣһ��Ԫ��
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
			// ����if��break��ʽ����ΪholeIndexʵ��Ҫ��Ϊ�����Ż���ֹѭ��
			// ��distance��һ���޷������������ж�
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
			// ����if��break��ʽ����ΪholeIndexʵ��Ҫ��Ϊ�����Ż���ֹѭ��
			// ��distance��һ���޷������������ж�
			if (holeIndex == 0) return;
			--holeIndex;
		}
	}

	template <class RandomIter, class Compare>
	void make_heap(RandomIter first, RandomIter last, Compare comp) {
		make_heap_aux(first, last, esgstl::distance_type(first),comp);
	}

}