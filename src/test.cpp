#include "memory.h"
#include<iostream>
#include"vector.h"
#include"unordered_set.h"
#include <chrono>
#include"list.h"
#include"deque.h"
#include"set.h"
#include"map.h"


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


int main() {
	using namespace esgstl;
	
	/*vector<int> v = { 9,7,5,5,6,10,3,2,1};
	
	set<int> s(v.begin(), v.end());
	for (const int& a : s) {
		std::cout << a << '\t';
	}
	std::cout << std::endl;*/
	multimap<int, int> mm = { {1,2},{1,2},{2,3},{0,4} };
	for (const auto& p : mm) {
		std::cout << p.first << ',' << p.second << '\t' ;
	}
	std::cout << std::endl;
	
}