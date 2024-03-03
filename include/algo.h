#pragma once

#include <cstddef>
#include <ctime>
#include "iterator.h"
#include "algobase.h"
#include "heap_algo.h"
#include "memory.h"

namespace esgstl {

	// adjacent_find
	// �ҵ������е�һ����ȵ�����Ԫ�ص���Ԫ��,�Ҳ�����������β������
	/****************************************************************************/
	// �汾1����==�е�
	template <class ForwardIter>
	ForwardIter adjacent_find(ForwardIter first, ForwardIter last) {
		if (first == last) return last;
		auto next = first;
		while (++next != last) {
			if (*first == *next) return first;
			first = next;
		}
		return last;
	}

	// �汾2���÷º���pred�е�
	template <class ForwardIter, class BinaryPred>
	ForwardIter adjacent_find(ForwardIter first, ForwardIter last, BinaryPred pred) {
		if (first == last) return last;
		auto next = first;
		while (++next != last) {
			if (pred(*first, *next)) return first;
			first = next;
		}
		return last;
	}
	/****************************************************************************/

	// count
	// ����������ĳ��value���ֵĴ���
	/****************************************************************************/
	template <class InputIter, class T>
	iterator_traits<InputIter>::difference_type
		count(InputIter first, InputIter last, const T& value) {
		typename iterator_traits<InputIter>::difference_type n = 0;
		while (first != last) {
			if (*first == value) ++n;
			++first;
		}
		return n;
	}
	/****************************************************************************/

	// count_if
	// ��������������������Ԫ�ظ���
	/****************************************************************************/
	template <class InputIter, class UnaryPred>
	iterator_traits<InputIter>::difference_type
		count_if(InputIter first, InputIter last, UnaryPred pred) {
		typename iterator_traits<InputIter>::difference_type n = 0;
		while (first != last) {
			if (pred(*first)) ++n;
			++first;
		}
		return n;
	}
	/****************************************************************************/

	// find
	// ���������е�һ�γ���value��λ��
	/****************************************************************************/
	// �汾1����==�е�
	template <class InputIter, class T>
	InputIter find(InputIter first, InputIter last, const T& value) {
		while (first != last) {
			if (*first == value) return first;
			++first;
		}
		return last;
	}

	// �汾2����Compare�º����е�
	template <class InputIter, class T, class Compare>
	InputIter find(InputIter first, InputIter last, const T& value, Compare comp) {
		while (first != last) {
			if (comp(*first, value)) return first;
			++first;
		}
		return last;
	}
	/****************************************************************************/

	// find_if
	// ���������е�һ������һԪν��������λ��
	/****************************************************************************/
	template <class InputIter, class UnaryPred>
	InputIter find_if(InputIter first, InputIter last, UnaryPred pred) {
		while (first != last) {
			if (pred(*first)) return first;
		}
		return last;
	}
	/****************************************************************************/

	// search
	// ��Ѱ����1���Ƿ���� ���� ������2
	/****************************************************************************/
	// �汾1����==�е�
	template <class ForwardIter1, class ForwardIter2>
	ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2) {
		auto len1 = esgstl::distance(first1, last1);
		auto len2 = esgstl::distance(first2, last2);

		if (len1 < len2) return last1;
		// ����ָ��ֱ�ָ������1������2��ͷ��
		auto cur1 = first1;
		auto cur2 = first2;
		// ָ��cur2�ߵ�����2βʱ��ƥ��ɹ�������first1
		while (cur2 != last2) {
			if (*cur1 == *cur2) {
				++cur1; ++cur2;
			}
			else {
				if (len1 == len2) return last1;
				--len1;
				// ��first1����һ��Ԫ�����¿�ʼƥ�䣬first1ҲҪ����
				cur1 = ++first1;
				cur2 = first2;
			}
		}
		return first1;
	}

	// �汾2���ø���Compare�º����е�
	template <class ForwardIter1, class ForwardIter2, class Compare>
	ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, Compare comp) {
		auto len1 = esgstl::distance(first1, last1);
		auto len2 = esgstl::distance(first2, last2);

		if (len1 < len2) return last1;
		// ����ָ��ֱ�ָ������1������2��ͷ��
		auto cur1 = first1;
		auto cur2 = first2;
		// ָ��cur2�ߵ�����2βʱ��ƥ��ɹ�������first1
		while (cur2 != last2) {
			if (comp(*cur1, *cur2)) {
				++cur1; ++cur2;
			}
			else {
				if (len1 == len2) return last1;
				--len1;
				// ��first1����һ��Ԫ�����¿�ʼƥ�䣬first1ҲҪ����
				cur1 = ++first1;
				cur2 = first2;
			}
		}
		return first1;
	}
	/****************************************************************************/

	// find_end
	// �ҵ�����������[first2,last2��������[first1,last1)�г��ֵ�����λ��
	/****************************************************************************/
	// �汾1����==�е�
	// �����ѣ��������һ���ѵ��Ľ��
	template <class ForwardIter1, class ForwardIter2>
	ForwardIter1 find_end_cat(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2,
		esgstl::forward_iterator_tag, esgstl::forward_iterator_tag) {
		if (first2 == last2) return last1;
		auto res = last1;
		while (true) {
			auto new_res = search(first1, last1, first2, last2);
			if (new_res == last1) return res;
			first1 = new_res;
			res = new_res;
			++first1;
		}
		return last1;
	}

	// �����ѣ��ѵ��ĵ�һ�����
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter1 find_end_cat(BidirectionalIter1 first1, BidirectionalIter1 last1,
		BidirectionalIter2 first2, BidirectionalIter2 last2,
		esgstl::bidirectional_iterator_tag, esgstl::bidirectional_iterator_tag) {
		typedef esgstl::reverse_iterator<BidirectionalIter1> revIter1;
		typedef esgstl::reverse_iterator<BidirectionalIter2> revIter2;
		revIter1 rf1(last1);
		revIter1 rl1(first1);
		revIter1 rf2(last2);
		revIter1 rl2(first2);
		auto tmp = search(rf1, rl1, rf2, rl2);
		if (tmp == rl1) return last1;
		auto res = tmp.base();
		return esgstl::advance(res, -distance(first2, last2));
	}

	template <class ForwardIter1, class ForwardIter2>
	ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2) {
		return find_end_cat(first1, last1, first2, last2,
			esgstl::iterator_category(first1), esgstl::iterator_category(first2));
	}

	// �汾2���ø���Compare�º����е�
	template <class ForwardIter1, class ForwardIter2, class Compare>
	ForwardIter1 find_end_cat(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2,
		esgstl::forward_iterator_tag, esgstl::forward_iterator_tag,
		Compare comp) {
		if (first2 == last2) return last1;
		auto res = last1;
		while (true) {
			auto new_res = search(first1, last1, first2, last2, comp);
			if (new_res == last1) return res;
			first1 = new_res;
			res = new_res;
			++first1;
		}
		return last1;
	}

	template <class BidirectionalIter1, class BidirectionalIter2, class Compare>
	BidirectionalIter1 find_end_cat(BidirectionalIter1 first1, BidirectionalIter1 last1,
		BidirectionalIter2 first2, BidirectionalIter2 last2,
		esgstl::bidirectional_iterator_tag, esgstl::bidirectional_iterator_tag,
		Compare comp) {
		typedef esgstl::reverse_iterator<BidirectionalIter1> revIter1;
		typedef esgstl::reverse_iterator<BidirectionalIter2> revIter2;
		revIter1 rf1(last1);
		revIter1 rl1(first1);
		revIter1 rf2(last2);
		revIter1 rl2(first2);
		auto tmp = search(rf1, rl1, rf2, rl2);
		if (tmp == rl1) return last1;
		auto res = tmp.base();
		return esgstl::advance(res, -distance(first2, last2));
	}

	template <class ForwardIter1, class ForwardIter2, class Compare>
	ForwardIter1 find_end(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2,
		Compare comp) {
		return find_end_cat(first1, last1, first2, last2,
			esgstl::iterator_category(first1), esgstl::iterator_category(first2), comp);
	}
	/****************************************************************************/

	// find_first_of
	// ��������2�е��κ�Ԫ��������1�е�һ�γ��ֵ�λ��
	/****************************************************************************/
	// �汾1��ʹ��==�е�
	template <class InputIter, class ForwardIter>
	InputIter find_first_of(InputIter first1, InputIter last1,
		ForwardIter first2, ForwardIter last2) {
		while (first1 != last1) {
			for (auto cur = first2; cur != last2; ++cur) {
				if (*first1 == *cur) return first1;
			}
			++first1;
		}
		return last1;
	}

	//�汾2���ø���Compare�º����е�
	template <class InputIter, class ForwardIter, class Compare>
	InputIter find_first_of(InputIter first1, InputIter last1,
		ForwardIter first2, ForwardIter last2, Compare comp) {
		while (first1 != last1) {
			for (auto cur = first2; cur != last2; ++cur) {
				if (comp(*first1, *cur)) return first1;
			}
			++first1;
		}
		return last1;
	}
	/****************************************************************************/

	// for_each
	// ����������Ԫ������ͬ�Ĳ���������֤�ܹ����ܸ�ֵ��Ϊ
	/****************************************************************************/
	template <class InputIter, class UnaryPred>
	void for_each(InputIter first, InputIter last, UnaryPred pred) {
		while (first != last) {
			pred(*first);
			++first;
		}
	}
	/****************************************************************************/

	// generate
	// ���º���Generate�Ľ����д������[first,last)��
	/****************************************************************************/
	template <class ForwardIter, class Generate>
	void generate(ForwardIter first, ForwardIter last, Generate gen) {
		while (first != last) {
			*first = gen();
			++first;
		}
	}
	/****************************************************************************/

	// generate_n
	// ���º���Generate�Ľ����д����first��ʼ����Ϊn��������
	/****************************************************************************/
	template <class OutputIter, class Size, class Generate>
	void generate(OutputIter first, Size n, Generate gen) {
		while (n > 0) {
			*first = gen();
			++first; --n;
		}
	}
	/****************************************************************************/

	// includes
	// �ж���������2�Ƿ�����������1���Ӽ�
	/****************************************************************************/
	// �汾1����С�ڱȽ�Ԫ��
	template <class ForwardIter1, class ForwardIter2>
	bool includes(ForwardIter1 first1, ForwardIter2 last1,
		ForwardIter2 first2, ForwardIter2 last2) {
		auto len1 = esgstl::distance(first1, last1);
		auto len2 = esgstl::distance(first2, last2);
		if (len1 < len2) return false;
		while (first1 != last1) {
			if (first2 == last2) return true;
			if (*first1 < *first2) ++first1;
			else if (*first2 < *first1) {
				return false;
			}
			else {
				++first1; ++first2;
			}
		}
		return false;
	}

	// �汾2����Compare�º����Ƚ�Ԫ��
	template <class ForwardIter1, class ForwardIter2, class Compare>
	bool includes(ForwardIter1 first1, ForwardIter2 last1,
		ForwardIter2 first2, ForwardIter2 last2, Compare comp) {
		auto len1 = esgstl::distance(first1, last1);
		auto len2 = esgstl::distance(first2, last2);
		if (len1 < len2) return false;
		while (first1 != last1) {
			if (first2 == last2) return true;
			if (comp(*first1, *first2)) ++first1;
			else if (comp(*first2, *first1)) {
				return false;
			}
			else {
				++first1; ++first2;
			}
		}
		return false;
	}
	/****************************************************************************/

	// max_element
	// �����������ֵ�ĵ�����
	/****************************************************************************/
	// �汾1����<�Ƚ�
	template <class ForwardIter>
	ForwardIter max_element(ForwardIter first, ForwardIter last) {
		if (first == last) return first;
		auto res = first;
		while (++first != last) {
			if (*res < *first) *res = *first;
		}
		return res;
	}

	// �汾2����Compare�º����Ƚ�
	template <class ForwardIter, class Compare>
	ForwardIter max_element(ForwardIter first, ForwardIter last, Compare comp) {
		if (first == last) return first;
		auto res = first;
		while (++first != last) {
			if (comp(*res, *first)) *res = *first;
		}
		return res;
	}
	/****************************************************************************/

	// min_element
	// �����������ֵ�ĵ�����
	/****************************************************************************/
	// �汾1����<�Ƚ�
	template <class ForwardIter>
	ForwardIter min_element(ForwardIter first, ForwardIter last) {
		if (first == last) return first;
		auto res = first;
		while (++first != last) {
			if (*first < *res) *res = *first;
		}
		return res;
	}

	// �汾2����Compare�º����Ƚ�
	template <class ForwardIter, class Compare>
	ForwardIter min_element(ForwardIter first, ForwardIter last, Compare comp) {
		if (first == last) return first;
		auto res = first;
		while (++first != last) {
			if (comp(*first, *res)) *res = *first;
		}
		return res;
	}
	/****************************************************************************/

	// merge_backward
	// ���źϲ���������
	/****************************************************************************/
	// �汾1����<�ϲ�
	template <class BidirectionalIter1, class BidirectionalIter2, class BidirectionalIter3>
	BidirectionalIter3 merge_backward(BidirectionalIter1 first1, BidirectionalIter1 last1,
		BidirectionalIter2 first2, BidirectionalIter2 last2, BidirectionalIter3 result) {
		if (first1 == last1) return copy_backward(first2, last2, result);
		if (first2 == last2) return copy_backward(first1, last1, result);
		--last1; --last2;
		while (true) {
			if (*last1 < *last2) {
				*--result = *last2;
				if (last2 == first2) return copy_backward(first1, ++last1, result);
				--last2;
			}
			else {
				*--result = *last1;
				if (last1 == first1) return copy_backward(first2, ++last2, result);
				--last1;
			}
		}
	}

	// �汾2����Compare�º����ϲ�
	template <class BidirectionalIter1, class BidirectionalIter2, class BidirectionalIter3, class Compare>
	BidirectionalIter3 merge_backward(BidirectionalIter1 first1, BidirectionalIter1 last1,
		BidirectionalIter2 first2, BidirectionalIter2 last2,
		BidirectionalIter3 result, Compare comp) {
		if (first1 == last1) return copy_backward(first2, last2, result);
		if (first2 == last2) return copy_backward(first1, last1, result);
		--last1; --last2;
		while (true) {
			if (comp(*last1, *last2)) {
				*--result = *last2;
				if (last2 == first2) return copy_backward(first1, ++last1, result);
				--last2;
			}
			else {
				*--result = *last1;
				if (last1 == first1) return copy_backward(first2, ++last2, result);
				--last1;
			}
		}
	}
	/****************************************************************************/

	// merge
	// ���źϲ���������
	/****************************************************************************/
	// �汾1����<�ϲ�
	template <class InputIter1, class InputIter2, class OutputIter>
	OutputIter merge(InputIter1 first1, InputIter1 last1,
		InputIter2 first2, InputIter2 last2, OutputIter result) {
		while (first1 != last1 && first2 != last2) {
			if (*first2 < *first1) {
				*result = *first2;
				++first2;
			}
			else {
				*result = *first1;
				++first1;
			}
			++result;
		}
		return copy(first2, last2, copy(first1, last1, result));
	}

	// �汾2����Compare�º����ϲ�
	template <class InputIter1, class InputIter2, class OutputIter, class Compare>
	OutputIter merge(InputIter1 first1, InputIter1 last1,
		InputIter2 first2, InputIter2 last2, OutputIter result, Compare comp) {
		while (first1 != last1 && first2 != last2) {
			if (comp(*first2, *first1)) {
				*result = *first2;
				++first2;
			}
			else {
				*result = *first1;
				++first1;
			}
			++result;
		}
		return copy(first2, last2, copy(first1, last1, result));
	}
	/****************************************************************************/

	// partition
	// �����䰴������һԪν�ʣ�һ��Ϊ���������н�ĵ�����
	/****************************************************************************/
	template <class BidirectionalIter, class UnaryPred>
	BidirectionalIter partition(BidirectionalIter first, BidirectionalIter last, UnaryPred pred) {
		auto left = first;
		auto right = last - 1;
		while (true) {
			while (left != right && pred(*left)) ++left;
			while (right != left && !pred(*right)) --right;
			if (left == right) return left;
			else {
				iter_swap(left, right);
			}
		}
	}
	/****************************************************************************/

	// remove_copy
	/****************************************************************************/
	// �������в�����ָ��ֵ��Ԫ�ذ����˳�򿽱���������������������ؽ�������β������
	template <class InputIter, class OutputIter, class T>
	OutputIter remove_copy(InputIter first, InputIter last, OutputIter result, const T& value) {
		while (first != last) {
			if ((*first) != value) {
				*result = *first;
				++result;
			}
			++first;
		}
		return result;
	}
	/****************************************************************************/

	// remove_copy_if
	/****************************************************************************/
	// �������в�����ɾ��������Ԫ�ذ����˳�򿽱���������������������ؽ�������β������
	template <class InputIter, class OutputIter, class UnaryPred>
	OutputIter remove_copy_if(InputIter first, InputIter last, OutputIter result, UnaryPred pred) {
		while (first != last) {
			if (!(pred(*first))) {
				*result = *first;
				++result;
			}
			++first;
		}
		return result;
	}
	/****************************************************************************/

	// remove
	// �������е���ָ��ֵ��Ԫ���ƶ�������ĩβ�����ء���ɾ�����䡱��ͷ������
	/****************************************************************************/
	template <class ForwardIter, class T>
	ForwardIter remove(ForwardIter first, ForwardIter last, const T& value) {
		first = find(first, last, value);
		return first == last ? first : remove_copy(first, last, first, value);
	}
	/****************************************************************************/

	// remove_if
	// ������������ɾ��������Ԫ���ƶ�������ĩβ�����ء���ɾ�����䡱��ͷ������
	/****************************************************************************/
	template <class ForwardIter, class UnaryPred>
	ForwardIter remove_if(ForwardIter first, ForwardIter last, UnaryPred pred) {
		first = find_if(first, last, pred);
		return first == last ? first : remove_copy_if(first, last, first, pred);
	}
	/****************************************************************************/

	// replace
	// �������еĵ��ڸ���ֵ�ľ�ֵ�滻Ϊ��ֵ
	/****************************************************************************/
	template <class ForwardIter, class T>
	void replace(ForwardIter first, ForwardIter last, const T& oldVal, const T& newVal) {
		while (first != last) {
			if (*first == oldVal) *first = newVal;
			++first;
		}
	}
	/****************************************************************************/

	// replace_copy
	// ����ԭ�����䣬�����������еĵ��ڸ���ֵ�ľ�ֵ�滻Ϊ��ֵ�����ؿ��������β������
	/****************************************************************************/
	template <class ForwardIter, class OutputIter, class T>
	void replace(ForwardIter first, ForwardIter last, OutputIter result, const T& oldVal, const T& newVal) {
		while (first != last) {
			if (*first == oldVal) {
				*result = newVal;
			}
			else {
				*result = *first;
			}
			++result;
			++first;
		}
		return result;
	}
	/****************************************************************************/

	// replace_if
	// �����������������ľ�ֵ�滻Ϊ��ֵ
	/****************************************************************************/
	template <class ForwardIter, class T, class UnaryPred>
	void replace_if(ForwardIter first, ForwardIter last, UnaryPred pred, const T& newVal) {
		while (first != last) {
			if (pred(*first)) *first = newVal;
			++first;
		}
	}
	/****************************************************************************/

	// replace_copy_if
	// ����ԭ�����䣬���������������������ľ�ֵ�滻Ϊ��ֵ�����ؿ��������β������
	/****************************************************************************/
	template <class ForwardIter, class OutputIter, class T, class UnaryPred>
	void replace_copy_if(ForwardIter first, ForwardIter last, OutputIter result, UnaryPred pred, const T& newVal) {
		while (first != last) {
			if (pred(*first)) {
				*result = newVal;
			}
			else {
				*result = *first;
			}
			++result;
			++first;
		}
		return result;
	}
	/****************************************************************************/

	// reverse
	// ��ת����
	/****************************************************************************/
	// ˫��������汾
	template <class BidirectionalIter>
	void reverse_cat(BidirectionalIter first, BidirectionalIter last, esgstl::bidirectional_iterator_tag) {
		while (true) {
			// ����������˳�ѭ����first��last����ʱ�������first��last����ʱ
			if (first == last || first == --last) return;
			// ����������������������㣬last�Ѿ�ǰ�ƣ�����swap��ֻ��Ҫ�ƶ�first����
			iter_swap(first, last);
			++first;
		}
	}

	// ����������汾
	template <class RandomIter>
	void reverse_cat(RandomIter first, RandomIter last, esgstl::random_access_iterator_tag) {
		while (first < last) {
			iter_swap(first, --last);
			++first;
		}
	}

	template <class BidirectionalIter>
	void reverse(BidirectionalIter first, BidirectionalIter last) {
		reverse_cat(first, last, esgstl::iterator_category(first));
	}
	/****************************************************************************/

	// reverse_copy
	// ��ת���䣬�����Ƶ���������
	/****************************************************************************/
	template <class BidirectionalIter, class OutputIter>
	OutputIter reverse_copy(BidirectionalIter first, BidirectionalIter last, OutputIter result) {
		while (last != first) {
			--last;
			*result = *last;
			++result;
		}
		return result;
	}
	/****************************************************************************/

	// ratate
	// ������������������ת��������
	/****************************************************************************/
	// ǰ�������
	template <class ForwardIter>
	void rotate_cat(ForwardIter first, ForwardIter mid, ForwardIter last, esgstl::forward_iterator_tag) {
		while (first != mid) {
			auto pre = esgstl::distance(first, mid);
			auto post = esgstl::distance(mid, last);
			if (post < pre) {
				first = swap_range(mid, last, first);
			}
			else {
				auto tmp = swap_range(first, mid, mid);
				first = mid;
				mid = tmp;
			}
		}
	}

	// ˫�������
	template <class BidirectionalIter>
	void rotate_cat(BidirectionalIter first, BidirectionalIter mid, BidirectionalIter last, esgstl::bidirectional_iterator_tag) {
		reverse(first, mid);
		reverse(mid, last);
		reverse(first, last);
	}

	// ˳ʱ����ת��������������汾rotate����������
	// initial��ָ��ʼ��ת��λ��
	//					1
	//				8	    2
	//			7				3
	//				6		4
	//					5
	// ��midλ��3����pre = 2��������������
	//		1		3	  |		2		4
	//		7		5	  |		8		6
	// ��midλ��5����pre = 4���������ĸ���
	// 1 5	 |	2 6	 |	3 7	 |	4  8
	// rotate_cycleÿ������ֻ��ʱ����תһ����
	template <class RandomIter, class Distance>
	void rotate_cycle(RandomIter first, RandomIter last, RandomIter initial, Distance pre) {
		// ��¼��ʼֵ
		auto ini_val = *initial;
		// ��ָ��
		auto p1 = initial;
		// ��ָ��
		auto p2 = initial + pre;
		while (p2 != initial) {
			// ��ָ��Ԫ��ֵ��ֵΪ��ָ��Ԫ��ֵ��ʵ����ʱ����ת����
			*p1 = *p2;
			// ��ָ����ǰ��
			p1 = p2;
			// ��ָ����ǰ�ߣ��ɻ��Ǽ���ģ���������ʵ�ģ���˳�����Χ�Ĳ���Ҫ��ͷ��ʼ��
			if (pre < last - p2) {
				p2 = p2 + pre;
			}
			else {
				p2 = first + (pre - (last - p2));
			}
		}
		*p1 = ini_val;
	}

	template <class RandomIter>
	void rotate_cat(RandomIter first, RandomIter mid, RandomIter last, esgstl::random_access_iterator_tag) {
		auto pre = mid - first;
		auto all = last - first;
		// ��all��pre���������m
		auto m = all, n = pre;
		while (n != 0) {
			auto tmp = m % n;
			m = n;
			n = tmp;
		}
		// mֵ��Ϊ��Ҫ��ת�Ļ�������
		// �����ܳ���Ϊǰ��εĳ���pre
		for (int i = 0; i < m; ++i) {
			rotate_cycle(first, last, first + i, pre);
		}
	}

	template <class ForwardIter>
	void rotate(ForwardIter first, ForwardIter mid, ForwardIter last) {
		if (first == mid || mid == last) return;
		rotate_cat(first, mid, last, esgstl::iterator_category(first));
	}
	/****************************************************************************/

	// ratate_copy
	// ������������������ת�������䣬�������ŵ���������
	/****************************************************************************/
	template <class InputIter, class OutputIter>
	OutputIter rotate_copy(InputIter first, InputIter mid, InputIter last, OutputIter result) {
		return copy(first, mid, copy(mid, last, result));
	}
	/****************************************************************************/

	// search_n
	// Ѱ��ָ�����ȵ������ֵ��ȵ����䣬��������ͷ������
	/****************************************************************************/
	// �汾1����==�е�
	template <class ForwardIter, class Size, class T>
	ForwardIter search_n(ForwardIter first, ForwardIter last, Size n, const T& value) {
		while (first != last) {
			first = find(first, last, value);
			if (first == last) return last;
			auto count = n;
			auto tmp = first;
			while (--count > 0 && ++tmp != last) {
				if (*tmp != value) break;
			}
			if (count == 0) return first;
			else first = tmp;
		}
		return last;
	}

	// �汾2����Compare�º����е�
	template <class ForwardIter, class Size, class T, class Compare>
	ForwardIter search_n(ForwardIter first, ForwardIter last, Size n, const T& value, Compare comp) {
		while (first != last) {
			first = find(first, last, value, comp);
			if (first == last) return last;
			auto count = n;
			auto tmp = first;
			while (--count > 0 && ++tmp != last) {
				if (comp(*tmp, value)) break;
			}
			if (count == 0) return first;
			else first = tmp;
		}
		return last;
	}
	/****************************************************************************/

	// transform
	/****************************************************************************/
	// �汾1
	// ������[first,last)��Ԫ�ؾ���UnaryPred�Ĳ���������������ָ������
	template <class InputIter, class OutputIter, class UnaryPred>
	OutputIter transform(InputIter first, InputIter last, OutputIter result, UnaryPred pred) {
		while (first != last) {
			*result = pred(*first);
			++result; ++first;
		}
		return result;
	}

	// �汾2
	// ������[first1,last1) ������[first2,?)��Ԫ��һһ��Ӧ��BinaryPred�Ĳ���������������ָ������
	template <class InputIter1, class InputIter2, class OutputIter, class BinaryPred>
	OutputIter transform(InputIter1 first1, InputIter1 last1, InputIter2 first2, OutputIter result, BinaryPred pred) {
		while (first1 != last1) {
			*result = pred(*first1, *first2);
			++result; ++first1; ++first2;
		}
		return result;
	}
	/****************************************************************************/

	// unique_copy
	// ����������ȥ�أ�������������ָ������
	/****************************************************************************/
	// �汾1����==�е�

	// ���������Ϊ���������ʱ
	template <class InputIter, class OutputIter>
	OutputIter unique_copy_cat(InputIter first, InputIter last, OutputIter result,
		esgstl::output_iterator_tag) {
		if (first == last) return result;
		auto pre = *first;
		*result = *first;
		while (++first != last) {
			if (*first != pre) {
				pre = *first;
				*++result = *first;
			}
		}
		return ++result;
	}

	// ���������Ϊ���������ʱ
	template <class InputIter, class ForwardIter>
	ForwardIter unique_copy_cat(InputIter first, InputIter last, ForwardIter result,
		esgstl::forward_iterator_tag) {
		if (first == last) return result;
		*result = *first;
		while (++first != last) {
			if (*first != *result) *++result = *first;
		}
		return ++result;
	}

	template <class InputIter, class OutputIter>
	OutputIter unique_copy(InputIter first, InputIter last, OutputIter result) {
		return unique_copy_cat(first, last, result, esgstl::iterator_category(result));
	}

	// �汾2����Compare�º����е�

	// ���������Ϊ���������ʱ
	template <class InputIter, class ForwardIter, class Compare>
	ForwardIter unique_copy_cat(InputIter first, InputIter last, ForwardIter result,
		esgstl::forward_iterator_tag, Compare comp) {
		if (first == last) return result;
		*result = *first;
		while (++first != last) {
			if (!comp(*first, *result)) *++result = *first;
		}
		return ++result;
	}

	// ���������Ϊ���������ʱ
	template <class InputIter, class OutputIter, class Compare>
	OutputIter unique_copy_cat(InputIter first, InputIter last, OutputIter result,
		esgstl::output_iterator_tag, Compare comp) {
		if (first == last) return result;
		auto pre = *first;
		*result = *first;
		while (++first != last) {
			if (!comp(*first, pre)) {
				pre = *first;
				*++result = *first;
			}
		}
		return ++result;
	}

	template <class InputIter, class OutputIter, class Compare>
	OutputIter unique_copy(InputIter first, InputIter last, OutputIter result, Compare comp) {
		return unique_copy_cat(first, last, result, esgstl::iterator_category(result), comp);
	}

	/****************************************************************************/


	// unique
	// ����������ȥ�أ�����ȥ�غ������β������
	/****************************************************************************/
	// �汾1����==�е�
	template <class ForwardIter>
	ForwardIter unique(ForwardIter first, ForwardIter last) {
		return unique_copy(first, last, first);
	}

	// �汾2����Compare�º����е�
	template <class ForwardIter, class Compare>
	ForwardIter unique(ForwardIter first, ForwardIter last, Compare comp) {
		return unique_copy(first, last, first, comp);
	}
	/****************************************************************************/

	// next_premutation
	// ��������������Ϊ������һ���У�����ǰ����û����һ�����У�����fasle
	/****************************************************************************/
	// �汾1����<��С��
	template <class BidirectionalIter>
	bool next_premutation(BidirectionalIter first, BidirectionalIter last) {
		if (first == last) return false;
		auto i = last;
		--i;
		if (first == i) return false;
		while (true) {
			// ���ȴӺ���ǰ�ҵ�һ������*i < *ii��Ԫ�أ��̶�ii
			auto ii = i;
			--i;
			if (*i < *ii) {
				auto j = last;
				// Ȼ��Ӻ���ǰ�ҵ���һ������*i < *j��Ԫ�أ�����i��j��ָԪ��
				while (!(*i < *--j));
				iter_swap(i, j);
				// ���תii������������
				reverse(ii, last);
				return true;
			}
			// ��i==first��ûreturn��ȥ��˵���Ѿ�û����һ�������ˣ����������ǵݼ�����
			// ��ת�������䣬������false
			if (i == first) {
				reverse(first, last);
				return false;
			}
		}
	}

	// �汾2���÷º���Compare��С��
	template <class BidirectionalIter, class Compare>
	bool next_premutation(BidirectionalIter first, BidirectionalIter last, Compare comp) {
		if (first == last) return false;
		auto i = last;
		--i;
		if (first == i) return false;
		while (true) {
			// ���ȴӺ���ǰ�ҵ�һ������*i < *ii��Ԫ�أ��̶�ii
			auto ii = i;
			--i;
			if (comp(*i, *ii)) {
				auto j = last;
				// Ȼ��Ӻ���ǰ�ҵ���һ������*i < *j��Ԫ�أ�����i��j��ָԪ��
				while (!comp(*i, *--j));
				iter_swap(i, j);
				// ���תii������������
				reverse(ii, last);
				return true;
			}
			// ��i==first��ûreturn��ȥ��˵���Ѿ�û����һ�������ˣ����������ǵݼ�����
			// ��ת�������䣬������false
			if (i == first) {
				reverse(first, last);
				return false;
			}
		}
	}
	/****************************************************************************/

	// prev_premutation
	// ��������������Ϊ������һ���У���������û����һ���У�����false
	/****************************************************************************/
	// �汾1����<��С��
	template <class BidirectionalIter>
	bool prev_premutation(BidirectionalIter first, BidirectionalIter last) {
		if (first == last) return false;
		auto i = last;
		--i;
		if (first == i) return false;
		while (true) {
			auto ii = i;
			--i;
			if (*ii < *i) {
				auto j = last;
				while (!(*--j < *i));
				iter_swap(i, j);
				reverse(ii, last);
				return true;
			}
			if (i == first) {
				reverse(first, last);
				return false;
			}
		}
	}

	// �汾2����Compare�º�����С��
	template <class BidirectionalIter, class Compare>
	bool prev_premutation(BidirectionalIter first, BidirectionalIter last, Compare comp) {
		if (first == last) return false;
		auto i = last;
		--i;
		if (first == i) return false;
		while (true) {
			auto ii = i;
			--i;
			if (comp(*ii, *i)) {
				auto j = last;
				while (!comp(*--j < *i));
				iter_swap(i, j);
				reverse(ii, last);
				return true;
			}
			if (i == first) {
				reverse(first, last);
				return false;
			}
		}
	}
	/****************************************************************************/

	// random_shuffle
	// ��ȡ�������е����ĳ������
	/****************************************************************************/
	// �汾1��������rand������ȡ�����
	template <class RandomIter>
	void random_shuffle(RandomIter first, RandomIter last) {
		if (first == last) return;
		// Ŀ����Ϊ�˳�ʼ���������������Random Number Generator, RNG����
		// ����һ������������������еĺ������� time(0) ���ص�ǰʱ�������
		// ���ÿ�γ�������ʱ������õ�һ����ͬ��ʱ��ֵ
		srand((unsigned)time(0));
		for (auto i = first + 1; i != last; ++i) {
			esgstl::iter_swap(i, first + rand() % (i - first + 1));
		}
	}

	// �ø�����������ɶ������������
	template <class RandomIter, class RandomNumGenerator>
	void random_shuffle(RandomIter first, RandomIter last, RandomNumGenerator& generator) {
		if (first == last) return;
		auto len = last - first;
		for (auto i = first + 1; i != last; ++i) {
			esgstl::iter_swap(i, first + generator(i - first + 1) % len);
		}
	}
	/****************************************************************************/

	// partial_sort
	// �ֲ����򣬽�[first,last)���䰴������mid��ʹ(mid-first)��Ԫ������(����)������[first,mid)��
	/****************************************************************************/
	// Ĭ�ϰ�<���Ƚ�Ԫ�أ�����
	template <class RandomIter>
	void partial_sort(RandomIter first, RandomIter last, RandomIter mid) {
		esgstl::make_heap(first, mid);
		for (auto i = mid; i != last; ++i) {
			if (*i < *first)
				esgstl::pop_heap_aux(first, mid, i, *i, esgstl::distance_type(first));
		}
		esgstl::sort_heap(first, mid);
	}

	// Ĭ�ϰ�����Compare�º������Ƚ�Ԫ��
	template <class RandomIter, class Compare>
	void partial_sort(RandomIter first, RandomIter last, RandomIter mid, Compare comp) {
		esgstl::make_heap(first, mid, comp);
		for (auto i = mid; i != last; ++i) {
			if (comp(*i, *first))
				esgstl::pop_heap_aux(first, mid, i, *i, esgstl::distance_type(first), comp);
		}
		esgstl::sort_heap(first, mid, comp);
	}
	/****************************************************************************/

	// partial_sort_copy
	// �ֲ����򣬽�[first,last)���䰴������mid��ʹ(mid-first)��Ԫ������(����)��������������
	/****************************************************************************/
	// Ĭ�ϰ�<���Ƚ�Ԫ�أ�����
	template <class RandomIter, class Distance>
	RandomIter partial_sort_copy_aux(RandomIter first, RandomIter last,
		RandomIter result_first, RandomIter result_last, Distance*) {
		if (result_first == result_last) return result_last;
		auto result_mid = result_first;
		// ��ԭ�����Ԫ�ؿ�����������
		while (first != last && result_mid != result_last) {
			*result_mid = *first;
			++first;
			++result_mid;
		}
		// �����佨��
		make_heap(result_first, result_mid);
		// ��ʣ��δ����������ԭ����Ԫ�س��Լ������
		while (first != last) {
			if (*first < *result_first)
				adjust_heap(result_first, static_cast<Distance>(0), result_mid - result_first, *first);
			++first;
		}
		// �����
		sort_heap(result_first, result_mid);
		return result_mid;
	}

	template <class RandomIter>
	RandomIter partial_sort_copy(RandomIter first, RandomIter last,
		RandomIter result_first, RandomIter result_last) {
		return partial_sort_copy_aux(first, last, result_first, result_last, esgstl::distance_type(first));
	}

	// Ĭ�ϰ�����Compare�º������Ƚ�Ԫ��
	template <class RandomIter, class Distance, class Compare>
	RandomIter partial_sort_copy_aux(RandomIter first, RandomIter last,
		RandomIter result_first, RandomIter result_last, Distance*, Compare comp) {
		if (result_first == result_last) return result_last;
		auto result_mid = result_first;
		while (first != last && result_mid != result_last) {
			*result_mid = *first;
			++first;
			++result_mid;
		}
		make_heap(result_first, result_mid, comp);
		while (first != last) {
			if (comp(*first, *result_first))
				adjust_heap(result_first, static_cast<Distance>(0), result_mid - result_first, *first, comp);
			++first;
		}
		sort_heap(result_first, result_mid, comp);
		return result_mid;
	}

	template <class RandomIter, class Compare>
	RandomIter partial_sort_copy(RandomIter first, RandomIter last,
		RandomIter result_first, RandomIter result_last, Compare comp) {
		return partial_sort_copy_aux(first, last, result_first, result_last, esgstl::distance_type(first), comp);
	}
	/****************************************************************************/

	// ��������
	// �������������򣬸��Ӷ�O(N^2)
	/****************************************************************************/
	// �汾1����<��������
	// ��������unchecked_linear_insert����last������ǰ��ֵΪ������ָԪ�ظ����Ԫ��
	template <class RandomIter, class T>
	void unchecked_linear_insert(RandomIter last, T value) {
		auto pre = last;
		--pre;
		while (value < *pre) {
			*last = *pre;
			last = pre;
			--pre;
		}
		*last = value;
	}

	template <class RandomIter>
	void insert_sort(RandomIter first, RandomIter last) {
		if (first == last) return;
		for (auto i = first + 1; i != last; ++i) {
			auto value = *i;
			if (value < *first) {
				copy_backward(first, i, i + 1);
				*first = value;
			}
			else {
				unchecked_linear_insert(i, value);
			}
		}
	}

	// �汾2���ø���Compare�º�������
	template <class RandomIter, class T, class Compare>
	void unchecked_linear_insert(RandomIter last, T value, Compare comp) {
		auto pre = last;
		--pre;
		while (comp(value, *pre)) {
			*last = *pre;
			last = pre;
			--pre;
		}
		*last = value;
	}

	template <class RandomIter, class Compare>
	void insert_sort(RandomIter first, RandomIter last, Compare comp) {
		if (first == last) return;
		for (auto i = first + 1; i != last; ++i) {
			auto value = *i;
			if (comp(value, *first)) {
				copy_backward(first, i, i + 1);
				*first = value;
			}
			else {
				unchecked_linear_insert(i, value, comp);
			}
		}
	}
	/****************************************************************************/

	// sort
	// ��������+������+��������
	/****************************************************************************/
	// ��������median����ֵȡ��
	template <class T>
	const T& median(const T& a, const T& b, const T& c) {
		if (a < b) {
			if (b < c) return b; // a < b < c
			else if (a < c) return c; // a < c <= b
			else return a; // c <= a < b
		}
		else {
			if (c < b) return b; // c < b <= a
			else if (c < a) return c; // b <= c < a
			else return a; // b <= a <= c
		}
	}

	template <class T, class Compare>
	const T& median(const T& a, const T& b, const T& c, Compare comp) {
		if (comp(a, b)) {
			if (comp(b, c)) return b; // a < b < c
			else if (comp(a, c)) return c; // a < c <= b
			else return a; // c <= a < b
		}
		else {
			if (comp(c, b)) return b; // c < b <= a
			else if (comp(c, a)) return c; // b <= c < a
			else return a; // b <= a <= c
		}
	}

	// ��������unchecked_partition�������´��������������
	template <class RandomIter, class T>
	RandomIter unchecked_partition(RandomIter first, RandomIter last, T pivot) {
		while (true) {
			while (*first < pivot) ++first;
			while (pivot < *--last);
			if (!(first < last)) return first;
			iter_swap(first, last);
			++first;
		}
	}

	template <class RandomIter, class T, class Compare>
	RandomIter unchecked_partition(RandomIter first, RandomIter last, T pivot, Compare comp) {
		while (true) {
			while (comp(*first, pivot)) ++first;
			while (comp(pivot, *--last));
			if (!(first < last)) return first;
			iter_swap(first, last);
			++first; --last;
		}
	}

	// ���䳤����ֵ������С����������Ͳ��ٷָ������ˣ�ֱ��ʹ��
	const int esgstl_threshold = 16;

	// �������������ڼ������ݹ����
	template <class Size>
	Size _log2(Size n) {
		Size res = 0;
		while (n >>= 1) {
			++res;
		}
		return res;
	}

	// ��������introsort_loop���������򣬲��ÿ���˼��
	// ����������������Ϊ���С���䣨����ǰ���������������С�ں�������䣩
	// �����仮�ֹ����ж�����ʱ����ʱ���ö�������Ϊ���
	template <class RandomIter, class Size>
	void introsort_loop(RandomIter first, RandomIter last, Size loop_depth) {
		while (last - first > esgstl_threshold) {
			if (loop_depth == 0) {
				// �����޶��ݹ���Ⱥ�ʹ�ö�����
				// ����partial_sort(first,last,mid)��
				// mid�������Ϊlast����Ϊ�����Ķ�����
				partial_sort(first, last, last);
				return;
			}
			--loop_depth;
			auto right = unchecked_partition(first, last,
				median(*first, *(last - 1), *(first + (last - first) / 2)));
			introsort_loop(first, right, loop_depth);
			introsort_loop(right, last, loop_depth);
		}
	}

	template <class RandomIter, class Size, class Compare>
	void introsort_loop(RandomIter first, RandomIter last, Size loop_depth, Compare comp) {
		while (last - first > esgstl_threshold) {
			if (loop_depth == 0) {
				// �����޶��ݹ���Ⱥ�ʹ�ö�����
				// ����partial_sort(first,last,mid)��
				// mid�������Ϊlast����Ϊ�����Ķ�����
				partial_sort(first, last, last, comp);
				return;
			}
			--loop_depth;
			auto right = unchecked_partition(first, last,
				median(*first, *(last - 1), *(first + (last - first) / 2), comp), comp);
			introsort_loop(first, right, loop_depth, comp);
			introsort_loop(right, last, loop_depth, comp);
		}
	}

	// ��������unchecked_insert_sort�����������������Ԫ��������ǰ������������
	template <class RandomIter>
	void unchecked_insert_sort(RandomIter first, RandomIter last) {
		while (first != last) {
			unchecked_linear_insert(first, *first);
		}
	}

	template <class RandomIter, class Compare>
	void unchecked_insert_sort(RandomIter first, RandomIter last, Compare comp) {
		while (first != last) {
			unchecked_linear_insert(first, *first, comp);
		}
	}


	// ��������final_insert_sort������ʹ�ò������򽫴�������õĸ���С��������
	template <class RandomIter>
	void final_insert_sort(RandomIter first, RandomIter last) {
		// ���������䳤�ȴ���16�����Ȳ�������ǰ16��Ԫ�أ���ʣ�µ����������β��뵽����������
		if (last - first > esgstl_threshold) {
			insert_sort(first, first + esgstl_threshold);
			unchecked_insert_sort(first + esgstl_threshold, last);
		}
		else {
			// ����ֱ�Ӳ������򼴿�
			insert_sort(first, last);
		}
	}

	template <class RandomIter, class Compare>
	void final_insert_sort(RandomIter first, RandomIter last, Compare comp) {
		// ���������䳤�ȴ���16�����Ȳ�������ǰ16��Ԫ�أ���ʣ�µ����������β��뵽����������
		if (last - first > esgstl_threshold) {
			insert_sort(first, first + esgstl_threshold, comp);
			unchecked_insert_sort(first + esgstl_threshold, last, comp);
		}
		else {
			// ����ֱ�Ӳ������򼴿�
			insert_sort(first, last, comp);
		}
	}

	// ***********sort������***********
	template <class RandomIter>
	void sort(RandomIter first, RandomIter last) {
		if (first != last) {
			introsort_loop(first, last, _log2(last - first));
			final_insert_sort(first, last);
		}
	}

	template <class RandomIter, class Compare>
	void sort(RandomIter first, RandomIter last, Compare comp) {
		if (first != last) {
			introsort_loop(first, last, _log2(last - first), comp);
			final_insert_sort(first, last, comp);
		}
	}
	/****************************************************************************/

	// lower_bound
	// ��[first, last)�в��ҵ�һ����С�� value ��Ԫ�ص�λ�ã����ظ�λ��
	// ����ȷ�ı����� ���ز�Ӱ������ԭ��������ɵģ���һ�����Բ���value ��λ��
	/*****************************************************************/
	// �汾1����<�жϡ�С�ڡ�������������������Ѱ
	template <class ForwardIter, class T>
	ForwardIter lower_bound_cat(ForwardIter first, ForwardIter last, const T& value,
		esgstl::forward_iterator_tag) {
		auto len = esgstl::distance(first, last);
		auto half = len;
		while (len > 0) {
			half = half >> 1;
			auto mid = first;
			esgstl::advance(mid, half);
			// ����ʱ��������ң��������ֻʣhalf��Ԫ��
			// ��ֵС��valueʱ�������ң��ұ�ʣlen-half-1��Ԫ��
			if (*mid < value) {
				first = mid;
				++first;
				len = len - half - 1;
			}
			else {
				len = half;
			}
		}
		return first;
	}

	template <class RandomIter, class T>
	RandomIter lower_bound_cat(RandomIter first, RandomIter last, const T& value,
		esgstl::random_access_iterator_tag) {
		auto len = last - first;
		auto half = len;
		while (len > 0) {
			half = half >> 1;
			auto mid = first + half;
			// ����ʱ��������ң��������ֻʣhalf��Ԫ��
			// ��ֵС��valueʱ�������ң��ұ�ʣlen-half-1��Ԫ��
			if (*mid < value) {
				first = mid + 1;
				len = len - half - 1;
			}
			else {
				len = half;
			}
		}
		return first;
	}

	template <class ForwardIter, class T>
	ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& value) {
		return lower_bound_cat(first, last, value, esgstl::iterator_category(first));
	}

	// �汾2����Compare�º����жϡ�С�ڡ�������������������Ѱ
	template <class ForwardIter, class T, class Compare>
	ForwardIter lower_bound_cat(ForwardIter first, ForwardIter last, const T& value,
		esgstl::forward_iterator_tag, Compare comp) {
		auto len = esgstl::distance(first, last);
		while (len > 0) {
			auto half = len >> 1;
			auto mid = first;
			esgstl::advance(mid, half);
			if (comp(*mid, value)) {
				first = mid;
				++first;
				len = len - half - 1;
			}
			else {
				len = half;
			}
		}
		return first;
	}

	template <class RandomIter, class T, class Compare>
	RandomIter lower_bound_cat(RandomIter first, RandomIter last, const T& value,
		esgstl::random_access_iterator_tag, Compare comp) {
		auto left = first;
		auto len = last - first;
		while (len > 0) {
			auto half = len >> 1;
			auto mid = first + half;
			if (comp(*mid, value)) {
				first = mid + 1;
				len = len - half - 1;
			}
			else {
				len = half;
			}
		}
		return first;
	}

	template <class ForwardIter, class T, class Compare>
	ForwardIter lower_bound(ForwardIter first, ForwardIter last, const T& value, Compare comp) {
		return lower_bound_cat(first, last, value, esgstl::iterator_category(first), comp);
	}
	/*****************************************************************/

	// upper_bound
	// ��[first, last)�в��ҵ�һ�������ڡ� value ��Ԫ�ص�λ�ã����ظ�λ��
	// ����ȷ�ı����� ���ز�Ӱ������ԭ��������ɵģ����һ�����Բ���value ��λ��
	/*****************************************************************/
	// �汾1����value < ��ֵ������������������Ѱ
	template <class ForwardIter, class T>
	ForwardIter upper_bound_cat(ForwardIter first, ForwardIter last, const T& value,
		esgstl::forward_iterator_tag) {
		auto len = esgstl::distance(first, last);
		auto half = len;
		while (len > 0) {
			half = half >> 1;
			auto mid = first;
			esgstl::advance(mid, half);
			// С��������
			if (value < *mid) {
				len = half;
			}
			// ���ڵ���������
			else {
				first = mid;
				++first;
				len = len - half - 1;
			}
		}
		return first;
	}

	template <class RandomIter, class T>
	RandomIter upper_bound_cat(RandomIter first, RandomIter last, const T& value,
		esgstl::random_access_iterator_tag) {
		auto len = last - first;
		auto half = len;
		while (len > 0) {
			half = half >> 1;
			auto mid = first + half;
			// С��������
			if (value < *mid) {
				len = half;
			}
			// ���ڵ���������
			else {
				first = mid + 1;
				len = len - half - 1;
			}
		}
		return first;
	}

	template <class ForwardIter, class T>
	ForwardIter upper_bound(ForwardIter first, ForwardIter last, const T& value) {
		return upper_bound_cat(first, last, value, esgstl::iterator_category(first));
	}

	// �汾2����comp(value,mid)��ֵ����������������Ѱ
	template <class ForwardIter, class T, class Compare>
	ForwardIter upper_bound_cat(ForwardIter first, ForwardIter last, const T& value,
		esgstl::forward_iterator_tag, Compare comp) {
		auto len = esgstl::distance(first, last);
		while (len > 0) {
			auto half = len >> 1;
			auto mid = first;
			esgstl::advance(mid, half);
			if (comp(value, *mid)) {
				len = half;
			}
			else {
				first = mid;
				++first;
				len = len - half - 1;
			}
		}
		return first;
	}

	template <class RandomIter, class T, class Compare>
	RandomIter upper_bound_cat(RandomIter first, RandomIter last, const T& value,
		esgstl::random_access_iterator_tag, Compare comp) {
		auto left = first;
		auto len = last - first;
		while (len > 0) {
			auto half = len >> 1;
			auto mid = first + half;
			// ������
			if (comp(value, *mid)) {
				len = half;
			}
			// ������
			else {
				first = mid + 1;
				len = len - half - 1;
			}
		}
		return first;
	}

	template <class ForwardIter, class T, class Compare>
	ForwardIter upper_bound(ForwardIter first, ForwardIter last, const T& value, Compare comp) {
		return upper_bound_cat(first, last, value, esgstl::iterator_category(first), comp);
	}
	/*****************************************************************/

	// binary_search
	// ���ֲ��Ҹ���Ԫ���Ƿ����������[first,last)��
	/*****************************************************************/
	// �汾1����<�Ƚϴ�С
	template <class ForwardIter, class T>
	bool binary_search(ForwardIter first, ForwardIter last, const T& value) {
		auto iter = lower_bound(first, last, value);
		return iter != last && !(value < *iter);
	}

	// �汾2���÷º���Compare�Ƚϴ�С
	template <class ForwardIter, class T, class Compare>
	bool binary_search(ForwardIter first, ForwardIter last, const T& value, Compare comp) {
		auto iter = lower_bound(first, last, value, comp);
		return iter != last && !comp(value, *iter);
	}

	// equal_range
	// �ҵ�����������ĳ��ֵ���ֵ�����
	/****************************************************************************/
	// �汾1����<�Ƚϴ�С

	// ǰ��������汾
	template <class ForwardIter, class T>
	pair<ForwardIter, ForwardIter>
		equal_range_cat(ForwardIter first, ForwardIter last, const T& value, esgstl::forward_iterator_tag) {
		auto len = esgstl::distance(first, last);
		ForwardIter mid, left, right;
		while (len) {
			auto half = len >> 1;
			mid = first;
			esgstl::advance(mid, half);
			if (*mid < value) {
				len = len - half - 1;
				first = mid;
				++first;
			}
			else if (value < *mid) {
				len = half;
			}
			else {
				left = lower_bound(first, mid, value);
				right = upper_bound(++mid, last, value);
				return make_pair(left, right);
			}
		}
		return make_pair(first, first);
	}

	// ����������汾
	template <class RandomIter, class T>
	pair<RandomIter, RandomIter>
		equal_range_cat(RandomIter first, RandomIter last, const T& value, esgstl::random_access_iterator_tag) {
		auto len = esgstl::distance(first, last);
		RandomIter mid, left, right;
		while (len) {
			auto half = len >> 1;
			mid = first + half;
			if (*mid < value) {
				len = len - half - 1;
				first = mid + 1;
			}
			else if (value < *mid) {
				len = half;
			}
			else {
				left = lower_bound(first, mid, value);
				right = upper_bound(++mid, last, value);
				return make_pair(left, right);
			}
		}
		return make_pair(first, first);
	}

	template <class ForwardIter, class T>
	pair<ForwardIter, ForwardIter>
		equal_range(ForwardIter first, ForwardIter last, const T& value) {
		return equal_range_cat(first, last, value, esgstl::iterator_category(first));
	}

	// �汾2����Compare�º����Ƚϴ�С
	// ǰ��������汾
	template <class ForwardIter, class T, class Compare>
	pair<ForwardIter, ForwardIter>
		equal_range_cat(ForwardIter first, ForwardIter last, const T& value,
			esgstl::forward_iterator_tag, Compare comp) {
		auto len = esgstl::distance(first, last);
		ForwardIter mid, left, right;
		while (len) {
			auto half = len >> 1;
			mid = first;
			esgstl::advance(mid, half);
			if (comp(*mid, value)) {
				len = len - half - 1;
				first = mid;
				++first;
			}
			else if (comp(value, *mid)) {
				len = half;
			}
			else {
				left = lower_bound(first, mid, value, comp);
				right = upper_bound(++mid, last, value, comp);
				return make_pair(left, right);
			}
		}
		return make_pair(first, first);
	}

	// ����������汾
	template <class RandomIter, class T, class Compare>
	pair<RandomIter, RandomIter>
		equal_range_cat(RandomIter first, RandomIter last, const T& value,
			esgstl::random_access_iterator_tag, Compare comp) {
		auto len = esgstl::distance(first, last);
		RandomIter mid, left, right;
		while (len) {
			auto half = len >> 1;
			mid = first + half;
			if (comp(*mid, value)) {
				len = len - half - 1;
				first = mid + 1;
			}
			else if (comp(value, *mid)) {
				len = half;
			}
			else {
				left = lower_bound(first, mid, value, comp);
				right = upper_bound(++mid, last, value, comp);
				return make_pair(left, right);
			}
		}
		return make_pair(first, first);
	}

	template <class ForwardIter, class T, class Compare>
	pair<ForwardIter, ForwardIter>
		equal_range(ForwardIter first, ForwardIter last, const T& value, Compare comp) {
		return equal_range_cat(first, last, value, esgstl::iterator_category(first), comp);
	}
	/****************************************************************************/

	// merge_inplace
	// �ϲ�[first,mid) �� [mid,last)������������
	/****************************************************************************/

	// ��������rotate_adaptive�����ǻ���������ת����
	template <class BidirecionalIter, class Distance, class Pointer>
	BidirecionalIter rotate_adaptive(BidirecionalIter first, BidirecionalIter mid,
		BidirecionalIter last, Distance len1, Distance len2, Pointer buffer, Distance buff_size) {
		if (len2 < len1 && len2 <= buff_size) {
			auto buffer_end = copy(mid, last, buffer);
			copy_backward(first, mid, last);
			return copy(buffer, buffer_end, first);
		}
		else if (len1 <= buff_size) {
			auto buffer_end = copy(first, mid, buffer);
			copy(mid, last, first);
			return copy_backward(buffer, buffer_end, last);
		}
		else {
			rotate(first, mid, last);
			advance(first, len2);
			return first;
		}
	}

	// ��������merge_adaptive�����ǻ���������������
	// �汾1����<�Ƚϴ�С
	template <class BidirecionalIter, class Pointer, class Distance>
	void merge_adaptive(BidirecionalIter first, BidirecionalIter mid, BidirecionalIter last,
		Distance len1, Distance len2, Pointer buffer, Distance buff_size) {
		if (len1 <= len2 && len1 <= buff_size) {
			auto buffer_end = copy(first, mid, buffer);
			merge(mid, last, buffer, buffer_end, first);
		}
		else if (len2 <= buff_size) {
			auto buffer_end = copy(mid, last, buffer);
			merge_backward(first, mid, buffer, buffer_end, last);
		}
		else {
			Distance len11, len22;
			BidirecionalIter first_cut, second_cut;
			if (len2 < len1) {
				len22 = len2 >> 1;
				second_cut = mid;
				advance(second_cut, len22);
				first_cut = lower_bound(first, mid, *second_cut);
				len11 = distance(first, first_cut);
			}
			else {
				len11 = len1 >> 1;
				first_cut = first;
				advance(first_cut, len11);
				second_cut = upper_bound(mid, last, *first_cut);
				len22 = distance(mid, second_cut);
			}
			auto newmid = rotate_adaptive(first_cut, mid, second_cut, len1 - len11, len22, buffer, buff_size);
			merge_adaptive(first, first_cut, newmid, len11, len22, buffer, buff_size);
			merge_adaptive(newmid, second_cut, last, len1 - len11, len2 - len22, buffer, buff_size);
		}
	}

	// �汾2����Compare�Ƚϴ�С
	template <class BidirecionalIter, class Pointer, class Distance, class Compare>
	void merge_adaptive(BidirecionalIter first, BidirecionalIter mid, BidirecionalIter last,
		Distance len1, Distance len2, Pointer buffer, Distance buff_size, Compare comp) {
		if (len1 <= len2 && len1 <= buff_size) {
			auto buffer_end = copy(first, mid, buffer);
			merge(mid, last, buffer, buffer_end, first, comp);
		}
		else if (len2 <= buff_size) {
			auto buffer_end = copy(mid, last, buffer);
			merge_backward(first, mid, buffer, buffer_end, last, comp);
		}
		else {
			Distance len11, len22;
			BidirecionalIter first_cut, second_cut;
			if (len2 < len1) {
				len22 = len2 >> 1;
				second_cut = mid;
				advance(second_cut, len22);
				first_cut = lower_bound(first, mid, *second_cut, comp);
				len11 = distance(first, first_cut);
			}
			else {
				len11 = len1 >> 1;
				first_cut = mid;
				advance(first_cut, len11);
				second_cut = upper_bound(mid, last, *first_cut, comp);
				len22 = distance(mid, second_cut);
			}
			auto newmid = rotate_adaptive(first_cut, mid, second_cut, len1 - len11, len22, buffer, buff_size);
			merge_adaptive(first, first_cut, newmid, len11, len22, buffer, buff_size, comp);
			merge_adaptive(newmid, second_cut, last, len1 - len11, len2 - len22, buffer, buff_size, comp);
		}
	}


	// ��������merge_without_buffer�������ǻ���������������
	// �汾1����<�Ƚϴ�С
	template <class BidirecionalIter, class Distance>
	void merge_without_buffer(BidirecionalIter first, BidirecionalIter mid, BidirecionalIter last,
		Distance len1, Distance len2) {
		// �ݹ����1������һ�����䳤��Ϊ0ʱ�����ϲ�
		if (len1 == 0 || len2 == 0) return;
		// �ݹ����2����ֻʣ������Ԫ��ʱ�����������ȷ�ת����Ԫ�أ�����ֱ�ӽ���
		if (len1 + len2 == 2) {
			if (*mid < *first) iter_swap(first, mid);
			return;
		}
		Distance len11, len22;
		BidirecionalIter first_cut, second_cut;
		if (len1 < len2) {
			len22 = len2 >> 1;
			second_cut = mid;
			advance(second_cut, len22);
			first_cut = lower_bound(first, mid, *second_cut);
			len11 = distance(first, first_cut);
		}
		else {
			len11 = len1 >> 1;
			first_cut = first;
			advance(first_cut, len11);
			second_cut = upper_bound(mid, last, *first_cut);
			len22 = distance(mid, second_cut);
		}
		rotate(first_cut, mid, second_cut);
		auto newmid = first_cut;
		advance(newmid, len22);
		merge_without_buffer(first, first_cut, newmid, len11, len22);
		merge_without_buffer(newmid, second_cut, last, len1 - len11, len2 - len22);
	}

	// �汾2����Compare�Ƚϴ�С
	template <class BidirecionalIter, class Distance, class Compare>
	void merge_without_buffer(BidirecionalIter first, BidirecionalIter mid, BidirecionalIter last,
		Distance len1, Distance len2, Compare comp) {
		// �ݹ����1������һ�����䳤��Ϊ0ʱ�����ϲ�
		if (len1 == 0 || len2 == 0) return;
		// �ݹ����2����ֻʣ������Ԫ��ʱ�����������ȷ�ת����Ԫ�أ�����ֱ�ӽ���
		if (len1 + len2 == 2) {
			if (comp(*mid, *first) iter_swap(first, mid);
			return;
		}
		Distance len11, len22;
		BidirecionalIter first_cut, second_cut;
		if (len2 < len1) {
			len22 = len2 >> 1;
			second_cut = mid;
			advance(second_cut, len22);
			first_cut = lower_bound(first, mid, *second_cut);
			len11 = distance(first, first_cut);
		}
		else {
			len11 = len1 >> 1;
			first_cut = first;
			advance(first_cut, len11);
			second_cut = upper_bound(mid, last, *first_cut);
			len22 = distance(mid, second_cut);
		}
		rotate(first_cut, mid, second_cut);
		auto newmid = first_cut;
		advance(newmid, len22);
		merge_without_buffer(first, first_cut, newmid, len11, len22, comp);
		merge_without_buffer(newmid, second_cut, last, len1 - len11, len2 - len22, comp);
	}

	// merge_inplace��ʵ�߼�����
	// �汾1����<�Ƚϴ�С
	template <class BidirectionalIter, class Distance, class T>
	void merge_inplace_aux(BidirectionalIter first, BidirectionalIter mid,
		BidirectionalIter last, Distance*, T*) {
		if (first == mid || mid == last) return;
		temporary_buffer<BidirectionalIter, T> tmp_buff(first, last);
		Distance len1 = esgstl::distance(first, mid);
		Distance len2 = esgstl::distance(mid, last);
		if (tmp_buff.begin()) {
			// ���뵽�˻���
			merge_adaptive(first, mid, last, len1, len2, tmp_buff.begin(), tmp_buff.size());
		}
		else {
			// û�����뵽����
			merge_without_buffer(first, mid, last, len1, len2);
		}
	}

	// �汾2����Compare�Ƚϴ�С
	template <class BidirectionalIter, class Distance, class T, class Compare >
	void merge_inplace_aux(BidirectionalIter first, BidirectionalIter mid,
		BidirectionalIter last, Distance*, T*, Compare comp) {
		if (first == mid || mid == last) return;
		temporary_buffer<BidirectionalIter, T> tmp_buff(first, last);
		Distance len1 = esgstl::distance(first, mid);
		Distance len2 = esgstl::distance(mid, last);
		if (tmp_buff.begin()) {
			// ���뵽�˻���
			merge_adaptive(first, mid, last, len1, len2, tmp_buff.begin(), tmp_buff.size(), comp);
		}
		else {
			// û�����뵽����
			merge_without_buffer(first, mid, last, len1, len2, comp);
		}
	}

	// merge_inplace�û��ӿ�
	// �汾1����<�Ƚϴ�С
	template <class BidirectionalIter>
	void merge_inplace(BidirectionalIter first, BidirectionalIter mid, BidirectionalIter last) {
		merge_inplace_aux(first, mid, last, esgstl::distance_type(first), esgstl::value_type(first));
	}

	// �汾2����Compare�Ƚϴ�С
	template <class BidirectionalIter, class Compare>
	void merge_inplace(BidirectionalIter first, BidirectionalIter mid,
		BidirectionalIter last, Compare comp) {
		merge_inplace_aux(first, mid, last, esgstl::distance_type(first), esgstl::value_type(first), comp);
	}
	/****************************************************************************/

	// nth_element
	// ������[first,last)�������򣬻�ȡ��ȫ�����˳��Ϊn��Ԫ��
	/****************************************************************************/
	// �汾1����<�Ƚϴ�С
	template <class RandomIter>
	void nth_element(RandomIter first, RandomIter nth, RandomIter last) {
		while (last - first > 3) {
			auto cut = unchecked_partition(first, last,
				median(*first, *last, *(first + (last - first) / 2)));
			//display(first, last);
			if (nth < cut) last = cut;
			else first = cut;
		}
		insert_sort(first, last);
	}

	// �汾1����Compare�º����Ƚϴ�С
	template <class RandomIter, class Compare>
	void nth_element(RandomIter first, RandomIter nth, RandomIter last, Compare comp) {
		while (last - first > 3) {
			auto cut = unchecked_partition(first, last,
				median(*first, *last, *(first + (last - first) / 2), comp), comp);
			if (nth < cut) last = cut;
			else first = cut;
		}
		insert_sort(first, last, comp);
	}
	/****************************************************************************/

	// merge_sort
	// �鲢����
	/****************************************************************************/
	template <class RandomIter>
	void merge_sort(RandomIter first, RandomIter last) {
		if (first == last || first == last - 1) return;
		auto mid = first + (last - first) / 2;
		merge_sort(first, mid);
		merge_sort(mid, last);
		merge_inplace(first, mid, last);
	}
	/****************************************************************************/

}