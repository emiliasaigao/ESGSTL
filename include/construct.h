#pragma once
#include <new>

#include "type_traits.h"
#include "iterator.h"

// ���ͷ�ļ������������� construct��destroy
// construct : �������Ĺ���
// destroy   : ������������


// ��֪������ĺ�
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER

namespace esgstl {
	// construct ����
	/***************************************************/
	template <class Ty>
	void construct(Ty* ptr) {
		// ����placement new ��ptr��ָλ�õ��ù��캯��
		// ����placement new �β���void*������һ��ǿת
		::new ((void*)ptr) Ty();
	}

	template <class Ty1,class Ty2>
	void construct(Ty1* ptr, const Ty2& value) {
		::new ((void*)ptr) Ty1(value);
	}

	template <class Ty1,class... Args>
	void construct(Ty1* ptr, Args&&... args) {
		::new ((void*)ptr) Ty1(esgstl::forward<Args>(args)...);
	}
	/***************************************************/

	// destroy ����
	// ��һ����
	template <class Ty>
	void destroy_one(Ty , std::true_type) {};

	template <class Ty>
	void destroy_one(Ty* ptr, std::false_type) {
		if (ptr != nullptr) {
			ptr->~Ty();
		}
	}

	template <class Ty>
	void destory(Ty* ptr) {
		destroy_one(ptr, std::is_trivially_destructible<Ty>());
	}

	// ��������
	template <class ForwardIter>
	void destory_cat(ForwardIter ,ForwardIter,std::true_type) {}

	template <class ForwardIter>
	void destory_cat(ForwardIter first, ForwardIter last, std::false_type) {
		while (first != last) {
			destory(&*first);
			++first;
		}
	}

	template <class ForwardIter>
	void destory(ForwardIter first, ForwardIter last) {
		destory_cat(first, last, std::is_trivially_destructible<
			typename iterator_traits<ForwardIter>::value_type>());
	}


}
