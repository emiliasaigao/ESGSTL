#pragma once
#include <cstddef>
#include <cstdlib>
#include <climits>
#include <cassert>

// 这个头文件负责更高级的动态内存管理
// 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及智能指针 auto_ptr 和 shared_ptr

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"
#include "util.h"
#include "functional.h"


namespace esgstl {

	// 获取空间地址
	template <class T>
	constexpr T* address_of(T& value) noexcept{
		return &value;
	}

	// 获取临时缓冲区
	template <class T>
	esgstl::pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*) {
		// len是申请的缓冲区大小
		// 当len过大，调整len
		if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))) len = INT_MAX / sizeof(T);
		while (len > 0) {
			T* tmp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
			// 如果直接申请到内存，返回结果
			if (tmp) return esgstl::pair<T*, ptrdiff_t>(tmp, len);
			// 否则申请大小减半
			len /= 2;
		}
		// 没申请到内存
		return esgstl::pair<T*, ptrdiff_t>(nullptr, 0);
	}

	// 要么显式提供模板参数
	template <class T>
	esgstl::pair<T*, ptrdiff_t> get_temporary_helper(ptrdiff_t len) {
		return get_buffer_helper(len, static_cast<T*>(0));
	}

	// 要么提供一个指针类型用来类型判断
	template <class T>
	esgstl::pair<T*, ptrdiff_t> get_temporary_helper(ptrdiff_t len,T*) {
		return get_buffer_helper(len, static_cast<T*>(0));
	}

	// 销毁临时缓冲区
	template <class T>
	void release_tmeporary_buffer(T* ptr) {
		free(ptr);
	}

	// 模板类temporary_buffer
	// 进行缓冲区的申请与释放
	/*****************************************************************/
	template <class ForwardIter,class T>
	class temporary_buffer {
	private:
		ptrdiff_t original_len; // 申请的大小
		ptrdiff_t len;			// 实际大小
		T* buffer;				// 指向缓冲区的指针

	public:
		// 构造函数
		temporary_buffer(ForwardIter first, ForwardIter last);

		// 析构函数
		~temporary_buffer() {
			esgstl::destory(buffer, buffer + len);
			free(buffer);
		}

	public:
		ptrdiff_t size()			const noexcept { return len; }
		ptrdiff_t requested_size()	const noexcept { return original_len; }
		T* begin()						  noexcept { return buffer; }
		T* end()						  noexcept { return buffer + len; }

	private:
		void allocate_buffer();
		void initialize_buffer(const T&,std::true_type){}
		void initialize_buffer(const T& value, std::false_type) {
			esgstl::uninitialized_fill_n(buffer, len, value);
		}

	private:
		// 私有的拷贝构造函数和拷贝赋值运算符,阻止拷贝
		temporary_buffer(const temporary_buffer&);
		void operator=(const temporary_buffer&);
	};

	template <class ForwardIter, class T>
	void temporary_buffer<ForwardIter, T>::allocate_buffer() {
		original_len = len;
		auto p = esgstl::get_temporary_helper<T>(len);
		buffer = p.first;
		len = p.second;
	}

	template <class ForwardIter,class T>
	temporary_buffer<ForwardIter, T>::temporary_buffer(ForwardIter first, ForwardIter last) {
		try
		{
			len = esgstl::distance(first, last);
			allocate_buffer();
			if (len > 0) initialize_buffer(*first, std::is_trivially_default_constructible<T>{});
		}
		catch (...)
		{
			free(buffer);
			buffer = nullptr;
			len = 0;
		}
	}

	// 模板类unique_ptr
	// 具有严格对象所有权的智能指针
	/******************************************************************/
	template <class T>
	class unique_ptr {
	public:
		typedef T value_type;
	
	private:
		T* m_ptr; // 指向实际对象的指针

	public:
		explicit unique_ptr(T* p = nullptr) :m_ptr(p) {}
		unique_ptr(unique_ptr&& rhs) noexcept : m_ptr(rhs.release()) {}
		template <class U>
		unique_ptr(unique_ptr<U>&& rhs) noexcept : m_ptr(rhs.release()) {}

		// 拷贝赋值运算符
		unique_ptr& operator=(unique_ptr&& rhs) noexcept {
			if (this != &rhs) {
				reset(rhs.release());
			}
			return *this;
		}

		template<class U>
		unique_ptr& operator=(unique_ptr<U>&& rhs) noexcept {
			if (this->get() != rhs.get()) {
				reset(rhs.release());
			}
			return *this;
		}

		~unique_ptr() { delete m_ptr; }

		// 禁止拷贝赋值/构造操作
		unique_ptr(const unique_ptr& rhs) = delete;
		unique_ptr& operator=(const unique_ptr& rhs) = delete;

		explicit operator bool() const noexcept {
			return m_ptr != nullptr;
		}

	public:
		// 重载 operator* 和 operator->
		T& operator*()  const { return *m_ptr; }
		T* operator->() const { return m_ptr; }
		
		// 获得底层原始指针
		T* get() const { return m_ptr; }
		
		// 释放对m_ptr的管理权
		T* release() {
			T* tmp = m_ptr;
			m_ptr = nullptr;
			return tmp;
		}

		// 重设需要管理的原始指针
		void reset(T* p = nullptr) {
			if (p != m_ptr) {
				delete m_ptr;
				m_ptr = p;
			}
		}
		
	};
	/******************************************************************/

	// 计数类
	struct ptr_count {
		int shared_count = 1;
		int weak_count = 0;
	};

	// weak_ptr是若共享指针，由shared_ptr初始化
	// 不增加管理资源的指针计数，当管理资源的shared_ptr计数清零后将阻止访问资源
	template <class T>
	class weak_ptr {
		template <class U> friend class shared_ptr;
	private:
		T* _data = nullptr;
		ptr_count* _ptr_count = nullptr;

	public:
		weak_ptr() = default;
		weak_ptr(const weak_ptr& rhs) noexcept
			: _data(rhs._data), _ptr_count(rhs._ptr_count) {
			if (_ptr_count) ++_ptr_count->weak_count;
		}

		weak_ptr(const shared_ptr<T>& rhs) noexcept
			: _data(rhs._data), _ptr_count(rhs._ptr_count) {
			if (_ptr_count) ++_ptr_count->weak_count;
		}

		weak_ptr(weak_ptr&& rhs) noexcept {
			swap(rhs);
			rhs.reset();
		}

		weak_ptr& operator=(const weak_ptr& rhs) noexcept {
			weak_ptr tmp(rhs);
			swap(tmp);
			return *this;
		}
		
		weak_ptr& operator=(weak_ptr&& rhs) noexcept {
			weak_ptr tmp(esgstl::move(rhs));
			swap(tmp);
			return *this;
		}

		~weak_ptr() {
			release();
		}

		void reset() noexcept {
			release();
			_data = nullptr;
			_ptr_count = nullptr;
		}

		int use_count() const noexcept {
			return _data == nullptr ? 0 : _ptr_count->shared_count;
		}

		bool expired() const noexcept { return use_count() == 0; }

		shared_ptr<T> lock() {
			return expired() ? shared_ptr<T>() : shared_ptr<T>(*this);
		}

		private:
			void swap(weak_ptr& rhs) noexcept {
				esgstl::swap(_data, rhs._data);
				esgstl::swap(_ptr_count, rhs._ptr_count);
			}

			void release() {
				if (_ptr_count) {
					if (_ptr_count->shared_count == 0 && --_ptr_count->weak_count == 0) {
						delete _ptr_count;
						_ptr_count = nullptr;
					}
				}
			}
	};

	// shared_ptr允许有多个智能指针同时指向一个底部资源
	// 当最后一个指向资源的shared_ptr被析构时，会删除资源
	/******************************************************************/
	template <class T>
	class shared_ptr {
		template<class U> friend class shared_ptr;
		template<class U> friend class weak_ptr;
		private:
			T* _data = nullptr;
			ptr_count* _ptr_count = nullptr;

		public:
			shared_ptr() = default;

			explicit shared_ptr(T* data) : _data(data) {
				if (data) {
					try {
						_ptr_count = new ptr_count();
					}
					catch (...) {
						delete data;
						throw;
					}
				}
			}
			
			shared_ptr(const shared_ptr& rhs) noexcept 
				: _data(rhs._data), _ptr_count(rhs._ptr_count) {
				if (_ptr_count) ++_ptr_count->shared_count;
			}

			shared_ptr(shared_ptr&& rhs) noexcept {
				swap(rhs);
				rhs.reset();
			}

			template <class U>
			shared_ptr(const shared_ptr<U>& rhs) noexcept
				: _data(rhs._data), _ptr_count(rhs._ptr_count) {
				if (_ptr_count) ++_ptr_count->shared_count;
			}

			explicit shared_ptr(const weak_ptr<T>& rhs) noexcept
				: _data(rhs._data), _ptr_count(rhs._ptr_count) {
				if (_ptr_count) ++_ptr_count->shared_count;
			}

			shared_ptr& operator=(const shared_ptr& rhs) noexcept {
				shared_ptr tmp(rhs);
				swap(tmp);
				return *this;
			}

			template <class U>
			shared_ptr& operator=(const shared_ptr<U>& rhs) noexcept {
				shared_ptr tmp(rhs);
				swap(tmp);
				return *this;
			}

			shared_ptr& operator=(shared_ptr&& rhs) noexcept {
				shared_ptr tmp(esgstl::move(rhs));
				swap(tmp);
				return *this;
			}

			explicit operator bool() const noexcept {
				return _data != nullptr;
			}

			~shared_ptr() {
				release();
			}

			bool unique() const noexcept {
				return _data != nullptr  && _ptr_count->shared_count == 1;
			}

			int use_count() const noexcept {
				return _data == nullptr ? 0 : _ptr_count->shared_count;
			}

			void reset() {
				release();
				_data = nullptr;
				_ptr_count = nullptr;
			}

			void reset(T* data) {
				reset();
				shared_ptr tmp(data);
				swap(tmp);
			}

			T& operator*() const noexcept {
				// 这里会调用operator bool如果_data为空会中断程序
				assert(*this);
				return *_data;
			}

			T* operator->() const noexcept {
				return &operator*();
			}

			T* get() const noexcept {
				return _data;
			}

		private:
			void swap(shared_ptr& rhs) noexcept {
				esgstl::swap(_data, rhs._data);
				esgstl::swap(_ptr_count, rhs._ptr_count);
			}

			void release() {
				if (_ptr_count) {
					if (--_ptr_count->shared_count == 0) {
						delete _data;
						_data = nullptr;
						if (_ptr_count->weak_count == 0) {
							delete _ptr_count;
							_ptr_count = nullptr;
						}
					}
				}
			}
	};

	template <class T, class U>
	bool operator==(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) { return lhs.get() == rhs.get(); }
	template <class T, class U>
	bool operator!=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) { return lhs.get() != rhs.get(); }
	template <class T, class U>
	bool operator<=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) { return lhs.get() <= rhs.get(); }
	template <class T, class U>
	bool operator<(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) { return lhs.get() < rhs.get(); }
	template <class T, class U>
	bool operator>=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) { return lhs.get() >= rhs.get(); }
	template <class T, class U>
	bool operator>(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs) { return lhs.get() > rhs.get(); }

	// 智能指针静态强制类型转换
	template <class T, class U>
	shared_ptr<T> static_pointer_cast(shared_ptr<U>& ptr) {
		return shared_ptr<T>(ptr, static_cast<shared_ptr<T>::element_type*>(ptr.get()));
	}

	// 智能指针动态强制类型转换
	template <class T, class U>
	shared_ptr<T> dynamic_pointer_cast(shared_ptr<U>& ptr) {
		T* p = dynamic_cast<shared_ptr<T>::element_type*>(ptr.get());
		if (p) return shared_ptr<T>(ptr, p);
		return shared_ptr<T>();
	}

	template <class T,class Alloc = allocator<T>, class... Args>
	shared_ptr<T> make_shared(Args&&... args) {
		T* ptr = new T(forward<Args>(args)...);		
		return shared_ptr<T>(ptr);
	}

}
