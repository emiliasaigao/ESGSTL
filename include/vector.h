#pragma once

#include <initializer_list>  // 使用了初始化列表

#include "iterator.h"	// 纳入迭代器体系
#include "memory.h"		// 开辟空间
#include "exceptdef.h"	// 使用了一些报错机制
#include "util.h"	// 使用了move、swap等工具函数

namespace esgstl {

// 取消在别的地方设置的宏
#ifdef max
#pragma message("#undefing marco max")
#undef max
#endif // max

#ifdef min
#pragma message("#undefing marco min")
#undef min
#endif // min

	template <class T>
	class vector {
		static_assert(!std::is_same<bool, T>::value, "vector<bool> is abandoned in esgstl");
	public:
		typedef esgstl::allocator<T>							allocator_type;
		typedef esgstl::allocator<T>							data_allocator;

		typedef typename allocator_type::value_type				value_type;
		typedef typename allocator_type::pointer				pointer;
		typedef typename allocator_type::reference				reference;
		typedef typename allocator_type::const_pointer			const_pointer;
		typedef typename allocator_type::const_reference		const_reference;
		typedef typename allocator_type::size_type				size_type;
		typedef typename allocator_type::difference_type		difference_type;

		typedef value_type*										iterator;
		typedef const value_type*								const_iterator;
		typedef esgstl::reverse_iterator<iterator>				reverse_iterator;
		typedef const esgstl::reverse_iterator<iterator>		const_reverse_iterator;

		// 返回当前vector的allocator对象
		allocator_type get_allocator() { return data_allocator(); }

	private:
		iterator begin_;	// 当前分配的空间的头部
		iterator end_;		// 当前已使用的空间的尾部
		iterator cap_;		// 当前分配的空间的尾部

	public:
		// 赋值、构造、析构函数
		/*****************************************************************/
		vector() noexcept { try_init(); }
		explicit vector(size_type n) { fill_init(n, value_type()); }
		vector(size_type n, const value_type& value) { fill_init(n, value); }

		template <class Iter, typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value, int>::type = 0>
		vector(Iter first, Iter last) {
			ESGSTL_DEBUG(first < last);
			copy_init(first, last);
		}
		vector(const vector& rhs) { copy_init(rhs.begin_, rhs.end_); }

		vector(vector&& rhs) noexcept
			:begin_(rhs.begin_), end_(rhs.end_), cap_(rhs.cap_) {
			rhs.begin_ = nullptr;
			rhs.end_ = nullptr;
			rhs.cap_ = nullptr;
		}

		vector(std::initializer_list<value_type> ilist) { copy_init(ilist.begin(), ilist.end()); }

		vector& operator=(const vector& rhs);
		vector& operator=(vector&& rhs) noexcept;
		vector& operator=(std::initializer_list<value_type> ilist) {
			vector tmp(ilist);
			swap(tmp);
			return *this;
		}

		~vector() {
			destory_and_recover(begin_, end_, cap_ - begin_);
			begin_ = nullptr;
			end_ = nullptr;
			begin_ = nullptr;
		}
		/*****************************************************************/


	public:
		// 迭代器相关操作
		/*****************************************************************/
		iterator begin() noexcept { return begin_; }
		iterator end() noexcept { return end_; }
		const_iterator begin() const noexcept { return begin_; }
		const_iterator end() const noexcept { return end_; }

		const_iterator cbegin() const noexcept { return begin_; }
		const_iterator cend() const noexcept { return end_; }

		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
		/*****************************************************************/

		// 容量相关操作
		/*****************************************************************/
		bool empty() const noexcept { return begin_ == end_; }
		size_type size() const noexcept { return static_cast<size_type>(end_ - begin_); }
		size_type max_size() const noexcept {
			return static_cast<size_type>(-1) / sizeof(T);
		}
		size_type capacity() const noexcept { return static_cast<size_type>(cap_ - begin_); }
		void reserve(size_type n);
		void shrink_to_fit();
		/*****************************************************************/

		// 元素访问相关操作
		/*****************************************************************/
		reference operator[](size_type n) noexcept { 
			ESGSTL_DEBUG(n < size());
			return *(begin_ + n); 
		}
		const_reference operator[](size_type n) const noexcept { 
			ESGSTL_DEBUG(n < size());
			return *(begin_ + n); 
		}

		reference at(size_type n) noexcept {
			ESGSTL_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
			return *(begin_ + n);
		}
		const_reference at(size_type n) const noexcept {
			ESGSTL_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
			return *(begin_ + n);
		}

		reference front() noexcept {
			ESGSTL_DEBUG(!empty());
			return *begin_;
		}
		const_reference front() const noexcept {
			ESGSTL_DEBUG(!empty());
			return *begin_;
		}
		reference back() noexcept {
			ESGSTL_DEBUG(!empty());
			return *(end_ - 1);
		}
		const_reference back() const noexcept {
			ESGSTL_DEBUG(!empty());
			return *(end_ - 1);
		}

		pointer data() noexcept { return begin_; }
		const_pointer data() const noexcept { return begin_; }
		/*****************************************************************/

		// 修改容器相关操作
		/*****************************************************************/
		//// assign:重设容器
		void assign(size_type n, const value_type& value) { fill_assign(n, value); }
		template <class Iter, typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value,int>::type = 0>
		void assign(Iter first, Iter last) {
			copy_assign(first, last);
		}
		void assign(std::initializer_list<value_type> ilist) { copy_assign(ilist.begin(), ilist.end()); }

		//// emplace/ emplace_back
		template <class... Args>
		iterator emplace(const_iterator pos, Args&& ...args);
		template <class ...Args>
		void emplace_back(Args&& ...args);

		//// push_back / pop_back
		void push_back(const value_type& value);
		void push_back(value_type&& value) { emplace_back(esgstl::move(value)); }

		void pop_back();

		//// insert
		iterator insert(const_iterator pos, const value_type& value);
		iterator insert(const_iterator pos, value_type&& value) {
			return emplace(pos, esgstl::move(value));
		}
		iterator insert(const_iterator pos, size_type n, const value_type& value) {
			ESGSTL_DEBUG(pos >= begin() && pos <= end());
			return fill_insert(const_cast<iterator>(pos), n, value);
		}
		template <class Iter, typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value,int>::type = 0>
		void insert(const_iterator pos,Iter first, Iter last) {
			ESGSTL_DEBUG(pos >= begin() && pos <= end() && first < last);
			copy_insert(const_cast<iterator>(pos), first, last);
		}

		//// resize
		void resize(size_type n) { resize(n, value_type()); }
		void resize(size_type n, const value_type& value);

		//// erase/clear
		iterator erase(const_iterator pos);
		iterator erase(const_iterator first, const_iterator last);
		void clear() { erase(begin_, end_); }

		//// 工具函数
		void swap(vector& rhs) noexcept;
		/*****************************************************************/

	private:
		//// 初始化/析构真实逻辑函数
		void try_init() noexcept;
		void init_space(size_type size, size_type cap);
		void fill_init(size_type n, const value_type& value);
		template <class Iter>
		void copy_init(Iter first, Iter last);
		void destory_and_recover(iterator first, iterator last, size_type n);

		
		// shrink_to_fit真实逻辑函数
		void reinsert(size_type n);

		//// 计算所需的新空间大小
		size_type get_new_cap(size_type add_size);

		//// assign真实逻辑函数
		void fill_assign(size_type n, const value_type& value);
		template <class Iter>
		void copy_assign(Iter first, Iter last);

		//// reallocate重新分配空间
		template <class... Args>
		void reallocate_emplace(iterator pos, Args&& ...args);
		void reallocate_insert(iterator pos, const value_type& value);

		//// insert真实逻辑函数
		iterator fill_insert(iterator pos, size_type n, const value_type& value);
		template <class Iter>
		void copy_insert(iterator pos, Iter first, Iter last);
	};

	// 相当于replace
	template <class T>
	vector<T>& vector<T>::operator=(const vector& rhs) {
		if (this != &rhs) {
			copy_assign(rhs.begin_, rhs.end_);
		}
		return *this;
	}

	template <class T>
	vector<T>& vector<T>::operator=(vector&& rhs) noexcept {
		destory_and_recover(begin_, end_, cap_ - begin_);
		begin_ = rhs.begin_;
		end_ = rhs.end_;
		cap_ = rhs.cap_;
		rhs.begin_ = nullptr;
		rhs.end_ = nullptr;
		rhs.cap_ = nullptr;
		return *this;
	}

	template<class T>
	inline void vector<T>::reserve(size_type n) {
		if (capacity() < n) {
			ESGSTL_LENGTH_ERROR_IF(max_size() < n,
				"n can not larger than max_size() in vector<T>::reserve(n)");
			const size_type old_size = size();
			auto tmp = data_allocator::allocate(n);
			esgstl::uninitialized_move(begin_, end_, tmp);
			begin_ = tmp;
			end_ = begin_ + old_size;
			cap_ = begin_ + n;
		}
	}

	template<class T>
	inline void vector<T>::shrink_to_fit() {
		if (end_ < cap_) {
			reinsert(size());
		}
	}

	template<class T>
	inline void vector<T>::resize(size_type n, const value_type& value) {
		if (n > size()){
			insert(end_, n - size(), value);
		}
		else {
			erase(begin_ + n, end_);
			shrink_to_fit();
		}
	}

	template<class T>
	inline vector<T>::iterator vector<T>::erase(const_iterator pos) {
		ESGSTL_DEBUG(pos >= begin() && pos < end());
		// 返回删完元素后，与pos相同位置的迭代器
		iterator xpos = begin_ + (pos - begin_);
		esgstl::move(xpos + 1, end_, xpos);
		data_allocator::destory(end_ - 1);
		--end_;
		return xpos;
	}

	template<class T>
	inline vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
		ESGSTL_DEBUG(first >= begin() && last <= end() && first <= last);
		iterator xpos = begin_ + (first - begin_);
		if (first == last) return xpos;
		const size_type len = static_cast<size_type>(last - first);
		esgstl::move(xpos+len, end_, xpos);
		data_allocator::destory(end_ - len, end_);
		end_ = end_ - len;
		return xpos;
	}

	template<class T>
	inline void vector<T>::try_init() noexcept {
		try {
			begin_ = data_allocator::allocate(16);
			end_ = begin_;
			cap_ = begin_ + 16;
		}
		catch (const std::exception&) {
			begin_ = nullptr;
			end_ = nullptr;
			cap_ = nullptr;
		}
	}

	template<class T>
	inline void vector<T>::init_space(size_type size, size_type cap) {
		try {
			begin_ = data_allocator::allocate(cap);
			end_ = begin_ + size;
			cap_ = begin_ + cap;
		}
		catch (...) {
			begin_ = nullptr;
			end_ = nullptr;
			cap_ = nullptr;
			throw;
		}
	}
	
	template<class T>
	inline void vector<T>::fill_init(size_type n, const value_type& value) {
		const size_type init_size = esgstl::max(static_cast<size_type>(16), n);
		init_space(n, init_size);
		esgstl::uninitialized_fill(begin_, end_, value);
	}
	
	template<class T>
	template<class ...Args>
	inline vector<T>::iterator vector<T>::emplace(const_iterator pos, Args&& ...args) {
		ESGSTL_DEBUG(pos >= begin_ && pos <= end_);
		const size_type dis = static_cast<size_type>(pos - begin_);
		iterator xpos = begin_ + dis;
		if (pos == end_ && end_ != cap_) {
			data_allocator::construct(esgstl::address_of(*end_), esgstl::forward<Args>(args)...);
			++end_;
		}
		else if (end_ != cap_) {
			auto old_end = end_;
			data_allocator::construct(end_);
			++end_;
			esgstl::copy_backward(xpos, old_end, end_);
			*xpos = value_type(esgstl::forward<Args>(args)...);
		}
		else {
			reallocate_emplace(xpos, esgstl::forward<Args>(args)...);
		}
		return begin_ + dis;
	}

	template<class T>
	template<class ...Args>
	inline void vector<T>::emplace_back(Args && ...args) {
		if (end_ == cap_) {
			reallocate_emplace(end_, esgstl::forward<Args>(args)...);
		}
		else {
			*end_ = value_type(esgstl::forward<Args>(args)...);
			++end_;
		}
	}

	template <class T>
	inline void vector<T>::push_back(const value_type& value) {
		if (end_ == cap_) {
			reallocate_insert(end_, value);
		}
		else {
			data_allocator::construct(end_, value);
			++end_;
		}
	}

	template <class T>
	inline void vector<T>::pop_back() {
		ESGSTL_DEBUG(!empty());
		data_allocator::destory(end_ - 1);
		--end_;
	}

	template<class T>
	inline vector<T>::iterator 
	vector<T>::insert(const_iterator pos, const value_type& value) {
		ESGSTL_DEBUG(pos >= begin_ && pos <= end_);
		const size_type dis = static_cast<size_type>(pos - begin_);
		iterator xpos = begin_ + dis;
		if (pos == end_ && end_ != cap_) {
			data_allocator::construct(end_,value);
			++end_;
		}
		else if (end_ != cap_) {
			auto old_end = end_;
			data_allocator::construct(end_);
			++end_;
			esgstl::copy_backward(xpos, old_end, end_);
			*xpos = value_type(value);
		}
		else {
			reallocate_insert(xpos, value);
		}
		return begin_ + dis;
	}

	template<class T>
	template<class Iter>
	inline void vector<T>::copy_init(Iter first, Iter last) {
		const size_type len = static_cast<size_type>(esgstl::distance(first, last));
		const size_type init_size = esgstl::max(static_cast<size_type>(16), len);
		init_space(len, init_size);
		esgstl::uninitialized_copy_n(first, len, begin_);
	}

	template<class T>
	template<class Iter>
	inline void vector<T>::copy_assign(Iter first, Iter last) {
		auto cur = begin_;
		for (; first != last && cur != end_; ++first, ++cur) {
			*cur = *first;
		}
		if (first == last) {
			erase(cur, end_);
		}
		else {
			insert(end_, first, last);
		}
	}

	template<class T>
	template<class ...Args>
	inline void vector<T>::reallocate_emplace(iterator pos, Args && ...args) {
		const size_type newsize = get_new_cap(1);
		auto newbeg = data_allocator::allocate(newsize);
		auto newend = newbeg;
		try {
			auto e1 = esgstl::uninitialized_copy(begin_, pos, newbeg);
			data_allocator::construct(e1, esgstl::forward<Args>(args)...);
			newend = esgstl::uninitialized_copy(pos, end_, e1 + 1);
			
		}
		catch (const std::exception&) {
			data_allocator::deallocate(newbeg);
			throw;
		}
		destory_and_recover(begin_, end_, cap_ - begin_);
		begin_ = newbeg;
		end_ = newend;
		cap_ = begin_ + newsize;
	}

	template<class T>
	template<class Iter>
	inline void vector<T>::copy_insert(iterator pos, Iter first, Iter last) {
		const size_type n = static_cast<size_type>(esgstl::distance(first, last));
		size_type dis = static_cast<size_type>(pos - begin_);
		if (size() > capacity() - n) {
			size_type newcap = get_new_cap(n);
			auto newbeg = data_allocator::allocate(newcap);
			auto newend = newbeg;
			try {
				newend = esgstl::uninitialized_move(begin_, pos, newbeg);
				newend = esgstl::uninitialized_copy_n(first, n, newend);
				newend = esgstl::uninitialized_move(pos, end_, newend);
			}
			catch (...) {
				data_allocator::deallocate(newbeg);
				throw;
			}
			destory_and_recover(begin_, end_, cap_ - begin_);
			begin_ = newbeg;
			end_ = newend;
			cap_ = begin_ + newcap;
		}
		else {
			const size_type after_elems = end_ - pos;
			auto old_end = end_;
			if (after_elems > n) {
				// 扩充的空间长度小于pos后面元素的个数，说明扩充的空间全是pos后面的元素
				esgstl::uninitialized_copy(end_ - n, end_, end_);
				end_ += n;
				esgstl::move_backward(pos, old_end - n, old_end);
				esgstl::uninitialized_copy_n(first, n, end_);
			}
			else {
				// 扩充的空间长度小于pos后面元素的个数，因此新扩充的空间前面全是value
				auto mid = first;
				esgstl::advance(mid, n - after_elems);
				end_ = esgstl::uninitialized_copy_n(mid,n - after_elems,old_end);
				end_ = esgstl::uninitialized_move(pos, old_end, end_);
				esgstl::uninitialized_copy_n(first, after_elems, pos);
			}
		}
	}


	template<class T>
	inline void vector<T>::
		destory_and_recover(iterator first, iterator last, size_type n) {
		data_allocator::destory(first, last);
		data_allocator::deallocate(first);
	}

	template<class T>
	inline void vector<T>::swap(vector& rhs) noexcept {
		if (this != &rhs) {
			esgstl::swap(begin_, rhs.begin_);
			esgstl::swap(end_, rhs.end_);
			esgstl::swap(cap_, rhs.cap_);
		}
	}

	template<class T>
	inline void vector<T>::reinsert(size_type n) {
		auto tmp = data_allocator::allocate(n);
		try {
			esgstl::uninitialized_move(begin_, end_, tmp);
		}
		catch (...) {
			data_allocator::deallocate(tmp);
		}
		destory_and_recover(begin_, end_, cap_ - begin_);
		begin_ = tmp;
		cap_ = end_ = tmp + n;
	}

	template<class T>
	inline vector<T>::size_type vector<T>::get_new_cap(size_type add_size) {
		const auto old_size = capacity();
		ESGSTL_LENGTH_ERROR_IF(old_size > max_size() - add_size,
			"vector<T>'s size too big");
		if (old_size > max_size() - old_size / 2) {
			return old_size > max_size() - old_size / 2 - 16 ?
				old_size + add_size : old_size + add_size + 16;
		}
		return old_size == 0 ?
			esgstl::max(static_cast<size_type>(16), add_size) :
			esgstl::max(old_size + old_size / 2, old_size + add_size);
	}

	template<class T>
	inline void vector<T>::fill_assign(size_type n, const value_type& value) {
		if (n < size()) {
			erase(esgstl::fill_n(begin_, n, value),end_);
		}
		else if (n < capacity()){
			esgstl::fill(begin_, end_, value);
			end_ = esgstl::uninitialized_fill_n(end_, n - size(), value);
		}
		else {
			vector tmp(n, value);
			swap(tmp);
		}
	}

	template<class T>
	inline void vector<T>::reallocate_insert(iterator pos, const value_type& value) {
		const size_type newsize = get_new_cap(1);
		auto newbeg = data_allocator::allocate(newsize);
		auto newend = newbeg;
		try {
			auto e1 = esgstl::uninitialized_copy(begin_, pos, newbeg);
			data_allocator::construct(e1, value);
			newend = esgstl::uninitialized_copy(pos, end_, e1 + 1);

		}
		catch (const std::exception&) {
			data_allocator::deallocate(newbeg);
			throw;
		}
		destory_and_recover(begin_, end_, cap_ - begin_);
		begin_ = newbeg;
		end_ = newend;
		cap_ = begin_ + newsize;
	}

	template<class T>
	inline vector<T>::iterator 
	vector<T>::fill_insert(iterator pos, size_type n, const value_type& value) {
		if (n == 0) return pos;
		size_type dis = static_cast<size_type>(pos - begin_);
		if (size() > capacity() - n) {
			size_type newcap = get_new_cap(n);
			auto newbeg = data_allocator::allocate(newcap);
			auto newend = newbeg;
			try {
				newend = esgstl::uninitialized_move(begin_, pos, newbeg);
				newend = esgstl::uninitialized_fill_n(newend, n, value);
				newend = esgstl::uninitialized_move(pos, end_, newend);
			}
			catch (...) {
				data_allocator::deallocate(newbeg);
				throw;
			}
			destory_and_recover(begin_, end_, cap_ - begin_);
			begin_ = newbeg;
			end_ = newend;
			cap_ = begin_ + newcap;
		}
		else {
			const size_type after_elems = end_ - pos;
			auto old_end = end_;
			if (after_elems > n) {
				// 扩充的空间长度小于pos后面元素的个数，说明扩充的空间全是pos后面的元素
				esgstl::uninitialized_copy(end_ - n, end_, end_);
				end_ += n;
				esgstl::move_backward(pos, old_end - n, old_end);
				esgstl::uninitialized_fill_n(pos, n, value);
			}
			else {
				// 扩充的空间长度小于pos后面元素的个数，因此新扩充的空间前面全是value
				end_ = esgstl::uninitialized_fill_n(old_end, n - after_elems, value);
				end_ = esgstl::uninitialized_move(pos, old_end, end_);
				esgstl::uninitialized_fill_n(pos, after_elems, value);
			}
		}
		return begin_ + dis;
	}

	// 重载的类外比较运算符
	template <class T>
	bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
		return lhs.size() == rhs.size() &&
			esgstl::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template <class T>
	bool operator<(const vector<T>& lhs, const vector<T>& rhs) {
		return esgstl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), lhs.end());
	}

	template <class T>
	bool operator!=(const vector<T>& lhs, const vector<T>& rhs) {
		return !(lhs == rhs);
	}

	template <class T>
	bool operator>(const vector<T>& lhs, const vector<T>& rhs) {
		return rhs < lhs;
	}

	template <class T>
	bool operator<=(const vector<T>& lhs, const vector<T>& rhs) {
		return !(rhs < lhs);
	}

	template <class T>
	bool operator>=(const vector<T>& lhs, const vector<T>& rhs) {
		return !(lhs < rhs);
	}

	// 重载 esgstl 的 swap
	template <class T>
	void swap(vector<T>& lhs, vector<T>& rhs) {
		lhs.swap(rhs);
	}

}