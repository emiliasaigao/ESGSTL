#pragma once
#include <new>

#include "type_traits.h"
#include "iterator.h"

// 这个头文件包含两个函数 construct，destroy
// construct : 负责对象的构造
// destroy   : 负责对象的析构


// 不知道干嘛的宏
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4100)  // unused parameter
#endif // _MSC_VER

namespace esgstl {
	// construct 函数
	/***************************************************/
	template <class Ty>
	void construct(Ty* ptr) {
		// 调用placement new 在ptr所指位置调用构造函数
		// 由于placement new 形参是void*所以做一次强转
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

	// destroy 函数
	// 单一析构
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

	// 区间析构
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
