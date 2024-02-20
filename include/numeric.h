#pragma once

#include "iterator.h"
#include "functional.h"

namespace esgstl {

	// accumulate
	// 接受一对至少为InputIter级别的迭代器和初值init，返回其与区间值累加和
	template <class InputIter, class T>
	T accumulate(InputIter first, InputIter last, T init) {
		while (first != last) {
			init = init + *first;
			++first;
		}
		return init;
	}

	template <class InputIter, class T, class BinaryPred>
	T accumulate(InputIter first, InputIter last, T init, BinaryPred pred) {
		while (first != last) {
			init = pred(init, *first++);
			++first;
		}
		return init;
	}

	// adjacent_diffence
	// 接受一对至少为InputIter级别的迭代器，以及一个输出迭代器
	// 将区间初值和逐位差（或其他二元操作）输出至输出迭代器
	template <class InputIter, class OutputIter>
	OutputIter adjacent_diffence(InputIter first, InputIter last, OutputIter result) {
		if (first == last) return result;
		*result = *first;
		auto pre = *first;
		while (++first != last) {
			auto cur = *first;
			*++result = cur - pre;
			pre = cur;
		}
		return ++result;
	}

	template <class InputIter, class OutputIter, class BinaryPred>
	OutputIter adjacent_diffence(InputIter first, InputIter last, OutputIter result, BinaryPred pred) {
		if (first == last) return result;
		*result = *first;
		auto pre = *first;
		while (++first != last) {
			auto cur = *first;
			*++result = pred(cur,pre);
			pre = cur;
		}
		return ++result;
	}

	// inner_product
	// 以区间[first1,last1)为长度基准，计算与区间二[first2,?)的内积（两两相乘再相加）或其他二元运算的和
	template <class InputIter, class OutputIter, class T>
	T inner_prodcut(InputIter first1, InputIter last1, InputIter first2, T init) {
		while (first1 != last1) {
			init = init + (*first1 * (*first2));
			++first1; ++first2;
		}
		return init;
	}

	template <class InputIter, class OutputIter, class T, class BinaryPred>
	T inner_prodcut(InputIter first1, InputIter last1, InputIter first2, T init, BinaryPred pred) {
		while (first1 != last1) {
			init = init + pred(*first1 , *first2);
			++first1; ++first2;
		}
		return init;
	}

	// partial_sum
	// 接受一对至少为InputIter级别的迭代器，以及一个输出迭代器
	// 将区间初值和累加和（或其他二元操作）输出至输出迭代器
	template <class InputIter, class OutputIter>
	OutputIter partial_sum(InputIter first, InputIter last, OutputIter result) {
		if (first == last) return result;
		*result = *first;
		auto sum = *first;
		while (++first != last) {
			sum = sum + *first;
			*++result = sum;
		}
		return ++result;
	}

	template <class InputIter, class OutputIter, class BinaryPred>
	OutputIter partial_sum(InputIter first, InputIter last, OutputIter result, BinaryPred pred) {
		if (first == last) return result;
		*result = *first;
		auto sum = *first;
		while (++first != last) {
			sum = pred(sum,*first);
			*++result = sum;
		}
		return ++result;
	}

	// power
	template <class T, class Integer>
	T power(T value, Integer n) {
		return power(value, n, mutiplies<T>());
	}

	template <class T, class Integer, class BinaryPred>
	T power(T value, Integer n, BinaryPred pred) {
		if (n == 0) return identity_element(pred);
		// 优化前逻辑：
		/*
			auto tmp = identity_element(pred);
			while (n >> 1) {
				// 奇数进这
				if (n & 1) {
					tmp = pred(tmp, value);
					--n;
				}
				// 偶数进这
				else {
					value = pred(value, value);
					n >>= 1;
				}
			}
			value = pred(value, tmp);
			return value;
		*/
		// 优化后逻辑:
		// 主要优化，因为n不是偶数就是奇数，这次是奇数--后就一定是偶数，因此不需要再次判断
		// 即，无论是奇数还是偶数，n只要不是0，都会自己乘自己一次
		while ((n & 1) != 0) {
			value = pred(value, value);
			n >>= 1;
		}
		// 此时n一定是奇数
		// res记录多余的奇数乘积
		auto res = value;
		n >>= 1;
		// 如果n已经是1，上面右移一位将不会进入循环，res就是结果
		// 如果n不是1，因为右移了一次，value必须自乘一次
		while (n != 0) {
			// 进循环先自乘
			value = pred(value, value);
			// 再判是否为奇数，奇数则乘到res上去
			// 最后一次进循环n一定是1，所以value的值是一定会乘到res上去的
			if (n & 1)
				res = pred(res, value);
			n >>= 1;
		}
		return res;
	}

}