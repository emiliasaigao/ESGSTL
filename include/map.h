#pragma once

#include<initializer_list>
#include"rb_tree.h"

namespace esgstl {
	// 模板类 map，键值不允许重复
	template <class Key,class T, class Compare = esgstl::less<Key>>
	class map {
	public:
		// map 中的value类型是键值对
		typedef Key							key_type;
		typedef T							mapped_type;
		typedef esgstl::pair<const Key, T>  value_type;
		typedef Compare						key_compare;

		// 类中类，只用来返回map中value的比较函数子
		class value_compare : public binary_function <value_type, value_type, bool> {
			friend class map<Key, T, Compare>;
		private:
			Compare comp;
			value_compare(Compare c) : comp(c) {}
		public:
			bool operator()(const value_type& lhs, const value_type& rhs) const
			{
				return comp(lhs.first, rhs.first);  // 比较键值的大小
			}
		};

	private:
		typedef esgstl::rb_tree<value_type, key_compare> base_type;
		base_type tree_;

	public:
		typedef typename base_type::node_type              node_type;
		typedef typename base_type::const_pointer          pointer;
		typedef typename base_type::const_pointer          const_pointer;
		typedef typename base_type::const_reference        reference;
		typedef typename base_type::const_reference        const_reference;
		typedef typename base_type::const_iterator         iterator;
		typedef typename base_type::const_iterator         const_iterator;
		typedef typename base_type::const_reverse_iterator reverse_iterator;
		typedef typename base_type::const_reverse_iterator const_reverse_iterator;
		typedef typename base_type::size_type              size_type;
		typedef typename base_type::difference_type        difference_type;
		typedef typename base_type::allocator_type         allocator_type;

	public:
		// 构造、赋值、移动、析构相关
		map() = default;

		template<class InputIterator>
		map(InputIterator first, InputIterator last) : tree_() {
			tree_.insert_unique(first, last);
		}

		map(std::initializer_list<value_type> ilist) : tree_() {
			tree_.insert_unique(ilist.begin(), ilist.end());
		}

		map(const map& rhs) : tree_(rhs.tree_) {}
		map(map&& rhs) : tree_(esgstl::move(rhs.tree_)) {}

		map& operator=(const map& rhs) {
			tree_ = rhs.tree_;
			return *this;
		}

		map& operator=(map&& rhs) {
			tree_ = esgstl::move(rhs.tree_);
			return *this;
		}

		map& operator=(std::initializer_list<value_type> ilist) {
			map tmp(ilist);
			swap(tmp);
			return *this;
		}

		~map() = default;

		key_compare key_comp() const { return tree_.key_comp(); }
		value_compare value_comp() const { return value_compare(tree_.key_comp()); }
		allocator_type get_allocator() const { return tree_.get_allocator(); }

	public:
		friend bool operator==(const map& lhs, const map& rhs) { return lhs.tree_ == rhs.tree_; }
		friend bool operator< (const map& lhs, const map& rhs) { return lhs.tree_ < rhs.tree_; }

	public:
		// 迭代器相关
		iterator begin() noexcept { return tree_.begin(); }
		const_iterator begin() const noexcept { return tree_.cbegin(); }

		iterator end() noexcept { return tree_.end(); }
		const_iterator end() const noexcept { return tree_.cend(); }

		const_iterator cbegin() const noexcept { return begin(); }
		const_iterator cend() const noexcept { return end(); }

		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

		reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

		reverse_iterator crbegin() const noexcept { return rbegin(); }
		const_reverse_iterator crend() const noexcept { return rend(); }

	public:
		// 容量相关操作
		bool empty() const noexcept { return tree_.empty(); }
		size_type size() const noexcept { return tree_.size(); }
		size_type max_size() const noexcept { return tree_.size(); }

	public:
		// 访问元素操作
		// map 允许用户使用at及[]下标的方式在给定键值的情况下得到其实值的引用
		mapped_type& at(const key_type& key) {
			iterator it = lower_bound(key);
			ESGSTL_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
				"map<Key, T> no such element exists");
			return it->second;
		}

		const mapped_type& at(const key_type& key) const {
			iterator it = lower_bound(key);
			ESGSTL_OUT_OF_RANGE_IF(it == end() || key_comp()(it->first, key),
				"map<Key, T> no such element exists");
			return it->second;
		}

		mapped_type& operator[](const key_type& key) {
			iterator it = lower_bound(key);
			if (it == end() || key_comp()(it->first, key)) {
				tree_.emplace_hint(it, key, mapped_type());
			}
			return it->second;
		}

		mapped_type& operator[](key_type&& key) {
			iterator it = lower_bound(key);
			if (it == end() || key_comp()(it->first, key)) {
				tree_.emplace_hint(it, esgstl::move(key), mapped_type());
			}
			return it->second;
		}

		// 插入删除操作
		template <class ...Args>
		pair<iterator, bool> emplace(Args&& ...args) {
			return tree_.emplace_unique(esgstl::forward<Args>(args)...);
		}

		template <class ...Args>
		iterator emplace_hint(iterator hint, Args&& ...args) {
			return tree_.emplace_unique_use_hint(hint, esgstl::forward<Args>(args)...);
		}

		pair<iterator, bool> insert(const value_type& value) {
			return tree_.insert_unique(value);
		}
		pair<iterator, bool> insert(value_type&& value) {
			return tree_.insert_unique(esgstl::move(value));
		}

		iterator insert(iterator hint, const value_type& value) {
			return tree_.insert_unique(hint, value);
		}
		iterator insert(iterator hint, value_type&& value) {
			return tree_.insert_unique(hint, esgstl::move(value));
		}

		template <class InputIterator>
		void insert(InputIterator first, InputIterator last) {
			tree_.insert_unique(first, last);
		}

		// 删除相关
		void erase(iterator position) { tree_.erase(position); }
		size_type erase(const key_type& key) { return tree_.erase_unique(key); }
		void erase(iterator first, iterator last) { tree_.erase(first, last); }

		void clear() { tree_.clear(); }
		void swap(map& rhs) {
			tree_.swap(rhs.tree_);
		}

		// 查找相关
		iterator find(const key_type& key) { return tree_.find(key); }
		const_iterator find(const key_type& key) const { return tree_.find(key); }

		size_type count(const key_type& key) const { return tree_.count_unique(key); }

		iterator lower_bound(const key_type& key) { return tree_.lower_bound(key); }
		const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

		iterator upper_bound(const key_type& key) { return tree_.upper_bound(key); }
		const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

		pair<iterator, iterator>
			equal_range(const key_type& key) { return tree_.equal_range_unique(key); }
		pair<const_iterator, const_iterator>
			equal_range(const key_type& key) const { return tree_.equal_range_unique(key); }

	};

	template <class Key, class Compare>
	bool operator!=(const map<Key, Compare>& lhs, const map<Key, Compare>& rhs) {
		return !(lhs == rhs);
	}

	template <class Key, class Compare>
	bool operator>(const map<Key, Compare>& lhs, const map<Key, Compare>& rhs) {
		return rhs < lhs;
	}

	template <class Key, class Compare>
	bool operator<=(const map<Key, Compare>& lhs, const map<Key, Compare>& rhs) {
		return !(rhs < lhs);
	}

	template <class Key, class Compare>
	bool operator>=(const map<Key, Compare>& lhs, const map<Key, Compare>& rhs) {
		return !(lhs < rhs);
	}

	// 重载swap
	template <class Key, class Compare>
	void swap(map<Key, Compare>& lhs, map<Key, Compare>& rhs) noexcept {
		lhs.swap(rhs);
	}

	/******************************************************************************/
	// 模板类 multimap，键值允许重复
	template <class Key, class T,class Compare = esgstl::less<Key>>
	class multimap {
		// map 中的value类型是键值对
		typedef Key							key_type;
		typedef T							mapped_type;
		typedef esgstl::pair<const Key, T>  value_type;
		typedef Compare						key_compare;

		// 类中类，只用来返回map中value的比较函数子
		class value_compare : public binary_function <value_type, value_type, bool> {
			friend class map<Key, T, Compare>;
		private:
			Compare comp;
			value_compare(Compare c) : comp(c) {}
		public:
			bool operator()(const value_type& lhs, const value_type& rhs) const
			{
				return comp(lhs.first, rhs.first);  // 比较键值的大小
			}
		};

	private:
		typedef esgstl::rb_tree<value_type, key_compare> base_type;
		base_type tree_;

	public:
		typedef typename base_type::node_type              node_type;
		typedef typename base_type::const_pointer          pointer;
		typedef typename base_type::const_pointer          const_pointer;
		typedef typename base_type::const_reference        reference;
		typedef typename base_type::const_reference        const_reference;
		typedef typename base_type::const_iterator         iterator;
		typedef typename base_type::const_iterator         const_iterator;
		typedef typename base_type::const_reverse_iterator reverse_iterator;
		typedef typename base_type::const_reverse_iterator const_reverse_iterator;
		typedef typename base_type::size_type              size_type;
		typedef typename base_type::difference_type        difference_type;
		typedef typename base_type::allocator_type         allocator_type;

	public:
		// 构造、赋值、移动、析构相关
		multimap() = default;

		template<class InputIterator>
		multimap(InputIterator first, InputIterator last) : tree_() {
			tree_.insert_multi(first, last);
		}

		multimap(std::initializer_list<value_type> ilist) : tree_() {
			tree_.insert_multi(ilist.begin(), ilist.end());
		}

		multimap(const multimap& rhs) : tree_(rhs.tree_) {}
		multimap(multimap&& rhs) : tree_(esgstl::move(rhs.tree_)) {}

		multimap& operator=(const multimap& rhs) {
			tree_ = rhs.tree_;
			return *this;
		}

		multimap& operator=(multimap&& rhs) {
			tree_ = esgstl::move(rhs.tree_);
			return *this;
		}

		multimap& operator=(std::initializer_list<value_type> ilist) {
			multimap tmp(ilist);
			swap(tmp);
			return *this;
		}

		~multimap() = default;

		key_compare key_comp() const { return tree_.key_comp(); }
		value_compare value_comp() const { return value_compare(tree_.key_comp()); }
		allocator_type get_allocator() const { return tree_.get_allocator(); }

	public:
		friend bool operator==(const multimap& lhs, const multimap& rhs) { return lhs.tree_ == rhs.tree_; }
		friend bool operator< (const multimap& lhs, const multimap& rhs) { return lhs.tree_ < rhs.tree_; }

	public:
		// 迭代器相关
		iterator begin() noexcept { return tree_.begin(); }
		const_iterator begin() const noexcept { return tree_.cbegin(); }

		iterator end() noexcept { return tree_.end(); }
		const_iterator end() const noexcept { return tree_.cend(); }

		const_iterator cbegin() const noexcept { return begin(); }
		const_iterator cend() const noexcept { return end(); }

		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

		reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

		reverse_iterator crbegin() const noexcept { return rbegin(); }
		const_reverse_iterator crend() const noexcept { return rend(); }

	public:
		// 容量相关操作
		bool empty() const noexcept { return tree_.empty(); }
		size_type size() const noexcept { return tree_.size(); }
		size_type max_size() const noexcept { return tree_.size(); }

	public:
		// 插入删除操作
		template <class ...Args>
		pair<iterator, bool> emplace(Args&& ...args) {
			return tree_.emplace_multi(esgstl::forward<Args>(args)...);
		}

		template <class ...Args>
		iterator emplace_hint(iterator hint, Args&& ...args) {
			return tree_.emplace_multi_use_hint(hint, esgstl::forward<Args>(args)...);
		}

		pair<iterator, bool> insert(const value_type& value) {
			return tree_.insert_multi(value);
		}
		pair<iterator, bool> insert(value_type&& value) {
			return tree_.insert_multi(esgstl::move(value));
		}

		iterator insert(iterator hint, const value_type& value) {
			return tree_.insert_multi(hint, value);
		}
		iterator insert(iterator hint, value_type&& value) {
			return tree_.insert_multi(hint, esgstl::move(value));
		}

		template <class InputIterator>
		void insert(InputIterator first, InputIterator last) {
			tree_.insert_multi(first, last);
		}

		// 删除相关
		void erase(iterator position) { tree_.erase(position); }
		size_type erase(const key_type& key) { return tree_.erase_multi(key); }
		void erase(iterator first, iterator last) { tree_.erase(first, last); }

		void clear() { tree_.clear(); }
		void swap(multimap& rhs) {
			tree_.swap(rhs.tree_);
		}

		// 查找相关
		iterator find(const key_type& key) { return tree_.find(key); }
		const_iterator find(const key_type& key) const { return tree_.find(key); }

		size_type count(const key_type& key) const { return tree_.count_multi(key); }

		iterator lower_bound(const key_type& key) { return tree_.lower_bound(key); }
		const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

		iterator upper_bound(const key_type& key) { return tree_.upper_bound(key); }
		const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

		pair<iterator, iterator>
			equal_range(const key_type& key) { return tree_.equal_range_multi(key); }
		pair<const_iterator, const_iterator>
			equal_range(const key_type& key) const { return tree_.equal_range_multi(key); }

	};

	template <class Key, class Compare>
	bool operator!=(const multimap<Key, Compare>& lhs, const multimap<Key, Compare>& rhs) {
		return !(lhs == rhs);
	}

	template <class Key, class Compare>
	bool operator>(const multimap<Key, Compare>& lhs, const multimap<Key, Compare>& rhs) {
		return rhs < lhs;
	}

	template <class Key, class Compare>
	bool operator<=(const multimap<Key, Compare>& lhs, const multimap<Key, Compare>& rhs) {
		return !(rhs < lhs);
	}

	template <class Key, class Compare>
	bool operator>=(const multimap<Key, Compare>& lhs, const multimap<Key, Compare>& rhs) {
		return !(lhs < rhs);
	}

	// 重载swap
	template <class Key, class Compare>
	void swap(multimap<Key, Compare>& lhs, multimap<Key, Compare>& rhs) noexcept {
		lhs.swap(rhs);
	}
}
