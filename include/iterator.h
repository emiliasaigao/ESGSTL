#pragma once
#include <cstddef>

#include "type_traits.h"

// ���ͷ�ļ����ڵ�������ƣ�������һЩģ������ȫ�ֺ���

namespace esgstl {

	// ���ֵ���������
	struct input_iterator_tag {}; // ֻ��������
	struct output_iterator_tag {}; // ֻд������
	struct forward_iterator_tag : public input_iterator_tag {}; // ǰ�������
	struct bidirectional_iterator_tag : public forward_iterator_tag {}; // ˫�������
	struct random_access_iterator_tag : public bidirectional_iterator_tag {}; // ������ʵ�����

	// ������ģ����
	template <class Category, class T, class Distance = ptrdiff_t, 
		class Pointer = T*,class Refference = T&>
	struct iterator {
		typedef Category					iterator_category;
		typedef T							value_type;
		typedef Pointer						pointer;
		typedef Refference					reference;
		typedef Distance					difference_type;

	};

	// ��������һЩ�����жϵ�����������Ϣ��ģ����

	template <class T>
	struct has_iterator_cat			// ����������ж�����T�Ƿ����category��Ϣ
	{
	private:
		struct two { char a; char b; };
		template <class U> static two test(...);
		template <class U> static char test(typename U::iterator_category* = 0);
	public:
		static const bool value = sizeof(test<T>(0)) == sizeof(char);
	};


	// ������������ȡ��ʵ��ģ����
	template <class Iterator,bool>
	struct iterator_traits_impl {};

	// iterator_traits_impl��bool����Ϊtrue��ʵ�����汾
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

	// �����࣬�����ж���Iterator�Ƿ���STL�������������֮һ
	// ֻ���ǵ�ʱ������������ȡIterator��������Ϣ
	template <class Iterator>
	struct iterator_traits_helper<Iterator,true>
		: public iterator_traits_impl<Iterator,
		std::is_convertible<typename Iterator::iterator_category,input_iterator_tag>::value ||
		std::is_convertible<typename Iterator::iterator_category,output_iterator_tag>::value>{};


	// �����ĵ�����������ȡ����ֻ����Iterator�����category��Ϣ��ʱ�����һ����Ч��
	template <class Iterator>
	struct iterator_traits 
		: public iterator_traits_helper<Iterator,has_iterator_cat<Iterator>::value> {};

	// ���ԭ��ָ���ƫ�ػ��汾
	template <class T>
	struct iterator_traits<T*> {
		typedef random_access_iterator_tag					iterator_category;
		typedef T											value_type;
		typedef T*											pointer;
		typedef T&											reference;
		typedef ptrdiff_t									difference_type;
	};

	// ���ԭ������ָ���ƫ�ػ��汾
	template <class T>
	struct iterator_traits<const T*> {
		typedef random_access_iterator_tag					iterator_category;
		typedef T											value_type;
		typedef const T* pointer;
		typedef const T& reference;
		typedef ptrdiff_t									difference_type;
	};

	// ������������������жϵ�����������,���bool�ж�Ϊtrue�������������ж�
	template <class T,class U,bool = has_iterator_cat<iterator_traits<T>>::value>
	struct has_iterator_cat_of 
		: public m_bool_constant<std::is_convertible<
		typename iterator_traits<T>::iterator_category,U>::value> {};

	// ����ж�Ϊfalseֱ��ʵ������������汾
	template <class T,class U>
	struct has_iterator_cat_of<T,U,false> : public m_false_type {};

	// ���������жϵ��������͵�ģ����
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

	
	// ���ص�����ĳ��������Ϣ��ģ�庯��
	/********************************************/
	
	// ���ڷ��ص�������Category����
	template <class Iter>
	inline typename iterator_traits<Iter>::iterator_category
		iterator_category(const Iter&) {
		typedef iterator_traits<Iter>::iterator_category Category;
		return Category();
	}


	// ���ڷ��ص�������Distance����
	// ����ָ����Ϊ�˼��ٿռ�Ŀ���
	template <class Iter>
	inline typename iterator_traits<Iter>::difference_type*
		distance_type(const Iter&) {
		return static_cast<typename iterator_traits<Iter>::difference_type*>(0);
	}

	// ���ڷ��ص�������value_type����
	template <class Iter>
	inline typename iterator_traits<Iter>::value_type*
		value_type(const Iter&) {
		return static_cast<typename iterator_traits<Iter>::value_type*>(0);
	}

	/********************************************/

	
	// ��������������distance��غ���
	/********************************************/

	// ʵ�ʼ������������ĺ����Ĳ�ͬ���ذ汾
	// input_iterator_tag �İ汾
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

	// random_access_iterator_tag �İ汾
	template <class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type
		distance_dispatch(InputIterator first, InputIterator last, random_access_iterator_tag) {
		return last - first;
	}

	// ���Ÿ��û��ļ������������ӿں���
	template <class Iter>
	inline typename iterator_traits<Iter>::difference_type
		distance(Iter first, Iter last) {
		return distance_dispatch(first, last, iterator_category(first));
	}
	/********************************************/

	
	// ʹ������ǰ��һ�������advance��غ���
	/********************************************/

	// input_iterator_tag�汾
	template <class InputIterator,class Distance>
	inline void advance_dispatch(InputIterator& iter, Distance n,input_iterator_tag) {
		while (n--) ++iter;
	}

	//bidirectional_iterator_tag�汾
	template <class InputIterator, class Distance>
	inline void advance_dispatch(InputIterator& iter, Distance n, bidirectional_iterator_tag) {
		if (n > 0)
			while (n--) ++iter;
		else
			while (n++) --iter;
	}

	// random_access_iterator_tag�汾
	template <class InputIterator, class Distance>
	inline void advance_dispatch(InputIterator& iter, Distance n, random_access_iterator_tag) {
		iter += n;
	}

	// ���Ÿ��û���ʹ������ǰ��һ������Ľӿں���
	template <class Iter, class Distance>
	inline void advance(Iter& iter, Distance n) {
		advance_dispatch(iter, n, iterator_category(iter));
	}
	/********************************************/


	// ���������reverse_iteratorģ����
	template <class Iter>
	class reverse_iterator {
		private:
			Iter current;
		public:
			// ���������������������Ϣ�������Ӧ������������е���ͬ
			typedef typename iterator_traits<Iter>::iterator_category			iterator_category;
			typedef typename iterator_traits<Iter>::value_type					value_type;
			typedef typename iterator_traits<Iter>::pointer						pointer;
			typedef typename iterator_traits<Iter>::reference					reference;
			typedef typename iterator_traits<Iter>::difference_type				difference_type;

			// �������������ͺ��������ͷ������ʹ��
			typedef Iter												iterator_type;
			typedef reverse_iterator<Iter>								self;

		public:
			// ���캯��
			reverse_iterator(){}
			explicit reverse_iterator(iterator_type iter) : current(iter){}
			reverse_iterator(const self& rhs) : current(rhs.current){}

		public:
			// ����������Ĳ����ӿ�
			// ���ض�Ӧ�����������
			iterator_type base() const {
				return current;
			}

			// ȡ�����������ָ��ʵ�ʶ���
			reference operator*() const {
				auto tmp = current;
				return *--tmp;
			}

			// ȡ�����������ָ��ʵ�ʶ����ָ��
			pointer operator->() const {
				return &(operator*());
			}

			// ǰ����Ϊcurrent�ĺ���
			self& operator++() {
				--current;
				return *this;
			}

			self operator++(int) {
				self tmp = *this;
				--current;
				return tmp;
			}

			// ���˱�Ϊcurrent��ǰ��
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

	// �����������һ�����غ���
	template <class Iterator>
	typename iterator_traits<Iterator>::difference_type
		operator-(const reverse_iterator<Iterator>& lhs,
			const reverse_iterator<Iterator>& rhs) {
		return lhs.base() - rhs.base();
	}

	// ���رȽϲ�����
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