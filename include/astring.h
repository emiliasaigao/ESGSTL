#pragma once

// ∂®“Âstring°¢wstring°¢u16string°¢u32string
#include "basic_string.h"

namespace esgstl {
	using string = basic_string<char>;
	using wstring = basic_string<wchar_t>;
	using u16string = basic_string<char16_t>;
	using u32string = basic_string<char32_t>;
}