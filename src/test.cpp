#include "memory.h"
#include<iostream>
#include"vector.h"
#include"unordered_set.h"
#include <chrono>
#include"list.h"
#include<list>
#include"deque.h"
#include"set.h"
#include"map.h"
#include"tuple.h"
#include"astring.h"
#include"iterator.h"
#include<vector>
#include"algorithm.h"
#include<algorithm>

class A{
public:
	A(){}
	A(int i,int j) :a(i),b(j) {} 
	int a,b;
	virtual void name() {
		std::cout << "A";
	}
};
class C {
	int b;
};
class B:public A{
	void name() override {
		std::cout << "B";
	}
};

std::ostream& operator<<(std::ostream& os,A* a){
	a->name();
	return os;
}

template<class T1,class T2>
std::ostream& operator<<(std::ostream& os, esgstl::pair<T1,T2>& p) {
	os << p.first << "<==>" << p.second;
	return os;
}

template <class T>
void printht(const esgstl::vector<esgstl::hashtable_node<T>*>& v) {
	for (int i = 0; i < v.size(); ++i) {
		std::cout << "<<" << i << ">>:";
		auto first = v[i];
		while (first) {
			std::cout << (first->value) << '\t';
			first = first->next;
		}
		std::cout << std::endl;
	}
}

template <class T>
void printv(const esgstl::vector<T>& v) {
	for (const auto& i : v) std::cout << i << '\t';
	std::cout << std::endl;
}

void printl(const esgstl::list<int>& l) {
	for (const auto& i : l) std::cout << i << '\t';
	std::cout << std::endl;
}

void printd(const esgstl::deque<int>& d) {
	for (const auto& i : d) std::cout << i << '\t';
	std::cout << std::endl;
}


template <class InputIter>
void display(InputIter first, InputIter last) {
	while (first != last) {
		std::cout << (*first) << '\t';
		++first;
	}
	std::cout << std::endl;
}

using namespace esgstl;
void test_unique_ptr() {
    {
        unique_ptr<int> up1(new int(1));
        assert(*up1 == 1);
        assert(*up1.get() == 1);
    }

    {
        unique_ptr<int> up1(new int(1));
        unique_ptr<int> up2(std::move(up1));
    }

    {
        unique_ptr<int> up1(new int(1));
        unique_ptr<int> up2;
        up2 = std::move(up1);
    }

    {
        unique_ptr<int> up1(new int(1));
        up1 = std::move(up1);
    }

    {
        unique_ptr<int> up1(new int(1));
        up1.reset(new int(2));
        assert(*up1 == 2);
        up1.reset();
    }

    {
        unique_ptr<int> up1(new int(1));
        int* data = up1.release();
        assert(!up1);
        delete data;
    }
}

void test_shared_ptr() {
    {
        shared_ptr<int> sp1;
        assert(sp1.get() == nullptr);
        assert(!sp1);
    }

    {
        shared_ptr<int> sp1(new int(1));
        assert(*sp1 == 1);
        assert(*sp1.get() == 1);
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(sp1);
        assert(*sp1 == 1);
        assert(*sp2 == 1);
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(std::move(sp1));
        assert(sp1.get() == nullptr);
        assert(!sp1);
        assert(*sp2 == 1);
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(2));
        shared_ptr<int> sp3;

        sp3 = sp1;
        assert(*sp1 == 1);
        assert(*sp3 == 1);

        sp3 = sp2;
        assert(*sp2 == 2);
        assert(*sp3 == 2);
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(2));

        shared_ptr<int> sp3;

        sp3 = std::move(sp1);
        assert(sp1.get() == nullptr);
        assert(!sp1);
        assert(*sp3 == 1);

        sp3 = std::move(sp2);
        assert(sp2.get() == nullptr);
        assert(!sp2);
        assert(*sp3 == 2);
    }

    {
        shared_ptr<int> sp1(new int(1));
        sp1.reset(new int(2));
        assert(*sp1 == 2);
        sp1.reset();
        assert(sp1.get() == nullptr);
        assert(!sp1);
    }

    {
        class Base {};
        class Derived : public Base {};
        shared_ptr<Derived> sp1(new Derived);
        shared_ptr<Base> sp2(sp1);
    }
}

void test_weak_ptr() {
    {
        weak_ptr<int> wp1;
        assert(!wp1.lock());
        assert(wp1.expired());
    }

    {
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp1(sp1);
        assert(wp1.lock());
        wp1.reset();
        assert(wp1.expired());
    }

    {
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(wp1);
        assert(wp1.lock());
        assert(wp2.lock());
    }

    {
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(std::move(wp1));
        assert(wp1.expired());
        assert(wp2.lock());
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(2));
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(sp2);
        weak_ptr<int> wp3;
        wp3 = wp1;
        assert(wp1.lock());
        assert(wp3.lock());
        wp3 = wp2;
        assert(wp2.lock());
        assert(wp3.lock());
    }

    {
        shared_ptr<int> sp1(new int(1));
        shared_ptr<int> sp2(new int(1));
        weak_ptr<int> wp1(sp1);
        weak_ptr<int> wp2(sp1);
        weak_ptr<int> wp3;
        wp3 = std::move(wp1);
        assert(wp1.expired());
        assert(wp3.lock());
        wp3 = std::move(wp2);
        assert(wp2.expired());
        assert(wp3.lock());
    }

    {
        shared_ptr<int> sp1(new int(1));
        weak_ptr<int> wp(sp1);
        {
            auto sp2 = wp.lock();
            assert(sp2);
        }
        sp1.reset();
        assert(!wp.lock());
        assert(wp.expired());
    }
}

int main() {
	using namespace esgstl;
	
	/*vector<int> v = { 9,7,5,5,6,10,3,2,1};
	
	set<int> s(v.begin(), v.end());
	for (const int& a : s) {
		std::cout << a << '\t';
	}
	std::cout << std::endl;*/
	/*multimap<int, int> mm = { {1,2},{1,2},{2,3},{0,4} };
	for (const auto& p : mm) {
		std::cout << p.first << ',' << p.second << '\t' ;
	}
	std::cout << std::endl;*/
	//auto t1 = make_tuple(1, string(6, 'a'), 3.651);
	//auto t2 = make_tuple(1, string(6, 'a'), 3.651);
	//bool flag = t1 == t2;
	//std::cout << flag << std::endl;
	//std::cout << t1 << std::endl;
	//int i;
	//string c;
	//double d;
	//tie(i, c, d) = t1;
	//std::cout << i << '\t' << c << '\t' << d << std::endl;
	//std::cout << get<0>(t1) << " " << get<1>(t1) << " " << get<2>(t1) << std::endl;
    /*test_unique_ptr();
    test_shared_ptr();
    test_weak_ptr();*/
    //std::list<int> l1 = { 1,2,3 };
    //std::list<int> l2 = { 4,5,6 };
    //l1.splice(l1.begin(), l2);
    //for (const int& i : l1) {
    //    std::cout << i << std::endl;
    //}
    //vector<vector<int>> v;
    //v.push_back({ 1,2,3 });
    //v.emplace_back(std::initializer_list({ 3, 4, 5 }));
    //for (const auto& vv : v) {
    //    //printv(vv);
    //    for (int i : vv) std::cout << i << '\t';
    //    std::cout << std::endl;
    //}
    //set<int> s = { 1,2,3,4,5 };
    //std::cout << (--s.begin() == s.end()) << std::endl;
    //set<int> s1 = { 1,2 };
    //std::cout << (--s1.begin() == s1.end()) << std::endl;
    /*rb_tree<int, esgstl::less<int>> tree;
    tree.insert_unique(2);
    tree.insert_unique(1);
    std::cout << rb_tree_is_red(tree.root()) << std::endl;
    std::cout << tree.root()->get_node_ptr()->value << std::endl;
    std::cout << rb_tree_is_red(tree.begin().node) << std::endl;
    std::cout << *tree.begin() << std::endl;
    std::cout << "*********************" << std::endl;
    tree.erase_unique(2);
    std::cout << rb_tree_is_red(tree.root()) << std::endl;
    std::cout << tree.root()->get_node_ptr()->value << std::endl;*/
    vector<int> v = { 9,2,1,4,5,6,7 };
    merge_sort(v.begin(), v.end());
    printv(v);
    
}