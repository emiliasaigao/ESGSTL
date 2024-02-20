#pragma once

#include "iterator.h"	// 设计了hashtable的迭代器
#include "exceptdef.h"	// 使用了一些异常处理
#include "memory.h"		// 创建和构造节点
#include "functional.h"	// 使用了预先定义的仿函数作为默认参数
#include "util.h"		// 使用了swap、forward等函数
#include "vector.h"		// 作为底层容器存储链表
#include "algo.h"		// 使用了lower_bound


namespace esgstl {
	// 前置声明
	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	class hashtable;
	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	struct hashtable_iterator;
	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	struct hashtable_const_iterator;

	template <class T>
	struct hashtable_node {
		T value;
		hashtable_node* next;
	};

	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	struct hashtable_iterator {
		typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>					hashtable;
		typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>			iterator;
		typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>		const_iterator;
		typedef hashtable_node<Value>													node;

		typedef size_t										size_type;
		typedef esgstl::forward_iterator_tag				iterator_category;
		typedef Value										value_type;
		typedef Value*										pointer;
		typedef Value&										reference;
		typedef ptrdiff_t									difference_type;

		node* cur; // 当前迭代器所指节点
		hashtable* ht; // 当前迭代器与hashtable的连接

		// 构造函数
		hashtable_iterator() noexcept : cur(nullptr),ht(nullptr) {}
		hashtable_iterator(node* node, hashtable* hashtable) noexcept : cur(node), ht(hashtable) {};

		hashtable_iterator(const hashtable_iterator& rhs) noexcept : cur(rhs.cur), ht(rhs.ht) {}
		hashtable_iterator(hashtable_iterator&& rhs) noexcept : cur(rhs.cur), ht(rhs.ht) {
			rhs.cur = nullptr;
			rhs.ht = nullptr;
		}
		

		// 赋值运算符
		iterator& operator=(const hashtable_iterator& rhs) {
			if (this != &rhs) {
				cur = rhs.cur;
				ht = rhs.ht;
			}
			return *this;
		}

		iterator& operator=(hashtable_iterator&& rhs) {
			if (this != &rhs) {
				cur = rhs.cur;
				ht = rhs.ht;
				rhs.cur = nullptr;
				rhs.ht = nullptr;
			}
			return *this;
		}

		// 重载运算符
		reference operator*() const { return cur->value; }
		pointer operator->() const { return &(operator*()); }

		iterator& operator++() {
			ESGSTL_DEBUG(cur != nullptr);
			const node* old = cur;
			cur = cur->next;
			if (cur == nullptr) {
				auto index = ht->bkt_num(old->value);
				while (!cur && ++index < ht->bucket_size_) {
					cur = ht->buckets_[index];
				}
			}
			return *this;
		}
		iterator operator++(int) {
			iterator tmp(*this);
			++(*this);
			return tmp;
		}

	};

	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	struct hashtable_const_iterator {
		typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>					hashtable;
		typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>			iterator;
		typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>		const_iterator;
		typedef hashtable_node<Value>													node;

		typedef size_t										size_type;
		typedef esgstl::forward_iterator_tag				iterator_category;
		typedef Value										value_type;
		typedef const Value*								pointer;
		typedef const Value&								reference;
		typedef ptrdiff_t									difference_type;

		node* cur; // 当前迭代器所指节点
		hashtable* ht; // 当前迭代器与hashtable的连接

		// 构造函数
		hashtable_const_iterator() noexcept : cur(nullptr), ht(nullptr) {}
		hashtable_const_iterator(node* node, hashtable* hashtable) noexcept : cur(node), ht(hashtable) {};

		hashtable_const_iterator(const const_iterator& rhs) noexcept : cur(rhs.cur), ht(rhs.ht) {}
		hashtable_const_iterator(const_iterator&& rhs) noexcept : cur(rhs.cur), ht(rhs.ht) {
			rhs.cur = nullptr;
			rhs.ht = nullptr;
		}
		hashtable_const_iterator(const iterator& rhs) noexcept : cur(rhs.cur), ht(rhs.ht) {}

		// 赋值运算符
		const_iterator& operator=(const const_iterator& rhs) {
			if (this != &rhs) {
				cur = rhs.cur;
				ht = rhs.ht;
			}
			return *this;
		}

		const_iterator& operator=(const_iterator&& rhs) {
			if (this != &rhs) {
				cur = rhs.cur;
				ht = rhs.ht;
				rhs.cur = nullptr;
				rhs.ht = nullptr;
			}
			return *this;
		}

		const_iterator& operator=(const iterator& rhs) {
			cur = rhs.cur;
			ht = rhs.ht;
			return *this;
		}

		// 重载运算符
		reference operator*() const { return cur->value; }
		pointer operator->() const { return &(operator*()); }

		const_iterator& operator++() {
			ESGSTL_DEBUG(cur != nullptr);
			const node* old = cur;
			cur = cur->next;
			if (cur == nullptr) {
				auto index = ht->bkt_num(old->value);
				while (!cur && ++index < ht->bucket_size_) {
					cur = ht->buckets_[index];
				}
			}
			return *this;
		}
		const_iterator operator++(int) {
			const_iterator tmp(*this);
			++(*this);
			return tmp;
		}
	};

	// 比较运算符,不允许比较大小
	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	bool operator==(const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>& lhs,
		const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>& rhs)
	{ return lhs.cur == rhs.cur; }

	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	bool operator!=(const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>& lhs,
		const hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>& rhs)
	{ return lhs.cur != rhs.cur; }

	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	bool operator==(const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>& lhs,
		const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>& rhs)
	{ return lhs.cur == rhs.cur; }

	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	bool operator!=(const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>& lhs,
		const hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>& rhs)
	{ return lhs.cur != rhs.cur; }

#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
#define SYSTEM_64 1
#else
#define SYSTEM_32 1
#endif

#ifdef SYSTEM_64

#define PRIME_NUM 99
	// 1. start with p = 101
	// 2. p = next_prime(p * 1.7)
	// 3. if p < (2 << 63), go to step 2, otherwise, go to step 4
	// 4. end with p = prev_prime(2 << 63 - 1)
	static constexpr size_t ht_prime_list[] = {
	  101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
	  4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
	  119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
	  2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
	  23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
	  264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
	  2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
	  15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
	  115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull,
	  585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull,
	  2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
	  15013704261371ull, 22520556392057ull, 33780834588157ull, 50671251882247ull,
	  76006877823377ull, 114010316735089ull, 171015475102649ull, 256523212653977ull,
	  384784818980971ull, 577177228471507ull, 865765842707309ull, 1298648764060979ull,
	  1947973146091477ull, 2921959719137273ull, 4382939578705967ull, 6574409368058969ull,
	  9861614052088471ull, 14792421078132871ull, 22188631617199337ull, 33282947425799017ull,
	  49924421138698549ull, 74886631708047827ull, 112329947562071807ull, 168494921343107851ull,
	  252742382014661767ull, 379113573021992729ull, 568670359532989111ull, 853005539299483657ull,
	  1279508308949225477ull, 1919262463423838231ull, 2878893695135757317ull, 4318340542703636011ull,
	  6477510814055453699ull, 9716266221083181299ull, 14574399331624771603ull, 18446744073709551557ull
	};

#else

#define PRIME_NUM 44

	// 1. start with p = 101
	// 2. p = next_prime(p * 1.7)
	// 3. if p < (2 << 31), go to step 2, otherwise, go to step 4
	// 4. end with p = prev_prime(2 << 31 - 1)
	static constexpr size_t ht_prime_list[] = {
	  101u, 173u, 263u, 397u, 599u, 907u, 1361u, 2053u, 3083u, 4637u, 6959u,
	  10453u, 15683u, 23531u, 35311u, 52967u, 79451u, 119179u, 178781u, 268189u,
	  402299u, 603457u, 905189u, 1357787u, 2036687u, 3055043u, 4582577u, 6873871u,
	  10310819u, 15466229u, 23199347u, 34799021u, 52198537u, 78297827u, 117446801u,
	  176170229u, 264255353u, 396383041u, 594574583u, 891861923u, 1337792887u,
	  2006689337u, 3010034021u, 4294967291u,
	};

#endif

	// 找出最接近并大于等于 n 的那个质数
	inline size_t ht_next_prime(size_t n) {
		const size_t* first = ht_prime_list;
		const size_t* last = ht_prime_list + PRIME_NUM;
		const size_t* pos = esgstl::lower_bound(first, last, n);
		return pos == last ? *(last - 1) : *pos;
	}


	template <class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	class hashtable {
		friend struct hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>;
		friend struct hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>;
	public:
		typedef hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>					self;
		typedef hashtable_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>			iterator;
		typedef hashtable_const_iterator<Value, Key, HashFcn, ExtractKey, EqualKey>		const_iterator;
		
		typedef hashtable_node<Value>				node;
		typedef node*								node_ptr;
		typedef esgstl::vector<node*>				bucket_type;

		typedef Key						key_type;
		typedef Value					value_type;
		typedef HashFcn					hasher;
		typedef EqualKey				key_equal;
		typedef size_t					size_type;

		typedef esgstl::allocator<node>				node_allocator;
		typedef esgstl::allocator<value_type>		data_allocator;

	private:
		bucket_type buckets_;		// 底层容器，存放链表的vector
		size_type	bucket_size_;	// vector的大小
		size_type	element_size_;	// hashtable中元素的个数
		float mlf_;					// hashtable的满载指数，默认为1，调大的话可以存放更多元素
		hasher hash_;				// 计算hash值的仿函数
		key_equal equal_;			// 比较键值是否相等的仿函数
		ExtractKey get_key_;		// 从value_type中获取键值的仿函数

	public:
		// hashtable没有默认构造器
		explicit hashtable(size_type bucket_count,
			const HashFcn& hash = HashFcn(),
			const EqualKey& equal = EqualKey())
			: element_size_(0), mlf_(1.0f), hash_(hash), equal_(equal), get_key_(ExtractKey()) {
			init(bucket_count);
		}
		template <class InputIter, 
			typename std::enable_if<esgstl::is_input_iterator<InputIter>::value, int>::type = 0>
		hashtable(InputIter first, InputIter last,
			const HashFcn& hash = HashFcn(),
			const EqualKey& equal = EqualKey())
			: element_size_(0), mlf_(1.0f), hash_(hash), equal_(equal), get_key_(ExtractKey()) {
			init(esgstl::distance(first,last));
		}

		hashtable(const hashtable& rhs) 
			: mlf_(rhs.mlf_),hash_(rhs.hash_), equal_(rhs.equal_), get_key_(rhs.get_key_) {
			copy_init(rhs);
		}
		hashtable(hashtable&& rhs) noexcept
			: bucket_size_(rhs.bucket_size_),
			element_size_(rhs.element_size_),
			mlf_(rhs.mlf_),
			hash_(rhs.hash_),
			equal_(rhs.equal_),
			get_key_(rhs.get_key_) {
			buckets_ = esgstl::move(rhs.buckets_);
			rhs.bucket_size_ = 0;
			rhs.element_size_ = 0;
			rhs.mlf_ = 0.0;
		}

		self& operator=(const hashtable& rhs);
		self& operator=(hashtable&& rhs) noexcept;

		~hashtable() { clear(); }

		// 迭代器相关函数
		/*****************************************************************/
		iterator begin() noexcept { return _begin(); }
		const_iterator begin() const noexcept { return _begin(); }

		iterator end() noexcept { return iterator(nullptr, this); }
		const_iterator end() const noexcept { return _const_iter(nullptr); }

		const_iterator cbegin() const noexcept { return begin(); }
		const_iterator cend() const noexcept { return end(); }
		/*****************************************************************/

		// 容量相关函数
		/*****************************************************************/
		bool empty() const noexcept { return element_size_ == 0; }
		size_type size() const noexcept { return element_size_; }
		size_type max_size() const noexcept { return static_cast<size_type>(-1); }
		/*****************************************************************/

		// 修改容器相关操作
		/*****************************************************************/
		/// emplace系列
		template <class ...Args>
		iterator emplace_multi(Args&& ...args);
		template <class ...Args>
		pair<iterator,bool> emplace_unique(Args&& ...args);

		/// insert系列
		iterator insert_multi_noresize(const value_type& value);
		pair<iterator, bool> insert_unique_noresize(const value_type& value);

		iterator insert_multi(const value_type& value) {
			rehash_if_need(1);
			return insert_multi_noresize(value);
		}
		pair<iterator, bool> insert_unique(const value_type& value) {
			rehash_if_need(1);
			return insert_unique_noresize(value);
		}

		iterator insert_multi(value_type&& value) {
			return emplace_multi(esgstl::move(value));
		}
		pair<iterator, bool> insert_unique(value_type&& value) {
			return emplace_unique(esgstl::move(value));
		}

		template <class InputIter>
		void insert_multi(InputIter first, InputIter last) {
			copy_insert_multi(first, last);
		}
		template <class InputIter>
		void insert_unique(InputIter first, InputIter last) {
			copy_insert_unique(first, last);
		}

		/// erase系列
		void erase(const_iterator pos);
		void erase(const_iterator first, const_iterator last);

		size_type erase_multi(const key_type& key);
		size_type erase_unique(const key_type& key);

		void clear();
		/*****************************************************************/

		/// swap工具函数
		void swap(hashtable& rhs) noexcept;

		// 空间重整相关操作
		/*****************************************************************/
		/// rehash：根据给定的大小，判断是否需要重新拿个容器来装元素
		void rehash(size_type intend_bucket_count);
		void rehash_if_need(size_type need_bucket_count);

		/// reserve：预先准备空间
		void reserve(size_type n) {
			rehash(static_cast<size_type>(float(n) / max_load_factor() + 0.5f));
		}
		/*****************************************************************/

		// 查找相关操作
		/*****************************************************************/
		/// count
		size_type count(const key_type& key) const;

		/// find
		iterator find(const key_type& key);
		const_iterator find(const key_type& key) const;

		/// equal_range系列
		pair<iterator, iterator> eqaul_range_multi(const key_type& key);
		pair<const_iterator, const_iterator> eqaul_range_multi(const key_type& key) const;

		pair<iterator, iterator> eqaul_range_unique(const key_type& key);
		pair<const_iterator, const_iterator> eqaul_range_unique(const key_type& key) const;
		/*****************************************************************/

		// hashtable属性相关操作
		/*****************************************************************/
		/// bucket尺寸相关函数
		size_type bucket_size() const noexcept { return bucket_size_; }
		size_type max_bucket_size() const noexcept { return ht_prime_list[PRIME_NUM-1]; }

		/// load_factor：负载因子相关函数
		float load_factor() const noexcept {
			return element_size_ == 0 ? 0.0f : ((float)element_size_ / (float)bucket_size_);
		}
		float max_load_factor() const noexcept { return mlf_; }
		void set_max_load_factor(float mlf) {
			ESGSTL_DEBUG(mlf == mlf && mlf > 0.0f);
			mlf_ = mlf;
		}

		/// 返回bucket_供遍历
		const bucket_type& get_buckets() const { return buckets_; }

		/// 返回仿函数对象
		hasher hash_fcn() const { return hash_; }
		key_equal key_eq() const { return equal_; }
		/*****************************************************************/

	private:
		// 迭代器相关辅助函数
		const_iterator _const_iter(node_ptr node) const {
			return const_iterator(node, const_cast<hashtable*>(this));
		}
		iterator _begin() noexcept {
			for (size_type i = 0; i < bucket_size_; ++i) {
				if (buckets_[i]) return iterator(buckets_[i], this);
			}
			return iterator(nullptr, this);
		}
		const_iterator _begin() const noexcept {
			for (size_type i = 0; i < bucket_size_; ++i) {
				if (buckets_[i]) return _const_iter(buckets_[i]);
			}
			return _const_iter(nullptr);
		}

		// 初始化辅助函数
		void init(size_type bucket_count);
		void copy_init(const hashtable& rhs);

		// hash 相关辅助函数
		size_type next_size(size_type bucket_count);
		/// bkt_num：获取基于hash值的应置bucket下标
		size_type bkt_num(const value_type& value) const noexcept { return bkt_num_key(get_key_(value)); }
		size_type bkt_num(const value_type& value, size_type bucket_count) const noexcept{
			return bkt_num_key(get_key_(value),bucket_count); 
		}
		size_type bkt_num_key(const key_type& key) const noexcept { return bkt_num_key(key,bucket_size_); }
		size_type bkt_num_key(const key_type& key, size_type bucket_count) const noexcept {
			return hash_(key) % bucket_count;
		}

		// node相关函数
		template <class ...Args>
		node_ptr create_node(Args&& ...args);
		void delete_node(node_ptr node);

		// emplace真实逻辑函数
		iterator insert_node_multi(node_ptr node);
		pair<iterator, bool> insert_node_unique(node_ptr node);

		// insert区间真实逻辑函数
		template <class InputIter>
		void copy_insert_multi(InputIter first, InputIter last);
		template <class InputIter>
		void copy_insert_unique(InputIter first, InputIter last);

		// erase区间辅助函数
		void erase_bucket(size_type bucket_num, node_ptr last);
		void erase_bucket(size_type bucket_num, node_ptr first, node_ptr last);
		
		// replace_buckets：重新开辟空间并将原节点移动到新空间
		void replace_buckets(size_type bucket_count);

	};

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	operator=(const hashtable& rhs) {
		if (this != &rhs) {
			hashtable tmp(rhs);
			swap(rhs);
		}
		return *this;
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>&
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	operator=(hashtable&& rhs) noexcept {
		hashtable tmp(rhs);
		swap(tmp);
		return *this;
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	insert_multi_noresize(const value_type& value) {
		const auto bkt_num_ = bkt_num(value, bucket_size_);
		auto first = buckets_[bkt_num_];
		for (auto cur = first; cur; cur = cur->next) {
			if (equal_(get_key_(cur->value), get_key_(value))) {
				node_ptr node = create_node(value);
				node->next = cur->next;
				cur->next = node;
				++element_size_;
				return iterator(node, this);
			}
		}
		node_ptr node = create_node(value);
		node->next = first;
		buckets_[bkt_num_] = node;
		++element_size_;
		return iterator(node, this);
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator, bool>
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	insert_unique_noresize(const value_type& value) {
		const auto bkt_num_ = bkt_num(value, bucket_size_);
		auto first = buckets_[bkt_num_];
		for (auto cur = first; cur; cur = cur->next) {
			if (equal_(get_key_(cur->value), get_key_(value))) {
				return esgstl::make_pair(iterator(cur, this), false);
			}
		}
		node_ptr node = create_node(value);
		node->next = first;
		buckets_[bkt_num_] = node;
		++element_size_;
		return esgstl::make_pair(iterator(node, this), true);
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	erase(const_iterator pos) {
		auto node = pos.cur;
		if (node == nullptr) return;
		size_type bkt_index = bkt_num(node->value);
		if (buckets_[bkt_index] == node) {
			buckets_[bkt_index] = node->next;
			delete_node(node);
			--element_size_;
		}
		else {
			node_ptr cur = buckets_[bkt_index];
			for (auto nxt = cur->next; nxt; nxt = nxt->next, cur = cur->next) {
				if (nxt == node) {
					cur->next = nxt->next;
					delete_node(nxt);
					--element_size_;
					return;
				}
			}
		}
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	erase(const_iterator first, const_iterator last) {
		if (first == last) return;
		size_type index1 = first.cur ? bkt_num(*first):bucket_size_;
		size_type index2 = last.cur ? bkt_num(*last):bucket_size_;
		if (index1 == index2) erase_bucket(index1, first.cur, last.cur);
		else {
			erase_bucket(index1, first.cur, nullptr);
			for (auto i = index1 + 1; i < index2; ++i) {
				if (buckets_[i]) erase_bucket(i, nullptr);
			}
			if (index2 != bucket_size_) erase_bucket(index2, last.cur);
		}
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::size_type 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	erase_multi(const key_type& key) {
		size_type old_size = element_size_;
		auto p = eqaul_range_multi(key);
		erase(p.first, p.second);
		return old_size - element_size_;
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::size_type 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::erase_unique(const key_type& key) {
		size_type old_size = element_size_;
		auto p = eqaul_range_unique(key);
		erase(p.first, p.second);
		return old_size - element_size_;
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::clear() {
		if (element_size_ == 0) return;
		for (size_type i = 0; i < bucket_size_; ++i) {
			node_ptr first = buckets_[i];
			if (first) {
				erase_bucket(i, nullptr);
			}
		}
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::size_type 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::count(const key_type& key) const {
		const auto index = bkt_num_key(key);
		if (buckets_[index] == nullptr) return 0;
		size_type res = 0;
		for (auto cur = buckets_[index]; cur; cur = cur->next) {
			if (equal_(get_key_(cur->value),key)) ++res;
		}
		return res;
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::find(const key_type& key) {
		const auto index = bkt_num_key(key);
		auto cur = buckets_[index];
		for (; cur; cur = cur->next) {
			if (equal_(get_key_(cur->value), key)) break;
		}
		return iterator(cur,this);
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::const_iterator
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::find(const key_type& key) const {
		const auto index = bkt_num_key(key);
		auto cur = buckets_[index];
		for (; cur; cur = cur->next) {
			if (equal_(get_key_(cur->value), key)) break;
		}
		return _const_iter(cur);
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator, 
		typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator> 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	eqaul_range_multi(const key_type& key) {
		const auto index = bkt_num_key(key);
		auto first = buckets_[index];
		for (; first; first = first->next) {
			if (equal_(get_key_(first->value), key)) break;
		}
		// 若区间头就没找到，直接返回空区间
		if (first == nullptr) 
			return esgstl::make_pair(iterator(nullptr, this), iterator(nullptr, this));
		auto second = first->next;
		while (second && equal_(get_key_(second->value), key)) second = second->next;
		// 若second位于bucket末尾的空节点，则令它等于下一个有元素的bucket的头
		// 目的是形成有效的迭代器，便于erase进行删除等
		if (second == nullptr) {
			for (size_type i = index + 1; i < bucket_size_; ++i)
				if (buckets_[i]) {
					second = buckets_[i];
					break;
				}
		}
		return esgstl::make_pair(iterator(first, this), iterator(second, this));
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::const_iterator,
		typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::const_iterator> 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	eqaul_range_multi(const key_type& key) const {
		const auto index = bkt_num_key(key);
		auto first = buckets_[index];
		for (; first; first = first->next) {
			if (equal_(get_key_(first->value), key)) break;
		}
		// 若区间头就没找到，直接返回空区间
		if (first == nullptr)
			return esgstl::make_pair(_const_iter(nullptr), _const_iter(nullptr));
		auto second = first->next;
		while (second && equal_(get_key_(second->value), key)) second = second->next;
		// 若second位于bucket末尾的空节点，则令它等于下一个有元素的bucket的头
		// 目的是形成有效的迭代器，便于erase进行删除等
		if (second == nullptr) {
			for (size_type i = index + 1; i < bucket_size_; ++i)
				if (buckets_[i]) {
					second = buckets_[i];
					break;
				}
		}
		return esgstl::make_pair(_const_iter(first), _const_iter(second));
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator,
		typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator>
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	eqaul_range_unique(const key_type& key) {
		const auto index = bkt_num_key(key);
		auto first = buckets_[index];
		for (; first; first = first->next) {
			if (equal_(get_key_(first->value), key)) break;
		}
		// 若区间头就没找到，直接返回空区间
		if (first == nullptr)
			return esgstl::make_pair(iterator(nullptr, this), iterator(nullptr, this));
		auto second = first->next;
		// 若second位于bucket末尾的空节点，则令它等于下一个有元素的bucket的头
		// 目的是形成有效的迭代器，便于erase进行删除等
		if (second == nullptr) {
			for (size_type i = index + 1; i < bucket_size_; ++i)
				if (buckets_[i]) {
					second = buckets_[i];
					break;
				}
		}
		return esgstl::make_pair(iterator(first, this), iterator(second, this));
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::const_iterator,
		typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::const_iterator>
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	eqaul_range_unique(const key_type& key) const {
		const auto index = bkt_num_key(key);
		auto first = buckets_[index];
		for (; first; first = first->next) {
			if (equal_(get_key_(first->value), key)) break;
		}
		// 若区间头就没找到，直接返回空区间
		if (first == nullptr)
			return esgstl::make_pair(_const_iter(nullptr), _const_iter(nullptr));
		auto second = first->next;
		// 若second位于bucket末尾的空节点，则令它等于下一个有元素的bucket的头
		// 目的是形成有效的迭代器，便于erase进行删除等
		if (second == nullptr) {
			for (size_type i = index + 1; i < bucket_size_; ++i)
				if (buckets_[i]) {
					second = buckets_[i];
					break;
				}
		}
		return esgstl::make_pair(_const_iter(first), _const_iter(second));
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	rehash(size_type intend_bucket_count) {
		const size_type n = next_size(intend_bucket_count);
		// 所需bucket的大小大于当前buckets的最大承载能力
		if ((float)n > (float)bucket_size_ * max_load_factor()) {
			replace_buckets(n);
		}
		// 所需bucket的大小小于当前的buckets的大小，并且此时负载量较小，
		else if ((float)element_size_ / (float)n < max_load_factor() - 0.25f &&
			(float)n < (float)bucket_size_ * 0.75f) {
			replace_buckets(n);
		}
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::rehash_if_need(size_type need_bucket_count) {
		if ((float)(element_size_ + need_bucket_count) > (float)bucket_size_ * max_load_factor())
			rehash(element_size_ + need_bucket_count);
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	init(size_type bucket_count) {
		auto bucket_num = next_size(bucket_count);
		buckets_.reserve(bucket_num);
		buckets_.assign(bucket_num, nullptr);
		bucket_size_ = buckets_.size();
		element_size_ = 0;
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	copy_init(const hashtable& rhs) {
		buckets_.reserve(rhs.bucket_size_);
		buckets_.assign(rhs.bucket_size_, nullptr);
		try {
			for (size_type i = 0; i < rhs.bucket_size_; ++i) {
				if (rhs.buckets_[i]) {
					auto cur = rhs.buckets_[i];
					auto newnode = create_node(cur->value);
					buckets_[i] = newnode;
					while (cur->next != nullptr) {
						newnode->next = create_node(cur->next->value);
						newnode = newnode->next;
						cur = cur->next;
					}
				}
			}
		}
		catch (...) {
			clear();
		}
		bucket_size_ = rhs.bucket_size_;
		element_size_ = rhs.element_size_;
	}

	// 获取大于等于给定的bucket_count的最小质数（用作新的bucket_size_）
	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::size_type 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	next_size(size_type bucket_count) {
		return ht_next_prime(bucket_count);
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	swap(hashtable& rhs) noexcept {
		if (this != &rhs) {
			esgstl::swap(buckets_, rhs.buckets_);
			esgstl::swap(bucket_size_, rhs.bucket_size_);
			esgstl::swap(element_size_, rhs.element_size_);
			esgstl::swap(mlf_, rhs.mlf_);
			esgstl::swap(hash_, rhs.hash_);
			esgstl::swap(equal_, rhs.equal_);
			esgstl::swap(get_key_, rhs.get_key_);
		}
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::delete_node(node_ptr node) {
		if (node != nullptr) {
			data_allocator::destory(&(node->value));
			node_allocator::deallocate(node);
			node = nullptr;
		}
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	insert_node_multi(node_ptr node) {
		const size_type bkt_num_ = bkt_num(node->value, bucket_size_);
		auto first = buckets_[bkt_num_];
		for (auto cur = first; cur; cur = cur->next) {
			if (equal_(get_key_(cur->value), get_key_(node->value))) {
				node->next = cur->next;
				cur->next = node;
				++element_size_;
				return iterator(node, this);
			}
		}
		node->next = first;
		buckets_[bkt_num_] = node;
		++element_size_;
		return iterator(node, this);
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator, bool> 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	insert_node_unique(node_ptr node) {
		const auto bkt_num_ = bkt_num(node->value, bucket_size_);
		auto first = buckets_[bkt_num_];
		for (auto cur = first; cur; cur = cur->next) {
			if (equal_(get_key_(cur->value), get_key_(node->value))) {
				return esgstl::make_pair(iterator(cur, this), false);
			}
		}
		node->next = first;
		buckets_[bkt_num_] = node;
		++element_size_;
		return esgstl::make_pair(iterator(node, this), true);
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	erase_bucket(size_type bucket_num, node_ptr last) {
		node_ptr first = buckets_[bucket_num];
		auto cur = first;
		while (cur != last) {
			first = first->next;
			delete_node(cur);
			cur = first;
			--element_size_;
		}
		buckets_[bucket_num] = last;
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	erase_bucket(size_type bucket_num, node_ptr first, node_ptr last) {
		node_ptr cur = buckets_[bucket_num];
		if (cur == first) {
			erase_bucket(bucket_num, last);
			return;
		}
		while (cur->next != first) cur = cur->next;
		node_ptr tmp = first;
		while (tmp != last) {
			first = first->next;
			delete_node(tmp);
			tmp = first;
			--element_size_;
		}
		cur->next = last;
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	replace_buckets(size_type bucket_count) {
		bucket_type tmp(bucket_count);
		for (size_type i = 0; i < bucket_size_; ++i) {
			auto first = buckets_[i];
			while (first) {
				auto new_bkt_num = bkt_num(first->value, bucket_count);
				buckets_[i] = first->next;
				first->next = tmp[new_bkt_num];
				tmp[new_bkt_num] = first;
				first = buckets_[i];
			}
		}
		buckets_.swap(tmp);
		bucket_size_ = buckets_.size();
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	template<class ...Args>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	emplace_multi(Args && ...args) {
		node_ptr node = create_node(esgstl::forward<Args>(args)...);
		try {
			if (float(element_size_ + 1) > (float)bucket_size_ * max_load_factor())
				replace_buckets(element_size_ + 1);
		}
		catch (...) {
			delete_node(node);
			throw;
		}
		insert_node_multi(node);
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	template<class ...Args>
	inline pair<typename hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::iterator, bool>
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	emplace_unique(Args && ...args) {
		node_ptr node = create_node(esgstl::forward<Args>(args)...);
		try {
			if (float(element_size_ + 1) > (float)bucket_size_ * max_load_factor())
				replace_buckets(element_size_ + 1);
		}
		catch (...) {
			delete_node(node);
			throw;
		}
		insert_node_unique(node);
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	template<class ...Args>
	inline hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::node_ptr 
	hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	create_node(Args && ...args) {
		node_ptr newnode = node_allocator::allocate();
		try {
			data_allocator::construct(&(newnode->value), esgstl::forward<Args>(args)...);
			newnode->next = nullptr;
		}
		catch (...) {
			node_allocator::deallocate(newnode);
			throw;
		}
		return newnode;
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	template<class InputIter>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	copy_insert_multi(InputIter first, InputIter last) {
		auto len = esgstl::distance(first, last);
		rehash_if_need(len);
		while (len > 0) {
			insert_multi_noresize(*first);
			--len; ++first;
		}
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	template<class InputIter>
	inline void hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>::
	copy_insert_unique(InputIter first, InputIter last) {
		auto len = esgstl::distance(first, last);
		rehash_if_need(len);
		while (len > 0) {
			insert_unique_noresize(*first);
			--len; ++first;
		}
	}

	// 重载比较操作符
	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	bool operator==(const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& lhs,
					const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& rhs) {
		return lhs.size() == rhs.size() &&
			esgstl::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	bool operator!=(const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& lhs,
		const hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& rhs) {
		return !(lhs == rhs);
	}

	// 重载swap
	template<class Value, class Key, class HashFcn, class ExtractKey, class EqualKey>
	void swap(hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& lhs,
				hashtable<Value, Key, HashFcn, ExtractKey, EqualKey>& rhs) {
		lhs.swap(rhs);
	}

}