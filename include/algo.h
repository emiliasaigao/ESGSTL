#pragma once

#include <cstddef>
#include <ctime>
#include "iterator.h"
#include "algobase.h"
#include "heap_algo.h"
#include "memory.h"

namespace esgstl {

	// adjacent_find
	// 找到区间中第一对相等的相邻元素的首元素,找不到返回区间尾迭代器
	/****************************************************************************/
	// 版本1，用==判等
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

	// 版本2，用仿函数pred判等
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
	// 查找区间中某个value出现的次数
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
	// 查找区间中满足条件的元素个数
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
	// 查找区间中第一次出现value的位置
	/****************************************************************************/
	// 版本1，用==判等
	template <class InputIter, class T>
	InputIter find(InputIter first, InputIter last, const T& value) {
		while (first != last) {
			if (*first == value) return first;
			++first;
		}
		return last;
	}

	// 版本2，用Compare仿函数判等
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
	// 查找区间中第一次满足一元谓词条件的位置
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
	// 找寻序列1中是否存在 连续 子序列2
	/****************************************************************************/
	// 版本1，用==判等
	template <class ForwardIter1, class ForwardIter2>
	ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2) {
		auto len1 = esgstl::distance(first1, last1);
		auto len2 = esgstl::distance(first2, last2);

		if (len1 < len2) return last1;
		// 两个指针分别指向序列1和序列2的头部
		auto cur1 = first1;
		auto cur2 = first2;
		// 指针cur2走到序列2尾时，匹配成功，返回first1
		while (cur2 != last2) {
			if (*cur1 == *cur2) {
				++cur1; ++cur2;
			}
			else {
				if (len1 == len2) return last1;
				--len1;
				// 从first1的下一个元素重新开始匹配，first1也要调整
				cur1 = ++first1;
				cur2 = first2;
			}
		}
		return first1;
	}

	// 版本2，用给定Compare仿函数判等
	template <class ForwardIter1, class ForwardIter2, class Compare>
	ForwardIter1 search(ForwardIter1 first1, ForwardIter1 last1,
		ForwardIter2 first2, ForwardIter2 last2, Compare comp) {
		auto len1 = esgstl::distance(first1, last1);
		auto len2 = esgstl::distance(first2, last2);

		if (len1 < len2) return last1;
		// 两个指针分别指向序列1和序列2的头部
		auto cur1 = first1;
		auto cur2 = first2;
		// 指针cur2走到序列2尾时，匹配成功，返回first1
		while (cur2 != last2) {
			if (comp(*cur1, *cur2)) {
				++cur1; ++cur2;
			}
			else {
				if (len1 == len2) return last1;
				--len1;
				// 从first1的下一个元素重新开始匹配，first1也要调整
				cur1 = ++first1;
				cur2 = first2;
			}
		}
		return first1;
	}
	/****************************************************************************/

	// find_end
	// 找到连续子序列[first2,last2）在序列[first1,last1)中出现的最后的位置
	/****************************************************************************/
	// 版本1，用==判等
	// 正向搜，保存最后一次搜到的结果
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

	// 逆向搜，搜到的第一个结果
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

	// 版本2，用给定Compare仿函数判等
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
	// 返回序列2中的任何元素在序列1中第一次出现的位置
	/****************************************************************************/
	// 版本1，使用==判等
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

	//版本2，用给定Compare仿函数判等
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
	// 对区间所有元素做相同的操作，不保证能够接受赋值行为
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
	// 将仿函数Generate的结果填写在区间[first,last)上
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
	// 将仿函数Generate的结果填写在以first起始长度为n的区间上
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
	// 判断有序序列2是否是有序序列1的子集
	/****************************************************************************/
	// 版本1，用小于比较元素
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

	// 版本2，用Compare仿函数比较元素
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
	// 返回区间最大值的迭代器
	/****************************************************************************/
	// 版本1，用<比较
	template <class ForwardIter>
	ForwardIter max_element(ForwardIter first, ForwardIter last) {
		if (first == last) return first;
		auto res = first;
		while (++first != last) {
			if (*res < *first) *res = *first;
		}
		return res;
	}

	// 版本2，用Compare仿函数比较
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
	// 返回区间最大值的迭代器
	/****************************************************************************/
	// 版本1，用<比较
	template <class ForwardIter>
	ForwardIter min_element(ForwardIter first, ForwardIter last) {
		if (first == last) return first;
		auto res = first;
		while (++first != last) {
			if (*first < *res) *res = *first;
		}
		return res;
	}

	// 版本2，用Compare仿函数比较
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
	// 倒着合并有序区间
	/****************************************************************************/
	// 版本1，以<合并
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

	// 版本2，以Compare仿函数合并
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
	// 正着合并有序区间
	/****************************************************************************/
	// 版本1，以<合并
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

	// 版本2，以Compare仿函数合并
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
	// 将区间按条件（一元谓词）一分为二，返回中界的迭代器
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
	// 将区间中不等于指定值的元素按相对顺序拷贝到给定输出迭代器，返回结果区间的尾迭代器
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
	// 将区间中不满足删除条件的元素按相对顺序拷贝到给定输出迭代器，返回结果区间的尾迭代器
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
	// 将区间中等于指定值的元素移动到区间末尾，返回“被删除区间”的头迭代器
	/****************************************************************************/
	template <class ForwardIter, class T>
	ForwardIter remove(ForwardIter first, ForwardIter last, const T& value) {
		first = find(first, last, value);
		return first == last ? first : remove_copy(first, last, first, value);
	}
	/****************************************************************************/

	// remove_if
	// 将区间中满足删除条件的元素移动到区间末尾，返回“被删除区间”的头迭代器
	/****************************************************************************/
	template <class ForwardIter, class UnaryPred>
	ForwardIter remove_if(ForwardIter first, ForwardIter last, UnaryPred pred) {
		first = find_if(first, last, pred);
		return first == last ? first : remove_copy_if(first, last, first, pred);
	}
	/****************************************************************************/

	// replace
	// 将区间中的等于给定值的旧值替换为新值
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
	// 拷贝原本区间，将拷贝区间中的等于给定值的旧值替换为新值，返回拷贝区间的尾迭代器
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
	// 将区间中满足条件的旧值替换为新值
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
	// 拷贝原本区间，将拷贝区间中满足条件的旧值替换为新值，返回拷贝区间的尾迭代器
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
	// 反转区间
	/****************************************************************************/
	// 双向迭代器版本
	template <class BidirectionalIter>
	void reverse_cat(BidirectionalIter first, BidirectionalIter last, esgstl::bidirectional_iterator_tag) {
		while (true) {
			// 有两种情况退出循环，first与last相遇时，亦或是first与last相邻时
			if (first == last || first == --last) return;
			// 假如上述两种情况都不满足，last已经前移，下面swap后只需要移动first即可
			iter_swap(first, last);
			++first;
		}
	}

	// 随机迭代器版本
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
	// 翻转区间，并复制到给定出口
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
	// 基于三个迭代器，旋转两个区间
	/****************************************************************************/
	// 前向迭代器
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

	// 双向迭代器
	template <class BidirectionalIter>
	void rotate_cat(BidirectionalIter first, BidirectionalIter mid, BidirectionalIter last, esgstl::bidirectional_iterator_tag) {
		reverse(first, mid);
		reverse(mid, last);
		reverse(first, last);
	}

	// 顺时针旋转环（随机迭代器版本rotate辅助函数）
	// initial是指开始旋转的位置
	//					1
	//				8	    2
	//			7				3
	//				6		4
	//					5
	// 若mid位于3，则pre = 2，将会有两个环
	//		1		3	  |		2		4
	//		7		5	  |		8		6
	// 若mid位于5，则pre = 4，将会有四个环
	// 1 5	 |	2 6	 |	3 7	 |	4  8
	// rotate_cycle每次运行只逆时针旋转一个环
	template <class RandomIter, class Distance>
	void rotate_cycle(RandomIter first, RandomIter last, RandomIter initial, Distance pre) {
		// 记录起始值
		auto ini_val = *initial;
		// 慢指针
		auto p1 = initial;
		// 快指针
		auto p2 = initial + pre;
		while (p2 != initial) {
			// 慢指针元素值赋值为快指针元素值（实现逆时针旋转环）
			*p1 = *p2;
			// 慢指针往前走
			p1 = p2;
			// 快指针往前走，成环是假想的，并不是真实的，因此超出范围的部分要从头开始走
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
		// 求all和pre的最大公因数m
		auto m = all, n = pre;
		while (n != 0) {
			auto tmp = m % n;
			m = n;
			n = tmp;
		}
		// m值即为需要旋转的环的数量
		// 环的周长即为前半段的长度pre
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
	// 基于三个迭代器，旋转两个区间，将结果存放到给定出口
	/****************************************************************************/
	template <class InputIter, class OutputIter>
	OutputIter rotate_copy(InputIter first, InputIter mid, InputIter last, OutputIter result) {
		return copy(first, mid, copy(mid, last, result));
	}
	/****************************************************************************/

	// search_n
	// 寻找指定长度的与给定值相等的区间，返回区间头迭代器
	/****************************************************************************/
	// 版本1，以==判等
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

	// 版本2，以Compare仿函数判等
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
	// 版本1
	// 将区间[first,last)的元素均做UnaryPred的操作，将结果输出到指定出口
	template <class InputIter, class OutputIter, class UnaryPred>
	OutputIter transform(InputIter first, InputIter last, OutputIter result, UnaryPred pred) {
		while (first != last) {
			*result = pred(*first);
			++result; ++first;
		}
		return result;
	}

	// 版本2
	// 将区间[first1,last1) 和区间[first2,?)的元素一一对应做BinaryPred的操作，将结果输出到指定出口
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
	// 将有序区间去重，并将结果输出至指定出口
	/****************************************************************************/
	// 版本1，用==判等

	// 输出迭代器为输出迭代器时
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

	// 输出迭代器为单向迭代器时
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

	// 版本2，用Compare仿函数判等

	// 输出迭代器为单向迭代器时
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

	// 输出迭代器为输出迭代器时
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
	// 将有序区间去重，返回去重后的区间尾迭代器
	/****************************************************************************/
	// 版本1，用==判等
	template <class ForwardIter>
	ForwardIter unique(ForwardIter first, ForwardIter last) {
		return unique_copy(first, last, first);
	}

	// 版本2，用Compare仿函数判等
	template <class ForwardIter, class Compare>
	ForwardIter unique(ForwardIter first, ForwardIter last, Compare comp) {
		return unique_copy(first, last, first, comp);
	}
	/****************************************************************************/

	// next_premutation
	// 将给定排列排序为它的下一排列，若当前序列没有下一个排列，返回fasle
	/****************************************************************************/
	// 版本1，用<判小于
	template <class BidirectionalIter>
	bool next_premutation(BidirectionalIter first, BidirectionalIter last) {
		if (first == last) return false;
		auto i = last;
		--i;
		if (first == i) return false;
		while (true) {
			// 首先从后往前找到一对满足*i < *ii的元素，固定ii
			auto ii = i;
			--i;
			if (*i < *ii) {
				auto j = last;
				// 然后从后往前找到第一个满足*i < *j的元素，交换i，j所指元素
				while (!(*i < *--j));
				iter_swap(i, j);
				// 最后翻转ii及其后面的区间
				reverse(ii, last);
				return true;
			}
			// 若i==first还没return出去，说明已经没有下一个排列了，整个区间是递减的了
			// 翻转整个区间，并返回false
			if (i == first) {
				reverse(first, last);
				return false;
			}
		}
	}

	// 版本2，用仿函数Compare判小于
	template <class BidirectionalIter, class Compare>
	bool next_premutation(BidirectionalIter first, BidirectionalIter last, Compare comp) {
		if (first == last) return false;
		auto i = last;
		--i;
		if (first == i) return false;
		while (true) {
			// 首先从后往前找到一对满足*i < *ii的元素，固定ii
			auto ii = i;
			--i;
			if (comp(*i, *ii)) {
				auto j = last;
				// 然后从后往前找到第一个满足*i < *j的元素，交换i，j所指元素
				while (!comp(*i, *--j));
				iter_swap(i, j);
				// 最后翻转ii及其后面的区间
				reverse(ii, last);
				return true;
			}
			// 若i==first还没return出去，说明已经没有下一个排列了，整个区间是递减的了
			// 翻转整个区间，并返回false
			if (i == first) {
				reverse(first, last);
				return false;
			}
		}
	}
	/****************************************************************************/

	// prev_premutation
	// 将给定排列排序为它的下一排列，若该序列没有上一序列，返回false
	/****************************************************************************/
	// 版本1，用<判小于
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

	// 版本2，用Compare仿函数判小于
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
	// 获取给定序列的随机某种排序
	/****************************************************************************/
	// 版本1，用内置rand函数获取随机数
	template <class RandomIter>
	void random_shuffle(RandomIter first, RandomIter last) {
		if (first == last) return;
		// 目的是为了初始化随机数生成器（Random Number Generator, RNG）。
		// 这是一个用于生成随机数序列的函数，而 time(0) 返回当前时间的秒数
		// 因此每次程序运行时，都会得到一个不同的时间值
		srand((unsigned)time(0));
		for (auto i = first + 1; i != last; ++i) {
			esgstl::iter_swap(i, first + rand() % (i - first + 1));
		}
	}

	// 用给定随机数生成对象生成随机数
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
	// 局部排序，将[first,last)区间按给定的mid，使(mid-first)个元素有序(升序)排列在[first,mid)上
	/****************************************************************************/
	// 默认按<来比较元素，升序
	template <class RandomIter>
	void partial_sort(RandomIter first, RandomIter last, RandomIter mid) {
		esgstl::make_heap(first, mid);
		for (auto i = mid; i != last; ++i) {
			if (*i < *first)
				esgstl::pop_heap_aux(first, mid, i, *i, esgstl::distance_type(first));
		}
		esgstl::sort_heap(first, mid);
	}

	// 默认按给定Compare仿函数来比较元素
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
	// 局部排序，将[first,last)区间按给定的mid，使(mid-first)个元素有序(升序)排列在新区间上
	/****************************************************************************/
	// 默认按<来比较元素，升序
	template <class RandomIter, class Distance>
	RandomIter partial_sort_copy_aux(RandomIter first, RandomIter last,
		RandomIter result_first, RandomIter result_last, Distance*) {
		if (result_first == result_last) return result_last;
		auto result_mid = result_first;
		// 将原区间的元素拷贝到新区间
		while (first != last && result_mid != result_last) {
			*result_mid = *first;
			++first;
			++result_mid;
		}
		// 新区间建堆
		make_heap(result_first, result_mid);
		// 将剩余未拷贝过来的原区间元素尝试加入堆中
		while (first != last) {
			if (*first < *result_first)
				adjust_heap(result_first, static_cast<Distance>(0), result_mid - result_first, *first);
			++first;
		}
		// 排序堆
		sort_heap(result_first, result_mid);
		return result_mid;
	}

	template <class RandomIter>
	RandomIter partial_sort_copy(RandomIter first, RandomIter last,
		RandomIter result_first, RandomIter result_last) {
		return partial_sort_copy_aux(first, last, result_first, result_last, esgstl::distance_type(first));
	}

	// 默认按给定Compare仿函数来比较元素
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

	// 插入排序
	// 将给定区间排序，复杂度O(N^2)
	/****************************************************************************/
	// 版本1，用<排序，升序
	// 辅助函数unchecked_linear_insert，让last不断往前赋值为比其所指元素更大的元素
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

	// 版本2，用给定Compare仿函数排序
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
	// 快速排序+堆排序+插入排序
	/****************************************************************************/
	// 辅助函数median，三值取中
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

	// 辅助函数unchecked_partition，划分下次排序的两个区间
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

	// 区间长度阈值常量，小于这个数，就不再分割区间了，直接使用
	const int esgstl_threshold = 16;

	// 辅助函数，用于计算最大递归深度
	template <class Size>
	Size _log2(Size n) {
		Size res = 0;
		while (n >>= 1) {
			++res;
		}
		return res;
	}

	// 辅助函数introsort_loop，大致排序，采用快排思想
	// 将整个大区间排序为多个小区间（排在前面的区间其中内容小于后面的区间）
	// 当区间划分过多有恶化倾向时，及时采用堆排序作为替代
	template <class RandomIter, class Size>
	void introsort_loop(RandomIter first, RandomIter last, Size loop_depth) {
		while (last - first > esgstl_threshold) {
			if (loop_depth == 0) {
				// 超过限定递归深度后，使用堆排序
				// 函数partial_sort(first,last,mid)中
				// mid传入参数为last，则为完整的堆排序
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
				// 超过限定递归深度后，使用堆排序
				// 函数partial_sort(first,last,mid)中
				// mid传入参数为last，则为完整的堆排序
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

	// 辅助函数unchecked_insert_sort，将给定区间的所有元素依次向前插入有序区间
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


	// 辅助函数final_insert_sort，最终使用插入排序将大致排序好的各个小区间整合
	template <class RandomIter>
	void final_insert_sort(RandomIter first, RandomIter last) {
		// 若整体区间长度大于16，则先插入排序前16个元素，将剩下的区间再依次插入到有序区间中
		if (last - first > esgstl_threshold) {
			insert_sort(first, first + esgstl_threshold);
			unchecked_insert_sort(first + esgstl_threshold, last);
		}
		else {
			// 否则直接插入排序即可
			insert_sort(first, last);
		}
	}

	template <class RandomIter, class Compare>
	void final_insert_sort(RandomIter first, RandomIter last, Compare comp) {
		// 若整体区间长度大于16，则先插入排序前16个元素，将剩下的区间再依次插入到有序区间中
		if (last - first > esgstl_threshold) {
			insert_sort(first, first + esgstl_threshold, comp);
			unchecked_insert_sort(first + esgstl_threshold, last, comp);
		}
		else {
			// 否则直接插入排序即可
			insert_sort(first, last, comp);
		}
	}

	// ***********sort主函数***********
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
	// 在[first, last)中查找第一个不小于 value 的元素的位置，返回该位置
	// 更精确的表述： 返回不影响序列原本排序规律的，第一个可以插入value 的位置
	/*****************************************************************/
	// 版本1，用<判断“小于”，决定向左还是向右搜寻
	template <class ForwardIter, class T>
	ForwardIter lower_bound_cat(ForwardIter first, ForwardIter last, const T& value,
		esgstl::forward_iterator_tag) {
		auto len = esgstl::distance(first, last);
		auto half = len;
		while (len > 0) {
			half = half >> 1;
			auto mid = first;
			esgstl::advance(mid, half);
			// 等于时往左继续找，左边正好只剩half个元素
			// 中值小于value时，向右找，右边剩len-half-1个元素
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
			// 等于时往左继续找，左边正好只剩half个元素
			// 中值小于value时，向右找，右边剩len-half-1个元素
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

	// 版本2，用Compare仿函数判断“小于”，决定向左还是向右搜寻
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
	// 在[first, last)中查找第一个“大于” value 的元素的位置，返回该位置
	// 更精确的表述： 返回不影响序列原本排序规律的，最后一个可以插入value 的位置
	/*****************************************************************/
	// 版本1，用value < 中值，决定向左还是向右搜寻
	template <class ForwardIter, class T>
	ForwardIter upper_bound_cat(ForwardIter first, ForwardIter last, const T& value,
		esgstl::forward_iterator_tag) {
		auto len = esgstl::distance(first, last);
		auto half = len;
		while (len > 0) {
			half = half >> 1;
			auto mid = first;
			esgstl::advance(mid, half);
			// 小于向左找
			if (value < *mid) {
				len = half;
			}
			// 大于等于向右找
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
			// 小于向左找
			if (value < *mid) {
				len = half;
			}
			// 大于等于向右找
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

	// 版本2，用comp(value,mid)的值决定向左还是向右搜寻
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
			// 向左找
			if (comp(value, *mid)) {
				len = half;
			}
			// 向右找
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
	// 二分查找给定元素是否出现在序列[first,last)中
	/*****************************************************************/
	// 版本1，用<比较大小
	template <class ForwardIter, class T>
	bool binary_search(ForwardIter first, ForwardIter last, const T& value) {
		auto iter = lower_bound(first, last, value);
		return iter != last && !(value < *iter);
	}

	// 版本2，用仿函数Compare比较大小
	template <class ForwardIter, class T, class Compare>
	bool binary_search(ForwardIter first, ForwardIter last, const T& value, Compare comp) {
		auto iter = lower_bound(first, last, value, comp);
		return iter != last && !comp(value, *iter);
	}

	// equal_range
	// 找到有序序列中某个值出现的区间
	/****************************************************************************/
	// 版本1，用<比较大小

	// 前向迭代器版本
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

	// 随机迭代器版本
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

	// 版本2，用Compare仿函数比较大小
	// 前向迭代器版本
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

	// 随机迭代器版本
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
	// 合并[first,mid) 与 [mid,last)两段有序区间
	/****************************************************************************/

	// 辅助函数rotate_adaptive，考虑缓冲区的旋转区间
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

	// 辅助函数merge_adaptive，考虑缓冲区的区间整合
	// 版本1，用<比较大小
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

	// 版本2，用Compare比较大小
	template <class BidirecionalIter, class Pointer, class Distance, class Compare>
	void merge_adaptive(BidirecionalIter first, BidirecionalIter mid, BidirecionalIter last,
		Distance len1, Distance len2, Pointer buffer, Distance buff_size, Compare comp) {
		if (len1 <= len2 && len1 <= buff_size) {
			auto buffer_end = copy(first, mid, buffer);
			merge(first, mid, buffer, buffer_end, first, comp);
		}
		else if (len2 <= buff_size) {
			auto buffer_end = copy(mid, last, buffer);
			merge_backward(mid, last, buffer, buffer_end, last, comp);
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


	// 辅助函数merge_without_buffer，不考虑缓冲区的区间整合
	// 版本1，用<比较大小
	template <class BidirecionalIter, class Distance>
	void merge_without_buffer(BidirecionalIter first, BidirecionalIter mid, BidirecionalIter last,
		Distance len1, Distance len2) {
		// 递归出口1：当有一侧区间长度为0时结束合并
		if (len1 == 0 || len2 == 0) return;
		// 递归出口2：当只剩下两个元素时，若逆序，则先翻转两个元素，否则直接结束
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

	// 版本2，用Compare比较大小
	template <class BidirecionalIter, class Distance, class Compare>
	void merge_without_buffer(BidirecionalIter first, BidirecionalIter mid, BidirecionalIter last,
		Distance len1, Distance len2, Compare comp) {
		// 递归出口1：当有一侧区间长度为0时结束合并
		if (len1 == 0 || len2 == 0) return;
		// 递归出口2：当只剩下两个元素时，若逆序，则先翻转两个元素，否则直接结束
		if (len1 + len2 == 2) {
			if (*mid < *first) iter_swap(first, mid);
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

	// merge_inplace真实逻辑函数
	// 版本1，用<比较大小
	template <class BidirectionalIter, class Distance, class T>
	void merge_inplace_aux(BidirectionalIter first, BidirectionalIter mid,
		BidirectionalIter last, Distance*, T*) {
		if (first == mid || mid == last) return;
		temporary_buffer<BidirectionalIter, T> tmp_buff(first, last);
		Distance len1 = esgstl::distance(first, mid);
		Distance len2 = esgstl::distance(mid, last);
		if (tmp_buff.begin()) {
			// 申请到了缓存
			merge_adaptive(first, mid, last, len1, len2, tmp_buff.begin(), tmp_buff.size());
		}
		else {
			// 没有申请到缓存
			merge_without_buffer(first, mid, last, len1, len2);
		}
	}

	// 版本2，用Compare比较大小
	template <class BidirectionalIter, class Distance, class T, class Compare >
	void merge_inplace_aux(BidirectionalIter first, BidirectionalIter mid,
		BidirectionalIter last, Distance*, T*, Compare comp) {
		if (first == mid || mid == last) return;
		temporary_buffer<BidirectionalIter, T> tmp_buff(first, last);
		Distance len1 = esgstl::distance(first, mid);
		Distance len2 = esgstl::distance(mid, last);
		if (tmp_buff.begin()) {
			// 申请到了缓存
			merge_adaptive(first, mid, last, len1, len2, tmp_buff.begin(), tmp_buff.size(), comp);
		}
		else {
			// 没有申请到缓存
			merge_without_buffer(first, mid, last, len1, len2, comp);
		}
	}

	// merge_inplace用户接口
	// 版本1，用<比较大小
	template <class BidirectionalIter>
	void merge_inplace(BidirectionalIter first, BidirectionalIter mid, BidirectionalIter last) {
		merge_inplace_aux(first, mid, last, esgstl::distance_type(first), esgstl::value_type(first));
	}

	// 版本2，用Compare比较大小
	template <class BidirectionalIter, class Compare>
	void merge_inplace(BidirectionalIter first, BidirectionalIter mid,
		BidirectionalIter last, Compare comp) {
		merge_inplace_aux(first, mid, last, esgstl::distance_type(first), esgstl::value_type(first), comp);
	}
	/****************************************************************************/

	// nth_element
	// 将区间[first,last)部分排序，获取完全排序后顺次为n的元素
	/****************************************************************************/
	// 版本1，用<比较大小
	template <class RandomIter>
	void nth_element(RandomIter first, RandomIter nth, RandomIter last) {
		while (last - first > 3) {
			auto cut = unchecked_partition(first, last,
				median(*first, *last, *(first + (last - first) / 2)));
			display(first, last);
			if (nth < cut) last = cut;
			else first = cut;
		}
		insert_sort(first, last);
	}

	// 版本1，用Compare仿函数比较大小
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
	// 归并排序
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