#pragma once
#include <cstddef>
#include <cstdlib>
#include <climits>
#include <cassert>

// ���ͷ�ļ�������߼��Ķ�̬�ڴ����
// ����һЩ�����������ռ���������δ��ʼ���Ĵ���ռ�����Լ�����ָ�� auto_ptr �� shared_ptr

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"
#include "util.h"
#include "functional.h"


namespace esgstl {

	// ��ȡ�ռ��ַ
	template <class T>
	constexpr T* address_of(T& value) noexcept{
		return &value;
	}

	// ��ȡ��ʱ������
	template <class T>
	esgstl::pair<T*, ptrdiff_t> get_buffer_helper(ptrdiff_t len, T*) {
		// len������Ļ�������С
		// ��len���󣬵���len
		if (len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T))) len = INT_MAX / sizeof(T);
		while (len > 0) {
			T* tmp = static_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
			// ���ֱ�����뵽�ڴ棬���ؽ��
			if (tmp) return esgstl::pair<T*, ptrdiff_t>(tmp, len);
			// ���������С����
			len /= 2;
		}
		// û���뵽�ڴ�
		return esgstl::pair<T*, ptrdiff_t>(nullptr, 0);
	}

	// Ҫô��ʽ�ṩģ�����
	template <class T>
	esgstl::pair<T*, ptrdiff_t> get_temporary_helper(ptrdiff_t len) {
		return get_buffer_helper(len, static_cast<T*>(0));
	}

	// Ҫô�ṩһ��ָ���������������ж�
	template <class T>
	esgstl::pair<T*, ptrdiff_t> get_temporary_helper(ptrdiff_t len,T*) {
		return get_buffer_helper(len, static_cast<T*>(0));
	}

	// ������ʱ������
	template <class T>
	void release_tmeporary_buffer(T* ptr) {
		free(ptr);
	}

	// ģ����temporary_buffer
	// ���л��������������ͷ�
	/*****************************************************************/
	template <class ForwardIter,class T>
	class temporary_buffer {
	private:
		ptrdiff_t original_len; // ����Ĵ�С
		ptrdiff_t len;			// ʵ�ʴ�С
		T* buffer;				// ָ�򻺳�����ָ��

	public:
		// ���캯��
		temporary_buffer(ForwardIter first, ForwardIter last);

		// ��������
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
		// ˽�еĿ������캯���Ϳ�����ֵ�����,��ֹ����
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

	// ģ����unique_ptr
	// �����ϸ��������Ȩ������ָ��
	/******************************************************************/
	template <class T>
	class unique_ptr {
	public:
		typedef T value_type;
	
	private:
		T* m_ptr; // ָ��ʵ�ʶ����ָ��

	public:
		explicit unique_ptr(T* p = nullptr) :m_ptr(p) {}
		unique_ptr(unique_ptr&& rhs) noexcept : m_ptr(rhs.release()) {}
		template <class U>
		unique_ptr(unique_ptr<U>&& rhs) noexcept : m_ptr(rhs.release()) {}

		// ������ֵ�����
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

		// ��ֹ������ֵ/�������
		unique_ptr(const unique_ptr& rhs) = delete;
		unique_ptr& operator=(const unique_ptr& rhs) = delete;

		explicit operator bool() const noexcept {
			return m_ptr != nullptr;
		}

	public:
		// ���� operator* �� operator->
		T& operator*()  const { return *m_ptr; }
		T* operator->() const { return m_ptr; }
		
		// ��õײ�ԭʼָ��
		T* get() const { return m_ptr; }
		
		// �ͷŶ�m_ptr�Ĺ���Ȩ
		T* release() {
			T* tmp = m_ptr;
			m_ptr = nullptr;
			return tmp;
		}

		// ������Ҫ�����ԭʼָ��
		void reset(T* p = nullptr) {
			if (p != m_ptr) {
				delete m_ptr;
				m_ptr = p;
			}
		}
		
	};
	/******************************************************************/

	// ������
	struct ptr_count {
		int shared_count = 1;
		int weak_count = 0;
	};

	// weak_ptr��������ָ�룬��shared_ptr��ʼ��
	// �����ӹ�����Դ��ָ���������������Դ��shared_ptr�����������ֹ������Դ
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

	// shared_ptr�����ж������ָ��ͬʱָ��һ���ײ���Դ
	// �����һ��ָ����Դ��shared_ptr������ʱ����ɾ����Դ
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
				// ��������operator bool���_dataΪ�ջ��жϳ���
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

	// ����ָ�뾲̬ǿ������ת��
	template <class T, class U>
	shared_ptr<T> static_pointer_cast(shared_ptr<U>& ptr) {
		return shared_ptr<T>(ptr, static_cast<shared_ptr<T>::element_type*>(ptr.get()));
	}

	// ����ָ�붯̬ǿ������ת��
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
