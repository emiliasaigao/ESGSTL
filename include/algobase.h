#pragma once
#include <cstring>

#include "iterator.h"
#include "util.h"

// ���ͷ�ļ�������һЩ�����㷨


namespace esgstl {

	// ��֪������ĺ�
	#ifdef max
	#pragma message("#undefing marco max")
	#undef max
	#endif // max

	#ifdef min
	#pragma message("#undefing marco min")
	#undef min
	#endif // min


	// max �� min
	/*****************************************************************/
	
	// max:���߽ϴ�ֵ��Ĭ�ϰ汾
	template <class T>
	const T& max(const T& lhs, const T& rhs) {
		return lhs < rhs ? rhs : lhs;
	}

	// max:���߽ϴ�ֵ��ָ���ȽϷº����汾
	template <class T, class Compare>
	const T& max(const T& lhs, const T& rhs, Compare comp) {
		return comp(lhs, rhs) ? rhs : lhs;
	}

	// min:���߽�Сֵ��Ĭ�ϰ汾
	template <class T>
	const T& min(const T& lhs, const T& rhs) {
		return lhs < rhs ? lhs : rhs;
	}

	// max���߽�Сֵ��ָ���ȽϷº����汾
	template <class T, class Compare>
	const T& min(const T& lhs, const T& rhs, Compare comp) {
		return comp(lhs, rhs) ? lhs : rhs;
	}
	/*****************************************************************/


	// iter_swap
	/*****************************************************************/
	
	// iter_swap:�Ե�������������ָ����
	template <class ForwardIter1,class ForwardIter2>
	void iter_swap(ForwardIter1 lhs, ForwardIter2 rhs) {
		esgstl::swap(*lhs, *rhs);
	}
	/*****************************************************************/

	// copy ���򿽱�
	/*****************************************************************/

	// unchecked_copy_cat:ʵ�ʸ����߼����� ����������汾
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

	// unchecked_copy_cat:ʵ�ʸ����߼����� ����������汾��������ʡȥ�˱Ƚϵ������Ƿ���ȵĲ���
	template <class RandomIter,class OutputIter>
	OutputIter
		unchecked_copy_cat(RandomIter first, RandomIter last, OutputIter result, esgstl::random_access_iterator_tag) {
		for (auto n = last - first; n > 0; --n, ++first, ++result) {
			*result = *first;
		}
		return result;
	}

	// unchecked_copy:ʵ�ʸ����߼����� �������汾
	// ��������������Ĳ�ͬ���͵��ò�ͬ�Ŀ����߼�
	template <class InputIter, class OutputIter>
	OutputIter
		unchecked_copy(InputIter first, InputIter last, OutputIter result) {
		return unchecked_copy_cat(first, last, result, esgstl::iterator_category(first));
	}

	// unchecked_copy��Ϊ trivially_copy_assignable �����ṩ�ػ��汾
	// ��Up��Tpȥ��const���������ͬ�����������û����Ч�Ŀ���������Ϊʱ�����ģ��ᱻƥ�䵽
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

	// copy: �����û��Ŀ�������Ľӿ�
	template <class InputIter,class OutputIter>
	OutputIter copy(InputIter first, InputIter last, OutputIter result) {
		return esgstl::unchecked_copy(first, last, result);
	}
	/*****************************************************************/

	// copy_backward
	/*****************************************************************/

	// unchecked_copy_backward_cat: ʵ�ʸ����߼����� ˫��������汾
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_copy_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result, esgstl::bidirectional_iterator_tag) {
		while (last != first) {
			*--result = *--last;
			
		}
		return result;
	}

	// unchecked_copy_backward_cat: ʵ�ʸ����߼����� ����������汾
	template <class RandomIter,class BidirectionalIter>
	BidirectionalIter
		unchecked_copy_backward_cat(RandomIter first, RandomIter last, 
			BidirectionalIter result, esgstl::random_access_iterator_tag) {
		for (auto n = last - first; n > 0; --n) {
			*--result = *--last;
		}
		return result;
	}

	// unchecked_copy_backward: ʵ�ʸ����߼����� �������汾
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result) {
		return unchecked_copy_backward_cat(first, last, result, esgstl::iterator_category(first));
	}

	// unchecked_copy_backward:Ϊtrivially_copy_assignable �����ṩ�ػ��汾
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

	// copy_backward:���Ÿ��û������򿽱�����
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		copy_backward(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result) {
		return esgstl::unchecked_copy_backward(first, last, result);
	}

	/*****************************************************************/

	// copy_if
	// ��������������������ָ��λ��
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
	// ����ָ��������Ԫ�ص�ָ��λ�ã�����pair�ֱ�ָ�����������ĩβ
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
	// �ƶ�ָ�������Ԫ�ص�ָ��λ�ã������ƶ��յ�λ��
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

	// Ϊ trivially_copy_assignable �����ṩ�ػ��汾
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
	// �����ƶ�ָ�������Ԫ�ص�ָ��λ�ã������ƶ��յ�λ��
	/*****************************************************************/
	template <class BidirectionalIter1, class BidirectionalIter2>
	BidirectionalIter2
		unchecked_move_backward_cat(BidirectionalIter1 first, BidirectionalIter1 last,
			BidirectionalIter2 result, esgstl::bidirectional_iterator_tag) {
		while (first != last)
			*--result = esgstl::move(*--last);
		return result;
	}

	// random_access_iterator_tag �汾
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

	// Ϊ trivially_copy_assignable �����ṩ�ػ��汾
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
	// �Ƚ����������Ԫ���Ƿ����
	/*****************************************************************/
	template <class InputIter1, class InputIter2>
	bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2) {
		while (first1 != last1) {
			if (*first1++ != *first2++) return false;
		}
		return true;
	}

	// ʹ���û��ṩ�ıȽ������бȽ�
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
	// ��first��ʼ���n��ֵ
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

	// Ϊ one-byte �����ṩ�ػ��汾
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
	// ��ָ�������е�����Ԫ�ظ�ֵΪָ��ֵ
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
	// ���ֵ�˳���жϵ�һ�����Ƿ�С�ڵڶ�����
	// �Ƚ��߼������αȽ�����������ͬλ�õ�Ԫ�أ�����Ԫ�ز��ȣ���һ����С����true��С�ڣ������򷵻�false�����ڵ��ڣ�
	// �����һ�������ȵ�����ĩβ����������ǵ�һ���з���true��С�ڣ����ǵڶ������򷵻�false�����ڣ�
	// �����������ͬʱ����ĩβ���򷵻�false����ȣ�
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

	// ���C����ַ������ػ��汾
	bool lexicographical_compare(unsigned char* first1, unsigned char* last1,
		unsigned char* first2, unsigned char* last2) {
		auto len1 = last1 - first1;
		auto len2 = last2 - first2;
		// ���ں���memcmp
		// �������ֵ < 0�����ʾ str1 С�� str2��
		// �������ֵ > 0�����ʾ str2 С�� str1��
		// �������ֵ = 0�����ʾ str1 ���� str2��
		const auto result = std::memcmp(first1, first2, esgstl::min(len1, len2));
		// ����Ƚϵ�����������ȣ�˭��˭��
		// ������ȣ�����memcmp���صĽ���ж�˭��
		return result != 0 ? result < 0 : len1 < len2;
	}
	/*****************************************************************/

	// mismatch
	// ���αȽ��������У��ҵ���һ������ȵ�Ԫ�أ�����һ�Ե��������ֱ�ָ�����������в���ȵ�Ԫ��
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



