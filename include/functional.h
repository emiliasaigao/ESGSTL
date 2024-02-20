#pragma once

// ���ļ������� �º������������󣩺͹�ϣ����
#include <cstddef>

namespace esgstl {
	
	// һԪ����
	template <class Arg,class Result>
	struct unary_function {
		typedef Arg			argument_type;
		typedef Result		result_type;
	};

	// ��Ԫ����
	template <class Arg1,class Arg2,class Result>
	struct binary_function {
		typedef Arg1		first_argument_type;
		typedef Arg2		second_argument_type;
		typedef Result		result_type;
	};

	// �ӷ��º���
	template <class T>
	struct plus : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x + y; }
	};

	// �����º���
	template <class T>
	struct minus : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x - y; }
	};

	// �˷��º���
	template <class T>
	struct mutiplies : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x * y; }
	};

	// �����º���
	template <class T>
	struct divides : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x / y; }
	};

	// ȡģ�º���
	template <class T>
	struct modulus : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x % y; }
	};

	// ȡ���º���
	template <class T>
	struct negate : public unary_function<T, T> {
		T operator()(const T& x) const { return -x; }
	};

	// ˵����֤ͬԪ�غ���δ����׼����¼�������������ṩһ������ĳ������ǰ�ĳ�ֵ
	// ֤ͬԪ�ػ�ȡ����,�ӷ���֤ͬԪ����0
	template <class T>
	T identity_element(plus<T>) { return T(0); }

	// ֤ͬԪ�ػ�ȡ����,�˷���֤ͬԪ����1
	// ʹ��ʵ����numeric�е�power(�ظ���n������)������n����0ʱ���᷵�����������֤ͬԪ��
	template <class T>
	T identity_element(mutiplies<T>) { return T(1); }

	// ���ڷº���
	template <class T>
	struct equal_to : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x == y; }
	};

	// �����ڷº���
	template <class T>
	struct not_equal_to : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x != y; }
	};

	// С�ڷº���
	template <class T>
	struct less : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x < y; }
	};

	// С�ڵ��ڷº���
	template <class T>
	struct less_equal : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x <= y; }
	};

	// ���ڷº���
	template <class T>
	struct greater : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x > y; }
	};

	// ���ڵ��ڷº���
	template <class T>
	struct greater_equal : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x >= y; }
	};

	// �߼���º���
	template <class T>
	struct logical_and : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x && y; }
	};

	// �߼���º���
	template <class T>
	struct logical_or : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x || y; }
	};

	// �߼��Ƿº���
	template <class T>
	struct logical_not : public unary_function<T, bool> {
		bool operator()(const T& x) const { return !x; }
	};

	// ֤ͬ��ѡ���Ͷ��º���
	// ��Щ�º���ֻ���ҳϵط��ز����ı�����߲��֣��������ṩ�����
	// ���ַº�����set��map��ʹ�õ�
	// ֤ͬ�º���
	template <class T>
	struct identity : public unary_function<T, T> {
		const T& operator()(const T& x) const { return x; }
	};

	// ѡ����������һ��pair�������еĵ�һ��Ԫ��
	template <class Pair>
	struct select1st :public unary_function<Pair, typename Pair::first_type> {
		const typename Pair::first_type& operator()(const Pair& x) const {
			return x.first;
		}
	};

	// ѡ����������һ��pair�������еĵڶ���Ԫ��
	template <class Pair>
	struct select2nd :public unary_function<Pair, typename Pair::second_type> {
		const typename Pair::second_type& operator()(const Pair& x) const {
			return x.second;
		}
	};

	// Ͷ�亯�������ص�һ����
	template <class Arg1, class Arg2>
	struct projectfirst :public binary_function<Arg1, Arg2, Arg1> {
		Arg1 operator()(const Arg1& x, const Arg2&) const { return x; }
	};

	// Ͷ�亯�������صڶ�����
	template <class Arg1, class Arg2>
	struct projectsecond :public binary_function<Arg1, Arg2, Arg1> {
		Arg2 operator()(const Arg1&, const Arg2& y) const { return y; }
	};

	/********************************************************************/
	
	// ��ϣ��������
	// ���ڴ󲿷����ͣ�hash function ʲô������
	// ���ڲ�������������Щ���͵��࣬�û���Ҫ�����ṩhush_function
	template <class Key>
	struct hash {};

	// ���ָ���ƫ�ػ��汾
	template <class T>
	struct hash<T*>
	{
		size_t operator()(T* p) const noexcept
		{
			return reinterpret_cast<size_t>(p);
		}
	};

	// �����������ͣ�ֻ�Ƿ���ԭֵ
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

	// ���ڸ���������λ��ϣ
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