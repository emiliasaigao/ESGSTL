#pragma once
#include <cstddef>

#include "type_traits.h"

// 这个头文件用于迭代器设计，包含了一些模板类与全局函数

namespace esgstl {

	// 五种迭代器类型
	struct input_iterator_tag {}; // 只读迭代器
	struct output_iterator_tag {}; // 只写迭代器
	struct forward_iterator_tag : public input_iterator_tag {}; // 前向迭代器
	struct bidirectional_iterator_tag : public forward_iterator_tag {}; // 双向迭代器
	struct random_access_iterator_tag : public bidirectional_iterator_tag {}; // 随机访问迭代器

	// 迭代器模板类
	template <class Category, class T, class Distance = ptrdiff_t, 
		class Pointer = T*,class Refference = T&>
	struct iterator {
		typedef Category					iterator_category;
		typedef T							value_type;
		typedef Pointer						pointer;
		typedef Refference					reference;
		typedef Distance					difference_type;

	};

	// 接下来是一些帮助判断迭代器类型信息的模板类

	template <class T>
	struct has_iterator_cat			// 这个类用来判断类型T是否具有category信息
	{
	private:
		struct two { char a; char b; };
		template <class U> static two test(...);
		template <class U> static char test(typename U::iterator_category* = 0);
	public:
		static const bool value = sizeof(test<T>(0)) == sizeof(char);
	};


	// 迭代器类型萃取器实例模板类
	template <class Iterator,bool>
	struct iterator_traits_impl {};

	// iterator_traits_impl的bool参数为true的实例化版本
	template <class Iterator>
	struct iterator_traits_impl<Iterator,true> {
		typedef typename Iterator::iterator_category iterator_category;
		typedef typename Iterator::value_type        value_type;
		typedef typename Iterator::pointer           pointer;
		typedef typename Iterator::reference         reference;
		typedef typename Iterator::difference_type   difference_type;
	};

	template <class Iterator,bool>
	struct iterator_traits_helper {};

	// 工具类，用于判断类Iterator是否是STL定义的五种类型之一
	// 只有是的时候这个类才能萃取Iterator的类型信息
	template <class Iterator>
	struct iterator_traits_helper<Iterator,true>
		: public iterator_traits_impl<Iterator,
		std::is_convertible<typename Iterator::iterator_category,input_iterator_tag>::value ||
		std::is_convertible<typename Iterator::iterator_category,output_iterator_tag>::value>{};


	// 真正的迭代器类型萃取器，只有在Iterator类存在category信息的时候才是一个有效类
	template <class Iterator>
	struct iterator_traits 
		: public iterator_traits_helper<Iterator,has_iterator_cat<Iterator>::value> {};

	// 针对原生指针的偏特化版本
	template <class T>
	struct iterator_traits<T*> {
		typedef random_access_iterator_tag					iterator_category;
		typedef T											value_type;
		typedef T*											pointer;
		typedef T&											reference;
		typedef ptrdiff_t									difference_type;
	};

	// 针对原生常量指针的偏特化版本
	template <class T>
	struct iterator_traits<const T*> {
		typedef random_access_iterator_tag					iterator_category;
		typedef T											value_type;
		typedef const T* pointer;
		typedef const T& reference;
		typedef ptrdiff_t									difference_type;
	};

	// 下面这个类用来辅助判断迭代器的类型,如果bool判断为true，继续后续的判定
	template <class T,class U,bool = has_iterator_cat<iterator_traits<T>>::value>
	struct has_iterator_cat_of 
		: public m_bool_constant<std::is_convertible<
		typename iterator_traits<T>::iterator_category,U>::value> {};

	// 如果判断为false直接实例化下面这个版本
	template <class T,class U>
	struct has_iterator_cat_of<T,U,false> : public m_false_type {};

	// 真正用来判断迭代器类型的模板类
	/********************************************/
	template <class Iter>
	struct is_input_iterator : public has_iterator_cat_of<Iter,input_iterator_tag> {};

	template <class Iter>
	struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

	template <class Iter>
	struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

	template <class Iter>
	struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

	template <class Iter>
	struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

	template <class Iter>
	struct is_iterator : public m_bool_constant<is_input_iterator<Iter>::value 
		|| is_output_iterator<Iter>::value> {};
	/********************************************/

	
	// 返回迭代器某种类型信息的模板函数
	/********************************************/
	
	// 用于返回迭代器的Category类型
	template <class Iter>
	inline typename iterator_traits<Iter>::iterator_category
		iterator_category(const Iter&) {
		typedef iterator_traits<Iter>::iterator_category Category;
		return Category();
	}


	// 用于返回迭代器的Distance类型
	// 返回指针是为了减少空间的开销
	template <class Iter>
	inline typename iterator_traits<Iter>::difference_type*
		distance_type(const Iter&) {
		return static_cast<typename iterator_traits<Iter>::difference_type*>(0);
	}

	// 用于返回迭代器的value_type类型
	template <class Iter>
	inline typename iterator_traits<Iter>::value_type*
		value_type(const Iter&) {
		return static_cast<typename iterator_traits<Iter>::value_type*>(0);
	}

	/********************************************/

	
	// 计算迭代器距离的distance相关函数
	/********************************************/

	// 实际计算迭代器距离的函数的不同重载版本
	// input_iterator_tag 的版本
	template <class InputIterator> 
	inline typename iterator_traits<InputIterator>::difference_type 
		distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (first != last) {
			++first;
			++n;
		}
		return n;
	}

	// random_access_iterator_tag 的版本
	template <class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type
		distance_dispatch(InputIterator first, InputIterator last, random_access_iterator_tag) {
		return last - first;
	}

	// 开放给用户的计算迭代器距离接口函数
	template <class Iter>
	inline typename iterator_traits<Iter>::difference_type
		distance(Iter first, Iter last) {
		return distance_dispatch(first, last, iterator_category(first));
	}
	/********************************************/

	
	// 使迭代器前进一定距离的advance相关函数
	/********************************************/

	// input_iterator_tag版本
	template <class InputIterator,class Distance>
	inline void advance_dispatch(InputIterator& iter, Distance n,input_iterator_tag) {
		while (n--) ++iter;
	}

	//bidirectional_iterator_tag版本
	template <class InputIterator, class Distance>
	inline void advance_dispatch(InputIterator& iter, Distance n, bidirectional_iterator_tag) {
		if (n > 0)
			while (n--) ++iter;
		else
			while (n++) --iter;
	}

	// random_access_iterator_tag版本
	template <class InputIterator, class Distance>
	inline void advance_dispatch(InputIterator& iter, Distance n, random_access_iterator_tag) {
		iter += n;
	}

	// 开放给用户的使迭代器前进一定距离的接口函数
	template <class Iter, class Distance>
	inline void advance(Iter& iter, Distance n) {
		advance_dispatch(iter, n, iterator_category(iter));
	}
	/********************************************/


	// 反向迭代器reverse_iterator模板类
	template <class Iter>
	class reverse_iterator {
		private:
			Iter current;
		public:
			// 反向迭代器的五种类型信息和自身对应的正向迭代器中的相同
			typedef typename iterator_traits<Iter>::iterator_category			iterator_category;
			typedef typename iterator_traits<Iter>::value_type					value_type;
			typedef typename iterator_traits<Iter>::pointer						pointer;
			typedef typename iterator_traits<Iter>::reference					reference;
			typedef typename iterator_traits<Iter>::difference_type				difference_type;

			// 新增迭代器类型和自身类型方便后面使用
			typedef Iter												iterator_type;
			typedef reverse_iterator<Iter>								self;

		public:
			// 构造函数
			reverse_iterator(){}
			explicit reverse_iterator(iterator_type iter) : current(iter){}
			reverse_iterator(const self& rhs) : current(rhs.current){}

		public:
			// 反向迭代器的操作接口
			// 返回对应的正向迭代器
			iterator_type base() const {
				return current;
			}

			// 取反向迭代器所指的实际对象
			reference operator*() const {
				auto tmp = current;
				return *--tmp;
			}

			// 取反向迭代器所指的实际对象的指针
			pointer operator->() const {
				return &(operator*());
			}

			// 前进变为current的后退
			self& operator++() {
				--current;
				return *this;
			}

			self operator++(int) {
				self tmp = *this;
				--current;
				return tmp;
			}

			// 后退变为current的前进
			self& operator--() {
				++current;
				return *this;
			}
			
			self operator--(int)
			{
				self tmp = *this;
				++current;
				return tmp;
			}
			
			self& operator+=(difference_type n) {
				current -= n;
				return *this;
			}

			self operator+(difference_type n) const
			{
				return self(current - n);
			}

			self& operator-=(difference_type n)
			{
				current += n;
				return *this;
			}

			self operator-(difference_type n) const
			{
				return self(current + n);
			}

			reference operator[](difference_type n) {
				return *(*this + n);
			}
	};

	// 反向迭代器的一组重载函数
	template <class Iterator>
	typename iterator_traits<Iterator>::difference_type
		operator-(const reverse_iterator<Iterator>& lhs,
			const reverse_iterator<Iterator>& rhs) {
		return lhs.base() - rhs.base();
	}

	// 重载比较操作符
	template <class Iterator>
	bool operator==(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs) {
		return lhs.base() == rhs.base();
	}

	template <class Iterator>
	bool operator<(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs) {
		return rhs.base() < lhs.base();
	}

	template <class Iterator>
	bool operator!=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs) {
		return !(lhs == rhs);
	}

	template <class Iterator>
	bool operator>(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs) {
		return rhs < lhs;
	}

	template <class Iterator>
	bool operator<=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs) {
		return !(rhs < lhs);
	}

	template <class Iterator>
	bool operator>=(const reverse_iterator<Iterator>& lhs,
		const reverse_iterator<Iterator>& rhs) {
		return !(lhs < rhs);
	}


}