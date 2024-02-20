#pragma once

#include "util.h"
#include "algobase.h"
#include "construct.h"
#include "type_traits.h"
#include "iterator.h"

namespace esgstl {

	// uninitialized_copy
	// ��ָ�����临�Ƶ�һ��δ�����������
	/*****************************************************************/


	// ���캯���޹ؽ�Ҫʱ
	template <class InputIter, class ForwardIter>
	ForwardIter unchecked_uninitialized_copy(InputIter first, InputIter last, ForwardIter result, std::true_type) {
		return esgstl::copy(first, last, result);
	}

	// ���캯����Чʱ
	template <class InputIter, class ForwardIter>
	ForwardIter unchecked_uninitialized_copy(InputIter first, InputIter last, ForwardIter result, std::false_type) {
		auto cur = result;
		try
		{
			while (first != last) {
				esgstl::construct(&*cur, *first);
				++first;++cur;
			}
		}
		catch (...)
		{
			while (result != cur) {
				esgstl::destory(&*result);
				++result;
			}
		}
		return cur;
	}
	
	// �û��ӿ�
	template <class InputIter, class ForwardIter>
	ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter result) {
		return esgstl::unchecked_uninitialized_copy(first, last, result,
			std::is_trivially_copy_assignable<
			typename esgstl::iterator_traits<ForwardIter>::
			value_type>{});
	}
	/*****************************************************************/

	
	// uninitialized_copy_n
	// ��ָ����ͷ�ͳ��ȵ����临�Ƶ�δ��ʼ��������
	/*****************************************************************/


	// ���캯���޹ؽ�Ҫʱ
	template <class InputIter, class Size, class ForwardIter>
	ForwardIter unchecked_uninitialized_copy_n(InputIter first, Size n, ForwardIter result, std::true_type) {
		return esgstl::copy_n(first, n, result).second;
	}

	// ���캯����Чʱ
	template <class InputIter, class Size, class ForwardIter>
	ForwardIter unchecked_uninitialized_copy_n(InputIter first, Size n, ForwardIter result, std::false_type) {
		auto cur = result;
		try
		{
			while (n > 0) {
				esgstl::construct(&*cur, *first);
				++first;++cur;--n;
			}
		}
		catch (...)
		{
			while (result != cur) {
				esgstl::destory(&*result);
				++result;
			}
		}
		return cur;
	}

	// �û��ӿ�
	template <class InputIter, class Size, class ForwardIter>
	ForwardIter uninitialized_copy_n(InputIter first, Size n, ForwardIter result) {
		return esgstl::unchecked_uninitialized_copy_n(first, n, result, std::is_trivially_copy_assignable<
			typename esgstl::iterator_traits<ForwardIter>::value_type>{});
	}
	/*****************************************************************/

	
	// uninitialized_fill
	// ��ָ��δ��ʼ���������ָ��ֵ
	/*****************************************************************/


	template <class ForwardIter, class T>
	void unchecked_uninitialized_fill(ForwardIter first, ForwardIter last, const T& value, std::true_type) {
		esgstl::fill(first, last, value);
	}

	template <class ForwardIter, class T>
	void unchecked_uninitialized_fill(ForwardIter first, ForwardIter last, const T& value, std::false_type) {
		auto cur = first;
		try
		{
			while (cur != last) {
				esgstl::construct(&*cur, value);
				++cur;
			}
		}
		catch (...)
		{
			while (first != cur) {
				esgstl::destory(&*first);
				++first;
			}
		}
	}

	template <class ForwardIter, class T>
	void uninitialized_fill(ForwardIter first, ForwardIter last, const T& value) {
		esgstl::unchecked_uninitialized_fill(first, last, value, std::is_trivially_copy_assignable<
			typename esgstl::iterator_traits<ForwardIter>::value_type>{});
	}
	/*****************************************************************/
	
	
	// uninitialized_fill_n
	// ��ָ��δ��ʼ��ָ�����ͳ��ȵ��������ָ��ֵ��������������λ��
	/*****************************************************************/


	template <class ForwardIter, class Size, class T>
	ForwardIter unchecked_uninitialized_fill_n(ForwardIter first, Size n, const T& value, std::true_type) {
		return esgstl::fill_n(first, n, value);
	}

	template <class ForwardIter, class Size, class T>
	ForwardIter unchecked_uninitialized_fill_n(ForwardIter first, Size n, const T& value, std::false_type) {
		auto cur = first;
		try
		{
			while (n > 0) {
				esgstl::construct(&*cur, value);
				++cur;--n;
			}
		}
		catch (...)
		{
			while (first != cur) {
				esgstl::destory(&*first);
				++first;
			}
		}
		return cur;
	}

	template <class ForwardIter, class Size, class T>
	ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& value) {
		return esgstl::unchecked_uninitialized_fill_n(first, n, value, std::is_trivially_copy_assignable<
			typename esgstl::iterator_traits<ForwardIter>::value_type>{});
	}
	// uninitialized_move
	// ��ָ�������ƶ���һ��δ����������ϣ������ƶ�����λ��
	/*****************************************************************/


	// ���캯���޹ؽ�Ҫʱ
	template <class InputIter, class ForwardIter>
	ForwardIter unchecked_uninitialized_move(InputIter first, InputIter last, ForwardIter result, std::true_type) {
		return esgstl::move(first, last, result);
	}

	// ���캯����Чʱ
	template <class InputIter, class ForwardIter>
	ForwardIter unchecked_uninitialized_move(InputIter first, InputIter last, ForwardIter result, std::false_type) {
		auto cur = result;
		try
		{
			while (first != last) {
				esgstl::construct(&*cur,esgstl::move(*first));
				++first;++cur;
			}
		}
		catch (...)
		{
			esgstl::destory(result, cur);
		}
		return cur;
	}

	// �û��ӿ�
	template <class InputIter, class ForwardIter>
	ForwardIter uninitialized_move(InputIter first, InputIter last, ForwardIter result) {
		return esgstl::unchecked_uninitialized_move(first, last, result, std::is_trivially_copy_assignable<
			typename esgstl::iterator_traits<ForwardIter>::value_type>{});
	}
	/*****************************************************************/


	// uninitialized_move_n
	// ��ָ����ͷ�ͳ��ȵ����临�Ƶ�δ��ʼ��������
	/*****************************************************************/


	// ���캯���޹ؽ�Ҫʱ
	template <class InputIter, class Size, class ForwardIter>
	ForwardIter unchecked_uninitialized_move_n(InputIter first, Size n, ForwardIter result, std::true_type) {
		esgstl::move(first, first + n, result);
	}

	// ���캯����Чʱ
	template <class InputIter, class Size, class ForwardIter>
	ForwardIter unchecked_uninitialized_move_n(InputIter first, Size n, ForwardIter result, std::false_type) {
		auto cur = result;
		try
		{
			while (n > 0) {
				esgstl::construct(&*cur, esgstl::move(*first));
				++first;++cur;--n;
			}
		}
		catch (...)
		{
			esgstl::destory(result,cur);
		}
		return cur;
	}

	// �û��ӿ�
	template <class InputIter, class Size, class ForwardIter>
	ForwardIter uninitialized_move_n(InputIter first, Size n, ForwardIter result) {
		return esgstl::unchecked_uninitialized_move_n(first, n, result, std::is_trivially_copy_assignable<
			typename esgstl::iterator_traits<ForwardIter>::value_type>{});
	}
	/*****************************************************************/

	

}