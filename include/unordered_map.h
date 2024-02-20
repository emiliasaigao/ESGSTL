#pragma once

#include<initializer_list>

#include "hashtable.h"


namespace esgstl {
	template <class Key, class Value,
		class HashFcn = hash<Key>,
		class EqualKey = equal_to<Key>>
		class unordered_map {
		friend bool operator==(const unordered_map& lhs, const unordered_map& rhs);
		friend bool operator!=(const unordered_map& lhs, const unordered_map& rhs);
		friend void swap(const unordered_map& lhs, const unordered_map& rhs);
		public:
			typedef hashtable<esgstl::pair<const Key,Value>,Key,HashFcn,
				select1st<esgstl::pair<const Key, Value>>,EqualKey>					hashtable;
			typedef hashtable::iterator												iterator;
			typedef hashtable::const_iterator										const_iterator;

			typedef hashtable::key_type												key_type;
			typedef hashtable::value_type											value_type;
			typedef Value															mapped_type;
			typedef hashtable::hasher												hasher;
			typedef hashtable::key_equal											key_equal;
			typedef hashtable::size_type											size_type;
			typedef hashtable::bucket_type											bucket_type;

		private:
			hashtable ht;

		public:
			unordered_map() : ht(100) {}
			explicit unordered_map(size_type n,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
				: ht(n, hash, equal) {}
			unordered_map(const unordered_map& rhs) : ht(rhs.ht) {}
			unordered_map(unordered_map&& rhs) noexcept : ht(esgstl::move(rhs.ht)) {}

			template <class InputIter>
			unordered_map(InputIter first, InputIter last,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
				: ht(first, last, hash, equal) {
				ht.insert_unique(first, last);
			}
			unordered_map(std::initializer_list<value_type> ilist,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
				: ht(ilist.begin(), ilist.end(), hash, equal) {
				ht.insert_unique(ilist.begin(), ilist.end());
			}

			unordered_map& operator=(const unordered_map& rhs) {
				if (this != &rhs) {
					ht = rhs.ht;
				}
				return *this;
			}
			unordered_map& operator=(unordered_map&& rhs) {
				unordered_map tmp(rhs);
				ht.swap(tmp.ht);
				return *this;
			}

			unordered_map& operator=(std::initializer_list<value_type> ilist) {
				unordered_map tmp(ilist);
				ht.swap(tmp.ht);
				return *this;
			}

			~unordered_map() = default;

			// 重载比较操作符
			friend bool operator==(const unordered_map& lhs,
				const unordered_map& rhs) {
				return lhs.ht == rhs.ht;
			}
			friend bool operator!=(const unordered_map& lhs,
				const unordered_map& rhs) {
				return lhs.ht != rhs.ht;
			}

			// 特化swap
			friend void swap(const unordered_map& lhs,
				const unordered_map& rhs) {
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

			// 访问容器相关操作
			/*****************************************************************/
			/// at
			mapped_type& at(const key_type& key) {
				iterator it = find(key);
				ESGSTL_OUT_OF_RANGE_IF(it.node == nullptr, "unordered_map<Key, T> no such element exists");
				return it->second;
			}
			const mapped_type& at(const key_type& key) const {
				iterator it = find(key);
				ESGSTL_OUT_OF_RANGE_IF(it.node == nullptr, "unordered_map<Key, T> no such element exists");
				return it->second;
			}

			mapped_type& operator[](const key_type& key) {
				iterator it = find(key);
				if (it.node == nullptr) it = ht.emplace_unique(key, mapped_type{}).first;
				return it->second;
			}
			const mapped_type& operator[](const key_type& key) const {
				iterator it = find(key);
				if (it.node == nullptr) it = ht.emplace_unique(key, mapped_type{}).first;
				return it->second;
			}
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

			/// swap
			void swap(unordered_map& rhs) { ht.swap(rhs.ht); }

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

	template <class Key, class Value,
		class HashFcn = hash<Key>,
		class EqualKey = equal_to<Key>>
		class unordered_multimap {
		friend bool operator==(const unordered_multimap& lhs, const unordered_multimap& rhs);
		friend bool operator!=(const unordered_multimap& lhs, const unordered_multimap& rhs);
		friend void swap(const unordered_multimap& lhs, const unordered_multimap& rhs);
		public:
			typedef hashtable<esgstl::pair<const Key, Value>, Key, HashFcn,
				select1st<esgstl::pair<const Key, Value>>, EqualKey>					hashtable;
			typedef hashtable::iterator												iterator;
			typedef hashtable::const_iterator										const_iterator;

			typedef hashtable::key_type												key_type;
			typedef hashtable::value_type											value_type;
			typedef Value															mapped_type;
			typedef hashtable::hasher												hasher;
			typedef hashtable::key_equal											key_equal;
			typedef hashtable::size_type											size_type;
			typedef hashtable::bucket_type											bucket_type;

		private:
			hashtable ht;

		public:
			unordered_multimap() : ht(100) {}
			explicit unordered_multimap(size_type n,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
				: ht(n, hash, equal) {}
			unordered_multimap(const unordered_multimap& rhs) : ht(rhs.ht) {}
			unordered_multimap(unordered_multimap&& rhs) noexcept : ht(esgstl::move(rhs.ht)) {}

			template <class InputIter>
			unordered_multimap(InputIter first, InputIter last,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
				: ht(first, last, hash, equal) {
				ht.insert_multi(first, last);
			}
			unordered_multimap(std::initializer_list<value_type> ilist,
				const HashFcn& hash = HashFcn(),
				const EqualKey& equal = EqualKey())
				: ht(ilist.begin(), ilist.end(), hash, equal) {
				ht.insert_multi(ilist.begin(), ilist.end());
			}

			unordered_multimap& operator=(const unordered_multimap& rhs) {
				if (this != &rhs) {
					ht = rhs.ht;
				}
				return *this;
			}
			unordered_multimap& operator=(unordered_multimap&& rhs) {
				unordered_multimap tmp(rhs);
				ht.swap(tmp.ht);
				return *this;
			}

			unordered_multimap& operator=(std::initializer_list<value_type> ilist) {
				unordered_multimap tmp(ilist);
				ht.swap(tmp.ht);
				return *this;
			}

			~unordered_multimap() = default;

			// 重载比较操作符
			friend bool operator==(const unordered_multimap& lhs,
				const unordered_multimap& rhs) {
				return lhs.ht == rhs.ht;
			}
			friend bool operator!=(const unordered_multimap& lhs,
				const unordered_multimap& rhs) {
				return lhs.ht != rhs.ht;
			}

			// 特化swap
			friend void swap(const unordered_multimap& lhs,
				const unordered_multimap& rhs) {
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
			pair<iterator, bool> insert(const value_type& value) {
				return ht.insert_multi(value);
			}
			pair<iterator, bool> insert(value_type&& value) {
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

			/// swap
			void swap(unordered_multimap& rhs) { ht.swap(rhs.ht); }

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