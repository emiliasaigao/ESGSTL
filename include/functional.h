#pragma once

// 本文件包含了 仿函数（函数对象）和哈希函数
#include <cstddef>

namespace esgstl {
	
	// 一元函数
	template <class Arg,class Result>
	struct unary_function {
		typedef Arg			argument_type;
		typedef Result		result_type;
	};

	// 二元函数
	template <class Arg1,class Arg2,class Result>
	struct binary_function {
		typedef Arg1		first_argument_type;
		typedef Arg2		second_argument_type;
		typedef Result		result_type;
	};

	// 加法仿函数
	template <class T>
	struct plus : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x + y; }
	};

	// 减法仿函数
	template <class T>
	struct minus : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x - y; }
	};

	// 乘法仿函数
	template <class T>
	struct mutiplies : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x * y; }
	};

	// 除法仿函数
	template <class T>
	struct divides : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x / y; }
	};

	// 取模仿函数
	template <class T>
	struct modulus : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x % y; }
	};

	// 取反仿函数
	template <class T>
	struct negate : public unary_function<T, T> {
		T operator()(const T& x) const { return -x; }
	};

	// 说明：证同元素函数未被标准库收录，它的作用是提供一个进行某种运算前的初值
	// 证同元素获取函数,加法的证同元素是0
	template <class T>
	T identity_element(plus<T>) { return T(0); }

	// 证同元素获取函数,乘法的证同元素是1
	// 使用实例，numeric中的power(重复做n次运算)函数在n等于0时，会返回这种运算的证同元素
	template <class T>
	T identity_element(mutiplies<T>) { return T(1); }

	// 等于仿函数
	template <class T>
	struct equal_to : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x == y; }
	};

	// 不等于仿函数
	template <class T>
	struct not_equal_to : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x != y; }
	};

	// 小于仿函数
	template <class T>
	struct less : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x < y; }
	};

	// 小于等于仿函数
	template <class T>
	struct less_equal : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x <= y; }
	};

	// 大于仿函数
	template <class T>
	struct greater : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x > y; }
	};

	// 大于等于仿函数
	template <class T>
	struct greater_equal : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x >= y; }
	};

	// 逻辑与仿函数
	template <class T>
	struct logical_and : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x && y; }
	};

	// 逻辑或仿函数
	template <class T>
	struct logical_or : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x || y; }
	};

	// 逻辑非仿函数
	template <class T>
	struct logical_not : public unary_function<T, bool> {
		bool operator()(const T& x) const { return !x; }
	};

	// 证同、选择和投射仿函数
	// 这些仿函数只是忠诚地返回参数的本身或者部分，作用是提供间接性
	// 部分仿函数在set和map会使用到
	// 证同仿函数
	template <class T>
	struct identity : public unary_function<T, T> {
		const T& operator()(const T& x) const { return x; }
	};

	// 选择函数，接收一个pair返回其中的第一个元素
	template <class Pair>
	struct select1st :public unary_function<Pair, typename Pair::first_type> {
		const typename Pair::first_type& operator()(const Pair& x) const {
			return x.first;
		}
	};

	// 选择函数，接收一个pair返回其中的第二个元素
	template <class Pair>
	struct select2nd :public unary_function<Pair, typename Pair::second_type> {
		const typename Pair::second_type& operator()(const Pair& x) const {
			return x.second;
		}
	};

	// 投射函数：返回第一参数
	template <class Arg1, class Arg2>
	struct projectfirst :public binary_function<Arg1, Arg2, Arg1> {
		Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
	};

	// 投射函数：返回第二参数
	template <class Arg1, class Arg2>
	struct projectsecond :public binary_function<Arg1, Arg2, Arg1> {
		Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
	};

	/********************************************************************/
	
	// 哈希函数对象
	// 对于大部分类型，hash function 什么都不做
	// 对于不存在于下面这些类型的类，用户需要自行提供hush_function
	template <class Key>
	struct hash {};

	// 针对指针的偏特化版本
	template <class T>
	struct hash<T*>
	{
		size_t operator()(T* p) const noexcept
		{
			return reinterpret_cast<size_t>(p);
		}
	};

	// 对于整型类型，只是返回原值
	#define ESGSTL_TRIVIAL_HASH_FCN(Type)				\
	template <> struct hash<Type> {						\
		size_t operator()(Type val) const noexcept {	\
			return static_cast<size_t>(val);			\
		}												\
	};

	ESGSTL_TRIVIAL_HASH_FCN(bool)

	ESGSTL_TRIVIAL_HASH_FCN(char)

	ESGSTL_TRIVIAL_HASH_FCN(signed char)

	ESGSTL_TRIVIAL_HASH_FCN(unsigned char)

	ESGSTL_TRIVIAL_HASH_FCN(wchar_t)

	ESGSTL_TRIVIAL_HASH_FCN(char16_t)

	ESGSTL_TRIVIAL_HASH_FCN(char32_t)

	ESGSTL_TRIVIAL_HASH_FCN(short)

	ESGSTL_TRIVIAL_HASH_FCN(unsigned short)

	ESGSTL_TRIVIAL_HASH_FCN(int)

	ESGSTL_TRIVIAL_HASH_FCN(unsigned int)

	ESGSTL_TRIVIAL_HASH_FCN(long)

	ESGSTL_TRIVIAL_HASH_FCN(unsigned long)

	ESGSTL_TRIVIAL_HASH_FCN(long long)

	ESGSTL_TRIVIAL_HASH_FCN(unsigned long long)

	#undef ESGSTL_TRIVIAL_HASH_FCN

	// 对于浮点数，逐位哈希
	inline size_t bitwise_hash(const unsigned char* first, size_t count)
	{
	#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) &&__SIZEOF_POINTER__ == 8)
	const size_t fnv_offset = 14695981039346656037ull;
	const size_t fnv_prime = 1099511628211ull;
	#else
	const size_t fnv_offset = 2166136261u;
	const size_t fnv_prime = 16777619u;
	#endif
	size_t result = fnv_offset;
	for (size_t i = 0; i < count; ++i)
	{
		result ^= (size_t)first[i];
		result *= fnv_prime;
	}
	return result;
	}

	template <>
	struct hash<float>
	{
		size_t operator()(const float& val)
		{
			return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(float));
		}
	};

	template <>
	struct hash<double>
	{
		size_t operator()(const double& val)
		{
			return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(double));
		}
	};

	template <>
	struct hash<long double>
	{
		size_t operator()(const long double& val)
		{
			return val == 0.0f ? 0 : bitwise_hash((const unsigned char*)&val, sizeof(long double));
		}
	};

}