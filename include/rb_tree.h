#pragma once

#include<initializer_list>
#include<cassert>
// #include<iostream>
// #include"queue.h"  配合print_tree，Debug用的

#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "exceptdef.h"
#include "util.h"

namespace esgstl {
	// 定义红颜色为false，黑颜色为true
	typedef bool rb_tree_color_type;
	static constexpr rb_tree_color_type rb_tree_red = false;
	static constexpr rb_tree_color_type rb_tree_black = true;

	// 红黑树存储数据类型提取器（单值类型）
	template <class T, bool>
	struct rb_tree_value_traits_imp {
		typedef T key_type;
		typedef T mapped_type;
		typedef T value_type;

		template <class Ty>
		static const key_type& get_key(const Ty& value) {
			return value;
		}

		template <class Ty>
		static const value_type& get_value(const Ty& value) {
			return value;
		}
	};

	// 红黑树存储数据类型（特化）提取器（键值对类型）
	template <class T>
	struct rb_tree_value_traits_imp <T, true> {
		typedef std::remove_cv<typename T::first_type>::type	key_type;
		typedef T::second_type									mapped_type;
		typedef T												value_type;

		template <class Ty>
		static const key_type& get_key(const Ty& value) {
			return value.first;
		}

		template <class Ty>
		static const value_type& get_value(const Ty& value) {
			return value;
		}
	};

	// 上一层提供用户使用的红黑树存储数据类型提取器
	template <class T>
	struct rb_tree_value_traits {
		static constexpr bool ismap = esgstl::is_pair<T>::value;

		typedef rb_tree_value_traits_imp<T, ismap>				value_traits_type;
		typedef typename value_traits_type::key_type			key_type;
		typedef typename value_traits_type::mapped_type			mapped_type;
		typedef typename value_traits_type::value_type			value_type;

		template <class Ty>
		static const key_type& get_key(const Ty& value) {
			return value_traits_type::get_key(value);
		}

		template <class Ty>
		static const value_type& get_value(const Ty& value) {
			return value_traits_type::get_value(value);
		}
	};

	// 一些前置声明

	template <class T> struct rb_tree_node_base;
	template <class T> struct rb_tree_node;

	template <class T> struct rb_tree_iterator;
	template <class T> struct rb_tree_const_iterator;


	template <class T>
	struct rb_tree_node_traits {
		typedef rb_tree_color_type                 color_type;
		typedef rb_tree_value_traits<T>				value_traits;

		typedef typename value_traits::key_type		key_type;
		typedef typename value_traits::mapped_type	mapped_type;
		typedef typename value_traits::value_type	value_type;

		typedef rb_tree_node_base<T>* base_node_ptr;
		typedef rb_tree_node<T>* node_ptr;
	};

	// rb tree 的节点设计
	// 分为base_node 和node，node多了个value属性，暂时还不知道干嘛这么分
	// 现在知道了，红黑树的header就是一个base_node，因为它不需要存值，而且叶子节点我估计也是base_node

	template <class T>
	struct rb_tree_node_base {
		typedef rb_tree_color_type    color_type;
		typedef rb_tree_node_base<T>* base_node_ptr;
		typedef rb_tree_node<T>* node_ptr;

		base_node_ptr   parent;  // 父节点
		base_node_ptr   left;    // 左子节点
		base_node_ptr   right;   // 右子节点
		color_type color;   // 节点颜色

		base_node_ptr get_base_ptr() {
			return &*this;
		}

		node_ptr get_node_ptr() {
			return reinterpret_cast<node_ptr>(&*this);
		}

		node_ptr& get_node_ref() {
			return reinterpret_cast<node_ptr&>(*this);
		}
	};

	template <class T>
	struct rb_tree_node :public rb_tree_node_base<T> {
		typedef rb_tree_node_base<T>* base_node_ptr;
		typedef rb_tree_node<T>* node_ptr;

		T value;  // 节点值

		base_node_ptr get_base_ptr() {
			return static_cast<base_node_ptr>(&*this);
		}

		node_ptr get_node_ptr() {
			return &*this;
		}
	};

	// rb tree traits
	// 最顶层的traits了，放在rbtree的类里用来提取各种所需的类型
	template <class T>
	struct rb_tree_traits {
		typedef rb_tree_value_traits<T>				value_traits;

		typedef typename value_traits::key_type		key_type;
		typedef typename value_traits::mapped_type	mapped_type;
		typedef typename value_traits::value_type	value_type;

		typedef value_type*							pointer;
		typedef value_type&							reference;
		typedef const value_type*					const_pointer;
		typedef const value_type&					const_reference;

		typedef rb_tree_node_base<T>				base_node_type;
		typedef rb_tree_node<T>						node_type;

		typedef base_node_type*						base_node_ptr;
		typedef node_type*							node_ptr;
	};

	// 红黑树迭代器，也分为两层结构
	template <class T>
	struct rb_tree_iterator_base : public esgstl::iterator<esgstl::bidirectional_iterator_tag, T> {
		typedef typename rb_tree_traits<T>::base_node_ptr  base_node_ptr;

		base_node_ptr node;  // 底层的节点指针

		rb_tree_iterator_base() :node(nullptr) {}

		// 按照中序遍历的顺序前移迭代器
		void increment() {
			// 如果当前节点有右节点，那么一路走到右子树的最小节点
			if (node->right != nullptr) {
				// 这个函数定义在下面
				node = rb_tree_min(node->right);
			}
			else {
				// 如果没有右节点，则开始向上回溯，直到node是父节点的左节点，此时父节点是我们想要的目标位置
				auto y = node->parent;
				while (node == y->right) {
					node = y;
					y = y->parent;
				}
				// 有一种例外的情况，当前节点是根节点root，而y是header节点，经过上面的循环后
				// y会变成root，node会变成header，此时node->right == y，迭代器当前所在位置是正确的，就不再操作了
				if (node->right == y) return;
				node = y;
			}
		}

		void decrement() {
			// 假如node是header，那它的前一个正好是红黑树的最大节点，即自己的右节点
			if (node->color == rb_tree_red && node->parent->parent == node) {
				node = node->right;
			}
			else if (node->left != nullptr) {
				node = rb_tree_max(node->left);
			}
			else {
				auto y = node->parent;
				while (node == y->left) {
					node = y;
					y = y->parent;
				}
				// 不懂为什么这里下面这句不需要，如果是最小的节点再--没有下面那句最后结果就是根节点
				// if (node->left == y) return;
				node = y;
			}
		}

		bool operator==(const rb_tree_iterator_base& rhs) { return rhs.node == node; }
		bool operator!=(const rb_tree_iterator_base& rhs) { return rhs.node != node; }

	};

	template <class T>
	struct rb_tree_iterator : public rb_tree_iterator_base<T> {
		typedef rb_tree_traits<T>						tree_traits;

		typedef typename tree_traits::value_type		value_type;
		typedef typename tree_traits::pointer			pointer;
		typedef typename tree_traits::reference			reference;
		typedef typename tree_traits::base_node_ptr		base_node_ptr;
		typedef typename tree_traits::node_ptr			node_ptr;

		typedef rb_tree_iterator<T>						iterator;
		typedef rb_tree_const_iterator<T>				const_iterator;
		typedef iterator								self;

		using rb_tree_iterator_base<T>::node;

		rb_tree_iterator() {}
		rb_tree_iterator(base_node_ptr x) { node = x; }
		rb_tree_iterator(node_ptr x) { node = x; }
		rb_tree_iterator(const iterator& rhs) { node = rhs.node; }
		rb_tree_iterator(const const_iterator& rhs) { node = rhs.node; }

		reference operator*() { return node->get_node_ptr()->value; }
		pointer operator->() { return &(operator*()); }

		self& operator++() {
			this->increment();
			return *this;
		}
		self operator++(int) {
			self tmp(this);
			this->increment();
			return tmp;
		}

		self& operator--() {
			this->decrement();
			return *this;
		}
		self operator--(int) {
			self tmp(this);
			this->decrement();
			return tmp;
		}
	};

	template <class T>
	struct rb_tree_const_iterator :public rb_tree_iterator_base<T>
	{
		typedef rb_tree_traits<T>							tree_traits;

		typedef typename tree_traits::value_type			value_type;
		typedef typename tree_traits::const_pointer			pointer;
		typedef typename tree_traits::const_reference		reference;
		typedef typename tree_traits::base_node_ptr			base_node_ptr;
		typedef typename tree_traits::node_ptr				node_ptr;

		typedef rb_tree_iterator<T>							iterator;
		typedef rb_tree_const_iterator<T>					const_iterator;
		typedef const_iterator								self;

		using rb_tree_iterator_base<T>::node;

		// 构造函数
		rb_tree_const_iterator() {}
		rb_tree_const_iterator(base_node_ptr x) { node = x; }
		rb_tree_const_iterator(node_ptr x) { node = x; }
		rb_tree_const_iterator(const iterator& rhs) { node = rhs.node; }
		rb_tree_const_iterator(const const_iterator& rhs) { node = rhs.node; }

		// 重载操作符
		reference operator*()  const { return node->get_node_ptr()->value; }
		pointer   operator->() const { return &(operator*()); }

		self& operator++() {
			this->increment();
			return *this;
		}
		self operator++(int) {
			self tmp(*this);
			this->increment();
			return tmp;
		}
		self& operator--() {
			this->decrement();
			return *this;
		}
		self operator--(int) {
			self tmp(*this);
			this->decrement();
			return tmp;
		}
	};

	// 红黑树通用工具类函数
	/*********************************************************************************/
	template <class NodePtr>
	NodePtr rb_tree_min(NodePtr node) noexcept {
		while (node->left != nullptr) node = node->left;
		return node;
	}
	
	template <class NodePtr>
	NodePtr rb_tree_max(NodePtr node) noexcept {
		while (node->right != nullptr) node = node->right;
		return node;
	}

	template <class NodePtr>
	bool rb_tree_is_lchild(NodePtr node) noexcept {
		return node->parent->left == node;
	}
	
	template <class NodePtr>
	bool rb_tree_is_rchild(NodePtr node) noexcept {
		return node->parent->right == node;
	}

	template <class NodePtr>
	bool rb_tree_is_red(NodePtr node) noexcept {
		return node->color == rb_tree_red;
	}

	template <class NodePtr>
	bool rb_tree_set_red(NodePtr node) noexcept {
		return node->color = rb_tree_red;
	}

	template <class NodePtr>
	bool rb_tree_set_black(NodePtr node) noexcept {
		return node->color = rb_tree_black;
	}

	
	/*---------------------------------------*\
	|       p                         p       |
	|      / \                       / \      |
	|     x   d    rotate left      y   d     |
	|    / \       ===========>    / \        |
	|   a   y                     x   c       |
	|      / \                   / \          |
	|     b   c                 a   b         |
	\*---------------------------------------*/
	// 左旋节点，即让目标节点的右子节点来代替自己的位置
	// 因此进行左旋的目标节点必须有右节点
	template <class NodePtr>
	void rb_tree_left_rotate(NodePtr x, NodePtr& root) noexcept {
		// 注意，这里root是传递的指针的引用，因此在这个函数里是有可能改变root的
		// 这个root通常是传入header->parent，修改这个root会使header指向别的节点
		auto y = x->right;
		x->right = y->left;
		if (y->left != nullptr)
			y->left->parent = x;
		y->parent = x->parent;
		// 如果x和root指的是同一节点，则替换root为y
		if (x == root) {
			root = y;
		}
		else if(rb_tree_is_lchild(x)) {
			x->parent->left = y;
		}
		else {
			x->parent->right = y;
		}
		x->parent = y;
		y->left = x;
	}

	
	/*----------------------------------------*\
	|     p                         p          |
	|    / \                       / \         |
	|   d   x      rotate right   d   y        |
	|      / \     ===========>      / \       |
	|     y   a                     b   x      |
	|    / \                           / \     |
	|   b   c                         c   a    |
	\*----------------------------------------*/
	// 右旋节点，即让目标节点的左子节点来代替自己的位置
	// 因此进行右旋的目标节点必须有左节点
	template <class NodePtr>
	void rb_tree_right_rotate(NodePtr x, NodePtr& root) {
		auto y = x->left;
		x->left = y->right;
		if (y->right != nullptr) {
			y->right->parent = x;
		}
		y->parent = x->parent;
		if (x == root) {
			root = y;
		}
		else if (rb_tree_is_lchild(x)) {
			x->parent->left = y;
		}
		else {
			x->parent->right = y;
		}
		x->parent = y;
		y->right = x;
	}

	// 插入新节点后，平衡红黑树
	// 插入的情况比较少，按模式来分的话只有三种
	// 1.父节点是黑节点，不用处理，直接是平衡的
	// 2.父节点是红的，并且叔节点也是红的，这时两边黑节点数在没插入节点之前肯定就是相等的，只需要处理不让红节点
	// 相邻即可，将父节点和叔节点都变黑，而爷节点变红，爷节点的子树就平衡了，但是整颗树不一定平衡，将爷节点所在
	// 子树想象成一个新插入的红节点，再去递归平衡整棵树
	// 3.父节点是红的，叔节点是黑的，现在再插入一个红节点，那父节点这边怎么说也比叔节点那边要长2
	// 因此要通过旋转手段匀一点去叔子树那边
	// 如果爷-父-子在同一条线上，那好办，就旋转爷节点就可以，但是如果不在一条直线上，则需要先旋转一次父节点
	// 旋转完后，使父节点变黑，爷节点变红即可
	template <class NodePtr>
	void rb_tree_insert_balance(NodePtr x, NodePtr& root) {
		rb_tree_set_red(x);
		while (x != root && rb_tree_is_red(x->parent)) {
			// 如果父节点是左节点
			if (rb_tree_is_lchild(x->parent)) {
				auto f = x->parent;			// 父节点
				auto y = f->parent;			// 爷节点
				auto s = y->right;			// 叔节点
				// 如果叔节点不为空，并且它是红的
				if (s != nullptr && rb_tree_is_red(s)) {
					rb_tree_set_black(f);	// 父变黑
					rb_tree_set_black(s);	// 叔变黑
					rb_tree_set_red(y);		// 爷变红
					x = y;					// 视角变为爷，向上递归
				}
				// 如果叔节点是黑的
				else {
					// 如果爷-父-子不同线，则先旋转父节点，使得三者同线（在左）
					if (rb_tree_is_rchild(x)) {
						rb_tree_left_rotate(f,root);
						// 旋转完成后当前父子关系对调了
						esgstl::swap(x, f);
					}
					// 如果爷-父-子同线(并且均为左，则需要右旋爷节点)
					rb_tree_right_rotate(y, root);
					rb_tree_set_black(f);	// 父变黑
					rb_tree_set_red(y);		// 爷变红
				}
			}
			// 如果父节点是右节点
			else {
				auto f = x->parent;			// 父节点
				auto y = f->parent;			// 爷节点
				auto s = y->left;			// 叔节点
				// 如果叔节点是红的
				if (s != nullptr && rb_tree_is_red(s)) {
					rb_tree_set_black(f);	// 父变黑
					rb_tree_set_black(s);	// 叔变黑
					rb_tree_set_red(y);		// 爷变红
					x = y;					// 视角变为爷，向上递归
				}
				// 如果叔节点是黑的
				else {
					// 如果爷-父-子不同线，则先旋转父节点，使得三者同线（在右）
					if (rb_tree_is_lchild(x)) {
						rb_tree_right_rotate(f,root);
						// 旋转完成后当前父子关系对调了
						esgstl::swap(x, f);
					}
					// 如果爷-父-子同线(并且均为右，则需要左旋爷节点)
					rb_tree_left_rotate(y, root);
					rb_tree_set_black(f);	// 父变黑
					rb_tree_set_red(y);		// 爷变红
				}
			}
		}
		// 根节点永远为黑
		rb_tree_set_black(root);
	}


	// 给定要删除的节点，平衡红黑树
	// 删除的情况比较复杂，正向思维的代码容易让人感到混乱
	// 总共有4种模式：①单个红节点直接删除 ②拥有一个红节点的黑节点，红黑节点值互换，删除红
	// ③拥有两个子节点，找到能够替代自己的最接近的节点，互换节点值，再按①②处理
	// ④单个黑节点，将与兄节点和侄节点的颜色有关，又细分四种情况
	// 因此，需要想办法再精简一下，将除了④以外的情况都弄成一种情况
	// 经过观察，①②③总是收敛到①，即最后都变成了直接删掉红节点的情况
	// 所以采用逆向思维，先找到删掉z这个节点之后会填到这个节点上去的替代节点x
	// 这个节点的颜色受到待删除节点以及自身颜色的双重影响，会有红+黑、黑+黑的情况，前者对应①②③，后者对应④
	template <class NodePtr>
	void rb_tree_erase_balance(NodePtr z, NodePtr& root, NodePtr& minNode, NodePtr& maxNode) {
		// 就是因为不仅要删，还要同步更新minNode和maxNode导致正向逻辑更加麻烦
		// 现在先来找到替代节点x，第一个问题是，如果要删的节点有一个或者没有子节点，则替代节点就是它的一个子节点①
		// 但是如果要删的节点有两个子节点，则需要找到一个离他值最近的节点，先交换值和颜色，然后才会变回上面的情况②
		// 因此还需要一个中间变量就是y，用来存可能需要交换的节点（y=z就是①，y!=z就是②）
		NodePtr y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_max(z->left);
		// 注意：因为我们选的是离z最近的小值，因此x无论是否为空都在y的左边
		NodePtr x = y->left != nullptr ? y->left : y->right;
		// 因为x可能是nullptr所以有必要用x_parent来存储x的父节点
		NodePtr x_parent = nullptr;

		// 不等于z的时候说明需要交换y和z的值和颜色
		// 注意：不能直接swap两个value，因为value的key部分是const的，无法重新赋值
		if (y != z) {
			// 这里又分两种情况(abcd可能是null也可能是节点）
			//				z						z
			//			y		a				a		b
			//		x		b				  c	  y
			//									x   d
			// 如果y就是z的左节点，那么把y移上去x就不用动了，已经在正确位置了
			// 否则，先移动y上去，还要把x吸到y的父节点去

			// 先把z的右子树连给y的右边，y的右边一定是空的
			z->right->parent = y;
			y->right = z->right;

			// 下一步把x放到正确位置去，把z的左子树（如果有的话）给y的左边
			// 如果y就是z的左节点，交换上去，那x的父节点就是y，不用动了
			if (y == z->left) {
				x_parent = y;
			}
			// 否则交换上去后x就应该吸到y的父节点上去
			else {
				x_parent = y->parent;
				if (x) x->parent = y->parent;
				y->parent->right = x;

				// z的左子树给y的左边
				z->left->parent = y;
				y->left = z->left;
			}
			
			// 最后把z的parent连给y，检查z是否是root
			y->parent = z->parent;
			if (z == root) root = y;
			else if (rb_tree_is_lchild(z)) z->parent->left = y;
			else z->parent->right = y;
			// color可以直接swap
			esgstl::swap(y->color, z->color);
			y = z;
		}
		// y == z的时候说明不需要交换y和z，直接切断z，连上x就行了
		else {
			// 让z的parent连上x
			x_parent = z->parent;
			if (x) x->parent = z->parent;
			
			// 如果z是根节点的话，要变动root指针的指向，相当于直接切断z连上x了
			if (z == root) root = x;
			else if (rb_tree_is_lchild(z)) z->parent->left = x;
			else z->parent->right = x;
			
			// 现在这种情况z是有可能为最大值或者最小值的
			if (z == maxNode) maxNode = (x == nullptr) ? x_parent : rb_tree_max(x_parent);
			if (z == minNode) minNode = (x == nullptr) ? x_parent : rb_tree_min(x_parent);
		}

		// 此时y(z)指向准备删除的节点，x指向替换z的节点，要处理的情况剩下删除单黑节点的四种
		// y是红的不用处理，只有是黑的时候才要处理
		if (!rb_tree_is_red(y)) {
			while (x != root) {
				// 如果x是左节点
				if (x == x_parent->left) {
					auto x_brother = x_parent->right; // 兄弟节点
					auto x_shunzhi = x_brother->left; // 顺侄节点（与x同侧）
					auto x_duizhi = x_brother->right; // 对侄节点（与x异侧）
					// 兄红，双侄必黑且不为空，此时兄子树节点数量过多，匀一点过来再调整颜色
					if (rb_tree_is_red(x_brother)) {
						// 先旋父
						rb_tree_left_rotate(x_parent,root);
						// 兄变黑
						rb_tree_set_black(x_brother);
						// 父变红
						rb_tree_set_red(x_parent);
						// 还是拿x做为调整对象，继续循环
					}
					// 兄黑
					else {
						// 双侄黑
						if (!rb_tree_is_red(x_shunzhi) && !rb_tree_is_red(x_duizhi)) {
							// 直接兄变红，父变黑
							rb_tree_set_red(x_brother);
							// 如果父本来就是黑的，那调整对象变为整颗父节点的子树
							if (rb_tree_is_red(x_parent)) {
								rb_tree_set_black(x_parent);
								break; // 递归出口1：双侄黑，父红变黑，兄黑变红，调整完毕
							}
							else {
								x = x_parent;
								x_parent = x->parent;
							}
						}
						// 顺侄黑
						else if (!rb_tree_is_red(x_shunzhi)) {
							// 旋转兄节点，然后互换兄与顺侄节点的颜色，将其转化为对侄黑
							rb_tree_right_rotate(x_brother,root);
							esgstl::swap(x_brother->color, x_shunzhi->color);
						}
						// 对侄黑
						else {
							// 旋转父节点，兄变成父的颜色，父侄变黑
							rb_tree_left_rotate(x_parent,root);
							x_brother->color = x_parent->color;
							rb_tree_set_black(x_parent);
							rb_tree_set_black(x_duizhi);
							break;// 递归出口2:对侄黑，旋父，兄变父色，父侄变黑
						}
					}
				}
				// 如果x是右节点
				else {
					auto x_brother = x_parent->left; // 兄弟节点
					auto x_shunzhi = x_brother->right; // 顺侄节点（与x同侧）
					auto x_duizhi = x_brother->left; // 对侄节点（与x异侧）
					// 兄红，双侄必黑且不为空
					if (rb_tree_is_red(x_brother)) {
						// 先旋父
						rb_tree_right_rotate(x_parent,root);
						// 兄变黑
						rb_tree_set_black(x_brother);
						// 父变红
						rb_tree_set_red(x_parent);
						// 还是拿x做为调整对象，继续循环
					}
					// 兄黑
					else {
						// 双侄黑
						if (!rb_tree_is_red(x_shunzhi) && !rb_tree_is_red(x_duizhi)) {
							// 直接兄变红，父变黑
							rb_tree_set_red(x_brother);
							// 如果父本来就是黑的，那调整对象变为整颗父节点的子树
							if (rb_tree_is_red(x_parent)) {
								rb_tree_set_black(x_parent);
								break; // 递归出口1：对侄黑，父红变黑，兄黑变红，调整完毕
							}
							else {
								x = x_parent;
								x_parent = x->parent;
							}
						}
						// 顺侄黑
						else if (!rb_tree_is_red(x_shunzhi)) {
							// 旋转兄节点，然后互换兄与顺侄节点的颜色，将其转化为对侄黑
							rb_tree_left_rotate(x_brother,root);
							esgstl::swap(x_brother->color, x_shunzhi->color);
						}
						// 对侄黑
						else {
							// 旋转父节点，兄变成父的颜色，父侄变黑
							rb_tree_right_rotate(x_parent,root);
							x_brother->color = x_parent->color;
							rb_tree_set_black(x_parent);
							rb_tree_set_black(x_duizhi);
							break;// 递归出口2:对侄黑，旋父，兄变父色，父侄变黑
						}
					}
				}
			}
		}
	}

	/*********************************************************************************/

	// 红黑树模板类
	template <class T, class Compare>
	class rb_tree {
	public:
		typedef rb_tree_traits<T>							tree_traits;
		typedef rb_tree_value_traits<T>						value_traits;

		typedef typename tree_traits::base_node_type		base_node_type;
		typedef typename tree_traits::base_node_ptr			base_node_ptr;
		typedef typename tree_traits::node_type				node_type;
		typedef typename tree_traits::node_ptr				node_ptr;

		typedef typename tree_traits::key_type				key_type;
		typedef typename tree_traits::mapped_type			mapped_type;
		typedef typename tree_traits::value_type			value_type;

		typedef Compare										key_compare;

		// 分配子
		typedef esgstl::allocator<T>						allocator_type;
		typedef esgstl::allocator<T>						data_allocator;
		typedef esgstl::allocator<base_node_type>			base_node_allocator;
		typedef esgstl::allocator<node_type>				node_allocator;

		// 从分配子里面拿类型，记住T是node里存的数据的类型
		typedef typename allocator_type::pointer			pointer;
		typedef typename allocator_type::const_pointer		const_pointer;
		typedef typename allocator_type::reference			reference;
		typedef typename allocator_type::const_reference	const_reference;
		typedef typename allocator_type::size_type			size_type;
		typedef typename allocator_type::difference_type	difference_type;

		typedef rb_tree_iterator<T>							iterator;
		typedef rb_tree_const_iterator<T>					const_iterator;
		typedef esgstl::reverse_iterator<iterator>			reverse_iterator;
		typedef esgstl::reverse_iterator<const_iterator>	const_reverse_iterator;

		allocator_type get_allocator() const { return node_allocator(); }
		key_compare    key_comp()      const { return key_comp_; }

	private:
		base_node_ptr header_;
		size_type node_count_;
		key_compare key_comp_;

	private:
		base_node_ptr& root() const { return header_->parent; }
		base_node_ptr& leftmost() const { return header_->left; }
		base_node_ptr& rightmost() const { return header_->right; }

	public:
		// 构造、拷贝、移动赋值、析构函数
		rb_tree() { rb_tree_init(); }
		rb_tree(const rb_tree& rhs);
		rb_tree(rb_tree&& rhs) noexcept;
		rb_tree& operator=(const rb_tree& rhs);
		rb_tree& operator=(rb_tree&& rhs) noexcept;

		~rb_tree() { clear(); }

	public:
		// 迭代器相关操作
		iterator begin() noexcept { return leftmost(); }
		const_iterator begin() const noexcept { return leftmost(); }
		
		iterator end() noexcept { return header_; }
		const_iterator end() const noexcept { return header_; }

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
		bool empty() const noexcept { return node_count_ == 0; }
		size_type size() const noexcept { return node_count_; }
		size_type max_size() const noexcept { return static_cast<size_type>(-1); }

	public:
		// 修改容器相关操作
		/// emplace系列
		template <class ...Args>
		iterator emplace_multi(Args... args);

		template <class ...Args>
		esgstl::pair<iterator, bool> emplace_unique(Args... args);

		template <class ...Args>
		iterator emplace_multi_use_hint(iterator hint, Args... args);

		template <class ...Args>
		iterator emplace_unique_use_hint(iterator hint, Args... args);

		/// insert系列
		iterator insert_multi(const value_type& value);
		iterator insert_multi(value_type&& value) {
			return emplace_multi(esgstl::move(value));
		}
		esgstl::pair<iterator, bool> insert_unique(const value_type& value);
		esgstl::pair<iterator, bool> insert_unique(value_type&& value) {
			return emplace_unique(esgstl::move(value));
		}

		iterator insert_multi(iterator hint, const value_type& value) {
			return emplace_multi_use_hint(hint, value);
		}
		iterator insert_multi(iterator hint, value_type&& value) {
			return emplace_multi_use_hint(hint, esgstl::move(value));
		}
		iterator insert_unique(iterator hint, const value_type& value) {
			return emplace_unique_use_hint(hint, value);
		}
		iterator insert_unique(iterator hint, value_type&& value) {
			return emplace_unique_use_hint(hint, esgstl::move(value));
		}

		template <class InputIterator>
		void insert_multi(InputIterator first, InputIterator last) {
			auto n = esgstl::distance(first, last);
			ESGSTL_LENGTH_ERROR_IF(max_size() - n < node_count_, "rb_tree<T, Comp>'s size too big");
			for (; n > 0; --n) {
				insert_multi(end(), *first++);
				// print_tree();
			}
		}

		template <class InputIterator>
		void insert_unique(InputIterator first, InputIterator last) {
			auto n = esgstl::distance(first, last);
			for (; n > 0; --n) {
				insert_unique(end(), *first++);
			}
		}

		//void print_tree() {
		//	std::cout << "/***********************************/" << std::endl;
		//	if (root() != nullptr) {
		//		queue<base_node_ptr> que;
		//		que.push(root());
		//		while (!que.empty()) {
		//			int size = que.size();
		//			for (int i = 0; i < size; ++i) {
		//				auto node = que.front();
		//				que.pop();
		//				if (node->left) que.push(node->left);
		//				if (node->right) que.push(node->right);
		//				std::cout << node->get_node_ptr()->value << '\t';
		//			}
		//			std::cout << std::endl;
		//		}
		//	}
		//	std::cout << "/***********************************/" << std::endl;
		//}

		/// erase系列
		iterator erase(iterator hint);
		size_type erase_multi(const key_type& key);
		size_type erase_unique(const key_type& key);

		void erase(iterator begin, iterator end);
		void clear();

		void swap(rb_tree& rhs) noexcept;

		// 查找元素相关操作
		iterator find(const key_type& key);
		const_iterator find(const key_type& key) const;

		size_type count_multi(const key_type& key) {
			auto p = equal_range_multi(key);
			size_type res = static_cast<size_type>(esgstl::distance(p.first, p.second));
			return res;
		}
		size_type count_unique(const key_type& key) {
			return find(key) == end() ? 0 : 1;
		}

		iterator lower_bound(const key_type& key);
		const_iterator lower_bound(const key_type& key) const;
		iterator upper_bound(const key_type& key);
		const_iterator upper_bound(const key_type& key) const;

		esgstl::pair<iterator,iterator>
		equal_range_multi(const key_type& key) {
			return esgstl::pair<iterator,iterator>(lower_bound(key), upper_bound(key));
		}
		esgstl::pair<const_iterator, const_iterator>
		equal_range_multi(const key_type& key) const {
			return esgstl::pair<const_iterator, const_iterator>(lower_bound(key), upper_bound(key));
		}

		esgstl::pair<iterator, iterator>
		equal_range_unique(const key_type& key) {
			iterator it = find(key);
			auto next = it;
			return it == end() ? esgstl::make_pair(it,it) : esgstl::make_pair(it, ++next);
		}
		esgstl::pair<const_iterator, const_iterator>
		equal_range_unique(const key_type& key) const {
			const_iterator it = find(key);
			auto next = it;
			return it == end() ? esgstl::make_pair(it, it) : esgstl::make_pair(it, ++next);
		}


	private:

		// 初始化相关
		void rb_tree_init();
		void reset();

		// 增删节点相关
		// create_node:新增节点
		template <class ...Args>
		node_ptr create_node(Args&&... args);
		// clone_node:克隆节点
		node_ptr clone_node(base_node_ptr x);
		// destory_node：删除节点
		void destory_node(node_ptr x);

		// 寻找给定键值能够插入的位置
		esgstl::pair<base_node_ptr, bool> 
				get_insert_multi_pos(const key_type& key);
		esgstl::pair<esgstl::pair<base_node_ptr, bool>, bool> 
				get_insert_unique_pos(const key_type& key);

		// 将给定值插入给定的位置
		iterator insert_value_at(base_node_ptr x, const value_type& value, bool add_to_left);
		iterator insert_node_at(base_node_ptr x, node_ptr node, bool add_to_left);

		iterator insert_multi_use_hint(iterator hint, const key_type& key, node_ptr x);
		iterator insert_unique_use_hint(iterator hint, const key_type& key, node_ptr x);

		// 构造函数辅助拷贝函数
		base_node_ptr copy_from(base_node_ptr x, base_node_ptr p);
		// clear 辅助函数
		void erase_since(base_node_ptr x);
	};

	template<class T, class Compare>
	inline rb_tree<T, Compare>::
	rb_tree(const rb_tree& rhs) {
		rb_tree_init();
		if (rhs.node_count_ != 0) {
			root() = copy_from(rhs.root(), header_);
			leftmost() = rb_tree_min(root());
			rightmost() = rb_tree_max(root());
		}
		node_count_ = rhs.node_count_;
		key_comp_ = rhs.key_comp_;
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::
	rb_tree(rb_tree&& rhs) noexcept
		:header_(esgstl::move(rhs.header_)),
		node_count_(rhs.node_count_),
		key_comp_(rhs.key_comp_) {
		rhs.reset();
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>& 
	rb_tree<T, Compare>::
	operator=(const rb_tree& rhs) {
		if (this != &rhs) {
			rb_tree tmp(rhs);
			swap(tmp);
		}
		return *this;
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>& 
	rb_tree<T, Compare>::
	operator=(rb_tree&& rhs) noexcept{
		rb_tree tmp(rhs);
		swap(tmp);
		return *this;
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::iterator
	rb_tree<T, Compare>::
	insert_multi(const value_type& value) {
		ESGSTL_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
		auto p = get_insert_multi_pos(value_traits::get_key(value));
		return insert_node_at(p.first, value, p.second);
	}

	template<class T, class Compare>
	inline esgstl::pair<typename rb_tree<T, Compare>::iterator, bool>
	rb_tree<T, Compare>::
	insert_unique(const value_type& value) {
		ESGSTL_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
		auto p = get_insert_unique_pos(value_traits::get_key(value));
		if (p.second) return esgstl::make_pair(insert_node_at(p.first.first, value, p.first.second), true);
		return esgstl::make_pair(iterator(p.first.first), false);
	}

	template<class T, class Compare>
	inline typename rb_tree<T,Compare>::iterator 
	rb_tree<T, Compare>::
	erase(iterator hint) {
		auto node = hint.node->get_node_ptr();
		iterator next(node);
		++next;
		rb_tree_erase_balance(hint.node, root(), leftmost(), rightmost());
		destory_node(node);
		--node_count_;
		return next;
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::size_type
	rb_tree<T, Compare>::
	erase_multi(const key_type& key) {
		auto p = equal_range_multi(key);
		size_type n = static_cast<size_type>(esgstl::distance(p.first, p.second));
		erase(p.first, p.second);
		return n;
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::size_type
	rb_tree<T, Compare>::
	erase_unique(const key_type& key) {
		auto it = find(key);
		if (it != end()) {
			erase(it);
			return 1;
		}
		return 0;
	}

	template<class T, class Compare>
	inline void 
	rb_tree<T, Compare>::
	erase(iterator begin, iterator end) {
		if (begin == begin() && end == end()) clear();
		else {
			while (begin != end) {
				erase(begin);
				++begin();
			}
		}
	}

	template<class T, class Compare>
	inline void 
	rb_tree<T, Compare>::
	clear() {
		if (size()) {
			erase_since(root());
			leftmost() = header_;
			rightmost() = header_;
			root() = nullptr;
			node_count_ = 0;
		}
	}

	template<class T, class Compare>
	inline void 
	rb_tree<T, Compare>::
	swap(rb_tree& rhs) noexcept {
		if (this != &rhs) {
			esgstl::swap(header_, rhs.header_);
			esgstl::swap(node_count_, rhs.node_count_);
			esgstl::swap(key_comp_, rhs.key_comp_);
		}
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::iterator
	rb_tree<T, Compare>::
	find(const key_type& key) {
		auto y = header_;
		auto x = root();
		// 因为没有存判断键值相等的仿函数，需要两次小于比较来判等，因此要做一些优化
		while (x != nullptr) {
			if (key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
				// 大于往右走
				x = x->right;
			}
			else {
				// 小于等于挪动y等于x，x向左走
				// 如果是等于的情况y则记录了这个结果，x的键值因为往左走会变小key肯定会大于它
				// x则会一路向右直到变成nullptr退出循环
				// 如果一直找不到，y就会记录不小于给定key值的最近的节点
				y = x; x = x->left;
			}
		}
		// 如果没找到y要么是header，要么指向一个大于key的节点
		iterator res(y);
		return (res == end() || key_comp_(key, value_traits::get_key(*res))) ? end() : res;
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::const_iterator 
	rb_tree<T, Compare>::
	find(const key_type& key) const {
		auto y = header_;
		auto x = root();
		// 因为没有存判断键值相等的仿函数，需要两次小于比较来判等，因此要做一些优化
		while (x != nullptr) {
			if (key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
				// 大于往右走
				x = x->right;
			}
			else {
				// 小于等于挪动y等于x，x向左走
				// 如果是等于的情况y则记录了这个结果，x的键值因为往左走会变小key肯定会大于它
				// x则会一路向右直到变成nullptr退出循环
				// 如果一直找不到，y就会记录不小于给定key值的最近的节点
				y = x; x = x->left;
			}
		}
		// 如果没找到y要么是header，要么指向一个大于key的节点
		const_iterator res(y);
		return (res == end() || key_comp_(key, value_traits::get_key(*res))) ? end() : res;
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::iterator 
	rb_tree<T, Compare>::
	lower_bound(const key_type& key) {
		auto y = header_;
		auto x = root();
		while (x != nullptr) {
			if (key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
				x = x->right;
			}
			else {
				// 注意：y记录的是第一个不小于x的最近节点
				y = x; 
				x = x->left;
			}
		}
		return iterator(y);
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::const_iterator
	rb_tree<T, Compare>::
	lower_bound(const key_type& key) const {
		auto y = header_;
		auto x = root();
		while (x != nullptr) {
			if (key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
				x = x->right;
			}
			else {
				y = x;
				x = x->left;
			}
		}
		return const_iterator(y);
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::iterator 
	rb_tree<T, Compare>::
	upper_bound(const key_type& key) {
		auto y = header_;
		auto x = root();
		while (x != nullptr) {
			if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value))) {
				// 注意：y记录的是大于x的最近的节点
				y = x; x = x->left;
			}
			else {
				x = x->right;
			}
		}
		return iterator(y);
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::const_iterator
	rb_tree<T, Compare>::
	upper_bound(const key_type& key) const {
		auto y = header_;
		auto x = root();
		while (x != nullptr) {
			if (key_comp_(key, value_traits::get_key(x->get_node_ptr()->value))) {
				// 注意：y记录的是大于x的最近的节点
				y = x; x = x->left;
			}
			else {
				x = x->right;
			}
		}
		return const_iterator(y);
	}



	template<class T, class Compare>
	inline void 
	rb_tree<T, Compare>::
	rb_tree_init() {
		header_ = base_node_allocator::allocate();
		header_->color = rb_tree_red;  // header_ 节点颜色为红
		root() = nullptr;
		leftmost() = header_;
		rightmost() = header_;
		node_count_ = 0;
	}

	// reset 函数
	template <class T, class Compare>
	inline void 
	rb_tree<T, Compare>::
	reset() {
		header_ = nullptr;
		node_count_ = 0;
	}

	template<class T, class Compare>
	inline typename rb_tree<T, Compare>::node_ptr
	rb_tree<T, Compare>::
	clone_node(base_node_ptr x) {
		node_ptr cloned = create_node(x->get_node_ptr()->value);
		cloned->color = x->color;
		cloned->left = nullptr;
		cloned->right = nullptr;
		return cloned;
	}

	template<class T, class Compare>
	inline void 
	rb_tree<T, Compare>::
	destory_node(node_ptr x) {
		data_allocator::destory(&x->value);
		node_allocator::deallocate(x);
	}

	template<class T, class Compare>
	inline esgstl::pair<typename rb_tree<T, Compare>::base_node_ptr, bool>
	rb_tree<T, Compare>::
	get_insert_multi_pos(const key_type& key) {
		auto x = root();
		auto y = header_;
		bool add_to_left = true;
		while (x != nullptr) {
			y = x;
			add_to_left = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
			x = add_to_left ? x->left : x->right;
		}
		return esgstl::make_pair(y,add_to_left);
	}

	template<class T, class Compare>
	inline esgstl::pair<esgstl::pair<typename rb_tree<T, Compare>::base_node_ptr, bool>, bool>
	rb_tree<T, Compare>::
	get_insert_unique_pos(const key_type& key) {
		// 返回装着一个pair和bool的pair，外层pair的bool表示是否插入成功
		// 内层pair的bool表示插在给定节点的左边还是右边
		// 若插入成功，则内层pair表示插在哪个节点的下面，否则表示重复key值所在节点
		auto x = root();
		auto y = header_;
		bool add_to_left = true;
		while (x != nullptr) {
			bool x_less_key = key_comp_(value_traits::get_key(x->get_node_ptr()->value), key);
			bool key_less_x = key_comp_(key, value_traits::get_key(x->get_node_ptr()->value));
			if (!x_less_key && !key_less_x)
				return esgstl::make_pair(esgstl::make_pair(x, add_to_left), false);
			y = x;
			add_to_left = key_less_x;
			x = add_to_left ? x->left : x->right;
		}
		return esgstl::make_pair(esgstl::make_pair(y, add_to_left), true);
	}

	template<class T, class Compare>
	inline typename rb_tree<T, Compare>::iterator
	rb_tree<T, Compare>::
	insert_value_at(base_node_ptr x, const value_type& value, bool add_to_left) {
		base_node_ptr newnode = create_node(value);
		newnode->parent = x;
		if (x == header_){
			root() = newnode;
			leftmost() = newnode;
			rightmost() = newnode;
		}
		else if (add_to_left) {
			x->left = newnode;
			if (leftmost() == x)
				leftmost() = newnode;
		}
		else {
			x->right = newnode;
			if (rightmost() == x)
				rightmost() = newnode;
		}
		rb_tree_insert_balance(newnode, root());
		++node_count_;
		return iterator(newnode);
	}

	template<class T, class Compare>
	inline typename rb_tree<T, Compare>::iterator 
	rb_tree<T, Compare>::
	insert_node_at(base_node_ptr x, node_ptr node, bool add_to_left) {
		base_node_ptr newnode = node->get_base_ptr();
		newnode->parent = x;
		if (x == header_) {
			root() = newnode;
			leftmost() = newnode;
			rightmost() = newnode;
		}
		else if (add_to_left) {
			x->left = newnode;
			if (leftmost() == x)
				leftmost() = newnode;
		}
		else {
			x->right = newnode;
			if (rightmost() == x)
				rightmost() = newnode;
		}
		rb_tree_insert_balance(newnode, root());
		++node_count_;
		return iterator(newnode);
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::iterator 
	rb_tree<T, Compare>::
	insert_multi_use_hint(iterator hint, const key_type& key, node_ptr x) {
		iterator before = hint;
		--before;
		
		auto before_key = value_traits::get_key(*before);
		auto hint_key = value_traits::get_key(*hint);
		if (key_comp_(key, hint_key) && key_comp_(before_key, key)) {
			if (before.node->right == nullptr) {
				return insert_node_at(before.node, x, false);
			}
			else if (hint.node->left == nullptr) {
				return insert_node_at(hint.node, x, true);
			}
		}
		auto pos = get_insert_multi_pos(key);
		return insert_node_at(pos.first,x,pos.second);
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::iterator 
	rb_tree<T, Compare>::
	insert_unique_use_hint(iterator hint, const key_type& key, node_ptr x) {
		iterator before = hint;
		--before;

		auto before_key = value_traits::get_key(*before);
		auto hint_key = value_traits::get_key(*hint);
		if (key_comp_(key, hint_key) && key_comp_(before_key, key)) {
			if (before.node->right == nullptr) {
				return insert_node_at(before.node, x, false);
			}
			else if (hint.node->left == nullptr) {
				return insert_node_at(hint.node, x, true);
			}
		}
		auto p = get_insert_unique_pos(key);
		if (p.second) return insert_node_at(p.first.first, x, p.first.second);
		// 若添加失败，则先把newnode删了
		destory_node(x);
		return iterator(p.first.first);
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::base_node_ptr 
	rb_tree<T, Compare>::
	copy_from(base_node_ptr x, base_node_ptr p) {
		// x是需要拷贝的子树根节点，p是指定的根节点的父节点
		auto top = clone_node(x);
		top->parent = p;
		try {
			if (x->right != nullptr) 
				copy_from(x->right, top);
			p = top;
			x = x->left;
			while (x != nullptr) {
				auto y = clone_node(x);
				y->parent = p;
				p->left = y;
				if (x->right != nullptr) 
					copy_from(x->right, y);
				p = y;
				x = x->left;
			}
		}
		catch (...) {
			erase_since(top);
			throw;
		}
		return top;
	}

	template<class T, class Compare>
	inline void 
	rb_tree<T, Compare>::
	erase_since(base_node_ptr x) {
		while (x != nullptr) {
			// 逆中序遍历删除节点
			erase_since(x->right);
			auto y = x->left;
			destory_node(x->get_node_ptr());
			x = y;
		}
	}


	template<class T, class Compare>
	template<class ...Args>
	inline rb_tree<T, Compare>::iterator 
	rb_tree<T, Compare>::
	emplace_multi(Args ...args) {
		ESGSTL_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
		node_ptr newnode = create_node(esgstl::forward<Args>(args)...);
		auto p = get_insert_multi_pos(value_traits::get_key(newnode->value));
		return insert_node_at(p.first, newnode, p.second);
	}

	template<class T, class Compare>
	template<class ...Args>
	inline esgstl::pair<typename rb_tree<T, Compare>::iterator, bool>
	rb_tree<T, Compare>::emplace_unique(Args ...args) {
		ESGSTL_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
		node_ptr newnode = create_node(esgstl::forward<Args>(args)...);
		auto p = get_insert_unique_pos(value_traits::get_key(newnode->value));
		if (p.second) return esgstl::make_pair(insert_node_at(p.first.first, newnode, p.first.second),true);
		// 若添加失败，则先把newnode删了
		destory_node(newnode);
		return esgstl::make_pair(iterator(p.first.first), false);
	}

	template<class T, class Compare>
	template<class ...Args>
	inline rb_tree<T, Compare>::iterator 
	rb_tree<T, Compare>::
	emplace_multi_use_hint(iterator hint, Args ...args) {
		ESGSTL_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
		node_ptr newnode = create_node(esgstl::forward<Args>(args)...);
		if (node_count_ == 0) {
			return insert_node_at(header_, newnode, true);
		}
		auto key = value_traits::get_key(newnode->get_node_ptr()->value);
		if (hint == begin()) {
			if (key_comp_(key, value_traits::get_key(*hint))) {
				return insert_node_at(hint.node,newnode,true);
			}
			else {
				auto pos = get_insert_multi_pos(key);
				return insert_node_at(pos.first, newnode, pos.second);
			}
		}
		else if (hint == end()) {
			if (!key_comp_(key,value_traits::get_key(rightmost()->get_node_ptr()->value))) {
				return insert_node_at(rightmost(), newnode, false);
			}
			else {
				auto pos = get_insert_multi_pos(key);
				return insert_node_at(pos.first, newnode, pos.second);
			}
		}
		return insert_multi_use_hint(hint, key, newnode);
	}

	template<class T, class Compare>
	template<class ...Args>
	inline rb_tree<T, Compare>::iterator
	rb_tree<T, Compare>::
	emplace_unique_use_hint(iterator hint, Args ...args) {
		ESGSTL_LENGTH_ERROR_IF(node_count_ > max_size() - 1, "rb_tree<T, Comp>'s size too big");
		node_ptr newnode = create_node(esgstl::forward<Args>(args)...);
		if (node_count_ == 0) {
			return insert_node_at(header_, newnode, true);
		}
		auto key = value_traits::get_key(newnode->get_node_ptr()->value);
		if (hint == begin()) {
			if (key_comp_(key, value_traits::get_key(*hint))) {
				return insert_node_at(hint.node, newnode, true);
			}
			else {
				auto p = get_insert_unique_pos(key);
				if (p.second) 
					return insert_node_at(p.first.first, newnode, p.first.second);
				// 若添加失败，则先把newnode删了
				destory_node(newnode);
				return iterator(p.first.first);
			}
		}
		else if (hint == end()) {
			if (key_comp_(value_traits::get_key(rightmost()->get_node_ptr()->value),key)) {
				return insert_node_at(rightmost(), newnode, false);
			}
			else {
				auto p = get_insert_unique_pos(key);
				if (p.second) return insert_node_at(p.first.first, newnode, p.first.second);
				// 若添加失败，则先把newnode删了
				destory_node(newnode);
				return iterator(p.first.first);
			}
		}
		return insert_unique_use_hint(hint, key, newnode);
	}

	template<class T, class Compare>
	template<class ...Args>
	inline rb_tree<T,Compare>::node_ptr 
	rb_tree<T, Compare>::
	create_node(Args&&... args) {
		node_ptr newnode = node_allocator::allocate();
		try {
			data_allocator::construct(&(newnode->value), esgstl::forward<Args>(args)...);
			newnode->left = nullptr;
			newnode->right = nullptr;
			newnode->parent = nullptr;
		}
		catch (...) {
			node_allocator::deallocate(newnode);
			throw;
		}
		return newnode;
	}

	// 重载比较操作符
	template <class T, class Compare>
	bool operator==(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
		return lhs.size() == rhs.size() && esgstl::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	template <class T, class Compare>
	bool operator<(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
		return esgstl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
	}

	template <class T, class Compare>
	bool operator!=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
		return !(lhs == rhs);
	}

	template <class T, class Compare>
	bool operator>(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
		return rhs < lhs;
	}

	template <class T, class Compare>
	bool operator<=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
		return !(rhs < lhs);
	}

	template <class T, class Compare>
	bool operator>=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
		return !(lhs < rhs);
	}

	// 重载 mystl 的 swap
	template <class T, class Compare>
	void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) noexcept {
		lhs.swap(rhs);
	}
}