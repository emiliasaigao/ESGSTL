#pragma once
#include <cstddef>
#include <cstdlib>
#include <climits>

// ���ͷ�ļ�������߼��Ķ�̬�ڴ����
// ����һЩ�����������ռ���������δ��ʼ���Ĵ���ռ�����Լ�����ָ�� auto_ptr �� shared_ptr

#include "algobase.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"
#include "util.h"


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

	// ģ����auto_ptr
	// �����ϸ��������Ȩ������ָ��
	/******************************************************************/
	template <class T>
	class auto_ptr {
	public:
		typedef T value_type;
	
	private:
		T* m_ptr; // ָ��ʵ�ʶ����ָ��

	public:
		explicit auto_ptr(T* p = nullptr) :m_ptr(p) {}
		auto_ptr(auto_ptr& rhs) : m_ptr(rhs.release()) {}
		template <class U>
		auto_ptr(auto_ptr<U>& rhs) : m_ptr(rhs.release()) {}

		// ������ֵ�����
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

	// shared_ptr����ָ�������
	// shared_ptr�����ж������ָ��ͬʱָ��һ���ײ���Դ
	// �����һ��ָ����Դ��shared_ptr������ʱ����ɾ����Դ
	/******************************************************************/
	class shared_ptr_count{
	private:
		long* ptrcnt;
		
	public:
		// ���캯��
		shared_ptr_count():ptrcnt(nullptr){}
		shared_ptr_count(const shared_ptr_count& rhs) :ptrcnt(rhs.ptrcnt) {}

		void swap(shared_ptr_count& rhs) { esgstl::swap(ptrcnt, rhs.ptrcnt); }
		long use_count() const { return ptrcnt == nullptr ? 0 : *ptrcnt; }

		// ��ȡ/����ָ�������Ȩ����ʼ��/�������ü�����
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

		// �ͷ�pxָ�������Ȩ�����ʵ���ʱ�����ٶ���
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

	// shared_ptr_base ����ָ�����
	class shared_ptr_base {
	protected:
		shared_ptr_count spc;

		shared_ptr_base():spc(){}
		shared_ptr_base(const shared_ptr_base& rhs) :spc(rhs.spc) {}

	};

	// ����ָ��shared_ptr
	template <class T>
	class shared_ptr : public shared_ptr_base {
	public:
		typedef T element_type;
	private:
		// ָ�򱻹���ռ��ָ��
		T* base_ptr;
	public:
		// ���캯��
		explicit shared_ptr(T* ptr) :shared_ptr_base() {
			acquire(ptr);
		}

		shared_ptr(const shared_ptr& rhs) :shared_ptr_base(rhs) {
			acquire(rhs.base_ptr);
		}

		// ���������ֻ��������ָ������ǿת����ֻ��static_pointer_cast��ȫ�ֺ�������ʹ��
		template<class U>
		shared_ptr(const shared_ptr<U>& rhs,T* p) : shared_ptr_base(rhs) {
			acquire(p);
		}

		template <class U>
		shared_ptr(const shared_ptr<U>& rhs) : shared_ptr_base(rhs) {
			acquire(static_cast<shared_ptr<T>::element_type*>(rhs.get()));
		}

		// ������ֵ�����,ʹ���˿����뽻���ַ�
		shared_ptr& operator=(shared_ptr rhs) {
			// rhs����ʱ���󣬿����˴�������ʵ��
			swap(rhs);
			return *this;
		}

		~shared_ptr() { release(); }
		// ����ָ�룬�����������ֱ���ͷŹ���Ȩ
		void reset() { release(); }
		// ����ָ��Ϊ�����ԭʼָ��
		void reset(T* ptr) {
			release();
			acquire(ptr);
		}

		// ��ʽ����ת��Ϊbool��ֻ��if�����ᱻ��ʽִ��
		explicit operator bool() { return spc.use_count() > 0; }
		// �����Ƿ��ǵײ�ָ��Ψһ�Ĺ�����
		bool unique() { return spc.use_count() == 1; }
		// ���ص�ǰ�ײ�ָ�뱻���ٹ�����ͬʱ����
		long use_count() { return spc.use_count(); }

		T& operator*() { return *base_ptr; }
		T* operator->() { return base_ptr; }
		
		T* get() const { return base_ptr; }
		void swap(shared_ptr& rhs) {
			esgstl::swap(base_ptr, rhs.base_ptr);
			spc.swap(rhs.spc);
		}

	private:
		// �����������͸�ֵ�ײ�ָ��
		void acquire(T* ptr) {
			// ����share_ptr_base�е�share_ptr_count ��acquire
			// ��ʼ���������Ӽ���
			spc.acquire(ptr);
			base_ptr = ptr;
		}
		
		// �����ͷŹ���Ȩ�����ټ��������õײ�ָ��Ϊnullptr
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
