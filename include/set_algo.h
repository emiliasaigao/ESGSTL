#pragma once

#include "algobase.h"

namespace esgstl {

	// 区间1和区间2的并集
	template <class InputIter, class OutputIter>
	OutputIter set_union(InputIter first1, InputIter last1,
		InputIter first2, InputIter last2, OutputIter result) {
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2) {
				*result = *first1;
				++first1;
			}
			else if (*first2 < *first1) {
				*result = *first2;
				++first2;
			}
			else {
				*result = *first1;
				++first1; ++first2;
			}
			++result;
		}
		return copy(first2, last2, copy(first1, last1, result));
	}

	template <class InputIter, class OutputIter, class Compare>
	OutputIter set_union(InputIter first1, InputIter last1,
		InputIter first2, InputIter last2, OutputIter result, Compare comp) {
		while (first1 != last1 && first2 != last2) {
			if (comp(*first1,*first2)) {
				*result = *first1;
				++first1;
			}
			else if (comp(*first2 ,*first1)) {
				*result = *first2;
				++first2;
			}
			else {
				*result = *first1;
				++first1; ++first2;
			}
			++result;
		}
		return copy(first2, last2, copy(first1, last1, result));
	}

	// 区间1和区间2的交集
	template <class InputIter, class OutputIter>
	OutputIter set_intersection(InputIter first1, InputIter last1,
		InputIter first2, InputIter last2, OutputIter result) {
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2) {
				++first1;
			}
			else if (*first2 < first1) {
				++first2;
			}
			else {
				*result == *first1;
				++result;
				++first1; 
				++first2;
			}
		}
		return result;
	}

	template <class InputIter, class OutputIter, class Compare>
	OutputIter set_intersection(InputIter first1, InputIter last1,
		InputIter first2, InputIter last2, OutputIter result, Compare comp) {
		while (first1 != last1 && first2 != last2) {
			if (comp(*first1, *first2)) {
				++first1;
			}
			else if (comp(*first2, *first1)) {
				++first2;
			}
			else {
				*result == *first1;
				++result;
				++first1;
				++first2;
			}
		}
		return result;
	}

	// 只存在于区间1，不存在于区间2的差集
	template <class InputIter, class OutputIter>
	OutputIter set_difference(InputIter first1, InputIter last1,
		InputIter first2, InputIter last2, OutputIter result) {
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2) {
				*result = *first1;
				++first1;
				++result;
			}
			else if (*first2 < *first1) {
				++first2;
			}
			else {
				++first1;
				++first2;
			}
		}
		return copy(first1, last1, result);
	}

	template <class InputIter, class OutputIter, class Compare>
	OutputIter set_difference(InputIter first1, InputIter last1,
		InputIter first2, InputIter last2, OutputIter result, Compare comp) {
		while (first1 != last1 && first2 != last2) {
			if (comp(*first1, *first2)) {
				*result = *first1;
				++first1;
				++result;
			}
			else if (comp(*first2, *first1)) {
				++first2;
			}
			else {
				++first1;
				++first2;
			}
		}
		return copy(first1, last1, result);
	}

	// 只存在于区间1或只存在于区间2的对称差集
	template <class InputIter, class OutputIter>
	OutputIter set_symmetric_difference(InputIter first1, InputIter last1,
		InputIter first2, InputIter last2, OutputIter result) {
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2) {
				*result = *first1;
				++first1;
				++result;
			}
			else if (*first2 < *first1) {
				*result = *first2;
				++first2;
				++result;
			}
			else {
				++first1;
				++first2;
			}
		}
		return copy(first2, last2, copy(first1, last1, result));
	}

	template <class InputIter, class OutputIter, class Compare>
	OutputIter set_symmetric_difference(InputIter first1, InputIter last1,
		InputIter first2, InputIter last2, OutputIter result, Compare comp) {
		while (first1 != last1 && first2 != last2) {
			if (comp(*first1, *first2)) {
				*result = *first1;
				++first1;
				++result;
			}
			else if (comp(*first2, *first1)) {
				*result = *first2;
				++first2;
				++result;
			}
			else {
				++first1;
				++first2;
			}
		}
		return copy(first2,last2,copy(first1, last1, result));
	}
}