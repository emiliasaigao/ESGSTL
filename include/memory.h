#pragma once
#include <cstddef>
#include <cstdlib>
#include <climits>

// 这个头文件负责更高级的动态内存管理
// 包含一些基本函数、空间配置器、未初始化的储存空间管理，以及智能指针 auto_ptr 和 shared_ptr

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"
#include "util.h"


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

	// 模板类auto_ptr
	// 具有严格对象所有权的智能指针
	/******************************************************************/
	template <class T>
	class auto_ptr {
	public:
		typedef T value_type;
	
	private:
		T* m_ptr; // 指向实际对象的指针

	public:
		explicit auto_ptr(T* p = nullptr) :m_ptr(p) {}
		auto_ptr(auto_ptr& rhs) : m_ptr(rhs.release()) {}
		template <class U>
		auto_ptr(auto_ptr<U>& rhs) : m_ptr(rhs.release()) {}

		// 拷贝赋值运算符
		auto_ptr& operator=(auto_ptr& rhs) {
			if (this != &rhs) {
				reset(rhs.release());
			}
			return *this;
		}

		template<class U>
		auto_ptr& operator=(auto_ptr<U>& rhs) {
			if (this->get() != rhs.get()) {
				reset(rhs.release());
			}
			return *this;
		}

		~auto_ptr() { delete m_ptr; }
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

	// shared_ptr智能指针相关类
	// shared_ptr允许有多个智能指针同时指向一个底部资源
	// 当最后一个指向资源的shared_ptr被析构时，会删除资源
	/******************************************************************/
	class shared_ptr_count{
	private:
		long* ptrcnt;
		
	public:
		// 构造函数
		shared_ptr_count():ptrcnt(nullptr){}
		shared_ptr_count(const shared_ptr_count& rhs) :ptrcnt(rhs.ptrcnt) {}

		void swap(shared_ptr_count& rhs) { esgstl::swap(ptrcnt, rhs.ptrcnt); }
		long use_count() const { return ptrcnt == nullptr ? 0 : *ptrcnt; }

		// 获取/共享指针的所有权，初始化/增加引用计数器
		template <class U>
		void acquire(U* p) {
			if (p != nullptr) {
				if (ptrcnt == nullptr) {
					try
					{
						ptrcnt = new long(1);
					}
					catch (...)
					{
						delete p;
						throw;
					}
				}
				else {
					++(*ptrcnt);
				}
			}
		}

		// 释放px指针的所有权，在适当的时候销毁对象
		template <class U>
		void release(U* p) {
			if (ptrcnt != nullptr) {
				--(*ptrcnt);
				if (*ptrcnt == 0) {
					delete p;
					delete ptrcnt;
				}
				ptrcnt = nullptr;
			}
		}
	};

	// shared_ptr_base 智能指针基类
	class shared_ptr_base {
	protected:
		shared_ptr_count spc;

		shared_ptr_base():spc(){}
		shared_ptr_base(const shared_ptr_base& rhs) :spc(rhs.spc) {}

	};

	// 智能指针shared_ptr
	template <class T>
	class shared_ptr : public shared_ptr_base {
	public:
		typedef T element_type;
	private:
		// 指向被管理空间的指针
		T* base_ptr;
	public:
		// 构造函数
		explicit shared_ptr(T* ptr) :shared_ptr_base() {
			acquire(ptr);
		}

		shared_ptr(const shared_ptr& rhs) :shared_ptr_base(rhs) {
			acquire(rhs.base_ptr);
		}

		// 这个构造器只用于智能指针类型强转，即只在static_pointer_cast（全局函数）里使用
		template<class U>
		shared_ptr(const shared_ptr<U>& rhs,T* p) : shared_ptr_base(rhs) {
			acquire(p);
		}

		template <class U>
		shared_ptr(const shared_ptr<U>& rhs) : shared_ptr_base(rhs) {
			acquire(static_cast<shared_ptr<T>::element_type*>(rhs.get()));
		}

		// 拷贝赋值运算符,使用了拷贝与交换手法
		shared_ptr& operator=(shared_ptr rhs) {
			// rhs是临时对象，拷贝了传进来的实参
			swap(rhs);
			return *this;
		}

		~shared_ptr() { release(); }
		// 重设指针，不传入参数则直接释放管理权
		void reset() { release(); }
		// 重设指针为传入的原始指针
		void reset(T* ptr) {
			release();
			acquire(ptr);
		}

		// 显式类型转换为bool，只在if语句里会被隐式执行
		explicit operator bool() { return spc.use_count() > 0; }
		// 返回是否是底层指针唯一的管理类
		bool unique() { return spc.use_count() == 1; }
		// 返回当前底层指针被多少管理类同时管理
		long use_count() { return spc.use_count(); }

		T& operator*() { return *base_ptr; }
		T* operator->() { return base_ptr; }
		
		T* get() const { return base_ptr; }
		void swap(shared_ptr& rhs) {
			esgstl::swap(base_ptr, rhs.base_ptr);
			spc.swap(rhs.spc);
		}

	private:
		// 负责计算计数和赋值底层指针
		void acquire(T* ptr) {
			// 调用share_ptr_base中的share_ptr_count 的acquire
			// 初始化或者增加计数
			spc.acquire(ptr);
			base_ptr = ptr;
		}
		
		// 负责释放管理权，减少计数，并置底层指针为nullptr
		void release() {
			spc.release(base_ptr);
			base_ptr = nullptr;
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
