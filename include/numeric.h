#pragma once

#include "iterator.h"
#include "functional.h"

namespace esgstl {

	// accumulate
	// ����һ������ΪInputIter����ĵ������ͳ�ֵinit��������������ֵ�ۼӺ�
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
	// ����һ������ΪInputIter����ĵ��������Լ�һ�����������
	// �������ֵ����λ���������Ԫ��������������������
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
	// ������[first1,last1)Ϊ���Ȼ�׼�������������[first2,?)���ڻ��������������ӣ���������Ԫ����ĺ�
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
	// ����һ������ΪInputIter����ĵ��������Լ�һ�����������
	// �������ֵ���ۼӺͣ���������Ԫ��������������������
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
		// �Ż�ǰ�߼���
		/*
			auto tmp = identity_element(pred);
			while (n >> 1) {
				// ��������
				if (n & 1) {
					tmp = pred(tmp, value);
					--n;
				}
				// ż������
				else {
					value = pred(value, value);
					n >>= 1;
				}
			}
			value = pred(value, tmp);
			return value;
		*/
		// �Ż����߼�:
		// ��Ҫ�Ż�����Ϊn����ż���������������������--���һ����ż������˲���Ҫ�ٴ��ж�
		// ������������������ż����nֻҪ����0�������Լ����Լ�һ��
		while ((n & 1) != 0) {
			value = pred(value, value);
			n >>= 1;
		}
		// ��ʱnһ��������
		// res��¼����������˻�
		auto res = value;
		n >>= 1;
		// ���n�Ѿ���1����������һλ���������ѭ����res���ǽ��
		// ���n����1����Ϊ������һ�Σ�value�����Գ�һ��
		while (n != 0) {
			// ��ѭ�����Գ�
			value = pred(value, value);
			// �����Ƿ�Ϊ������������˵�res��ȥ
			// ���һ�ν�ѭ��nһ����1������value��ֵ��һ����˵�res��ȥ��
			if (n & 1)
				res = pred(res, value);
			n >>= 1;
		}
		return res;
	}

}