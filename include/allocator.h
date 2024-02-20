#pragma once
// 模板类 allocator用于管理内存的分配、释放，对象的构造、析构

#include "construct.h"
#include "util.h"


namespace esgstl {
	template <class T>
	class allocator {
		public:
			typedef T				value_type;
			typedef T*				pointer;
			typedef T&				reference;
			typedef const T*		const_pointer;
			typedef const T&		const_reference;
			typedef size_t			size_type;
			typedef ptrdiff_t		difference_type;

		public:
			// 没有构造函数，里面都是一些静态方法

			static T* allocate();
			static T* allocate(size_type n);

			static void deallocate(T* ptr);
			static void deallocate(T* ptr, size_type n);

			static void construct(T* ptr);
			static void construct(T* ptr, const T& value);
			static void construct(T* ptr, T&& value);

			template <class... Args>
			static void construct(T* ptr, Args&& ...args);

			static void destory(T* ptr);
			static void destory(T* fisrt, T* last);

	};

	template <class T>
	T* allocator<T>::allocate() {
		return static_cast<T*>(::operator new (sizeof(T)));
	}

	template <class T>
	T* allocator<T>::allocate(size_type n) {
		if (n == 0) return nullptr;
		return static_cast<T*>(::operator new (n * sizeof(T)));
	}

	template <class T>
	void allocator<T>::deallocate(T* ptr) {
		if (ptr != nullptr) {
			::operator delete(ptr);
		}
	}

	template <class T>
	void allocator<T>::deallocate(T* ptr, size_type) {
		if (ptr != nullptr) {
			::operator delete(ptr);
		}
	}

	template <class T>
	void allocator<T>::construct(T* ptr) {
		esgstl::construct(ptr);
	}

	template <class T>
	void allocator<T>::construct(T* ptr,const T& value) {
		esgstl::construct(ptr,value);
	}

	template <class T>
	void allocator<T>::construct(T* ptr,T&& value) {
		esgstl::construct(ptr,esgstl::move(value));
	}

	template <class T>
	template <class... Args>
	void allocator<T>::construct(T* ptr, Args&& ...args) {
		esgstl::construct(ptr, esgstl::forward<Args>(args)...);
	}

	template <class T>
	void allocator<T>::destory(T* ptr) {
		esgstl::destory(ptr);
	}

	template <class T>
	void allocator<T>::destory(T* first, T* last) {
		esgstl::destory(first, last);
	}
	
}