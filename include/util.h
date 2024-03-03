#pragma once
// 这个文件包含一些通用工具，包括 move, forward, swap 等函数，以及 pair 等

#include <cstddef>

#include "type_traits"

namespace esgstl {

	// 通用函数
	/*********************************************/
	
	// move
	template <class T>
	typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
		return static_cast<typename std::remove_reference<T>::type&&>(arg);
	}

	// forward
	template <class T>
	T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
		return static_cast<T&&>(arg);
	}

	template <class T>
	T&& forward(typename std::remove_reference<T>::type&& arg) noexcept{
		static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
		return static_cast<T&&>(arg);
	}

	// swap
	template <class Tp>
	void swap(Tp& lhs, Tp& rhs) {
		auto tmp(esgstl::move(lhs));
		lhs = esgstl::move(rhs);
		rhs = esgstl::move(tmp);
	}

	// 交换两个范围内的值
	template <class ForwardIter1,class ForwardIter2>
	ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2) {
		// "(void)"指，只对first2进行递增操作，而不使用递增后的值
		// 可以被用来抑制编译器产生的未使用变量的警告或错误
		for (;first1 != last1; ++first1, (void) ++first2) {
			swap(*first1, *first2);
		}
		return first2;
	}

	// 专门用于交换两个相同容量的c风格数组的内容
	template <class Tp,size_t N>
	void swap(Tp(&a)[N], Tp(&b)[N]) {
		esgstl::swap_range(a, a + N, b);
	}

	/*********************************************/

	// 结构体模板pair：对值
	/*********************************************/
	template <class T1,class T2>
	struct pair {
		typedef T1		first_type;
		typedef T2		second_type;

		first_type first;
		second_type second;


		// 模板默认构造函数
		// enable_if会控制这个模板函数的生成，只有满足条件语句，才会有这个模板
		template <class U1 = T1,class U2 = T2,
			typename std::enable_if<
			std::is_default_constructible<U1>::value &&
			std::is_default_constructible<U2>::value,bool>::type>
		constexpr pair() : first(),second(){}

        /***************************************/
        
        /***************************************/
        // 默认构造函数，只有T1和T2都可以默认构造时才会生成
        template <class Other1 = T1, class Other2 = T2,
            typename = typename std::enable_if<
            std::is_default_constructible<Other1>::value &&
            std::is_default_constructible<Other2>::value, void>::type>
        constexpr pair()
            : first(), second()
        {
        }

        // 只有T1和T2都可以被隐式用常量引用拷贝构造时才会生成
        template <class U1 = T1, class U2 = T2,
            typename std::enable_if<
            std::is_copy_constructible<U1>::value&&
            std::is_copy_constructible<U2>::value&&
            std::is_convertible<const U1&, T1>::value&&
            std::is_convertible<const U2&, T2>::value, int>::type = 0>
        constexpr pair(const T1& a, const T2& b)
            : first(a), second(b)
        {
        }

        // 假如其中有一个拷贝构造函数是explicit的，就生成这个
        template <class U1 = T1, class U2 = T2,
            typename std::enable_if<
            std::is_copy_constructible<U1>::value&&
            std::is_copy_constructible<U2>::value &&
            (!std::is_convertible<const U1&, T1>::value ||
                !std::is_convertible<const U2&, T2>::value), int>::type = 0>
        explicit constexpr pair(const T1& a, const T2& b)
            : first(a), second(b)
        {
        }

        pair(const pair& rhs) = default;
        pair(pair&& rhs) = default;

        // 假如Other1和Other2都可以隐式构造T1和T2则生成这个
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<T1, Other1>::value&&
            std::is_constructible<T2, Other2>::value&&
            std::is_convertible<Other1&&, T1>::value&&
            std::is_convertible<Other2&&, T2>::value, int>::type = 0>
        constexpr pair(Other1&& a, Other2&& b)
            : first(esgstl::forward<Other1>(a)),
            second(esgstl::forward<Other2>(b))
        {
        }

        // 假如至少有一个不能隐式构造则生成这个
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<T1, Other1>::value&&
            std::is_constructible<T2, Other2>::value &&
            (!std::is_convertible<Other1, T1>::value ||
                !std::is_convertible<Other2, T2>::value), int>::type = 0>
        explicit constexpr pair(Other1&& a, Other2&& b)
            : first(esgstl::forward<Other1>(a)),
            second(esgstl::forward<Other2>(b))
        {
        }

        // 假如Other1和Other2都可以隐式构造T1和T2则生成这个
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<T1, const Other1&>::value&&
            std::is_constructible<T2, const Other2&>::value&&
            std::is_convertible<const Other1&, T1>::value&&
            std::is_convertible<const Other2&, T2>::value, int>::type = 0>
        constexpr pair(const pair<Other1, Other2>& other)
            : first(other.first),
            second(other.second)
        {
        }

        // 假如至少有一个不能隐式构造则生成这个
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<T1, const Other1&>::value&&
            std::is_constructible<T2, const Other2&>::value &&
            (!std::is_convertible<const Other1&, T1>::value ||
                !std::is_convertible<const Other2&, T2>::value), int>::type = 0>
        explicit constexpr pair(const pair<Other1, Other2>& other)
            : first(other.first),
            second(other.second)
        {
        }

        // 假如Other1和Other2都可以隐式构造T1和T2则生成这个
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<T1, Other1>::value&&
            std::is_constructible<T2, Other2>::value&&
            std::is_convertible<Other1, T1>::value&&
            std::is_convertible<Other2, T2>::value, int>::type = 0>
        constexpr pair(pair<Other1, Other2>&& other)
            : first(esgstl::forward<Other1>(other.first)),
            second(esgstl::forward<Other2>(other.second))
        {
        }

        // 假如至少有一个不能隐式构造则生成这个
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<T1, Other1>::value&&
            std::is_constructible<T2, Other2>::value &&
            (!std::is_convertible<Other1, T1>::value ||
                !std::is_convertible<Other2, T2>::value), int>::type = 0>
        explicit constexpr pair(pair<Other1, Other2>&& other)
            : first(esgstl::forward<Other1>(other.first)),
            second(esgstl::forward<Other2>(other.second))
        {
        }

        // 拷贝赋值运算符
        pair& operator=(const pair& rhs) {
            // 防止自赋值
            if (this != &rhs) {
                first = rhs.first;
                second = rhs.second;
            }
            return *this;
        }


        // 移动赋值运算符
        pair& operator=(pair&& rhs) {
            if (this != &rhs) {
                first = esgstl::move(rhs.first);
                second = esgstl::move(rhs.second);
            }
            return *this;
        }

        template <class Other1,class Other2>
        pair& operator=(const pair<Other1, Other2>& other) {
            first = other.first;
            second = other.second;
            return *this;
        }

        template <class Other1,class Other2>
        pair& operator=(pair<Other1, Other2>&& other) {
            first = esgstl::move(other.first);
            second = esgstl::move(other.second);
            return *this;
        }

        ~pair() = default;

        void swap(pair& other) {
            if (this != &other) {
                esgstl::swap(first, other.first);
                esgstl::swap(second, other.second);
            }
        }

	};

    /****************************************************/

    // 重载比较操作符
    template <class T1, class T2>
    bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
        if (lhs.first == rhs.first) return lhs.second < rhs.second;
        return lhs.first < rhs.first;
    }

    template <class T1, class T2>
    bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }

    template <class T1, class T2>
    bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
        return !(lhs == rhs);
    }

    template <class T1, class T2>
    bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
        return rhs < lhs;
    }

    template <class T1, class T2>
    bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
        return !(rhs < lhs);
    }

    template <class T1, class T2>
    bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
        return !(lhs < rhs);
    }

    // 重载交换函数
    template <class T1, class T2>
    void swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs) {
        lhs.swap(rhs);
    }

    // 全局函数，制作pair
    template <class Ty1, class Ty2>
    pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second) {
        return pair<Ty1, Ty2>(esgstl::forward<Ty1>(first), esgstl::forward<Ty2>(second));
    }

}