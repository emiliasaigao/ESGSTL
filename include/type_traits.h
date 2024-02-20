#pragma once
#include <type_traits>

// ������ȡ�������ڱ�׼���ͬ���ļ������޸�

namespace esgstl {
	// ����һЩ������

	// ������������
	template<class T,T v>
	struct m_intergral_constant {
		static constexpr T value = v;
	};

	// ��֮Ŀ�ľ���Ϊ�������������ʶ�Ĵ���true��false�Ķ���
	template<bool b>
	using m_bool_constant = m_intergral_constant<bool, b>;

	typedef m_bool_constant<true> m_true_type;
	typedef m_bool_constant<false> m_false_type;

	// pair������utilͷ�ļ��У���������ǰ���������Ժ�Ϳ����ã�Ŀ���ǽ����ļ�֮���������
	// �����Effective C++������31
	template<class T1,class T2>
	struct pair;

	// ֻ��һ��ģ��������಻��pair
	template<class T>
	struct is_pair : esgstl::m_false_type{};

	template<class T1,class T2>
	struct is_pair<esgstl::pair<T1, T2>> : esgstl::m_true_type{};

}