#pragma once

#include <initializer_list>		// 引入初始化列表，接受{元素集}初始化

#include "iterator.h"			// 加入迭代器规范
#include "memory.h"				// 分配节点内存
#include "util.h"				// 使用swap、move等工具函数
#include "functional.h"			// 接受用户提供的一元谓词、二元谓词自定义行为
#include "exceptdef.h"			// 使用了一些报错机制

namespace esgstl {

	template <class T> struct list_node_base;
	template <class T> struct list_node;

	template <class T>
	struct node_traits {
		typedef typename list_node_base<T>*		base_ptr;
		typedef typename list_node<T>*			node_ptr;
	};

	template <class T>
	struct list_node_base {
		typedef typename node_traits<T>::base_ptr	base_ptr;
		typedef typename node_traits<T>::node_ptr	node_ptr;

		base_ptr prev;
		base_ptr next;

		list_node_base() = default;

		node_ptr as_node() { return static_cast<node_ptr>(self()); }
		base_ptr self() { return static_cast<base_ptr>(&*this); }
		void unlink() { prev = next = self(); }
	};

	template <class T>
	struct list_node : public list_node_base<T> {
		typedef typename node_traits<T>::base_ptr	base_ptr;
		typedef typename node_traits<T>::node_ptr	node_ptr;

		T value;

		list_node() = default;
		list_node(const T& val) : value(val) {}
		list_node(T&& val) : value(esgstl::move(val)) {}

		base_ptr as_base() { return static_cast<base_ptr>(self()); }
		node_ptr self() { return static_cast<node_ptr>(&*this); }
	};

	template <class T>
	struct list_iterator : public esgstl::iterator<esgstl::bidirectional_iterator_tag, T> {
		typedef T									value_type;
		typedef T*									pointer;
		typedef T&									reference;
		typedef typename node_traits<T>::base_ptr	base_ptr;
		typedef typename node_traits<T>::node_ptr	node_ptr;
		typedef list_iterator<T>					self;

		base_ptr node_;

		list_iterator() = default;
		list_iterator(base_ptr b) : node_(b) {}
		list_iterator(node_ptr n) : node_(n->as_base()) {}
		list_iterator(const list_iterator& rhs) : node_(rhs.node_) {}

		reference operator*() { return node_->as_node()->value; }
		pointer operator->() { return &(operator*()); }

		self& operator++() {
			ESGSTL_DEBUG(node_ != nullptr);
			node_ = node_->next;
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			++(*this);
			return tmp;
		}
		self& operator--() {
			ESGSTL_DEBUG(node_ != nullptr);
			node_ = node_->prev;
			return *this;
		}
		self operator--(int) {
			self tmp = *this;
			--(*this);
			return tmp;
		}

		self& operator+=(int n) {
			esgstl::advance(*this, n);
			return *this;
		}
		self& operator-=(int n) {
			return (*this) += -n;
		}
		self operator+(int n) {
			self tmp = *this;
			tmp += n;
			return tmp;
		}
		self operator-(int n) {
			self tmp = *this;
			tmp -= n;
			return tmp;
		}

		// 重载比较操作符
		bool operator==(const self& rhs) const { return node_ == rhs.node_; }
		bool operator!=(const self& rhs) const { return node_ != rhs.node_; }

	};

	template <class T>
	struct list_const_iterator : public iterator<bidirectional_iterator_tag, T>
	{
		typedef T									value_type;
		typedef const T*							pointer;
		typedef const T&							reference;
		typedef typename node_traits<T>::base_ptr	base_ptr;
		typedef typename node_traits<T>::node_ptr	node_ptr;
		typedef list_const_iterator<T>				self;

		base_ptr node_;

		list_const_iterator() = default;
		list_const_iterator(base_ptr x)
			:node_(x) {}
		list_const_iterator(node_ptr x)
			:node_(x->as_base()) {}
		list_const_iterator(const list_iterator<T>& rhs)
			:node_(rhs.node_) {}
		list_const_iterator(const list_const_iterator& rhs)
			:node_(rhs.node_) {}

		reference operator*()  const { return node_->as_node()->value; }
		pointer   operator->() const { return &(operator*()); }

		self& operator++() {
			ESGSTL_DEBUG(node_ != nullptr);
			node_ = node_->next;
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			++* this;
			return tmp;
		}
		self& operator--()
		{
			ESGSTL_DEBUG(node_ != nullptr);
			node_ = node_->prev;
			return *this;
		}
		self operator--(int) {
			self tmp = *this;
			--* this;
			return tmp;
		}
		self& operator+=(int n) {
			esgstl::advance(*this, n);
			return *this;
		}
		self& operator-=(int n) {
			return (*this) += -n;
		}
		self operator+(int n) {
			self tmp = *this;
			tmp += n;
			return tmp;
		}
		self operator-(int n) {
			self tmp = *this;
			tmp -= n;
			return tmp;
		}

		// 重载比较操作符
		bool operator==(const self& rhs) const { return node_ == rhs.node_; }
		bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
	};


	// list
	// 底层是双向链表，在插入上效率很高为O(1)
	template <class T>
	class list {
	public:
		// 一些型别定义
		typedef esgstl::allocator<T>							allocator_type;
		typedef esgstl::allocator<T>							data_allocator;
		typedef esgstl::allocator<list_node_base<T>>			base_allocator;
		typedef esgstl::allocator<list_node<T>>					node_allocator;

		typedef allocator_type::value_type						value_type;
		typedef allocator_type::pointer							pointer;
		typedef allocator_type::reference						reference;
		typedef allocator_type::const_pointer					const_pointer;
		typedef allocator_type::const_reference					const_reference;
		typedef allocator_type::size_type						size_type;
		typedef allocator_type::difference_type					difference_type;

		typedef list_iterator<T>								iterator;
		typedef list_const_iterator<T>							const_iterator;
		typedef esgstl::reverse_iterator<iterator>				reverse_iterator;
		typedef esgstl::reverse_iterator<const_iterator>		const_reverse_iterator;

		typedef typename node_traits<T>::base_ptr				base_ptr;
		typedef typename node_traits<T>::node_ptr				node_ptr;

		allocator_type get_allocator() { return data_allocator(); }

	private:
		base_ptr node_;// 指向末尾的空节点
		size_type size_;// 记录list大小

	public:
		// 构造、析构函数
		list() { fill_init(0, value_type()); }
		explicit list(size_type n) { fill_init(n, value_type()); }
		list(size_type n, const value_type& value) { fill_init(n, value); }

		template <class Iter, typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value, int>::type = 0>
		list(Iter first, Iter last) {
			ESGSTL_DEBUG(first < last);
			copy_init(first, last);
		}
		list(std::initializer_list<value_type> ilist) { copy_init(ilist.begin(), ilist.end()); }
		list(const list& rhs) { copy_init(rhs.cbegin(), rhs.cend()); }
		list(list&& rhs) noexcept :node_(rhs.node_), size_(rhs.size_) {
			rhs.node_ = nullptr;
			rhs.size_ = 0;
		}

		~list() {
			if (node_) {
				clear();
				base_allocator::deallocate(node_);
				node_ = nullptr;
				size_ = 0;

			}
		}

		list& operator=(const list& rhs) {
			if (this != &rhs) {
				assign(rhs.begin(), rhs.end());
			}
			return *this;
		}
		list& operator=(list&& rhs) noexcept {
			clear();
			splice(end(), rhs);
		}
		list& operator=(std::initializer_list<value_type> ilist) {
			list tmp(ilist);
			swap(tmp);
			return *this;
		}

	public:
		// 迭代器相关操作
		/*****************************************************************/
		iterator begin() noexcept { return node_->next; }
		iterator end() noexcept { return node_; }
		const_iterator begin() const noexcept { return node_->next; }
		const_iterator end() const noexcept { return node_; }

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
		size_type size() noexcept { return size_; }
		size_type max_size() noexcept { return static_cast<size_type>(-1); }
		bool empty() noexcept { return node_->next == node_; }
		/*****************************************************************/

		// 访问容器相关操作
		/*****************************************************************/
		reference front() {
			ESGSTL_DEBUG(!empty());
			return *(begin());
		}
		reference back() {
			ESGSTL_DEBUG(!empty());
			return *(--end());
		}

		const_reference front() const { return front(); }
		const_reference back() const { return back(); }
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

		//// emplace/emplace_front/emplace_back
		template <class ...Args>
		void emplace(const_iterator pos, Args&& ...args) {
			ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
			auto node = create_node(esgstl::forward<Args>(args)...);
			link_nodes(pos.node_, node->as_base(), node->as_base());
			++size_;
		}

		template <class ...Args>
		void emplace_front(Args&& ...args) {
			ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
			auto node = create_node(esgstl::forward<Args>(args)...);
			link_nodes_at_front(node->as_base(), node->as_base());
			++size_;
		}

		template <class ...Args>
		void emplace_back(Args&& ...args) {
			ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
			auto node = create_node(esgstl::forward<Args>(args)...);
			link_nodes_at_back(node->as_base(), node->as_base());
			++size_;
		}

		//// insert
		iterator insert(const_iterator pos, const value_type& value) {
			ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
			auto node = create_node(value);
			link_iter_node(pos, node->as_base());
			++size_;
		}

		iterator insert(const_iterator pos, value_type&& value) {
			emplace(pos, value);
			return iterator(pos.node_->prev);
		}

		iterator insert(const_iterator pos, size_type n, const value_type& value) {
			if (n == 0) return iterator(pos.node_);
			return fill_insert(pos,n,value);
		}

		template <class Iter, typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value, int>::type = 0>
		iterator insert(const_iterator pos, Iter first, Iter last) {
			return copy_insert(pos,first,last);
		}

		//// push_back/push_front
		void push_back(const value_type& value) { insert(end(), value); }
		void push_back(value_type&& value) { emplace_back(esgstl::move(value)); }

		void push_front(const value_type& value) { insert(begin(), value); }
		void push_front(value_type&& value) { emplace_front(esgstl::move(value)); }

		//// pop_back/pop_front
		void pop_back() {
			ESGSTL_DEBUG(!empty());
			auto e = end();
			const_iterator it(e.node_->prev);
			erase(it);
		}
		void pop_front() {
			ESGSTL_DEBUG(!empty());
			erase(cbegin());
		}

		//// erase/clear
		iterator erase(const_iterator pos);
		iterator erase(const_iterator first, const_iterator last);
		void clear();

		//// resize
		void resize(size_type n) { resize(n, value_type()); }
		void resize(size_type n, const value_type& v);

		/*****************************************************************/

		// list相关操作
		/*****************************************************************/
		//// splice:粘接其他list的节点
		void splice(const_iterator pos, list& other);
		void splice(const_iterator pos, list& other, const_iterator it);
		void splice(const_iterator pos, list& other, const_iterator first, const_iterator last);

		//// remove
		void remove(const value_type& value) {
			remove_if([&](const value_type& v) {return v == value; });
		}
		template <class UnaryPred>
		void remove_if(UnaryPred pred);

		//// unique
		void unique() { unique(esgstl::equal_to<T>()); }
		template <class BinaryPred>
		void unique(BinaryPred pred);

		//// merge:合并两个(有序)序列
		void merge(list& other) { merge(other, esgstl::less<T>()); }
		template <class BinaryPred>
		void merge(list& other, BinaryPred pred);

		//// sort
		void sort() { sort(esgstl::less<T>()); }
		template <class BinaryPred>
		void sort(BinaryPred pred);

		//// reverse
		void reverse();

		/// swap
		void swap(list& rhs);
		/*****************************************************************/

	private:
		// 节点创建与析构
		template<class ...Args>
		node_ptr create_node(Args&& ...args);
		void destory_node(node_ptr p);

		// 初始化真实逻辑函数
		void fill_init(size_type n, const value_type& value);
		template <class Iter>
		void copy_init(Iter first, Iter last);

		// assign真实逻辑函数
		void fill_assign(size_type n, const value_type& value);
		template <class Iter>
		void copy_assign(Iter first, Iter last);

		// insert辅助函数
		iterator fill_insert(const_iterator pos, size_type n, const value_type& value);
		template <class Iter>
		iterator copy_insert(const_iterator pos, Iter first, Iter last);

		// 在指定迭代器前插入node，返回相对位置不变的插入点迭代器
		iterator link_iter_node(const_iterator iter, base_ptr node);
		// 在指定位置前插入[first,last]
		void link_nodes(base_ptr p, base_ptr first, base_ptr last);
		// 在起始位置前插入[first,last]
		void link_nodes_at_front(base_ptr first, base_ptr last);
		// 在末尾前插入[first,last]
		void link_nodes_at_back(base_ptr first, base_ptr last);
		// 将[first,last]同list断开
		void unlink_nodes(base_ptr first, base_ptr last);


	};

	template <class T>
	void list<T>::fill_init(size_type n, const value_type& value) {
		node_ = base_allocator::allocate(1);
		node_->unlink();
		size_ = n;
		try {
			while (n > 0) {
				auto node = create_node(value);
				link_nodes_at_back(node, node);
				--n;
			}
		}
		catch (...) {
			clear();
			base_allocator::deallocate(node_);
			throw;
		}
	}

	template<class T>
	inline void list<T>::fill_assign(size_type n, const value_type& value) {
		auto cur = begin();
		auto e = end();
		while (cur != e && n > 0) {
			*cur = value;
			++cur; --n;
		}
		if (n > 0) insert(cur, n, value);
		else erase(cur, e);
	}

	template<class T>
	inline list<T>::iterator list<T>::fill_insert(const_iterator pos, size_type n, const value_type& value) {
		list tmp(n, value);
		auto pos_pre = pos.node_->prev;
		splice(pos, tmp);
		return iterator(pos_pre->next);
	}

	template<class T>
	inline list<T>::iterator list<T>::link_iter_node(const_iterator iter, base_ptr node) {
		link_nodes(iter.node_, node, node);
		return node;
	}

	template<class T>
	inline void list<T>::link_nodes(base_ptr p, base_ptr first, base_ptr last) {
		first->prev = p->prev;
		first->prev->next = first;
		p->prev = last;
		last->next = p;
	}

	template<class T>
	inline void list<T>::link_nodes_at_front(base_ptr first, base_ptr last) {
		auto p = node_->next;
		link_nodes(p, first, last);
	}

	template<class T>
	inline void list<T>::link_nodes_at_back(base_ptr first, base_ptr last) {
		auto p = node_;
		link_nodes(p, first, last);
	}

	template<class T>
	inline void list<T>::unlink_nodes(base_ptr first, base_ptr last) {
		first->prev->next = last->next;
		last->next->prev = first->prev;
	}

	template<class T>
	inline void list<T>::swap(list& rhs) {
		esgstl::swap(node_, rhs.node_);
		esgstl::swap(size_, rhs.size_);
	}

	template<class T>
	template<class UnaryPred>
	inline void list<T>::remove_if(UnaryPred pred) {
		auto b = begin();
		auto e = end();
		auto nxt = b;
		while (b != e) {
			++nxt;
			if (pred(*b)) {
				erase(b);
			}
			b = nxt;
		}
	}

	template<class T>
	template<class BinaryPred>
	inline void list<T>::unique(BinaryPred pred) {
		auto b = begin();
		auto e = end();
		auto nxt = b;
		++nxt;
		while (nxt != e) {
			if (pred(*b, *nxt)) {
				erase(b);
			}
			b = nxt++;
		}
	}

	template<class T>
	template<class BinaryPred>
	inline void list<T>::merge(list& other, BinaryPred pred) {
		auto b1 = begin();
		auto e1 = end();
		auto b2 = other.begin();
		auto e2 = other.end();
		while (b1 != e1 && b2 != e2) {
			if (pred(*b1, *b2)) {
				++b1;
			}
			else {
				auto nxt2 = b2;
				++nxt2;
				splice(b1, other, b2);
				b2 = nxt2;
			}
		}
		if (b2 != e2) {
			splice(end(), other);
		}
	}

	// 原理为归并排序
	template<class T>
	template<class BinaryPred>
	inline void list<T>::sort(BinaryPred pred) {
		if (node_->next == node_ || node_->next->next == node_) return;
		// 额外空间的临时储存区
		// carry是置放当前元素的有序list
		// counter是一个存放零散有序list的池子
		list carry;
		list counter[64];
		int fill = 0;
		while (!empty()) {
			// 将当前list头部元素取下来放到carry中
			carry.splice(carry.begin(), *this, begin());
			int i = 0;
			// 将carry合并到counter的0层，并依次往上合并
			// 直到某一层没有存放零散有序list，或者到达了用于存储当前最长零散list的层——fill
			while (i < fill && !counter[i].empty()) {
				// 合并carry
				counter[i].merge(carry, pred);
				// 将合并后的结果还给carry
				carry.swap(counter[i++]);
			}
			// 当前i层没有存放元素，将carry放到该层
			carry.swap(counter[i]);
			// 如果i层就是fill层的话，重新将fill设置为fill的下一层
			if (i == fill) ++fill;
		}
		// 此时counter里都是有序的list，从1层开始一路合并前一层到fill-1层
		for (int i = 1; i < fill; ++i) counter[i].merge(counter[i - 1],pred);
		// 将fill-1层还给当前list
		swap(counter[fill - 1]);
	}

	template<class T>
	template<class ...Args>
	typename list<T>::node_ptr list<T>::create_node(Args&& ...args) {
		node_ptr p = node_allocator::allocate(1);
		try {
			data_allocator::construct(&(p->value), esgstl::forward<Args>(args)...);
			p->prev = nullptr;
			p->next = nullptr;
		}
		catch (const std::exception&) {
			node_allocator::deallocate(p);
			throw;
		}
		return p;
	}

	template<class T>
	inline void list<T>::reverse() {
		if (node_ == node_->next || node_ == node_->next->next) return;
		auto b = begin().node_;
		auto e = end().node_;
		while (b != e) {
			esgstl::swap(b->prev, b->next);
			b = b->prev;
		}
		esgstl::swap(e->prev, e->next);
	}

	template<class T>
	inline void list<T>::destory_node(node_ptr p) {
		data_allocator::destory(&(p->value));
		node_allocator::deallocate(p);
	}

	template<class T>
	inline list<T>::iterator list<T>::erase(const_iterator pos) {
		ESGSTL_DEBUG(pos != cend());
		auto p = pos.node_;
		auto pos_nxt = p->next;
		unlink_nodes(p, p);
		destory_node(p->as_node());
		--size_;
		return iterator(pos_nxt);
	}

	template<class T>
	inline list<T>::iterator list<T>::erase(const_iterator first, const_iterator last) {
		if (first != last) {
			auto b = first.node_;
			auto e = last.node_;
			unlink_nodes(b, e->prev);
			while (b != e) {
				auto nxt = b->next;
				destory_node(b->as_node());
				b = nxt;
				--size_;
			}
		}
		return iterator(last.node_);
	}

	template<class T>
	inline void list<T>::clear() {
		if (!empty()) {
			auto cur = node_->next;
			while (cur != node_) {
				auto next = cur->next;
				destory_node(cur->as_node());
				cur = next;
			}
			node_->unlink();
			size_ = 0;
		}
	}

	template<class T>
	inline void list<T>::resize(size_type n, const value_type& v) {
		ESGSTL_LENGTH_ERROR_IF(n > max_size(), "list<T>'s size too big");
		if (size_ > n) {
			auto b = begin();
			b += (int)n;
			erase(b, end());
		}
		else {
			insert(cend(), n - size_, v);
		}
	}

	template<class T>
	inline void list<T>::splice(const_iterator pos, list& other) {
		ESGSTL_DEBUG(this != &other);
		if (other.empty()) return;
		ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - other.size_, "list<T>'s size too big");
		auto b = other.node_->next;
		auto e = other.node_->prev;
		other.unlink_nodes(b, e);
		link_nodes(pos.node_, b, e);
		size_ += other.size_;
		other.size_ = 0;
	}

	template<class T>
	inline void list<T>::splice(const_iterator pos, list& other, const_iterator it) {
		if (pos.node_ == it.node_ || it.node_->next == pos.node_) return;
		ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - 1, "list<T>'s size too big");
		auto b = it.node_;
		other.unlink_nodes(b, b);
		link_nodes(pos.node_, b, b);
		++size_; --other.size_;
	}

	template<class T>
	inline void list<T>::splice(const_iterator pos, list& other, const_iterator first, const_iterator last) {
		ESGSTL_DEBUG(this != &other);
		if (first == last) return;
		size_type n = static_cast<size_type>(esgstl::distance(first, last));
		ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - n, "list<T>'s size too big");
		auto b = first->node_;
		auto e = last->node_->prev;
		other.unlink_nodes(b, e);
		link_nodes(pos.node_, b, e);
		size_ += n;
		other.size_ -= n;
	}

	template<class T>
	template<class Iter>
	void list<T>::copy_init(Iter first, Iter last) {
		node_ = base_allocator::allocate(1);
		node_->unlink();
		size_type n = static_cast<size_type>(esgstl::distance(first, last));
		size_ = n;
		try {
			while (n > 0) {
				auto node = create_node(*(first));
				link_nodes_at_back(node->as_base(), node->as_base());
				++first; --n;
			}
		}
		catch (...) {
			clear();
			base_allocator::deallocate(node_);
			throw;
		}
	}

	template<class T>
	template<class Iter>
	inline void list<T>::copy_assign(Iter first, Iter last) {
		auto cur = begin();
		auto e = end();
		size_type n = static_cast<size_type>(esgstl::distance(first, last));
		while (cur != e && n > 0) {
			*cur = *first;
			++cur; ++first; --n;
		}
		if (n > 0) insert(cur, first, last);
		else erase(cur, e);
	}

	template<class T>
	template<class Iter>
	inline typename list<T>::iterator 
	list<T>::copy_insert(const_iterator pos, Iter first, Iter last) {
		list tmp(first, last);
		auto pos_pre = pos.node_->prev;
		splice(pos, tmp);
		return iterator(pos_pre->next);
	}

	// 重载比较操作符
	template <class T>
	bool operator==(const list<T>& lhs, const list<T>& rhs) {
		if (lhs.size() != rhs.size()) return false;
		auto i = lhs.begin();
		auto j = rhs.begin();
		auto e = lhs.end();
		while (i != e) {
			if (*i++ != *j++) return false;
		}
		return true;
	}

	template <class T>
	bool operator!=(const list<T>& lhs, const list<T>& rhs) {
		return !(lhs == rhs);
	}

	template <class T>
	bool operator<(const list<T>& lhs, const list<T>& rhs) {
		return esgstl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
	}

	template <class T>
	bool operator>(const list<T>& lhs, const list<T>& rhs) {
		return rhs < lhs;
	}

	template <class T>
	bool operator<=(const list<T>& lhs, const list<T>& rhs) {
		return !(rhs < lhs);
	}

	template <class T>
	bool operator>=(const list<T>& lhs, const list<T>& rhs) {
		return !(lhs < rhs);
	}

	// 重载swap
	template <class T>
	void swap(list<T>& lhs, list<T>& rhs) noexcept {
		lhs.swap(rhs);
	}
}