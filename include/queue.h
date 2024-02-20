#pragma once

#include "deque.h"  // queue�ĵײ�Ĭ����deque��Ϊ����
#include "functional.h" // ʹ����Ԥ�ȶ����less<T>�ȷº���
#include "vector.h" // priority_queue�ײ�Ĭ����vector��Ϊ����
#include "heap_algo.h"// priority_queue�ײ��߼�Ĭ��Ϊ�󶥶�

namespace esgstl {

    // ��ͨ������ģ��
    /************************************************************************************************/
    template <class T, class Container = esgstl::deque<T>>
    class queue {
    public:
        typedef Container                           container_type;
        // ʹ�õײ��������ͱ�
        typedef typename Container::value_type      value_type;
        typedef typename Container::size_type       size_type;
        typedef typename Container::reference       reference;
        typedef typename Container::const_reference const_reference;

        static_assert(std::is_same<T, value_type>::value,
            "the value_type of Container should be same with T");
    private:
        container_type c_;  // �õײ��������� queue

    public:
        // ���졢���ơ��ƶ�����

        queue() = default;

        explicit queue(size_type n) :c_(n) {}
        queue(size_type n, const value_type& value) :c_(n, value) {}

        template <class Iter>
        queue(Iter first, Iter last) : c_(first, last) {}

        queue(std::initializer_list<T> ilist) :c_(ilist.begin(), ilist.end()) {}

        queue(const Container& c)
            :c_(c) {}
        queue(Container&& c) noexcept(std::is_nothrow_move_constructible<Container>::value)
            :c_(esgstl::move(c)) {}

        queue(const queue& rhs) :c_(rhs.c_) {}
        queue(queue&& rhs) noexcept(std::is_nothrow_move_constructible<Container>::value)
            :c_(esgstl::move(rhs.c_)) {}

        queue& operator=(const queue& rhs) {
            c_ = rhs.c_;
            return *this;
        }
        queue& operator=(queue&& rhs) noexcept(std::is_nothrow_move_assignable<Container>::value) {
            c_ = esgstl::move(rhs.c_);
            return *this;
        }

        queue& operator=(std::initializer_list<T> ilist) {
            c_ = ilist;
            return *this;
        }

        ~queue() = default;

        // ����Ԫ����ز���
        reference       front() { return c_.front(); }
        const_reference front() const { return c_.front(); }
        reference       back() { return c_.back(); }
        const_reference back()  const { return c_.back(); }

        // ������ز���
        bool      empty() const noexcept { return c_.empty(); }
        size_type size()  const noexcept { return c_.size(); }

        // �޸�������ز���
        template <class ...Args>
        void emplace(Args&& ...args) { c_.emplace_back(esgstl::forward<Args>(args)...); }

        void push(const value_type& value) { c_.push_back(value); }
        void push(value_type&& value) { c_.emplace_back(esgstl::move(value)); }

        void pop() { c_.pop_front(); }

        void clear() {
            while (!empty())
                pop();
        }

        void swap(queue& rhs) noexcept(noexcept(esgstl::swap(c_, rhs.c_))) {
            esgstl::swap(c_, rhs.c_);
        }

    public:
        bool operator==(const queue& rhs) { return c_ == rhs.c_; }
        bool operator< (const queue& rhs) { return c_ < rhs.c_; }
    };

    // ���رȽϲ�����
    template <class T, class Container>
    bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
        return lhs == rhs;
    }

    template <class T, class Container>
    bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
        return !(lhs == rhs);
    }

    template <class T, class Container>
    bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
        return lhs < rhs;
    }

    template <class T, class Container>
    bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
        return rhs < lhs;
    }

    template <class T, class Container>
    bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
        return !(rhs < lhs);
    }

    template <class T, class Container>
    bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs) {
        return !(lhs < rhs);
    }

    // ���� esgstl �� swap
    template <class T, class Container>
    void swap(queue<T, Container>& lhs, queue<T, Container>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }

    /************************************************************************************************/

    // ���ȶ�����ģ��
    /************************************************************************************************/
    template <class T, class Container = esgstl::vector<T>, class Compare = esgstl::less<T>> 
    class priority_queue {
    public:
        typedef Container                           container_type;
        typedef Compare                             value_compare;
        // ʹ�õײ��������ͱ�
        typedef typename Container::value_type      value_type;
        typedef typename Container::size_type       size_type;
        typedef typename Container::reference       reference;
        typedef typename Container::const_reference const_reference;

        static_assert(std::is_same<T, value_type>::value,
            "the value_type of Container should be same with T");

    private:
        container_type c_;
        value_compare comp_;

    public:
        // ���졢���ơ��ƶ�����
        priority_queue() = default;

        priority_queue(const Compare& c) : c_(), comp_(c) {}

        explicit priority_queue(size_type n) : c_(n){}
        priority_queue(size_type n, const value_type& value):c_(n, value){}
        template <class Iter>
        priority_queue(Iter first, Iter last) : c_(first, last) { 
            esgstl::make_heap(c_.begin(), c_.end(), comp_); 
        }
        priority_queue(std::initializer_list<T> ilist) :c_(ilist) { 
            esgstl::make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue(const Container& s) :c_(s) {
            esgstl::make_heap(c_.begin(), c_.end(), comp_);
        }
        priority_queue(Container&& s) :c_(esgstl::move(s)) {
            esgstl::make_heap(c_.begin(), c_.end(), comp_);
        }

        priority_queue(const priority_queue& rhs)
            :c_(rhs.c_), comp_(rhs.comp_) {
            esgstl::make_heap(c_.begin(), c_.end(), comp_);
        }
        priority_queue(priority_queue&& rhs)
            :c_(esgstl::move(rhs.c_)), comp_(rhs.comp_) {
            esgstl::make_heap(c_.begin(), c_.end(), comp_);
        }
        
        priority_queue& operator=(const priority_queue& rhs) {
            c_ = rhs.c_;
            comp_ = rhs.comp_;
            esgstl::make_heap(c_.begin(), c_.end(), comp_);
            return *this;
        }
        priority_queue& operator=(priority_queue&& rhs) {
            c_ = esgstl::move(rhs.c_);
            comp_ = rhs.comp_;
            esgstl::make_heap(c_.begin(), c_.end(), comp_);
            return *this;
        }
        priority_queue& operator=(std::initializer_list<T> ilist) {
            c_ = ilist;
            comp_ = value_compare();
            esgstl::make_heap(c_.begin(), c_.end(), comp_);
            return *this;
        }

        ~priority_queue() = default;

    public:
        // Ԫ�ط�����ز���
        /************************************************************/
        const_reference top() const { return c_.front(); }
        /************************************************************/

        // ������ز���
        /************************************************************/
        bool empty() const noexcept { return c_.empty(); }
        size_type size() const noexcept { return c_.size(); }
        /************************************************************/

        // �����޸Ĳ���
        /************************************************************/
        /// emplace
        template <class ...Args>
        void emplace(Args&& ...args) {
            c_.emplace_back(esgstl::forward<Args>(args)...);
            esgstl::push_heap(c_.begin(), c_.end(), comp_);
        }

        /// push
        void push(const value_type& value) {
            c_.push_back(value);
            esgstl::push_heap(c_.begin(), c_.end(), comp_);
        }
        void push(value_type&& value) { emplace(value); }

        /// pop
        void pop() {
            esgstl::pop_heap(c_.begin(), c_.end(), comp_);
            c_.pop_back();
        }

        /// clear
        void clear() {
            c_.clear();
        }
        /************************************************************/
    };
    /************************************************************************************************/

}