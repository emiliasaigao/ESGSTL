#pragma once

#include<initializer_list>

#include "hashtable.h"


namespace esgstl {
	template <class T, 
		class HashFcn = hash<T>, 
		class EqualKey = equal_to<T>>
	class unordered_set {
		friend bool operator==(const unordered_set& lhs, const unordered_set& rhs);
		friend bool operator!=(const unordered_set& lhs, const unordered_set& rhs);
		friend void swap(const unordered_set& lhs, const unordered_set& rhs);
	public:
		typedef hashtable<T, T, HashFcn, identity<T>, EqualKey>					hashtable;
		typedef hashtable::iterator												iterator;
		typedef hashtable::const_iterator										const_iterator;

		typedef hashtable::key_type												key_type;
		typedef hashtable::value_type											value_type;
		typedef hashtable::hasher												hasher;
		typedef hashtable::key_equal											key_equal;
		typedef hashtable::size_type											size_type;
		typedef hashtable::bucket_type											bucket_type;

	private:
		hashtable ht;

	public:
		unordered_set() : ht(100){}
		explicit unordered_set(size_type n,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
			: ht(n, hash, equal){}
		unordered_set(const unordered_set& rhs) : ht(rhs.ht){}
		unordered_set(unordered_set&& rhs) noexcept : ht(esgstl::move(rhs.ht)) {}
		
		template <class InputIter>
		unordered_set(InputIter first, InputIter last,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
			: ht(first, last, hash, equal) {
			ht.insert_unique(first, last);
		}
		unordered_set(std::initializer_list<value_type> ilist,
			const HashFcn& hash = HashFcn(),
			const EqualKey& equal = EqualKey())
			: ht(ilist.begin(), ilist.end(), hash, equal) {
			ht.insert_unique(ilist.begin(), ilist.end());
		}

		unordered_set& operator=(const unordered_set& rhs) {
			if (this != &rhs) {
				ht = rhs.ht;
			}
			return *this;
		}
		unordered_set& operator=(unordered_set&& rhs) {
			unordered_set tmp(rhs);
			ht.swap(tmp.ht);
			return *this;
		}

		unordered_set& operator=(std::initializer_list<value_type> ilist) {
			unordered_set tmp(ilist);
			ht.swap(tmp.ht);
			return *this;
		}

		~unordered_set() = default;

		// 重载比较操作符
		friend bool operator==(const unordered_set& lhs,
			const unordered_set& rhs) {
			return lhs.ht == rhs.ht;
		}
		friend bool operator!=(const unordered_set& lhs,
			const unordered_set& rhs) {
			return lhs.ht != rhs.ht;
		}

		// 特化swap
		friend void swap(const unordered_set& lhs,
			const unordered_set& rhs) {
			lhs.ht.swap(rhs.ht);
		}

		// 迭代器相关函数
		/*****************************************************************/
		iterator begin() noexcept { return ht.begin(); }
		const_iterator begin() const noexcept { return ht.cbegin(); }

		iterator end() noexcept { return ht.end(); }
		const_iterator end() const noexcept { return ht.cend(); }

		const_iterator cbegin() const noexcept { return ht.cbegin(); }
		const_iterator cend() const noexcept { return ht.cend(); }
		/*****************************************************************/

		// 容量相关函数
		/*****************************************************************/
		bool empty() const noexcept { return ht.empty(); }
		size_type size() const noexcept { return ht.size(); }
		size_type max_size() const noexcept { return ht.max_size(); }
		/*****************************************************************/

		// 修改容器相关操作
		/*****************************************************************/
		/// emplace
		template <class ...Args>
		iterator emplace(Args&& ...args) { 
			return ht.emplace_unique(esgstl::forward<Args>(args)...); 
		}

		/// insert
		pair<iterator, bool> insert(const value_type& value) {
			return ht.insert_unique(value);
		}
		pair<iterator, bool> insert(value_type&& value) {
			return ht.emplace_unique(esgstl::move(value));
		}
		template <class InputIter>
		void insert(InputIter first, InputIter last) {
			ht.insert_unique(first, last);
		}

		/// erase/clear
		void erase(const_iterator pos) { ht.erase(pos); }
		void erase(const_iterator first, const_iterator last) { ht.erase(first, last); }
		size_type erase(const key_type& key) {
			return ht.erase_unique(key);
		}

		void clear() {
			ht.clear();
		}

		/// reserve：预先准备空间
		void reserve(size_type n) { ht.reserve(n); }
		/*****************************************************************/

		// 查找相关操作
		/*****************************************************************/
		/// count
		size_type count(const key_type& key) const { return ht.count(key); }

		/// find
		iterator find(const key_type& key) { return ht.find(key); }
		const_iterator find(const key_type& key) const { return ht.find(key); }

		/// equal_range
		pair<iterator, iterator> eqaul_range(const key_type& key) { return ht.eqaul_range_unique(key); }
		pair<const_iterator, const_iterator> eqaul_range(const key_type& key) const {
			return ht.eqaul_range_unique(key);
		}
		/*****************************************************************/

		// hashtable属性相关操作
		/*****************************************************************/
		/// bucket尺寸相关函数
		size_type bucket_size() const noexcept { return ht.bucket_size(); }
		size_type max_bucket_size() const noexcept { return ht.max_bucket_size(); }

		/// load_factor：负载因子相关函数
		float load_factor() const noexcept { return ht.load_factor(); }
		float max_load_factor() const noexcept { return ht.max_load_factor(); }
		void set_max_load_factor(float mlf) {
			ht.set_max_load_factor(mlf);
		}

		/// 返回bucket_供遍历
		const bucket_type& get_buckets() const { return ht.get_buckets(); }

		/// 返回仿函数对象
		hasher hash_fcn() const { return ht.hash_fcn(); }
		key_equal key_eq() const { return ht.key_eq(); }
		/*****************************************************************/
	};

	template <class T,
		class HashFcn = hash<T>,
		class EqualKey = equal_to<T>>
	class unordered_multiset {
		friend bool operator==(const unordered_multiset& lhs, const unordered_multiset& rhs);
		friend bool operator!=(const unordered_multiset& lhs, const unordered_multiset& rhs);
		friend void swap(const unordered_multiset& lhs, const unordered_multiset& rhs);
	public:
			typedef hashtable<T, T, HashFcn, identity<T>, EqualKey>					hashtable;
			typedef hashtable::iterator												iterator;
			typedef hashtable::const_iterator										const_iterator;

			typedef hashtable::key_type												key_type;
			typedef hashtable::value_type											value_type;
			typedef hashtable::hasher												hasher;
			typedef hashtable::key_equal											key_equal;
			typedef hashtable::size_type											size_type;
			typedef hashtable::bucket_type											bucket_type;

	private:
			hashtable ht;

	public:
			unordered_multiset() : ht(100) {}
			explicit unordered_multiset(size_type n,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
				: ht(n, hash, equal) {}
			unordered_multiset(const unordered_multiset& rhs) : ht(rhs.ht) {}
			unordered_multiset(unordered_multiset&& rhs) noexcept : ht(esgstl::move(rhs.ht)) {}

			template <class InputIter>
			unordered_multiset(InputIter first, InputIter last,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
				: ht(first, last, hash, equal) {
				ht.insert_multi(first, last);
			}
			unordered_multiset(std::initializer_list<value_type> ilist,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
				: ht(ilist.begin(), ilist.end(), hash, equal) {
				ht.insert_multi(ilist.begin(), ilist.end());
			}

			unordered_multiset& operator=(const unordered_multiset& rhs) {
				if (this != &rhs) {
					ht = rhs.ht;
				}
				return *this;
			}
			unordered_multiset& operator=(unordered_multiset&& rhs) {
				unordered_multiset tmp(rhs);
				ht.swap(tmp.ht);
				return *this;
			}

			unordered_multiset& operator=(std::initializer_list<value_type> ilist) {
				unordered_multiset tmp(ilist);
				ht.swap(tmp.ht);
				return *this;
			}

			~unordered_multiset() = default;

			// 重载比较操作符
			friend bool operator==(const unordered_multiset& lhs,
				const unordered_multiset& rhs) {
				return lhs.ht == rhs.ht;
			}
			friend bool operator!=(const unordered_multiset& lhs,
				const unordered_multiset& rhs) {
				return lhs.ht != rhs.ht;
			}

			// 特化swap
			friend void swap(const unordered_multiset& lhs,
				const unordered_multiset& rhs) {
				lhs.ht.swap(rhs.ht);
			}

			// 迭代器相关函数
			/*****************************************************************/
			iterator begin() noexcept { return ht.begin(); }
			const_iterator begin() const noexcept { return ht.cbegin(); }

			iterator end() noexcept { return ht.end(); }
			const_iterator end() const noexcept { return ht.cend(); }

			const_iterator cbegin() const noexcept { return ht.cbegin(); }
			const_iterator cend() const noexcept { return ht.cend(); }
			/*****************************************************************/

			// 容量相关函数
			/*****************************************************************/
			bool empty() const noexcept { return ht.empty(); }
			size_type size() const noexcept { return ht.size(); }
			size_type max_size() const noexcept { return ht.max_size(); }
			/*****************************************************************/

			// 修改容器相关操作
			/*****************************************************************/
			/// emplace
			template <class ...Args>
			iterator emplace(Args&& ...args) {
				return ht.emplace_multi(esgstl::forward<Args>(args)...);
			}

			/// insert
			iterator insert(const value_type& value) {
				return ht.insert_multi(value);
			}
			iterator insert(value_type&& value) {
				return ht.emplace_multi(esgstl::move(value));
			}
			template <class InputIter>
			void insert(InputIter first, InputIter last) {
				ht.insert_multi(first, last);
			}

			/// erase/clear
			void erase(const_iterator pos) { ht.erase(pos); }
			void erase(const_iterator first, const_iterator last) { ht.erase(first, last); }
			size_type erase(const key_type& key) {
				return ht.erase_multi(key);
			}

			void clear() {
				ht.clear();
			}

			/// reserve：预先准备空间
			void reserve(size_type n) { ht.reserve(n); }
			/*****************************************************************/

			// 查找相关操作
			/*****************************************************************/
			/// count
			size_type count(const key_type& key) const { return ht.count(key); }

			/// find
			iterator find(const key_type& key) { return ht.find(key); }
			const_iterator find(const key_type& key) const { return ht.find(key); }

			/// equal_range
			pair<iterator, iterator> eqaul_range(const key_type& key) { return ht.eqaul_range_multi(key); }
			pair<const_iterator, const_iterator> eqaul_range(const key_type& key) const {
				return ht.eqaul_range_multi(key);
			}
			/*****************************************************************/

			// hashtable属性相关操作
			/*****************************************************************/
			/// bucket尺寸相关函数
			size_type bucket_size() const noexcept { return ht.bucket_size(); }
			size_type max_bucket_size() const noexcept { return ht.max_bucket_size(); }

			/// load_factor：负载因子相关函数
			float load_factor() const noexcept { return ht.load_factor(); }
			float max_load_factor() const noexcept { return ht.max_load_factor(); }
			void set_max_load_factor(float mlf) {
				ht.set_max_load_factor(mlf);
			}

			/// 返回bucket_供遍历
			const bucket_type& get_buckets() const { return ht.get_buckets(); }

			/// 返回仿函数对象
			hasher hash_fcn() const { return ht.hash_fcn(); }
			key_equal key_eq() const { return ht.key_eq(); }
			/*****************************************************************/
	};
}