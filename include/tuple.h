#pragma once
#include<iostream>

namespace esgstl {
    
    // 空tuple类，作为底层出口
    template <typename... Types>
    class tuple {
    public:
        // 当给空tuple赋值一个有成员的tuple时，会直接返回空tuple
        template<typename... Types2>
        tuple<>& operator = (const tuple<Types2...>&) {
            return *this;
        }
    };

    // 用于获取tuple成员数量
    template<typename... Types>
    class tuple_size;

    // 用于获取tuple指定位置成员类型
    template<unsigned idx, typename... Types>
    class tuple_element;

    // 用于获取tuple指定位置成员
    template<unsigned idx, typename T>
    class GetHelper {};


    // tuple模板类
    template<typename T, typename... Types>
    struct tuple<T, Types...> {
        // 当前tuple只存最前面的类类型成员
        T current;
        // 其他tuple成员再用一个更少参数的tuple存储
        tuple<Types...> other;


        // 只允许显式构造tuple
        explicit tuple(const T& c, const Types&... t) : current(c), other(t...) {}

        tuple(const tuple& t) : current(t.current), other(t.other) {}

        // 默认构造函数
        tuple() = default;

        tuple<T, Types...>& operator = (const tuple<T, Types...>& src) {
            this->other = src.other;
            this->current = src.current;
            return *this;
        }

        template<typename... Types2>
        tuple& operator=(tuple<Types2...>& src) {
            this->other = src.other;
            this->current = src.current;
            return *this;
        }

        tuple& operator=(tuple<>& src) {
            return *this;
        }

        void swap(tuple<T, Types...>& other) {
            if (this != &other) {
                esgstl::swap(current, other.current);
                esgstl::swap(other, other.other);
            }
        }
    };

    // 重载比较操作符
    template <typename T, typename... Types>
    bool operator<(const tuple<T, Types...>& lhs, const tuple<T, Types...>& rhs) {
        if (lhs.current == rhs.current) return lhs.other < rhs.other;
        return lhs.current < rhs.current;
    }

    template <typename T, typename... Types>
    bool operator==(const tuple<T, Types...>& lhs, const tuple<T, Types...>& rhs) {
        return lhs.current == rhs.current && lhs.other == rhs.other;
    }

    template <typename ...Types>
    bool operator==(const tuple<Types...>& lhs, const tuple<Types...>& rhs) {
        return true;
    }

    template <typename T, typename... Types>
    bool operator!=(const tuple<T, Types...>& lhs, const tuple<T, Types...>& rhs) {
        return !(lhs == rhs);
    }

    template <typename T, typename... Types>
    bool operator>(const tuple<T, Types...>& lhs, const tuple<T, Types...>& rhs) {
        return rhs < lhs;
    }

    template <typename T, typename... Types>
    bool operator<=(const tuple<T, Types...>& lhs, const tuple<T, Types...>& rhs) {
        return !(rhs < lhs);
    }

    template <typename T, typename... Types>
    bool operator>=(const tuple<T, Types...>& lhs, const tuple<T, Types...>& rhs) {
        return !(lhs < rhs);
    }

    // 重载交换函数
    template <typename T, typename... Types>
    void swap(const tuple<T, Types...>& lhs, const tuple<T, Types...>& rhs) {
        lhs.swap(rhs);
    }

    /************************************************************************/
    // 全局模板类/函数

    // tuple_size模板类
    template<typename... Types>
    class tuple_size<tuple<Types...>> {
    public:
        // 静态成员，在类外初始化
        static const unsigned int value;
    };

    template<typename... Types>
    const unsigned int tuple_size<tuple<Types...>>::value = sizeof...(Types);

    // tuple_element模板类
    template<unsigned idx, typename T, typename... Types>
    class tuple_element<idx, tuple<T, Types...>> {
    public:
        using type = typename tuple_element<idx - 1, tuple<Types...>>::type;
    };

    // 底层出口（idx为0时结束）
    template<typename T, typename... Types>
    class tuple_element<0, tuple<T, Types...>> {
    public:
        using type = T;
    };

    // tuple输出函数
    template<typename T, typename... Types>
    std::ostream& operator<<(std::ostream& strm, tuple<T, Types...> const& t) {
        strm << t.current << " " << t.other;
        return strm;
    }

    // 底层出口（Types为空时结束）
    template<typename... Types>
    std::ostream& operator<<(std::ostream& strm, tuple<Types...> const& t) {
        return strm;
    }

    // make_tuple函数：根据给定形参构建tuple
    template <typename... Types>
    tuple<Types...> make_tuple(Types&&... t) {

        return tuple<Types...>(t...);
    }

    // tie函数：用于将tuple的元素解包到指定左值中
    template <typename... Types>
    tuple<Types&...> tie(Types&... t) {
        return tuple<Types&...>(t...);
    }

    // get函数：根据给定idx返回tuple成员
    template<unsigned idx, typename... Types>
    auto get(tuple<Types...>& t) {
        return GetHelper<idx, tuple<Types...>>::get(t);
    }

    template<unsigned idx, typename T, typename... Types>
    class GetHelper<idx, tuple<T, Types...>> {
    public:
        static auto get(tuple<T, Types...>& t) {
            return GetHelper<idx - 1, tuple<Types...>>::get(t.other);
        }
    };

    // 底层出口（idx=0时返回current）
    template<typename T, typename... Types>
    class GetHelper<0, tuple<T, Types...>> {
    public:
        static T const& get(tuple<T, Types...> const& t) {
            return t.current;
        }
    };
}