#pragma once

#include<initializer_list>
#include<cassert>
// #include<iostream>
// #include"queue.h"  ���print_tree��Debug�õ�

#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "exceptdef.h"
#include "util.h"

namespace esgstl {
	// �������ɫΪfalse������ɫΪtrue
	typedef bool rb_tree_color_type;
	static constexpr rb_tree_color_type rb_tree_red = false;
	static constexpr rb_tree_color_type rb_tree_black = true;

	// ������洢����������ȡ������ֵ���ͣ�
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

	// ������洢�������ͣ��ػ�����ȡ������ֵ�����ͣ�
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

	// ��һ���ṩ�û�ʹ�õĺ�����洢����������ȡ��
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

	// һЩǰ������

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

	// rb tree �Ľڵ����
	// ��Ϊbase_node ��node��node���˸�value���ԣ���ʱ����֪��������ô��
	// ����֪���ˣ��������header����һ��base_node����Ϊ������Ҫ��ֵ������Ҷ�ӽڵ��ҹ���Ҳ��base_node

	template <class T>
	struct rb_tree_node_base {
		typedef rb_tree_color_type    color_type;
		typedef rb_tree_node_base<T>* base_node_ptr;
		typedef rb_tree_node<T>* node_ptr;

		base_node_ptr   parent;  // ���ڵ�
		base_node_ptr   left;    // ���ӽڵ�
		base_node_ptr   right;   // ���ӽڵ�
		color_type color;   // �ڵ���ɫ

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

		T value;  // �ڵ�ֵ

		base_node_ptr get_base_ptr() {
			return static_cast<base_node_ptr>(&*this);
		}

		node_ptr get_node_ptr() {
			return &*this;
		}
	};

	// rb tree traits
	// ����traits�ˣ�����rbtree������������ȡ�������������
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

	// �������������Ҳ��Ϊ����ṹ
	template <class T>
	struct rb_tree_iterator_base : public esgstl::iterator<esgstl::bidirectional_iterator_tag, T> {
		typedef typename rb_tree_traits<T>::base_node_ptr  base_node_ptr;

		base_node_ptr node;  // �ײ�Ľڵ�ָ��

		rb_tree_iterator_base() :node(nullptr) {}

		// �������������˳��ǰ�Ƶ�����
		void increment() {
			// �����ǰ�ڵ����ҽڵ㣬��ôһ·�ߵ�����������С�ڵ�
			if (node->right != nullptr) {
				// �����������������
				node = rb_tree_min(node->right);
			}
			else {
				// ���û���ҽڵ㣬��ʼ���ϻ��ݣ�ֱ��node�Ǹ��ڵ����ڵ㣬��ʱ���ڵ���������Ҫ��Ŀ��λ��
				auto y = node->parent;
				while (node == y->right) {
					node = y;
					y = y->parent;
				}
				// ��һ��������������ǰ�ڵ��Ǹ��ڵ�root����y��header�ڵ㣬���������ѭ����
				// y����root��node����header����ʱnode->right == y����������ǰ����λ������ȷ�ģ��Ͳ��ٲ�����
				if (node->right == y) return;
				node = y;
			}
		}

		void decrement() {
			// ����node��header��������ǰһ�������Ǻ���������ڵ㣬���Լ����ҽڵ�
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
				// ����Ϊʲô����������䲻��Ҫ���������С�Ľڵ���--û�������Ǿ���������Ǹ��ڵ�
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

		// ���캯��
		rb_tree_const_iterator() {}
		rb_tree_const_iterator(base_node_ptr x) { node = x; }
		rb_tree_const_iterator(node_ptr x) { node = x; }
		rb_tree_const_iterator(const iterator& rhs) { node = rhs.node; }
		rb_tree_const_iterator(const const_iterator& rhs) { node = rhs.node; }

		// ���ز�����
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

	// �����ͨ�ù����ຯ��
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
	// �����ڵ㣬����Ŀ��ڵ�����ӽڵ��������Լ���λ��
	// ��˽���������Ŀ��ڵ�������ҽڵ�
	template <class NodePtr>
	void rb_tree_left_rotate(NodePtr x, NodePtr& root) noexcept {
		// ע�⣬����root�Ǵ��ݵ�ָ������ã������������������п��ܸı�root��
		// ���rootͨ���Ǵ���header->parent���޸����root��ʹheaderָ���Ľڵ�
		auto y = x->right;
		x->right = y->left;
		if (y->left != nullptr)
			y->left->parent = x;
		y->parent = x->parent;
		// ���x��rootָ����ͬһ�ڵ㣬���滻rootΪy
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
	// �����ڵ㣬����Ŀ��ڵ�����ӽڵ��������Լ���λ��
	// ��˽���������Ŀ��ڵ��������ڵ�
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

	// �����½ڵ��ƽ������
	// ���������Ƚ��٣���ģʽ���ֵĻ�ֻ������
	// 1.���ڵ��Ǻڽڵ㣬���ô���ֱ����ƽ���
	// 2.���ڵ��Ǻ�ģ�������ڵ�Ҳ�Ǻ�ģ���ʱ���ߺڽڵ�����û����ڵ�֮ǰ�϶�������ȵģ�ֻ��Ҫ�����ú�ڵ�
	// ���ڼ��ɣ������ڵ����ڵ㶼��ڣ���ү�ڵ��죬ү�ڵ��������ƽ���ˣ�������������һ��ƽ�⣬��ү�ڵ�����
	// ���������һ���²���ĺ�ڵ㣬��ȥ�ݹ�ƽ��������
	// 3.���ڵ��Ǻ�ģ���ڵ��Ǻڵģ������ٲ���һ����ڵ㣬�Ǹ��ڵ������ô˵Ҳ����ڵ��Ǳ�Ҫ��2
	// ���Ҫͨ����ת�ֶ���һ��ȥ�������Ǳ�
	// ���ү-��-����ͬһ�����ϣ��Ǻð죬����תү�ڵ�Ϳ��ԣ������������һ��ֱ���ϣ�����Ҫ����תһ�θ��ڵ�
	// ��ת���ʹ���ڵ��ڣ�ү�ڵ��켴��
	template <class NodePtr>
	void rb_tree_insert_balance(NodePtr x, NodePtr& root) {
		rb_tree_set_red(x);
		while (x != root && rb_tree_is_red(x->parent)) {
			// ������ڵ�����ڵ�
			if (rb_tree_is_lchild(x->parent)) {
				auto f = x->parent;			// ���ڵ�
				auto y = f->parent;			// ү�ڵ�
				auto s = y->right;			// ��ڵ�
				// �����ڵ㲻Ϊ�գ��������Ǻ��
				if (s != nullptr && rb_tree_is_red(s)) {
					rb_tree_set_black(f);	// �����
					rb_tree_set_black(s);	// ����
					rb_tree_set_red(y);		// ү���
					x = y;					// �ӽǱ�Ϊү�����ϵݹ�
				}
				// �����ڵ��Ǻڵ�
				else {
					// ���ү-��-�Ӳ�ͬ�ߣ�������ת���ڵ㣬ʹ������ͬ�ߣ�����
					if (rb_tree_is_rchild(x)) {
						rb_tree_left_rotate(f,root);
						// ��ת��ɺ�ǰ���ӹ�ϵ�Ե���
						esgstl::swap(x, f);
					}
					// ���ү-��-��ͬ��(���Ҿ�Ϊ������Ҫ����ү�ڵ�)
					rb_tree_right_rotate(y, root);
					rb_tree_set_black(f);	// �����
					rb_tree_set_red(y);		// ү���
				}
			}
			// ������ڵ����ҽڵ�
			else {
				auto f = x->parent;			// ���ڵ�
				auto y = f->parent;			// ү�ڵ�
				auto s = y->left;			// ��ڵ�
				// �����ڵ��Ǻ��
				if (s != nullptr && rb_tree_is_red(s)) {
					rb_tree_set_black(f);	// �����
					rb_tree_set_black(s);	// ����
					rb_tree_set_red(y);		// ү���
					x = y;					// �ӽǱ�Ϊү�����ϵݹ�
				}
				// �����ڵ��Ǻڵ�
				else {
					// ���ү-��-�Ӳ�ͬ�ߣ�������ת���ڵ㣬ʹ������ͬ�ߣ����ң�
					if (rb_tree_is_lchild(x)) {
						rb_tree_right_rotate(f,root);
						// ��ת��ɺ�ǰ���ӹ�ϵ�Ե���
						esgstl::swap(x, f);
					}
					// ���ү-��-��ͬ��(���Ҿ�Ϊ�ң�����Ҫ����ү�ڵ�)
					rb_tree_left_rotate(y, root);
					rb_tree_set_black(f);	// �����
					rb_tree_set_red(y);		// ү���
				}
			}
		}
		// ���ڵ���ԶΪ��
		rb_tree_set_black(root);
	}


	// ����Ҫɾ���Ľڵ㣬ƽ������
	// ɾ��������Ƚϸ��ӣ�����˼ά�Ĵ����������˸е�����
	// �ܹ���4��ģʽ���ٵ�����ڵ�ֱ��ɾ�� ��ӵ��һ����ڵ�ĺڽڵ㣬��ڽڵ�ֵ������ɾ����
	// ��ӵ�������ӽڵ㣬�ҵ��ܹ�����Լ�����ӽ��Ľڵ㣬�����ڵ�ֵ���ٰ��٢ڴ���
	// �ܵ����ڽڵ㣬�����ֽڵ��ֶ�ڵ����ɫ�йأ���ϸ���������
	// ��ˣ���Ҫ��취�پ���һ�£������ˢ�����������Ū��һ�����
	// �����۲죬�٢ڢ������������٣�����󶼱����ֱ��ɾ����ڵ�����
	// ���Բ�������˼ά�����ҵ�ɾ��z����ڵ�֮��������ڵ���ȥ������ڵ�x
	// ����ڵ����ɫ�ܵ���ɾ���ڵ��Լ�������ɫ��˫��Ӱ�죬���к�+�ڡ���+�ڵ������ǰ�߶�Ӧ�٢ڢۣ����߶�Ӧ��
	template <class NodePtr>
	void rb_tree_erase_balance(NodePtr z, NodePtr& root, NodePtr& minNode, NodePtr& maxNode) {
		// ������Ϊ����Ҫɾ����Ҫͬ������minNode��maxNode���������߼������鷳
		// ���������ҵ�����ڵ�x����һ�������ǣ����Ҫɾ�Ľڵ���һ������û���ӽڵ㣬������ڵ��������һ���ӽڵ��
		// �������Ҫɾ�Ľڵ��������ӽڵ㣬����Ҫ�ҵ�һ������ֵ����Ľڵ㣬�Ƚ���ֵ����ɫ��Ȼ��Ż�������������
		// ��˻���Ҫһ���м��������y�������������Ҫ�����Ľڵ㣨y=z���Ǣ٣�y!=z���Ǣڣ�
		NodePtr y = (z->left == nullptr || z->right == nullptr) ? z : rb_tree_max(z->left);
		// ע�⣺��Ϊ����ѡ������z�����Сֵ�����x�����Ƿ�Ϊ�ն���y�����
		NodePtr x = y->left != nullptr ? y->left : y->right;
		// ��Ϊx������nullptr�����б�Ҫ��x_parent���洢x�ĸ��ڵ�
		NodePtr x_parent = nullptr;

		// ������z��ʱ��˵����Ҫ����y��z��ֵ����ɫ
		// ע�⣺����ֱ��swap����value����Ϊvalue��key������const�ģ��޷����¸�ֵ
		if (y != z) {
			// �����ַ��������(abcd������nullҲ�����ǽڵ㣩
			//				z						z
			//			y		a				a		b
			//		x		b				  c	  y
			//									x   d
			// ���y����z����ڵ㣬��ô��y����ȥx�Ͳ��ö��ˣ��Ѿ�����ȷλ����
			// �������ƶ�y��ȥ����Ҫ��x����y�ĸ��ڵ�ȥ

			// �Ȱ�z������������y���ұߣ�y���ұ�һ���ǿյ�
			z->right->parent = y;
			y->right = z->right;

			// ��һ����x�ŵ���ȷλ��ȥ����z��������������еĻ�����y�����
			// ���y����z����ڵ㣬������ȥ����x�ĸ��ڵ����y�����ö���
			if (y == z->left) {
				x_parent = y;
			}
			// ���򽻻���ȥ��x��Ӧ������y�ĸ��ڵ���ȥ
			else {
				x_parent = y->parent;
				if (x) x->parent = y->parent;
				y->parent->right = x;

				// z����������y�����
				z->left->parent = y;
				y->left = z->left;
			}
			
			// ����z��parent����y�����z�Ƿ���root
			y->parent = z->parent;
			if (z == root) root = y;
			else if (rb_tree_is_lchild(z)) z->parent->left = y;
			else z->parent->right = y;
			// color����ֱ��swap
			esgstl::swap(y->color, z->color);
			y = z;
		}
		// y == z��ʱ��˵������Ҫ����y��z��ֱ���ж�z������x������
		else {
			// ��z��parent����x
			x_parent = z->parent;
			if (x) x->parent = z->parent;
			
			// ���z�Ǹ��ڵ�Ļ���Ҫ�䶯rootָ���ָ���൱��ֱ���ж�z����x��
			if (z == root) root = x;
			else if (rb_tree_is_lchild(z)) z->parent->left = x;
			else z->parent->right = x;
			
			// �����������z���п���Ϊ���ֵ������Сֵ��
			if (z == maxNode) maxNode = (x == nullptr) ? x_parent : rb_tree_max(x_parent);
			if (z == minNode) minNode = (x == nullptr) ? x_parent : rb_tree_min(x_parent);
		}

		// ��ʱy(z)ָ��׼��ɾ���Ľڵ㣬xָ���滻z�Ľڵ㣬Ҫ��������ʣ��ɾ�����ڽڵ������
		// y�Ǻ�Ĳ��ô���ֻ���Ǻڵ�ʱ���Ҫ����
		if (!rb_tree_is_red(y)) {
			while (x != root) {
				// ���x����ڵ�
				if (x == x_parent->left) {
					auto x_brother = x_parent->right; // �ֵܽڵ�
					auto x_shunzhi = x_brother->left; // ˳ֶ�ڵ㣨��xͬ�ࣩ
					auto x_duizhi = x_brother->right; // ��ֶ�ڵ㣨��x��ࣩ
					// �ֺ죬˫ֶ�غ��Ҳ�Ϊ�գ���ʱ�������ڵ��������࣬��һ������ٵ�����ɫ
					if (rb_tree_is_red(x_brother)) {
						// ������
						rb_tree_left_rotate(x_parent,root);
						// �ֱ��
						rb_tree_set_black(x_brother);
						// �����
						rb_tree_set_red(x_parent);
						// ������x��Ϊ�������󣬼���ѭ��
					}
					// �ֺ�
					else {
						// ˫ֶ��
						if (!rb_tree_is_red(x_shunzhi) && !rb_tree_is_red(x_duizhi)) {
							// ֱ���ֱ�죬�����
							rb_tree_set_red(x_brother);
							// ������������Ǻڵģ��ǵ��������Ϊ���Ÿ��ڵ������
							if (rb_tree_is_red(x_parent)) {
								rb_tree_set_black(x_parent);
								break; // �ݹ����1��˫ֶ�ڣ������ڣ��ֺڱ�죬�������
							}
							else {
								x = x_parent;
								x_parent = x->parent;
							}
						}
						// ˳ֶ��
						else if (!rb_tree_is_red(x_shunzhi)) {
							// ��ת�ֽڵ㣬Ȼ�󻥻�����˳ֶ�ڵ����ɫ������ת��Ϊ��ֶ��
							rb_tree_right_rotate(x_brother,root);
							esgstl::swap(x_brother->color, x_shunzhi->color);
						}
						// ��ֶ��
						else {
							// ��ת���ڵ㣬�ֱ�ɸ�����ɫ����ֶ���
							rb_tree_left_rotate(x_parent,root);
							x_brother->color = x_parent->color;
							rb_tree_set_black(x_parent);
							rb_tree_set_black(x_duizhi);
							break;// �ݹ����2:��ֶ�ڣ��������ֱ丸ɫ����ֶ���
						}
					}
				}
				// ���x���ҽڵ�
				else {
					auto x_brother = x_parent->left; // �ֵܽڵ�
					auto x_shunzhi = x_brother->right; // ˳ֶ�ڵ㣨��xͬ�ࣩ
					auto x_duizhi = x_brother->left; // ��ֶ�ڵ㣨��x��ࣩ
					// �ֺ죬˫ֶ�غ��Ҳ�Ϊ��
					if (rb_tree_is_red(x_brother)) {
						// ������
						rb_tree_right_rotate(x_parent,root);
						// �ֱ��
						rb_tree_set_black(x_brother);
						// �����
						rb_tree_set_red(x_parent);
						// ������x��Ϊ�������󣬼���ѭ��
					}
					// �ֺ�
					else {
						// ˫ֶ��
						if (!rb_tree_is_red(x_shunzhi) && !rb_tree_is_red(x_duizhi)) {
							// ֱ���ֱ�죬�����
							rb_tree_set_red(x_brother);
							// ������������Ǻڵģ��ǵ��������Ϊ���Ÿ��ڵ������
							if (rb_tree_is_red(x_parent)) {
								rb_tree_set_black(x_parent);
								break; // �ݹ����1����ֶ�ڣ������ڣ��ֺڱ�죬�������
							}
							else {
								x = x_parent;
								x_parent = x->parent;
							}
						}
						// ˳ֶ��
						else if (!rb_tree_is_red(x_shunzhi)) {
							// ��ת�ֽڵ㣬Ȼ�󻥻�����˳ֶ�ڵ����ɫ������ת��Ϊ��ֶ��
							rb_tree_left_rotate(x_brother,root);
							esgstl::swap(x_brother->color, x_shunzhi->color);
						}
						// ��ֶ��
						else {
							// ��ת���ڵ㣬�ֱ�ɸ�����ɫ����ֶ���
							rb_tree_right_rotate(x_parent,root);
							x_brother->color = x_parent->color;
							rb_tree_set_black(x_parent);
							rb_tree_set_black(x_duizhi);
							break;// �ݹ����2:��ֶ�ڣ��������ֱ丸ɫ����ֶ���
						}
					}
				}
			}
		}
	}

	/*********************************************************************************/

	// �����ģ����
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

		// ������
		typedef esgstl::allocator<T>						allocator_type;
		typedef esgstl::allocator<T>						data_allocator;
		typedef esgstl::allocator<base_node_type>			base_node_allocator;
		typedef esgstl::allocator<node_type>				node_allocator;

		// �ӷ��������������ͣ���סT��node�������ݵ�����
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
		// ���졢�������ƶ���ֵ����������
		rb_tree() { rb_tree_init(); }
		rb_tree(const rb_tree& rhs);
		rb_tree(rb_tree&& rhs) noexcept;
		rb_tree& operator=(const rb_tree& rhs);
		rb_tree& operator=(rb_tree&& rhs) noexcept;

		~rb_tree() { clear(); }

	public:
		// ��������ز���
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
		// ������ز���
		bool empty() const noexcept { return node_count_ == 0; }
		size_type size() const noexcept { return node_count_; }
		size_type max_size() const noexcept { return static_cast<size_type>(-1); }

	public:
		// �޸�������ز���
		/// emplaceϵ��
		template <class ...Args>
		iterator emplace_multi(Args... args);

		template <class ...Args>
		esgstl::pair<iterator, bool> emplace_unique(Args... args);

		template <class ...Args>
		iterator emplace_multi_use_hint(iterator hint, Args... args);

		template <class ...Args>
		iterator emplace_unique_use_hint(iterator hint, Args... args);

		/// insertϵ��
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

		/// eraseϵ��
		iterator erase(iterator hint);
		size_type erase_multi(const key_type& key);
		size_type erase_unique(const key_type& key);

		void erase(iterator begin, iterator end);
		void clear();

		void swap(rb_tree& rhs) noexcept;

		// ����Ԫ����ز���
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

		// ��ʼ�����
		void rb_tree_init();
		void reset();

		// ��ɾ�ڵ����
		// create_node:�����ڵ�
		template <class ...Args>
		node_ptr create_node(Args&&... args);
		// clone_node:��¡�ڵ�
		node_ptr clone_node(base_node_ptr x);
		// destory_node��ɾ���ڵ�
		void destory_node(node_ptr x);

		// Ѱ�Ҹ�����ֵ�ܹ������λ��
		esgstl::pair<base_node_ptr, bool> 
				get_insert_multi_pos(const key_type& key);
		esgstl::pair<esgstl::pair<base_node_ptr, bool>, bool> 
				get_insert_unique_pos(const key_type& key);

		// ������ֵ���������λ��
		iterator insert_value_at(base_node_ptr x, const value_type& value, bool add_to_left);
		iterator insert_node_at(base_node_ptr x, node_ptr node, bool add_to_left);

		iterator insert_multi_use_hint(iterator hint, const key_type& key, node_ptr x);
		iterator insert_unique_use_hint(iterator hint, const key_type& key, node_ptr x);

		// ���캯��������������
		base_node_ptr copy_from(base_node_ptr x, base_node_ptr p);
		// clear ��������
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
		// ��Ϊû�д��жϼ�ֵ��ȵķº�������Ҫ����С�ڱȽ����еȣ����Ҫ��һЩ�Ż�
		while (x != nullptr) {
			if (key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
				// ����������
				x = x->right;
			}
			else {
				// С�ڵ���Ų��y����x��x������
				// ����ǵ��ڵ����y���¼����������x�ļ�ֵ��Ϊ�����߻��Сkey�϶��������
				// x���һ·����ֱ�����nullptr�˳�ѭ��
				// ���һֱ�Ҳ�����y�ͻ��¼��С�ڸ���keyֵ������Ľڵ�
				y = x; x = x->left;
			}
		}
		// ���û�ҵ�yҪô��header��Ҫôָ��һ������key�Ľڵ�
		iterator res(y);
		return (res == end() || key_comp_(key, value_traits::get_key(*res))) ? end() : res;
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::const_iterator 
	rb_tree<T, Compare>::
	find(const key_type& key) const {
		auto y = header_;
		auto x = root();
		// ��Ϊû�д��жϼ�ֵ��ȵķº�������Ҫ����С�ڱȽ����еȣ����Ҫ��һЩ�Ż�
		while (x != nullptr) {
			if (key_comp_(value_traits::get_key(x->get_node_ptr()->value), key)) {
				// ����������
				x = x->right;
			}
			else {
				// С�ڵ���Ų��y����x��x������
				// ����ǵ��ڵ����y���¼����������x�ļ�ֵ��Ϊ�����߻��Сkey�϶��������
				// x���һ·����ֱ�����nullptr�˳�ѭ��
				// ���һֱ�Ҳ�����y�ͻ��¼��С�ڸ���keyֵ������Ľڵ�
				y = x; x = x->left;
			}
		}
		// ���û�ҵ�yҪô��header��Ҫôָ��һ������key�Ľڵ�
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
				// ע�⣺y��¼���ǵ�һ����С��x������ڵ�
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
				// ע�⣺y��¼���Ǵ���x������Ľڵ�
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
				// ע�⣺y��¼���Ǵ���x������Ľڵ�
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
		header_->color = rb_tree_red;  // header_ �ڵ���ɫΪ��
		root() = nullptr;
		leftmost() = header_;
		rightmost() = header_;
		node_count_ = 0;
	}

	// reset ����
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
		// ����װ��һ��pair��bool��pair�����pair��bool��ʾ�Ƿ����ɹ�
		// �ڲ�pair��bool��ʾ���ڸ����ڵ����߻����ұ�
		// ������ɹ������ڲ�pair��ʾ�����ĸ��ڵ�����棬�����ʾ�ظ�keyֵ���ڽڵ�
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
		// �����ʧ�ܣ����Ȱ�newnodeɾ��
		destory_node(x);
		return iterator(p.first.first);
	}

	template<class T, class Compare>
	inline rb_tree<T, Compare>::base_node_ptr 
	rb_tree<T, Compare>::
	copy_from(base_node_ptr x, base_node_ptr p) {
		// x����Ҫ�������������ڵ㣬p��ָ���ĸ��ڵ�ĸ��ڵ�
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
			// ���������ɾ���ڵ�
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
		// �����ʧ�ܣ����Ȱ�newnodeɾ��
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
				// �����ʧ�ܣ����Ȱ�newnodeɾ��
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
				// �����ʧ�ܣ����Ȱ�newnodeɾ��
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

	// ���رȽϲ�����
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

	// ���� mystl �� swap
	template <class T, class Compare>
	void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) noexcept {
		lhs.swap(rhs);
	}
}