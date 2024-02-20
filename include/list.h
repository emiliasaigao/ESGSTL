#pragma once

#include <initializer_list>		// �����ʼ���б�����{Ԫ�ؼ�}��ʼ��

#include "iterator.h"			// ����������淶
#include "memory.h"				// ����ڵ��ڴ�
#include "util.h"				// ʹ��swap��move�ȹ��ߺ���
#include "functional.h"			// �����û��ṩ��һԪν�ʡ���Ԫν���Զ�����Ϊ
#include "exceptdef.h"			// ʹ����һЩ�������

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

		// ���رȽϲ�����
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

		// ���رȽϲ�����
		bool operator==(const self& rhs) const { return node_ == rhs.node_; }
		bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
	};


	// list
	// �ײ���˫�������ڲ�����Ч�ʺܸ�ΪO(1)
	template <class T>
	class list {
	public:
		// һЩ�ͱ���
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
		base_ptr node_;// ָ��ĩβ�Ŀսڵ�
		size_type size_;// ��¼list��С

	public:
		// ���졢��������
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
		// ��������ز���
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

		// ������ز���
		/*****************************************************************/
		size_type size() noexcept { return size_; }
		size_type max_size() noexcept { return static_cast<size_type>(-1); }
		bool empty() noexcept { return node_->next == node_; }
		/*****************************************************************/

		// ����������ز���
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

		// �޸�������ز���
		/*****************************************************************/
		
		//// assign:��������
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

		// list��ز���
		/*****************************************************************/
		//// splice:ճ������list�Ľڵ�
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

		//// merge:�ϲ�����(����)����
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
		// �ڵ㴴��������
		template<class ...Args>
		node_ptr create_node(Args&& ...args);
		void destory_node(node_ptr p);

		// ��ʼ����ʵ�߼�����
		void fill_init(size_type n, const value_type& value);
		template <class Iter>
		void copy_init(Iter first, Iter last);

		// assign��ʵ�߼�����
		void fill_assign(size_type n, const value_type& value);
		template <class Iter>
		void copy_assign(Iter first, Iter last);

		// insert��������
		iterator fill_insert(const_iterator pos, size_type n, const value_type& value);
		template <class Iter>
		iterator copy_insert(const_iterator pos, Iter first, Iter last);

		// ��ָ��������ǰ����node���������λ�ò���Ĳ���������
		iterator link_iter_node(const_iterator iter, base_ptr node);
		// ��ָ��λ��ǰ����[first,last]
		void link_nodes(base_ptr p, base_ptr first, base_ptr last);
		// ����ʼλ��ǰ����[first,last]
		void link_nodes_at_front(base_ptr first, base_ptr last);
		// ��ĩβǰ����[first,last]
		void link_nodes_at_back(base_ptr first, base_ptr last);
		// ��[first,last]ͬlist�Ͽ�
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

	// ԭ��Ϊ�鲢����
	template<class T>
	template<class BinaryPred>
	inline void list<T>::sort(BinaryPred pred) {
		if (node_->next == node_ || node_->next->next == node_) return;
		// ����ռ����ʱ������
		// carry���÷ŵ�ǰԪ�ص�����list
		// counter��һ�������ɢ����list�ĳ���
		list carry;
		list counter[64];
		int fill = 0;
		while (!empty()) {
			// ����ǰlistͷ��Ԫ��ȡ�����ŵ�carry��
			carry.splice(carry.begin(), *this, begin());
			int i = 0;
			// ��carry�ϲ���counter��0�㣬���������Ϻϲ�
			// ֱ��ĳһ��û�д����ɢ����list�����ߵ��������ڴ洢��ǰ���ɢlist�Ĳ㡪��fill
			while (i < fill && !counter[i].empty()) {
				// �ϲ�carry
				counter[i].merge(carry, pred);
				// ���ϲ���Ľ������carry
				carry.swap(counter[i++]);
			}
			// ��ǰi��û�д��Ԫ�أ���carry�ŵ��ò�
			carry.swap(counter[i]);
			// ���i�����fill��Ļ������½�fill����Ϊfill����һ��
			if (i == fill) ++fill;
		}
		// ��ʱcounter�ﶼ�������list����1�㿪ʼһ·�ϲ�ǰһ�㵽fill-1��
		for (int i = 1; i < fill; ++i) counter[i].merge(counter[i - 1],pred);
		// ��fill-1�㻹����ǰlist
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

	// ���رȽϲ�����
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

	// ����swap
	template <class T>
	void swap(list<T>& lhs, list<T>& rhs) noexcept {
		lhs.swap(rhs);
	}
}