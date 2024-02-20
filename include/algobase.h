#pragma once
#include <cstring>

#include "iterator.h"
#include "util.h"

// 这个头文件包含了一些基本算法


namespace esgstl {

	// 不知道干嘛的宏
	#ifdef max
	#pragma message("#undefing marco max")
	#undef max
	#endif // max

	#ifdef min
	#pragma message("#undefing marco min")
	#undef min
	#endif // min


	// max 和 min
	/*****************************************************************/
	
	// max:两者较大值，默认版本
	template <class T>
	const T& max(const T& lhs, const T& rhs) {
		return lhs < rhs ? rhs : lhs;
	}

	// max:两者较大值，指定比较仿函数版本
	template <class T, class Compare>
	const T& max(const T& lhs, const T& rhs, Compare comp) {
		return comp(lhs, rhs) ? rhs : lhs;
	}

	// min:两者较小值，默认版本
	template <class T>
	const T& min(const T& lhs, const T& rhs) {
		return lhs < rhs ? lhs : rhs;
	}

	// max两者较小值，指定比较仿函数版本
	template <class T, class Compare>
	const T& min(const T& lhs, const T& rhs, Compare comp) {
		return comp(lhs, rhs) ? lhs : rhs;
	}
	/*****************************************************************/


	// iter_swap
	/*****************************************************************/
	
	// iter_swap:对调两个迭代器所指对象
	template <class ForwardIter1,class ForwardIter2>
	void iter_swap(ForwardIter1 lhs, ForwardIter2 rhs) {
		esgstl::swap(*lhs, *rhs);
	}
	/*****************************************************************/

	// copy 正向拷贝
	/*****************************************************************/

	// unchecked_copy_cat:实际复制逻辑函数 输入迭代器版本
	template <class InputIter,class OutputIter>
	OutputIter
		unchecked_copy_cat(InputIter first, InputIter last, OutputIter result, esgstl::input_iterator_tag) {
		while (first != last) {
			*result = *first;
			++result;
			++first;
		}
		return result;
	}

	// unchecked_copy_cat:实际复制逻辑函数 随机迭代器版本————省去了比较迭代器是否相等的步骤
	template <class RandomIter,class OutputIter>
	OutputIter
		unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result, esgstl::random_access_iterator_tag) {
		for (auto n = last - first; n > 0; --n, ++first, ++result) {
			*result = *first;
		}
		return result;
	}

	// unchecked_copy:实际复制逻辑函数 控制器版本
	// 根据输入迭代器的不同类型调用不同的拷贝逻辑
	template <class InputIter, class OutputIter>
	OutputIter
		unchecked_copy(InputIter first, InputIter last, OutputIter result) {
		return unchecked_copy_cat(first, last, result, esgstl::iterator_category(first));
	}

	// unchecked_copy：为 trivially_copy_assignable 类型提供特化版本
	// 当Up和Tp去掉const后的类型相同，且这个类型没有有效的拷贝控制行为时，这个模板会被匹配到
	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
		std::is_trivially_copy_assignable<Up>::value,
		Up*>::type
		unchecked_copy(Tp* first, Tp* last, Up* result) {
		const auto n = static_cast<size_t>(last - first);
		if (n != 0)
			std::memmove(result, first, n * sizeof(Up));
		return result + n;
	}

	// copy: 留给用户的拷贝区间的接口
	template <class InputIter,class OutputIter>
	OutputIter copy(InputIter first, InputIter last, OutputIter result) {
		return esgstl::unchecked_copy(first, last, result);
	}
	/*****************************************************************/

	// copy_backward
	/*****************************************************************/

	// unchecked_copy_backward_cat: 实际复制逻辑函数 双向迭代器版本
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result, esgstl::bidirectional_iterator_tag) {
		while (last != first) {
			*--result = *--last;
			
		}
		return result;
	}

	// unchecked_copy_backward_cat: 实际复制逻辑函数 随机迭代器版本
	template <class RandomIter,class BidirectionalIter>
	BidirectionalIter
		unchecked_copy_backward_cat(RandomIter first, RandomIter last, 
			BidirectionalIter result, esgstl::random_access_iterator_tag) {
		for (auto n = last - first; n > 0; --n) {
			*--result = *--last;
		}
		return result;
	}

	// unchecked_copy_backward: 实际复制逻辑函数 控制器版本
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result) {
		return unchecked_copy_backward_cat(first, last, result, esgstl::iterator_category(first));
	}

	// unchecked_copy_backward:为trivially_copy_assignable 类型提供特化版本
	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
		std::is_trivially_copy_assignable<Up>::value,
		Up*>::type
		unchecked_copy_backward(Tp* first, Tp* last, Up* result)
	{
		const auto n = static_cast<size_t>(last - first);
		if (n != 0)
		{
			result -= n;
			std::memmove(result, first, n * sizeof(Up));
		}
		return result;
	}

	// copy_backward:开放给用户的逆向拷贝函数
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result) {
		return esgstl::unchecked_copy_backward(first, last, result);
	}

	/*****************************************************************/

	// copy_if
	// 复制满足条件的区间至指定位置
	/*****************************************************************/

	template <class InputIter, class OutputIter, class UnaryPredicate>
	OutputIter unchecked_copy_if_cat(InputIter first, InputIter last, OutputIter result,
		UnaryPredicate unary_pred, esgstl::input_iterator_tag) {
		for (;first != last; ++first) {
			if (unary_pred(*first))
				*result++ = *first;
		}
		return result;
	}

	template <class InputIter, class OutputIter, class UnaryPredicate>
	OutputIter unchecked_copy_if_cat(InputIter first, InputIter last, OutputIter result,
		UnaryPredicate unary_pred, esgstl::random_access_iterator_tag) {
		for (auto n = last - first; n > 0; --n) {
			if (unary_pred(*first))
				*result++ = *first++;
		}
		return result;
	}

	template <class InputIter, class OutputIter, class UnaryPredicate>
	OutputIter unchecked_copy_if(InputIter first, InputIter last, OutputIter result, UnaryPredicate unary_pred) {
		return unchecked_copy_if_cat(first, last, result, unary_pred, esgstl::iterator_category(first));
	}

	template <class InputIter, class OutputIter, class UnaryPredicate>
	OutputIter copy_if(InputIter first, InputIter last, OutputIter result,
		UnaryPredicate unary_pred) {
		return esgstl::unchecked_copy_if(first,last,result,unary_pred);
	}
	/*****************************************************************/

	// copy_n
	// 复制指定数量的元素到指定位置，返回pair分别指向两个区间的末尾
	/*****************************************************************/
	template <class InputIter, class Size, class OutputIter>
	esgstl::pair<InputIter, OutputIter>
		unchecked_copy_n_cat(InputIter first, Size n, OutputIter result, esgstl::input_iterator_tag) {
		while (n > 0) {
			*result++ = *first++;
			--n;
		}
		return esgstl::pair<InputIter, OutputIter>(first, result);
	}

	template <class RandomIter, class Size, class OutputIter>
	esgstl::pair<RandomIter, OutputIter>
		unchecked_copy_n_cat(RandomIter first, Size n, OutputIter result, esgstl::random_access_iterator_tag) {
		auto last = first + n;
		return esgstl::pair<RandomIter, OutputIter>(last, esgstl::copy(first, last, result));
	}

	template <class InputIter, class Size, class OutputIter>
	esgstl::pair<InputIter, OutputIter>
		unchecked_copy_n(InputIter first, Size n, OutputIter result) {
		return unchecked_copy_n_cat(first, n, result, esgstl::iterator_category(first));
	}

	template <class InputIter, class Size, class OutputIter>
	esgstl::pair<InputIter,OutputIter>
		copy_n(InputIter first, Size n, OutputIter result) {
		return esgstl::unchecked_copy_n(first, n, result);
	}
	/*****************************************************************/

	// move
	// 移动指定区间的元素到指定位置，返回移动终点位置
	/*****************************************************************/

	template <class InputIter, class OutputIter>
	OutputIter
		unchecked_move(InputIter first, InputIter last, OutputIter result) {
		return unchecked_move_cat(first, last, result, esgstl::iterator_category(first));
	}

	template <class InputIter, class OutputIter>
	OutputIter
		unchecked_move_cat(InputIter first, InputIter last, OutputIter result,esgstl::input_iterator_tag) {
		while (first != last) {
			*result++ = esgstl::move(*first++);
		}
		return result;
	}

	template <class RandomIter, class OutputIter>
	OutputIter
		unchecked_move_cat(RandomIter first, RandomIter last, OutputIter result, esgstl::random_access_iterator_tag) {
		auto n = last - first;
		while (n > 0) {
			*result++ = esgstl::move(*first++);
			--n;
		}
		return result;
	}

	// 为 trivially_copy_assignable 类型提供特化版本
	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
		std::is_trivially_move_assignable<Up>::value,
		Up*>::type
		unchecked_move(Tp* first, Tp* last, Up* result) {
		const size_t n = static_cast<size_t>(last - first);
		if (n != 0)
			std::memmove(result, first, n * sizeof(Up));
		return result + n;
	}

	template <class InputIter, class OutputIter>
	OutputIter
		move(InputIter first, InputIter last, OutputIter result) {
		return unchecked_move(first, last, result);
	}
	/*****************************************************************/

	// move_backward
	// 反向移动指定区间的元素到指定位置，返回移动终点位置
	/*****************************************************************/
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result, esgstl::bidirectional_iterator_tag) {
		while (first != last)
			*--result = esgstl::move(*--last);
		return result;
	}

	// random_access_iterator_tag 版本
	template <class RandomIter1, class RandomIter2>
	RandomIter2
		unchecked_move_backward_cat(RandomIter1 first, RandomIter1 last,
			RandomIter2 result, esgstl::random_access_iterator_tag) {
		auto n = last - first;
		while (n > 0) {
			*--result = esgstl::move(*--last);
			--n;
		}
		return result;
	}

	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_move_backward(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result) {
		return unchecked_move_backward_cat(first, last, result, esgstl::iterator_category(first));
	}

	// 为 trivially_copy_assignable 类型提供特化版本
	template <class Tp, class Up>
	typename std::enable_if<
		std::is_same<typename std::remove_const<Tp>::type, Up>::value&&
		std::is_trivially_move_assignable<Up>::value,
		Up*>::type
		unchecked_move_backward(Tp* first, Tp* last, Up* result) {
		const size_t n = static_cast<size_t>(last - first);
		if (n != 0)
		{
			result -= n;
			std::memmove(result, first, n * sizeof(Up));
		}
		return result;
	}

	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		move_backward(BidirectionalIter1 first, BidirectionalIter1 last, BidirectionalIter2 result) {
		return unchecked_move_backward(first, last, result);
	}
	/*****************************************************************/

	// equal
	// 比较两个区间的元素是否相等
	/*****************************************************************/
	template <class InputIter1, class InputIter2>
	bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
		while (first1 != last1) {
			if (*first1++ != *first2++) return false;
		}
		return true;
	}

	// 使用用户提供的比较器进行比较
	template <class InputIter1, class InputIter2,class Compare>
	bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2, Compare comp) {
		while (first1 != last1) {
			if (!comp(*first1,*first2)) return false;
				++first1;++first2;
		}
		return true;
	}
	/*****************************************************************/

	// fill_n 
	// 从first开始填充n个值
	/*****************************************************************/
	template <class OutputIter, class Size, class T>
	OutputIter
		unchecked_fill_n(OutputIter first, Size n, const T& value) {
		while (n > 0) {
			*first++ = value;
			--n;
		}
		return first;
	}

	// 为 one-byte 类型提供特化版本
	template <class Tp, class Size, class Up>
	typename std::enable_if<
		std::is_integral<Tp>::value && sizeof(Tp) == 1 &&
		!std::is_same<Tp,bool>::value && 
		std::is_integral<Up>::value && sizeof(Up) == 1,Tp*>::type
		unchecked_fill_n(Tp* first, Size n, Up value) {
		if (n > 0) {
			std::memset(first, (unsigned char)value, (size_t)n);
		}
		return first + n;
	}

	template <class OutputIter, class Size, class T>
	OutputIter
		fill_n(OutputIter first, Size n, const T& value) {
		return unchecked_fill_n(first, n, value);
	}
	/*****************************************************************/

	// fill
	// 将指定区间中的所有元素赋值为指定值
	/*****************************************************************/

	template <class ForwardIter, class T>
	void unchecked_fill_cat(ForwardIter first, ForwardIter last, const T& value,
		esgstl::forward_iterator_tag) {
		while (first != last) {
			*first++ = value;
		}
	}

	template <class RandomIter, class T>
	void unchecked_fill_cat(RandomIter first, RandomIter last, const T& value,
		esgstl::random_access_iterator_tag) {
		fill_n(first, last - first, value);
	}

	template <class ForwardIter,class T>
	void unchecked_fill(ForwardIter first, ForwardIter last, const T& value) {
		esgstl::unchecked_fill_cat(first, last, value, esgstl::iterator_category(first));
	}

	template <class ForwardIter, class T>
	void fill(ForwardIter first, ForwardIter last, const T& value) {
		unchecked_fill(first, last, value);
	}
	/*****************************************************************/

	// lexicographical_compare
	// 以字典顺序判断第一序列是否小于第二序列
	// 比较逻辑：依次比较两个序列相同位置的元素，若两元素不等，第一序列小返回true（小于），否则返回false（大于等于）
	// 如果有一个序列先到达了末尾，这个序列是第一序列返回true（小于），是第二序列则返回false（大于）
	// 如果两个序列同时到达末尾，则返回false（相等）
	/*****************************************************************/
	template <class InputIter1, class InputIter2>
	bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
								 InputIter2 first2, InputIter2 last2) {
		while (first1 != last1 && first2 != last2) {
			if (*first1 < *first2) return true;
			else if (*first2 < *first1) return false;
			++first1;++first2;
		}
		if (first2 == last2) return false;
		return true;
	}

	template <class InputIter1, class InputIter2, class Compare>
	bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
		InputIter2 first2, InputIter2 last2, Compare comp) {
		while (first1 != last1 && first2 != last2) {
			if (comp(*first1,*first2)) return true;
			else if (comp(*first2, *first1)) return false;
			++first1;++first2;
		}
		if (first2 == last2) return false;
		return true;
	}

	// 针对C风格字符串的特化版本
	bool lexicographical_compare(unsigned char* first1, unsigned char* last1,
		unsigned char* first2, unsigned char* last2) {
		auto len1 = last1 - first1;
		auto len2 = last2 - first2;
		// 关于函数memcmp
		// 如果返回值 < 0，则表示 str1 小于 str2。
		// 如果返回值 > 0，则表示 str2 小于 str1。
		// 如果返回值 = 0，则表示 str1 等于 str2。
		const auto result = std::memcmp(first1, first2, esgstl::min(len1, len2));
		// 放入比较的两个区间相等，谁长谁大
		// 若不相等，根据memcmp返回的结果判断谁大
		return result != 0 ? result < 0 : len1 < len2;
	}
	/*****************************************************************/

	// mismatch
	// 依次比较两个序列，找到第一处不相等的元素，返回一对迭代器，分别指向两个序列中不相等的元素
	/*****************************************************************/
	template <class InputIter1, class InputIter2>
	esgstl::pair<InputIter1,InputIter2>
		mismatch(InputIter1 first1, InputIter1 last1,
			InputIter2 first2, InputIter2 last2) {
		while (first1 != last1 && *first1 == *first2) {
			++first1;++first2;
		}
		return esgstl::pair<InputIter1, InputIter2>(first1, first2);
	}

	template <class InputIter1, class InputIter2, class Compare>
	esgstl::pair<InputIter1, InputIter2>
		mismatch(InputIter1 first1, InputIter1 last1,
			InputIter2 first2, InputIter2 last2, Compare comp) {
		while (first1 != last1 && comp(*first1,*first2)) {
			++first1;++first2;
		}
		return esgstl::pair<InputIter1, InputIter2>(first1, first2);
	}
	/*****************************************************************/

}



