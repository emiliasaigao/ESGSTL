#pragma once
#include <type_traits>

// 类型萃取器，基于标准库的同名文件进行修改

namespace esgstl {
	// 定义一些辅助类

	// 编译器常数类
	template<class T,T v>
	struct m_intergral_constant {
		static constexpr T value = v;
	};

	// 总之目的就是为了造出编译器认识的代表true和false的对象
	template<bool b>
	using m_bool_constant = m_intergral_constant<bool, b>;

	typedef m_bool_constant<true> m_true_type;
	typedef m_bool_constant<false> m_false_type;

	// pair定义在util头文件中，这里做了前置声明了以后就可以用，目的是降低文件之间的依存性
	// 详见《Effective C++》条款31
	template<class T1,class T2>
	struct pair;

	// 只有一个模板参数的类不是pair
	template<class T>
	struct is_pair : esgstl::m_false_type{};

	template<class T1,class T2>
	struct is_pair<esgstl::pair<T1, T2>> : esgstl::m_true_type{};

}