#pragma once

#include "deque.h"

namespace esgstl {
	template <class T, class Container = esgstl::deque<T>>
	class stack {
	public :
		typedef Container						container_type;
		
		typedef Container::value_type			value_type;
		typedef Container::reference			reference;
		typedef Container::size_type			size_type;
		typedef Container::const_reference		const_reference;
		
		static_assert(std::is_same<T, value_type>::value,
			"the value_type of Container should be same with T");

	private:
		container_type c_;

	public:
		// ���졢���ơ��ƶ�����
		stack() = default;

		explicit stack(size_type n) :c_(n) {}
		stack(size_type n, const value_type& value) :c_(n, value) {}

		template <class Iter>
		stack(Iter first, Iter last) :c_(first, last) {}

		stack(std::initializer_list<T> ilist) :c_(ilist.begin(), ilist.end()) {}
		stack(const Container& c) :c_(c) {}
		stack(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
			:c_(esgstl::move(c)) {}

		stack(const stack& rhs) :c_(rhs.c_) {}
		stack(stack&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
			:c_(esgstl::move(rhs.c_)) {}

		stack& operator=(const stack& rhs) {
			c_ = rhs.c_;
			return *this;
		}
		stack& operator=(stack&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value) {
			c_ = esgstl::move(rhs.c_);
			return *this;
		}

		stack& operator=(std::initializer_list<T> ilist) {
			c_ = ilist;
			return *this;
		}

		~stack() = default;

		// ����Ԫ����ز���
		reference       top() { return c_.back(); }
		const_reference top() const { return c_.back(); }

		// ������ز���
		bool      empty() const noexcept { return c_.empty(); }
		size_type size()  const noexcept { return c_.size(); }

		// �޸�������ز���

		template <class... Args>
		void emplace(Args&& ...args) { c_.emplace_back(esgstl::forward<Args>(args)...); }

		void push(const value_type& value) { c_.push_back(value); }
		void push(value_type&& value) { c_.push_back(esgstl::move(value)); }

		void pop() { c_.pop_back();}

		void clear() {
			while (!empty())
				pop();
		}

		void swap(stack& rhs) noexcept(noexcept(esgstl::swap(c_, rhs.c_))) {esgstl::swap(c_, rhs.c_);}

	public:
		bool operator==(const stack& rhs) { return c_ == rhs.c_; }
		bool operator< (const stack& rhs) { return c_ < rhs.c_; }
	};

	// ���رȽϲ�����
	template <class T, class Container>
	bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
		return lhs == rhs;
	}

	template <class T, class Container>
	bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
		return lhs < rhs;
	}

	template <class T, class Container>
	bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
		return !(lhs == rhs);
	}

	template <class T, class Container>
	bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
		return rhs < lhs;
	}

	template <class T, class Container>
	bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
		return !(rhs < lhs);
	}

	template <class T, class Container>
	bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs) {
		return !(lhs < rhs);
	}

	// ���� mystl �� swap
	template <class T, class Container>
	void swap(stack<T, Container>& lhs, stack<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
		lhs.swap(rhs);
	}

}