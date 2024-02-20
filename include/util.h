#pragma once
// ����ļ�����һЩͨ�ù��ߣ����� move, forward, swap �Ⱥ������Լ� pair ��

#include <cstddef>

#include "type_traits"

namespace esgstl {

	// ͨ�ú���
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

	// ����������Χ�ڵ�ֵ
	template <class ForwardIter1,class ForwardIter2>
	ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2) {
		// "(void)"ָ��ֻ��first2���е�������������ʹ�õ������ֵ
		// ���Ա��������Ʊ�����������δʹ�ñ����ľ�������
		for (;first1 != last1; ++first1, (void) ++first2) {
			swap(*first1, *first2);
		}
		return first2;
	}

	// ר�����ڽ���������ͬ������c������������
	template <class Tp,size_t N>
	void swap(Tp(&a)[N], Tp(&b)[N]) {
		esgstl::swap_range(a, a + N, b);
	}

	/*********************************************/

	// �ṹ��ģ��pair����ֵ
	/*********************************************/
	template <class T1,class T2>
	struct pair {
		typedef T1		first_type;
		typedef T2		second_type;

		first_type first;
		second_type second;


		// ģ��Ĭ�Ϲ��캯��
		// enable_if��������ģ�庯�������ɣ�ֻ������������䣬�Ż������ģ��
		template <class U1 = T1,class U2 = T2,
			typename std::enable_if<
			std::is_default_constructible<U1>::value &&
			std::is_default_constructible<U2>::value,bool>::type>
		constexpr pair() : first(),second(){}

        /***************************************/
        // ����ģ�幹�캯������֪���ƣ��Ҵ��պ����
        /***************************************/
        // default constructiable
        template <class Other1 = T1, class Other2 = T2,
            typename = typename std::enable_if<
            std::is_default_constructible<Other1>::value&&
            std::is_default_constructible<Other2>::value, void>::type>
        constexpr pair()
            : first(), second()
        {
        }

        // implicit constructiable for this type
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

        // explicit constructible for this type
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

        // implicit constructiable for other type
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

        // explicit constructiable for other type
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

        // implicit constructiable for other pair
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

        // explicit constructiable for other pair
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

        // implicit constructiable for other pair
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

        // explicit constructiable for other pair
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

        // ������ֵ�����
        pair& operator=(const pair& rhs) {
            // ��ֹ�Ը�ֵ
            if (this != &rhs) {
                first = rhs.first;
                second = rhs.second;
            }
            return *this;
        }


        // �ƶ���ֵ�����
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

    // ���رȽϲ�����
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

    // ���ؽ�������
    template <class T1, class T2>
    void swap(pair<T1, T2>& lhs, pair<T1, T2>& rhs) {
        lhs.swap(rhs);
    }

    // ȫ�ֺ���������pair
    template <class Ty1, class Ty2>
    pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second) {
        return pair<Ty1, Ty2>(esgstl::forward<Ty1>(first), esgstl::forward<Ty2>(second));
    }

}