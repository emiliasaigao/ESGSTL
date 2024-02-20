#pragma once

#include<initializer_list> // 引入初始化列表，接受{元素集}初始化

#include"iterator.h"	// 使用了difference_type等返回迭代器属性的方法
#include"exceptdef.h"	// 使用了一些报错机制
#include"memory.h"		// 分配中控器，buffer的内存
#include"util.h"		// 使用了swap、move等工具函数

namespace esgstl {

#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif // !DEQUE_MAP_INIT_SIZE

	template <class T>
	struct deque_buf_size {
		static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;
	};

	template <class T, class Ref, class Ptr>
	struct deque_iterator {
		typedef deque_iterator<T, T&, T*>					iterator;
		typedef deque_iterator<T, const T&, const T*>		const_iterator;
		typedef deque_iterator<T, Ref, Ptr>					self;

		typedef size_t										size_type;
		typedef esgstl::random_access_iterator_tag			iterator_category;
		typedef T											value_type;
		typedef Ptr											pointer;
		typedef Ref											reference;
		typedef ptrdiff_t									difference_type;

		typedef T*		value_pointer;
		typedef T**		map_pointer;

		static const size_type buff_size = deque_buf_size<T>::value;
	
		value_pointer cur;
		value_pointer first;
		value_pointer last;
		map_pointer map_node;

		deque_iterator() noexcept :cur(nullptr),first(nullptr),last(nullptr), map_node(nullptr){}
		deque_iterator(value_pointer c, map_pointer m) noexcept 
			:cur(c),first(*m),last(*m + buff_size), map_node(m){}
		deque_iterator(const const_iterator& rhs) noexcept 
			:cur(rhs.cur),first(rhs.first),last(rhs.last), map_node(rhs.map_node){}
		deque_iterator(const iterator& rhs)
			:cur(rhs.cur), first(rhs.first), last(rhs.last), map_node(rhs.map_node) {}
		deque_iterator(iterator&& rhs) noexcept
			:cur(rhs.cur), first(rhs.first), last(rhs.last), map_node(rhs.map_node) {
			rhs.cur = nullptr;
			rhs.first = nullptr;
			rhs.last = nullptr;
			rhs.map_node = nullptr;
		}

		self& operator=(const iterator& rhs) {
			if (this != &rhs) {
				cur = rhs.cur;
				first = rhs.first;
				last = rhs.last;
				map_node = rhs.map_node;
			}
			return *this;
		}

		self& operator=(const const_iterator& rhs) {
			cur = rhs.cur;
			first = rhs.first;
			last = rhs.last;
			map_node = rhs.map_node;
			return *this;
		}

		void set_node(map_pointer newnode) {
			map_node = newnode;
			first = *map_node;
			last = first + buff_size;
		}

		reference operator*()  const { return *cur; }
		pointer   operator->() const { return cur; }

		self& operator++() {
			++cur;
			if (cur == last) {
				set_node(map_node + 1);
				cur = first;
			}
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			++(*this);
			return tmp;
		}

		self& operator--() {
			if (cur == first) {
				set_node(map_node - 1);
				cur = last;
			}
			--cur;
			return *this;
		}
		self operator--(int) {
			self tmp = *this;
			--(*this);
			return tmp;
		}

		self& operator+=(difference_type n) {
			difference_type offset = n + (cur - first);
			if (offset > 0 && offset < difference_type(buff_size)) {
				cur += n;
			}
			else {
				difference_type node_offset = offset > 0 ? offset / difference_type(buff_size)
					: -difference_type((-offset - 1) / buff_size + 1);
				set_node(map_node + node_offset);
				cur = first + offset - node_offset * difference_type(buff_size);
			}
			return *this;
		}
		self& operator-=(difference_type n) { return (*this) += -n; }
		
		self operator+(difference_type n) {
			self tmp = *this;
			tmp += n;
			return tmp;
		}
		self operator-(difference_type n) { 
			self tmp = *this;
			tmp -= n;
			return tmp;
		}

		difference_type operator-(const self& rhs) const {
			return difference_type(buff_size) * (map_node - rhs.map_node - 1) + (cur - first) 
				+ (rhs.last - rhs.cur);
		}

		bool operator==(const self& rhs) const { return cur == rhs.cur; }
		bool operator!=(const self& rhs) const { return !(*this == rhs); }
		bool operator<(const self& rhs) const {
			return map_node == rhs.map_node ? cur < rhs.cur
				: map_node < rhs.map_node;
		}
		bool operator> (const self& rhs) const { return rhs < *this; }
		bool operator<=(const self& rhs) const { return !(rhs < *this); }
		bool operator>=(const self& rhs) const { return !(*this < rhs); }
		reference operator[](difference_type n) { return *(*this + n); }
	};

	template <class T>
	class deque {
	public:
		typedef esgstl::allocator<T>			allocator_type;
		typedef esgstl::allocator<T>			data_allocator;
		typedef esgstl::allocator<T*>			map_allocator;

		typedef allocator_type::value_type						value_type;
		typedef allocator_type::pointer							pointer;
		typedef allocator_type::reference						reference;
		typedef allocator_type::const_pointer					const_pointer;
		typedef allocator_type::const_reference					const_reference;
		typedef allocator_type::size_type						size_type;
		typedef allocator_type::difference_type					difference_type;
		typedef pointer*										map_pointer;
		typedef const_pointer*									const_map_pointer;

		typedef deque_iterator<T, T&, T*>						iterator;
		typedef deque_iterator<T, const T&, const T*>			const_iterator;
		typedef esgstl::reverse_iterator<iterator>				reverse_iterator;
		typedef esgstl::reverse_iterator<const_iterator>		const_reverse_iterator;

		allocator_type get_allocator() { return allocator_type(); }

		static const size_t buff_size = deque_buf_size<T>::value;

	private:
		iterator begin_; // 指向第一个元素节点
		iterator end_; // 指向尾节点,尾节点没有元素
		map_pointer map_; // 中控器的头部，中控器里都是指向数组的指针
		size_type map_size_; // 中控器的尺寸

	public:
		deque() { fill_init(0, value_type()); }
		explicit deque(size_type n) { fill_init(n, value_type()); }
		deque(size_type n, const value_type& value) { fill_init(n, value); }

		template <class Iter, typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value, int>::type = 0>
		deque(Iter first, Iter last) {
			ESGSTL_DEBUG(first < last);
			copy_init(first, last);
		}
		deque(std::initializer_list<T> ilist) {
			copy_init(ilist.begin(), ilist.end());
		}
		deque(const deque& rhs) { copy_init(rhs.begin_, rhs.end_); }

		deque(deque&& rhs) noexcept 
			:begin_(esgstl::move(rhs.begin_)),
			end_(esgstl::move(rhs.end_)),
			map_(rhs.map_),
			map_size_(rhs.map_size_) {
			rhs.begin_ = iterator();
			rhs.end_ = iterator();
			rhs.map_ = nullptr;
			rhs.map_size_ = 0;
		}

		deque& operator=(const deque& rhs);
		deque& operator=(deque&& rhs);

		deque& operator=(std::initializer_list<T> ilist) {
			deque tmp(ilist);
			swap(tmp);
			return *this;
		}

		~deque() {
			if (map_ != nullptr) {
				clear();
				data_allocator::deallocate(*(begin_.map_node));
				*(begin_.map_node) = nullptr;
				map_allocator::deallocate(map_);
				map_ = nullptr;
			}
		}

	public:
		// 迭代器相关操作
		/*****************************************************************/
		iterator begin() noexcept { return begin_; }
		iterator end() noexcept { return end_; }
		const_iterator begin() const noexcept { return begin_; }
		const_iterator end() const noexcept { return end_; }

		const_iterator cbegin() const noexcept { return begin(); }
		const_iterator cend() const noexcept { return end(); }

		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		reverse_iterator rend()	noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }
		const_reverse_iterator rend()	const noexcept { return reverse_iterator(begin()); }

		const_reverse_iterator crbegin() const noexcept { return rbegin(); }
		const_reverse_iterator crend() const noexcept { return rend(); }
		/*****************************************************************/

		// 容量相关操作
		/*****************************************************************/
		bool empty() const noexcept { return begin_ == end_; }
		size_type size() const noexcept { return static_cast<size_type>(end_ - begin_); }
		size_type max_size() const noexcept {
			return static_cast<size_type>(-1) / sizeof(T);
		}
		void shrink_to_fit() noexcept;
		/*****************************************************************/

		// 元素访问相关操作
		/*****************************************************************/
		reference operator[](size_type n) noexcept {
			ESGSTL_DEBUG(n < size());
			return begin_[n];
		}
		const_reference operator[](size_type n) const noexcept {
			ESGSTL_DEBUG(n < size());
			return begin_[n];
		}

		reference at(size_type n) noexcept {
			ESGSTL_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
			return begin_[n];
		}
		const_reference at(size_type n) const noexcept {
			ESGSTL_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
			return begin_[n];
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
		/*****************************************************************/

		// 修改容器相关操作
		/*****************************************************************/
		//// assign:重设容器
		void assign(size_type n, const value_type& value) { fill_assign(n, value); }
		template <class Iter, typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value, int>::type = 0>
		void assign(Iter first, Iter last) {
			copy_assign(first, last);
		}
		void assign(std::initializer_list<value_type> ilist) { copy_assign(ilist.begin(), ilist.end()); }

		//// emplace/ emplace_back
		template <class... Args>
		iterator emplace(const_iterator pos, Args&& ...args);
		template <class ...Args>
		void emplace_front(Args&& ...args);
		template <class ...Args>
		void emplace_back(Args&& ...args);

		//// push_back / push_front
		void push_back(const value_type& value);
		void push_back(value_type&& value) { emplace_back(esgstl::move(value)); }
		void push_front(const value_type& value);
		void push_front(value_type&& value) { emplace_front(esgstl::move(value)); }

		void pop_front();
		void pop_back();

		//// insert
		iterator insert(const_iterator pos, const value_type& value);
		iterator insert(const_iterator pos, value_type&& value) {
			return emplace(pos, esgstl::move(value));
		}
		iterator insert(const_iterator pos, size_type n, const value_type& value) {
			ESGSTL_DEBUG(pos >= begin() && pos <= end());
			return fill_insert(pos, n, value);
		}
		template <class Iter, typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value, int>::type = 0>
		iterator insert(const_iterator pos, Iter first, Iter last) {
			ESGSTL_DEBUG(pos >= begin() && pos <= end() && first < last);
			return copy_insert(pos, first, last);
		}

		//// resize
		void resize(size_type n) { resize(n, value_type()); }
		void resize(size_type n, const value_type& value);

		//// erase/clear
		iterator erase(const_iterator pos);
		iterator erase(const_iterator first, const_iterator last);
		void clear();
		
		void swap(deque& rhs) noexcept;

		/*****************************************************************/
	private:
		// 初始化相关函数
		map_pointer create_map(size_type map_size);
		void create_buffer(map_pointer start, map_pointer finish);
		void map_init(size_type count);
		void fill_init(size_type n, const value_type& value);
		template<class Iter>
		void copy_init(Iter first, Iter last);
		void destory_buffer(map_pointer start, map_pointer finish);

		// assign真实逻辑函数
		void fill_assign(size_type n, const value_type& value);
		template <class Iter>
		void copy_assign(Iter first, Iter last);

		// insert真实逻辑函数
		template <class ...Args>
		iterator insert_aux(const_iterator pos, Args&& ...args);
		iterator fill_insert(const_iterator pos, size_type n, const value_type& value);
		template <class Iter>
		iterator copy_insert(const_iterator pos, Iter first, Iter last);

		// reallocate:重新开辟空间函数
		void require_capacity(size_type n, bool front);
		void reallocate_map_at_front(size_type need_node_size);
		void reallocate_map_at_back(size_type need_node_size);

	};

	template<class T>
	inline deque<T>& deque<T>::operator=(const deque& rhs) {
		if (this != &rhs) {
			copy_assign(rhs.begin(), rhs.end());
		}
		return *this;
	}

	template<class T>
	inline deque<T>& deque<T>::operator=(deque&& rhs) {
		deque tmp(rhs);
		swap(tmp);
		return *this;
	}

	template<class T>
	inline void deque<T>::push_back(const value_type& value) {
		if (end_.cur != end_.last - 1) {
			data_allocator::construct(end_.cur, value);
			++end_;
		}
		else {
			require_capacity(1, false);
			data_allocator::construct(end_.cur, value);
			++end_;
		}
	}

	template<class T>
	inline void deque<T>::push_front(const value_type& value) {
		if (begin_.cur != begin_.first) {
			--begin_;
			data_allocator::construct(begin_.cur, value);
		}
		else {
			require_capacity(1, true);
			try {
				--begin_;
				data_allocator::construct(begin_.cur, value);
			}
			catch (...) {
				++begin_;
				throw;
			}
		}
	}

	template<class T>
	inline void deque<T>::pop_front() {
		ESGSTL_DEBUG(!empty());
		if (begin_.cur == begin_.last - 1) {
			data_allocator::destory(begin_.cur);
			++begin_;
			destory_buffer(begin_.map_node - 1, begin_.map_node - 1);
		}
		else {
			data_allocator::destory(begin_.cur);
			++begin_;
		}
	}

	template<class T>
	inline void deque<T>::pop_back() {
		ESGSTL_DEBUG(!empty());
		if (end_.cur == end_.first) {
			data_allocator::destory(end_.cur);
			--end_;
			destory_buffer(end_.map_node + 1, end_.map_node + 1);
		}
		else {
			data_allocator::destory(end_.cur);
			--end_;
		}
	}

	template<class T>
	inline deque<T>::iterator 
	deque<T>::insert(const_iterator pos, const value_type& value) {
		if (pos.cur == begin_.cur) {
			push_front(value);
			return begin_;
		}
		else if (pos.cur == end_.cur) {
			push_back(value);
			return end_;
		}
		else {
			insert_aux(pos, value);
		}
	}

	template<class T>
	inline void deque<T>::resize(size_type n, const value_type& value) {
		if (n > size()) {
			insert(end_,n - size(), value);
		}
		else {
			erase(begin_ + n, end_);
		}
		shrink_to_fit();
	}

	template<class T>
	inline void deque<T>::clear() {
		// 把首尾列中间的列元素先destory
		for (auto i = begin_.map_node + 1; i < end_.map_node; ++i) {
			data_allocator::destory(*i, *i + buff_size);
		}
		// 若首尾列不为同一列
		if (begin_.map_node != end_.map_node) {
			data_allocator::destory(begin_.cur, begin_.last);
			data_allocator::destory(end_.first, end_.cur);
		}
		else {
			data_allocator::destory(begin_.cur, end_.cur);
		}
		end_ = begin_;
		// 回收空间
		shrink_to_fit();
	}

	template<class T>
	inline deque<T>::iterator deque<T>::erase(const_iterator pos) {
		ESGSTL_DEBUG(pos >= begin_ && pos < end_);
		const auto elems_before = pos - begin_;
		iterator p = pos;
		if (elems_before < size() / 2) {
			esgstl::copy_backward(begin_,p, p+1);
			pop_front();
		}
		else {
			esgstl::copy(p + 1, end_, p);
			pop_back();
		}
	}

	template<class T>
	inline deque<T>::iterator 
	deque<T>::erase(const_iterator first, const_iterator last) {
		ESGSTL_DEBUG(first >= begin() && last <= end() && first < last);
		if (first.cur == begin_.cur && last.cur == end_.cur) {
			clear();
			return begin_;
		}
		else {
			const size_type len = last - first;
			const size_type elems_before = first - begin_;
			iterator f = first;
			iterator l = last;
			if (elems_before < (size() - len) / 2) {
				iterator b = begin_ + len;
				esgstl::copy_backward(begin_, f, l);
				// 因为不清楚begin_和b之间有没有跨越列，因此只能使用针对迭代器区间的destory
				esgstl::destory(begin_, b);
				begin_ = b;
			}
			else {
				iterator e = end_ - len;
				esgstl::copy(l, e, f);
				esgstl::destory(e, end_);
				end_ = e;
			}
			return f;
		}
	}

	template<class T>
	inline deque<T>::map_pointer deque<T>::create_map(size_type map_size) {
		map_pointer p = nullptr;
		p = map_allocator::allocate(map_size);
		for (size_type i = 0; i < map_size; ++i)
			*(p + i) = nullptr;
		return p;
	}

	template<class T>
	inline void deque<T>::create_buffer(map_pointer start, map_pointer finish) {
		map_pointer p = start;
		try {
			while (p <= finish) {
				*p = data_allocator::allocate(buff_size);
				++p;
			}
		}
		catch (...) {
			while (start <= p) {
				data_allocator::deallocate(*start);
				*start = nullptr;
				++start;
			}
			throw;
		}
	}

	template<class T>
	inline void deque<T>::destory_buffer(map_pointer start, map_pointer finish) {
		while (start <= finish) {
			data_allocator::deallocate(*start);
			*start = nullptr;
			++start;
		}
	}

	template<class T>
	inline void deque<T>::map_init(size_type count) {
		const size_type node_num = count / buff_size + 1;
		map_size_ = esgstl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), node_num + 2);
		try {
			map_ = create_map(map_size_);
		}
		catch (...) {
			map_ = nullptr;
			map_size_ = 0;
			throw;
		}

		map_pointer map_start = map_ + (map_size_ - node_num) / 2;
		map_pointer map_finish = map_start + node_num - 1;

		try {
			create_buffer(map_start, map_finish);
		}
		catch (...) {
			map_allocator::deallocate(map_);
			map_ = nullptr;
			map_size_ = 0;
			throw;
		}
		begin_.set_node(map_start);
		begin_.cur = begin_.first;
		end_.set_node(map_finish);
		end_.cur = end_.first + (count % buff_size);
	}

	template<class T>
	inline void deque<T>::fill_init(size_type n, const value_type& value) {
		map_init(n);
		// 先fill begin_到end_所指map_node区间的所有元素
		for (auto i = begin_.map_node; i < end_.map_node; ++i)
			esgstl::uninitialized_fill_n(*i, buff_size, value);
		// 再单独 fill end_所指map_node这一条数组的元素
		esgstl::uninitialized_fill(end_.first, end_.cur, value);
	}

	template<class T>
	inline void deque<T>::swap(deque& rhs) noexcept {
		esgstl::swap(begin_, rhs.begin_);
		esgstl::swap(end_, rhs.end_);
		esgstl::swap(map_, rhs.map_);
		esgstl::swap(map_size_, rhs.map_size_);
	}

	template<class T>
	inline void deque<T>::fill_assign(size_type n, const value_type& value) {
		auto cur = begin_;
		for (; n > 0 && cur != end_;) {
			*cur = value;
			--n; ++cur;
		}
		if (n == 0) {
			erase(cur, end_);
		}
		else {
			insert(end_, n,value);
		}
	}

	template<class T>
	inline deque<T>::iterator 
	deque<T>::fill_insert(const_iterator pos, size_type n, const value_type& value) {
		const auto elems_before = pos - begin_;
		iterator p1;
		if (elems_before < size() / 2) {
			require_capacity(n, true);
			auto b1 = begin_;
			p1 = begin_ + elems_before;
			auto b2 = b1 - n;
			try {
				begin_ = b2;
				if (n > elems_before) {
					auto i1 = esgstl::uninitialized_copy(b1, p1, b2);
					esgstl::uninitialized_fill(i1, b1, value);
					esgstl::fill(b1, p1, value);
				}
				else {
					auto i1 = esgstl::uninitialized_copy_n(b1, n, b2);
					esgstl::copy_n(p1 - (elems_before - n), elems_before - n, i1);
					esgstl::fill(p1 - n, p1, value);
				}
			}
			catch (...) {
				if (b1.map_node != begin_.map_node) {
					destory_buffer(begin_.map_node, b1.map_node - 1);
				}
				begin_ = b1;
				throw;
			}
		}
		else {
			require_capacity(n, false);
			auto e1 = end_;
			p1 = begin_ + elems_before;
			auto e2 = e1 + n;
			auto elems_after = end_ - pos;
			try {
				end_ = e2;
				if (n > elems_after) {
					auto i1 = e2 - elems_after;
					esgstl::uninitialized_copy(p1, e1, i1);
					esgstl::uninitialized_fill(e1, i1, value);
					esgstl::fill(p1, e1, value);
				}
				else {
					auto i1 = e1 - n;
					auto i2 = e2 - n;
					esgstl::uninitialized_copy_n(i1, n, i2);
					auto i3 = esgstl::copy_backward(p1,i1,e1);
					esgstl::fill(p1, i3, value);
				}
			}
			catch (...) {
				if (e1.map_node != end_.map_node) {
					destory_buffer(e1.map_node + 1, end_.map_node);
				}
				end_ = e1;
				throw;
			}
		}
		return p1;
	}

	template<class T>
	inline void deque<T>::require_capacity(size_type n, bool front) {
		if (front && static_cast<size_type>(begin_.cur - begin_.first) < n) {
			// +1是向上取整
			const size_type need_node_size = (n - (begin_.cur - begin_.first)) / buff_size + 1;
			if (need_node_size > static_cast<size_type>(begin_.map_node - map_)) {
				reallocate_map_at_front(need_node_size);
				return;
			}
			// create_buffer会开辟[start,finish]区间的数组空间
			create_buffer(begin_.map_node - need_node_size, begin_.map_node - 1);
		}
		// " end_.last - end_.cur - 1 "的 -1 是因为尾节点不放元素
		else if (!front && static_cast<size_type>(end_.last - end_.cur - 1) < n) {
			const size_type need_node_size = (n - (end_.last - end_.cur - 1)) / buff_size + 1;
			// "map_ + map_size_ - end_.map_node - 1"是因为不减的话 end_.map_node 也被计数在内
			if (need_node_size > static_cast<size_type>(map_ + map_size_ - end_.map_node - 1)) {
				reallocate_map_at_back(need_node_size);
				return;
			}
			create_buffer(end_.map_node + 1, end_.map_node + need_node_size);
		}
	}

	template<class T>
	inline void deque<T>::reallocate_map_at_front(size_type need_node_size) {
		const auto new_map_size = esgstl::max(map_size_ << 1,
			map_size_ + need_node_size + DEQUE_MAP_INIT_SIZE);
		map_pointer new_map = create_map(new_map_size);
		auto old_buffer_size = end_.map_node - begin_.map_node + 1;
		auto new_buffer_size = old_buffer_size + need_node_size;

		auto new_begin = new_map + (new_map_size - new_buffer_size) / 2;
		auto mid = new_begin + need_node_size;
		auto new_end = new_begin + new_buffer_size - 1;
		create_buffer(new_begin, mid - 1);
		for (auto b1 = mid, b2 = begin_.map_node; b2 <= end_.map_node; ++b1, ++b2)
			*b1 = *b2;
		map_allocator::deallocate(map_);
		map_ = new_map;
		map_size_ = new_map_size;
		begin_ = iterator(*mid + (begin_.cur - begin_.first), mid);
		end_ = iterator(*new_end + (end_.cur - end_.first), new_end);
	}

	template<class T>
	inline void deque<T>::reallocate_map_at_back(size_type need_node_size) {
		const auto new_map_size = esgstl::max(map_size_ << 1,
			map_size_ + need_node_size + DEQUE_MAP_INIT_SIZE);
		map_pointer new_map = create_map(new_map_size);
		auto old_buffer_size = end_.map_node - begin_.map_node + 1;
		auto new_buffer_size = old_buffer_size + need_node_size;

		auto new_begin = new_map + (new_map_size - new_buffer_size) / 2;
		auto mid = new_begin + old_buffer_size;
		auto new_end = new_begin + new_buffer_size - 1;
		for (auto b1 = new_begin, b2 = begin_.map_node; b2 <= end_.map_node; ++b1, ++b2)
			*b1 = *b2;
		create_buffer(mid, new_end);
		map_allocator::deallocate(map_);
		map_ = new_map;
		map_size_ = new_map_size;
		begin_ = iterator(*new_begin + (begin_.cur - begin_.first), new_begin);
		end_ = iterator(*(mid-1) + (end_.cur - end_.first), mid-1);
	}

	template<class T>
	inline void deque<T>::shrink_to_fit() noexcept {
		// 把不包含begin_与end_区间元素的其他列的空间释放掉
		for (auto i = map_; i < begin_.map_node; ++i) {
			data_allocator::deallocate(*i);
			*i = nullptr;
		}
		for (auto i = end_.map_node + 1; i < map_ + map_size_; ++i) {
			data_allocator::deallocate(*i);
			*i = nullptr;
		}
	}
	
	template<class T>
	template<class ...Args>
	inline deque<T>::iterator deque<T>::emplace(const_iterator pos, Args && ...args) {
		if (pos.cur == begin_.cur) {
			emplace_front(pos, esgstl::forward<Args>(args)...);
			return begin_;
		}
		else if (pos.cur == end_.cur) {
			emplace_back(pos, esgstl::forward<Args>(args)...);
			return end_;
		}
		else {
			return insert_aux(pos, esgstl::forward<Args>(args)...);
		}
	}

	template<class T>
	template<class ...Args>
	inline void deque<T>::emplace_front(Args && ...args) {
		if (begin_.cur != begin_.first) {
			--begin_;
			data_allocator::construct(begin_.cur, esgstl::forward<Args>(args)...);
		}
		else {
			require_capacity(1, true);
			try {
				--begin_;
				data_allocator::construct(begin_.cur, esgstl::forward<Args>(args)...);
			}
			catch (...) {
				++begin_;
				throw;
			}
		}
	}

	template<class T>
	template<class ...Args>
	inline void deque<T>::emplace_back(Args && ...args) {
		if (end_.cur != end_.last - 1) {
			data_allocator::construct(end_.cur, esgstl::forward<Args>(args)...);
			++end_;
		}
		else {
			require_capacity(1, false);
			data_allocator::construct(end_.cur, esgstl::forward<Args>(args)...);
			++end_;
		}
	}

	template<class T>
	template<class Iter>
	inline void deque<T>::copy_init(Iter first, Iter last) {
		const auto n = esgstl::distance(first, last);
		map_init(n);
		for (auto i = begin_.map_node; i < end_.map_node; ++i) {
			auto next = first;
			esgstl::advance(next, buff_size);
			esgstl::uninitialized_copy(first, next, *i);
			first = next;
		}
		esgstl::uninitialized_copy(first, last, end_.first);
	}

	template<class T>
	template<class Iter>
	inline void deque<T>::copy_assign(Iter first, Iter last) {
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
	inline deque<T>::iterator 
	deque<T>::insert_aux(const_iterator pos, Args&& ...args) {
		const auto index = pos - begin_;
		if (index < size() / 2) {
			push_front(front());
			iterator f1 = begin_;
			++f1;
			iterator f2 = f1;
			++f2;
			iterator p1 = begin_ + index;
			++p1;
			esgstl::copy(f2, p1, f1);
		}
		else {
			push_back(back());
			iterator f1 = end_;
			--f1;
			iterator f2 = f1;
			--f2;
			iterator p1 = begin_+index;
			esgstl::copy_backward(p1, f2, f1);
		}
		iterator newpos = begin_ + index;
		value_type tmp(esgstl::forward<Args>(args)...);
		*newpos = esgstl::move(tmp);
		return newpos;
	}

	template<class T>
	template<class Iter>
	inline deque<T>::iterator 
	deque<T>::copy_insert(const_iterator pos, Iter first, Iter last) {
		const size_type n = static_cast<size_type>(esgstl::distance(first,last));
		const auto elems_before = pos - begin_;
		iterator p1;
		if (elems_before < size() / 2) {
			require_capacity(n, true);
			auto b1 = begin_;
			p1 = begin_ + elems_before;
			auto b2 = b1 - n;
			try {
				begin_ = b2;
				if (n > elems_before) {
					auto i1 = esgstl::uninitialized_copy(b1, p1, b2);
					auto mid = first;
					esgstl::advance(mid, n - elems_before);
					auto i2 = esgstl::uninitialized_copy(first,mid,i1);
					esgstl::copy(mid, last, i2);
				}
				else {
					auto i1 = esgstl::uninitialized_copy_n(b1, n, b2);
					esgstl::copy_n(p1 - (elems_before - n), elems_before - n, i1);
					esgstl::copy(first, last, p1-n);
				}
			}
			catch (...) {
				if (b1.map_node != begin_.map_node) {
					destory_buffer(begin_.map_node, b1.map_node - 1);
				}
				begin_ = b1;
				throw;
			}
		}
		else {
			require_capacity(n, false);
			auto e1 = end_;
			p1 = begin_ + elems_before;
			auto e2 = e1 + n;
			const auto elems_after = end_ - pos;
			try {
				end_ = e2;
				if (n > elems_after) {
					auto i1 = e2 - elems_after;
					esgstl::uninitialized_copy(p1, e1, i1);
					auto mid = first;
					esgstl::advance(mid, elems_after);
					esgstl::uninitialized_copy(mid,last,e1);
					esgstl::copy(first,mid,p1);
				}
				else {
					auto i1 = e1 - n;
					auto i2 = e2 - n;
					esgstl::uninitialized_copy_n(i1, n, i2);
					auto i3 = esgstl::copy_backward(p1, i1, e1);
					esgstl::copy(first,last,p1);
				}
			}
			catch (...) {
				if (e1.map_node != end_.map_node) {
					destory_buffer(e1.map_node + 1, end_.map_node);
				}
				end_ = e1;
				throw;
			}
		}
		return p1;
	}

	// 重载比较操作符
	template <class T>
	bool operator==(const deque<T>& lhs, const deque<T>& rhs) {
		return lhs.size() == rhs.size() &&
			esgstl::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template <class T>
	bool operator<(const deque<T>& lhs, const deque<T>& rhs) {
		return esgstl::lexicographical_compare(
			lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class T>
	bool operator!=(const deque<T>& lhs, const deque<T>& rhs) {
		return !(lhs == rhs);
	}

	template <class T>
	bool operator>(const deque<T>& lhs, const deque<T>& rhs) {
		return rhs < lhs;
	}

	template <class T>
	bool operator<=(const deque<T>& lhs, const deque<T>& rhs) {
		return !(rhs < lhs);
	}

	template <class T>
	bool operator>=(const deque<T>& lhs, const deque<T>& rhs) {
		return !(lhs < rhs);
	}

	// 重载 esgstl 的 swap
	template <class T>
	void swap(deque<T>& lhs, deque<T>& rhs) {
		lhs.swap(rhs);
	}
}