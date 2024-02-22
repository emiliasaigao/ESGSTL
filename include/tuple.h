#pragma once
#include<iostream>

namespace esgstl {
    
    // ��tuple�࣬��Ϊ�ײ����
    template <typename... Types>
    class tuple {
    public:
        // ������tuple��ֵһ���г�Ա��tupleʱ����ֱ�ӷ��ؿ�tuple
        template<typename... Types2>
        tuple<>& operator = (const tuple<Types2...>&) {
            return *this;
        }
    };

    // ���ڻ�ȡtuple��Ա����
    template<typename... Types>
    class tuple_size;

    // ���ڻ�ȡtupleָ��λ�ó�Ա����
    template<unsigned idx, typename... Types>
    class tuple_element;

    // ���ڻ�ȡtupleָ��λ�ó�Ա
    template<unsigned idx, typename T>
    class GetHelper {};


    // tupleģ����
    template<typename T, typename... Types>
    struct tuple<T, Types...> {
        // ��ǰtupleֻ����ǰ��������ͳ�Ա
        T current;
        // ����tuple��Ա����һ�����ٲ�����tuple�洢
        tuple<Types...> other;


        // ֻ������ʽ����tuple
        explicit tuple(const T& c, const Types&... t) : current(c), other(t...) {}

        tuple(const tuple& t) : current(t.current), other(t.other) {}

        // Ĭ�Ϲ��캯��
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

    // ���رȽϲ�����
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

    // ���ؽ�������
    template <typename T, typename... Types>
    void swap(const tuple<T, Types...>& lhs, const tuple<T, Types...>& rhs) {
        lhs.swap(rhs);
    }

    /************************************************************************/
    // ȫ��ģ����/����

    // tuple_sizeģ����
    template<typename... Types>
    class tuple_size<tuple<Types...>> {
    public:
        // ��̬��Ա���������ʼ��
        static const unsigned int value;
    };

    template<typename... Types>
    const unsigned int tuple_size<tuple<Types...>>::value = sizeof...(Types);

    // tuple_elementģ����
    template<unsigned idx, typename T, typename... Types>
    class tuple_element<idx, tuple<T, Types...>> {
    public:
        using type = typename tuple_element<idx - 1, tuple<Types...>>::type;
    };

    // �ײ���ڣ�idxΪ0ʱ������
    template<typename T, typename... Types>
    class tuple_element<0, tuple<T, Types...>> {
    public:
        using type = T;
    };

    // tuple�������
    template<typename T, typename... Types>
    std::ostream& operator<<(std::ostream& strm, tuple<T, Types...> const& t) {
        strm << t.current << " " << t.other;
        return strm;
    }

    // �ײ���ڣ�TypesΪ��ʱ������
    template<typename... Types>
    std::ostream& operator<<(std::ostream& strm, tuple<Types...> const& t) {
        return strm;
    }

    // make_tuple���������ݸ����βι���tuple
    template <typename... Types>
    tuple<Types...> make_tuple(Types&&... t) {

        return tuple<Types...>(t...);
    }

    // tie���������ڽ�tuple��Ԫ�ؽ����ָ����ֵ��
    template <typename... Types>
    tuple<Types&...> tie(Types&... t) {
        return tuple<Types&...>(t...);
    }

    // get���������ݸ���idx����tuple��Ա
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

    // �ײ���ڣ�idx=0ʱ����current��
    template<typename T, typename... Types>
    class GetHelper<0, tuple<T, Types...>> {
    public:
        static T const& get(tuple<T, Types...> const& t) {
            return t.current;
        }
    };
}