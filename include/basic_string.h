#pragma once

// ��ͷ�ļ�������string�ĵײ���ʵ���basic_stringģ���࣬�Լ�������ȡ���ͼ�������char_traitsģ����

#include<iostream> // �������������������

#include "exceptdef.h" // ʹ����һЩ�������
#include "iterator.h"  // �ж�ĳ���Ƿ�Ϊ�ض�����������
#include "functional.h" // Ϊ���ػ�hash��
#include "memory.h"	   // ���ٿռ�

namespace esgstl {

	template<class CharType>
	struct char_traits {
		typedef CharType char_type;

		// �����ַ����鳤��
		static size_t length(const char_type* str) {
			size_t len = 0;
			while (*str != char_type(0)) {
				++len; ++str;
			}
			return len;
		}

		// �Ƚ�s1��s2��ǰn����ĸ�Ƚ��µĴ�С������-1/0/1
		static int compare(const char_type* str1, const char_type* str2, size_t n) {
			while (n > 0) {
				if (*str1 < *str2) return -1;
				else if (*str2 < *str1) return 1;
				--n; ++str1; ++str2;
			}
			return 0;
		}

		// ��src���鿽��n���ַ���dst������dst�������ʱ��ָ��λ��
		static char_type* copy(char_type* dst, const char_type* src, size_t n) {
			ESGSTL_DEBUG(src + n <= dst || dst + n <= src);
			while (n > 0) {
				*dst++ = *src++;
				n--;
			}
			return dst;
		}

		// ��src����"�ƶ�"��ʵ�ʻ��ǿ�����n���ַ���dst������dst�������ʱ��ָ��λ�ã������ཻ��
		static char_type* move(char_type* dst, const char_type* src, size_t n) {
			auto res = dst + n;
			if (dst < src) {
				while (n > 0) {
					*dst++ = *src++;
					--n;
				}
			}
			else if (src < dst){
				src += n;
				dst += n;
				while (n > 0) {
					*--dst = *--src;
					n--;
				}
			}
			return res;
		}

		// ��dst��ָλ�����n��ch����������dst��ָ��λ��
		static char_type* fill(char_type* dst, char_type ch, size_t n) {
			while (n > 0) {
				*dst++ = ch;
				--n;
			}
			return dst;
		}
	};

	// ��char���ػ��棬ʹ����c�и��ײ���㷨
	template<>
	struct char_traits<char> {
		typedef char char_type;

		// �����ַ����鳤��
		static size_t length(const char_type* str) noexcept {
			return std::strlen(str);
		}

		// �Ƚ�s1��s2��ǰn����ĸ�Ƚ��µĴ�С������-1/0/1
		static int compare(const char_type* str1, const char_type* str2, size_t n) noexcept {
			return std::memcmp(str1, str2, n);
		}

		// ��src���鿽��n���ַ���dst������dst�������ʱ��ָ��λ��
		static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
			ESGSTL_DEBUG(src + n <= dst || dst + n <= src);
			std::memcpy(dst,src,n);
			return dst + n;
		}

		// ��src�����ƶ�n���ַ���dst������dst�������ʱ��ָ��λ�ã������ཻ��
		static char_type* move(char_type* dst, const char_type* src, size_t n) {
			std::memmove(dst, src, n);
			return dst+n;
		}

		// ��dst��ָλ�����n��ch����������dst��ָ��λ��
		static char_type* fill(char_type* dst, char_type ch, size_t n) {
			std::memset(dst, ch, n);
			return dst + n;
		}
	};

	// ��wchar_t���ػ��汾
	template<>
	struct char_traits<wchar_t> {
		typedef wchar_t char_type;

		// �����ַ����鳤��
		static size_t length(const char_type* str) noexcept {
			return std::wcslen(str);
		}

		// �Ƚ�s1��s2��ǰn����ĸ�Ƚ��µĴ�С������-1/0/1
		static int compare(const char_type* str1, const char_type* str2, size_t n) noexcept {
			return std::wmemcmp(str1, str2, n);
		}

		// ��src���鿽��n���ַ���dst������dst�������ʱ��ָ��λ��
		static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
			ESGSTL_DEBUG(src + n <= dst || dst + n <= src);
			std::wmemcpy(dst, src, n);
			return dst + n;
		}

		// ��src�����ƶ�n���ַ���dst������dst�������ʱ��ָ��λ�ã������ཻ��
		static char_type* move(char_type* dst, const char_type* src, size_t n) {
			std::wmemmove(dst, src, n);
			return dst + n;
		}

		// ��dst��ָλ�����n��ch����������dst��ָ��λ��
		static char_type* fill(char_type* dst, char_type ch, size_t n) {
			std::wmemset(dst, ch, n);
			return dst + n;
		}
	};

	// ��char16_t���ػ��汾�����з����������׳��쳣��Ҳ��Ӧ�ó����쳣
	template<>
	struct char_traits<char16_t> {
		typedef char16_t char_type;

		// �����ַ����鳤��
		static size_t length(const char_type* str) noexcept {
			size_t len = 0;
			while (*str != char_type(0)) {
				++len; ++str;
			}
			return len;
		}

		// �Ƚ�s1��s2��ǰn����ĸ�Ƚ��µĴ�С������-1/0/1
		static int compare(const char_type* str1, const char_type* str2, size_t n) noexcept {
			while (n > 0) {
				if (*str1 < *str2) return -1;
				else if (*str2 < *str1) return 1;
				--n; ++str1; ++str2;
			}
			return 0;
		}

		// ��src���鿽��n���ַ���dst������dst�������ʱ��ָ��λ��
		static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
			ESGSTL_DEBUG(src + n <= dst || dst + n <= src);
			while (n > 0) {
				*dst++ = *src++;
				n--;
			}
			return dst;
		}

		// ��src����"�ƶ�"��ʵ�ʻ��ǿ�����n���ַ���dst������dst�������ʱ��ָ��λ�ã������ཻ��
		static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept {
			auto res = dst + n;
			if (dst < src) {
				while (n > 0) {
					*dst++ = *src++;
					--n;
				}
			}
			else if (src < dst) {
				src += n;
				dst += n;
				while (n > 0) {
					*--dst = *--src;
					n--;
				}
			}
			return res;
		}

		// ��dst��ָλ�����n��ch����������dst��ָ��λ��
		static char_type* fill(char_type* dst, char_type ch, size_t n) noexcept {
			while (n > 0) {
				*dst++ = ch;
				--n;
			}
			return dst;
		}
	};

	// ��char32_t���ػ��汾�����з����������׳��쳣��Ҳ��Ӧ�ó����쳣
	template<>
	struct char_traits<char32_t> {
		typedef char32_t char_type;

		// �����ַ����鳤��
		static size_t length(const char_type* str) noexcept {
			size_t len = 0;
			while (*str != char_type(0)) {
				++len; ++str;
			}
			return len;
		}

		// �Ƚ�s1��s2��ǰn����ĸ�Ƚ��µĴ�С������-1/0/1
		static int compare(const char_type* str1, const char_type* str2, size_t n) noexcept {
			while (n > 0) {
				if (*str1 < *str2) return -1;
				else if (*str2 < *str1) return 1;
				--n; ++str1; ++str2;
			}
			return 0;
		}

		// ��src���鿽��n���ַ���dst������dst�������ʱ��ָ��λ��
		static char_type* copy(char_type* dst, const char_type* src, size_t n) noexcept {
			ESGSTL_DEBUG(src + n <= dst || dst + n <= src);
			while (n > 0) {
				*dst++ = *src++;
				n--;
			}
			return dst;
		}

		// ��src����"�ƶ�"��ʵ�ʻ��ǿ�����n���ַ���dst������dst�������ʱ��ָ��λ�ã������ཻ��
		static char_type* move(char_type* dst, const char_type* src, size_t n) noexcept {
			auto res = dst + n;
			if (dst < src) {
				while (n > 0) {
					*dst++ = *src++;
					--n;
				}
			}
			else if (src < dst) {
				src += n;
				dst += n;
				while (n > 0) {
					*--dst = *--src;
					n--;
				}
			}
			return res;
		}

		// ��dst��ָλ�����n��ch����������dst��ָ��λ��
		static char_type* fill(char_type* dst, char_type ch, size_t n) noexcept {
			while (n > 0) {
				*dst++ = ch;
				--n;
			}
			return dst;
		}
	};

// ��ʼ�� basic_string ���Է������С buffer ��С�����ܱ�����
#define STRING_INIT_SIZE 32

	template<class CharType, class CharTraits = esgstl::char_traits<CharType>>
	class basic_string {
	
	public:
		// һЩ�ͱ���
		typedef CharTraits										traits_type;
		typedef CharTraits										char_traits;

		typedef esgstl::allocator<CharType>						allocator_type;
		typedef esgstl::allocator<CharType>						data_allocator;
		
		typedef typename allocator_type::value_type				value_type;
		typedef typename allocator_type::pointer				pointer;
		typedef typename allocator_type::reference				reference;
		typedef typename allocator_type::const_pointer			const_pointer;
		typedef typename allocator_type::const_reference		const_reference;
		typedef typename allocator_type::size_type				size_type;
		typedef typename allocator_type::difference_type		difference_type;

		typedef value_type*										iterator;
		typedef const value_type*								const_iterator;
		typedef esgstl::reverse_iterator<iterator>				reverse_iterator;
		typedef const esgstl::reverse_iterator<iterator>		const_reverse_iterator;

	public:
		static_assert(std::is_trivially_copyable<CharType>::value, "Character type of basic_string must be a POD");
		static_assert(std::is_same<CharType, typename traits_type::char_type>::value,
			"CharType must be same as traits_type::char_type");

	public:
		// size_type ����size_t������npos�������Ϊ�����޷�����
		static constexpr size_type npos = static_cast<size_type>(-1);

	private:
		iterator buffer_;  // ָ����������ĸ��ָ��
		size_type size_;   // �ַ�������
		size_type capacity_; // �ַ�������

	public:
		// ���캯��
		basic_string():buffer_(nullptr),size_(0),capacity_(0){}
		basic_string(size_type n, value_type ch) 
			:buffer_(nullptr), size_(0), capacity_(0) 
		{
			fill_init(n, ch);
		}
		basic_string(const basic_string& other, size_type pos) 
			:buffer_(nullptr), size_(0), capacity_(0)
		{
			init_from(other.buffer_, pos, other.size_ - pos);
		}
		basic_string(const basic_string& other, size_type pos, size_type n) 
			:buffer_(nullptr), size_(0), capacity_(0)
		{
			init_from(other.buffer_, pos, n);
		}
		basic_string(const_pointer str) 
			:buffer_(nullptr), size_(0), capacity_(0) 
		{
			init_from(str, 0, char_traits::length(str));
		}
		basic_string(const_pointer str, size_type n)
			:buffer_(nullptr), size_(0), capacity_(0)
		{
			init_from(str, 0, n);
		}
		template <class Iter, typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value, int>::type = 0>
		basic_string(Iter first, Iter last)
		{
			copy_init(first, last, esgstl::iterator_category(first));
		}
		basic_string(const basic_string& rhs)
			:buffer_(nullptr), size_(0), capacity_(0)
		{
			init_from(rhs.buffer_, 0, rhs.size_);
		}
		basic_string(basic_string&& rhs) noexcept
			:buffer_(rhs.buffer_), size_(rhs.size_), capacity_(rhs.cap_)
		{
			rhs.buffer_ = nullptr;
			rhs.size_ = 0;
			rhs.cap_ = 0;
		}

		// ��ֵ�����
		basic_string& operator=(const basic_string& rhs);
		basic_string& operator=(basic_string&& rhs) noexcept;

		basic_string& operator=(const_pointer str);
		basic_string& operator=(value_type ch);
		
		// ��������
		~basic_string() { destory_buffer(); }

	public:
		// ��������ز���
		/*****************************************************************/
		//// ���������
		iterator				begin() noexcept { return buffer_; }
		const_iterator			begin() const noexcept { return buffer_; }
		iterator				end() noexcept { return buffer_ + size_; }
		const_iterator			end() const noexcept { return buffer_ + size_; }

		//// ���������
		reverse_iterator		rbegin() noexcept { return reverse_iterator(end()); }
		const_reverse_iterator	rbegin() const noexcept { return const_reverse_iterator(end()); }
		reverse_iterator		rend() noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator	rend() const noexcept { return const_reverse_iterator(begin()); }

		//// ����������
		const_iterator			cbegin() const noexcept { return begin(); }
		const_iterator			cend() const noexcept { return end(); }
		const_reverse_iterator	crbegin() const noexcept { return rbegin(); }
		const_reverse_iterator	crend() const noexcept { return rend(); }
		/*****************************************************************/

		// ������ز���
		/*****************************************************************/
		bool empty()			const noexcept { return size_ == 0; }
		size_type size()		const noexcept { return size_; }
		size_type capacity()	const noexcept { return capacity_; }
		size_type max_size()	const noexcept { return static_cast<size_type>(-1); }

		//// Ԥ׼���ռ�
		void reserve(size_type n);
		//// ��С�ַ����ݻ���ʵ�ʴ�С
		void shrink_to_fit();
		/*****************************************************************/

		// ����Ԫ����ز���
		/*****************************************************************/
		reference operator[](size_type n) {
			// ע����С�ڵ��ڣ�����ʱ����Խ��
			ESGSTL_DEBUG(n <= size_);
			if (n == size_)
				*(buffer_ + n) = value_type();
			return *(buffer_ + n);
		}

		const_reference operator[](size_type n) const {
			ESGSTL_DEBUG(n <= size_);
			if (n == size_)
				*(buffer_ + n) = value_type();
			return *(buffer_ + n);
		}

		reference at(size_type n) {
			ESGSTL_OUT_OF_RANGE_IF(n >= size_, "basic_string<Char, Traits>::at()"
				"subscript out of range");
			return (*this)[n];
		}

		const_reference at(size_type n) const {
			ESGSTL_OUT_OF_RANGE_IF(n >= size_, "basic_string<Char, Traits>::at()"
				"subscript out of range");
			return (*this)[n];
		}

		reference front() {
			ESGSTL_DEBUG(!empty());
			return *buffer_;
		}

		const_reference front() const {
			ESGSTL_DEBUG(!empty());
			return *buffer_;
		}

		reference back() {
			ESGSTL_DEBUG(!empty());
			return *(end() - 1);
		}

		const_reference back() const {
			ESGSTL_DEBUG(!empty());
			return *(end() - 1);
		}

		const_pointer data() const noexcept { return to_raw_pointer(); }
		const_pointer c_str() const noexcept { return to_raw_pointer(); }
		/*****************************************************************/

		// ��ɾ ��ز���
		/*****************************************************************/

		//// insertϵ��
		iterator insert(const_iterator pos, value_type ch);
		iterator insert(const_iterator pos, size_type n, value_type ch);
		template <class Iter>
		iterator insert(const_iterator pos, Iter first, Iter last);

		//// appendϵ��
		basic_string& append(size_type n, value_type ch);

		basic_string& append(const basic_string& str, size_type pos, size_type len);
		basic_string& append(const basic_string& str) {
			return append(str, 0, str.size_);
		}
		basic_string& append(const basic_string& str, size_type pos) {
			return append(str, pos, str.size_ - pos);
		}

		basic_string& append(const_pointer s, size_type len);
		basic_string& append(const_pointer s) {
			return append(s, char_traits::length(s));
		}

		template <class Iter,typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value,int>::type = 0>
		basic_string& append(Iter first, Iter last) {
			append_range(first, last);
		}

		//// push_back\pop_back
		void push_back(value_type ch) { append(1, ch); }
		void pop_back() {
			ESGSTL_DEBUG(!empty());
			--size_;
		}

		//// erase/clear
		iterator erase(const_iterator pos);
		iterator erase(const_iterator first, const_iterator last);
		void clear() noexcept { size_ = 0; };

		//// resize
		void resize(size_type n) { resize(n, value_type()); }
		void resize(size_type n, value_type ch);

		/*****************************************************************/

		// basic_string ������ز���
		/*****************************************************************/
		//// swap
		void swap(basic_string& rhs) noexcept;
		
		//// reverse
		void reverse() noexcept;

		//// compare
		int compare(const basic_string& rhs) const;
		int compare(size_type pos1, size_type count1, const basic_string& rhs) const;
		int compare(size_type pos1, size_type count1, const basic_string& rhs,
			size_type pos2, size_type count2 = npos) const;
		int compare(const_pointer s) const;
		int compare(size_type pos1, size_type count1, const_pointer s) const;
		int compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const;

		//// substr
		basic_string substr(size_type pos, size_type n = npos) {
			size_type len = esgstl::min(n, size_ - pos);
			return basic_string(*this, pos, len);
		}

		//// replace
		basic_string& replace(size_type pos, size_type count, const basic_string& str) {
			ESGSTL_OUT_OF_RANGE_IF(pos >= size_, "basic_string<Char, Traits>::replace's pos out of range");
			return replace_cstr(buffer_ + pos, count, str.buffer_, str.size_);
		}
		basic_string& replace(size_type pos, size_type count, const_iterator s) {
			ESGSTL_OUT_OF_RANGE_IF(pos >= size_, "basic_string<Char, Traits>::replace's pos out of range");
			return replace_cstr(buffer_ + size_, count, s, char_traits::length(s));
		}

		basic_string& replace(const_iterator first, const_iterator last, const basic_string& str) {
			ESGSTL_DEBUG(first < last&& first <= begin() && last <= end());
			return replace_cstr(first, static_cast<size_type>(last-first), str.buffer_, str.size_);
		}
		basic_string& replace(const_iterator first, const_iterator last, const_iterator s) {
			ESGSTL_DEBUG(first < last&& first <= begin() && last <= end());
			return replace_cstr(first, static_cast<size_type>(last - first), s, char_traits::length(s));
		}

		basic_string& replace(size_type pos, size_type count1, const_iterator s, size_type count2) {
			ESGSTL_OUT_OF_RANGE_IF(pos >= size_, "basic_string<Char, Traits>::replace's pos out of range");
			return replace_cstr(buffer_ + size_, count, s, count2);
		}
		basic_string& replace(const_iterator first, const_iterator last, const_iterator s, size_type count2) {
			ESGSTL_DEBUG(first < last&& first <= begin() && last <= end());
			return replace_cstr(first, static_cast<size_type>(last - first), s, count2);
		}
		
		basic_string& replace(size_type pos, size_type count, size_type n, value_type ch) {
			ESGSTL_OUT_OF_RANGE_IF(pos >= size_, "basic_string<Char, Traits>::replace's pos out of range");
			return replace_fill(buffer_ + pos,count, n, ch);
		}
		basic_string& replace(const_iterator first, const_iterator last, size_type n, value_type ch) {
			ESGSTL_DEBUG(first < last&& first <= begin() && last <= end());
			return replace_cstr(first, static_cast<size_type>(last - first), n, ch);
		}

		basic_string& replace(size_type pos1, size_type count1, const basic_string& str,
			size_type pos2, size_type count2 = npos) {
			ESGSTL_OUT_OF_RANGE_IF(pos1 >= size_ || pos2 >= str.size_ ,
				"basic_string<Char, Traits>::replace's pos out of range");
			return replace_cstr(buffer_ + pos1, count1, str.buffer_ + pos2, count2);
		}

		template <class Iter, typename std::enable_if<
			esgstl::is_input_iterator<Iter>::value, int>::type = 0>
		basic_string& replace(const_iterator first1, const_iterator last1, Iter first2, Iter last2) {
			ESGSTL_DEBUG(first1 < last1 && first1 <= begin() && last1 <= end());
			return replace_copy(first1, last1, first2, last2);
		}

		
		/*****************************************************************/

		// ������ز���
		/*****************************************************************/
		// find : n^2�����ѷ�
		size_type find(value_type ch, size_type pos = 0)						const noexcept;
		size_type find(const_iterator s, size_type pos = 0)						const noexcept;
		size_type find(const_iterator s, size_type pos, size_type count)		const noexcept;
		size_type find(const basic_string& str,size_type pos = 0)				const noexcept;

		size_type find_KMP(const basic_string& str)								const noexcept;

		// rfind
		size_type rfind(value_type ch, size_type pos = npos)					const noexcept;
		size_type rfind(const_pointer s, size_type pos = npos)					const noexcept;
		size_type rfind(const_pointer s, size_type pos, size_type count)		const noexcept;
		size_type rfind(const basic_string& str, size_type pos = npos)          const noexcept;

		// find_first_of:�ҵ�����һ���ַ���һ�γ��ֵ�λ��
		size_type find_first_of(value_type ch, size_type pos = 0)					const noexcept;
		size_type find_first_of(const_pointer s, size_type pos = 0)					const noexcept;
		size_type find_first_of(const_pointer s, size_type pos, size_type count)		const noexcept;
		size_type find_first_of(const basic_string& str, size_type pos = 0)			const noexcept;
		
		// find_first_not_of:�ҵ��ַ����е�һ���������ڵ�ָ�����ַ���λ��
		size_type find_first_not_of(value_type ch, size_type pos = 0)					const noexcept;
		size_type find_first_not_of(const_pointer s, size_type pos = 0)					const noexcept;
		size_type find_first_not_of(const_pointer s, size_type pos, size_type count)		const noexcept;
		size_type find_first_not_of(const basic_string& str, size_type pos = 0)			const noexcept;

		// find_last_of:��������ҵ�����һ���ַ���һ�γ��ֵ�λ��
		size_type find_last_of(value_type ch, size_type pos = npos)                       const noexcept;
		size_type find_last_of(const_pointer s, size_type pos = npos)                     const noexcept;
		size_type find_last_of(const_pointer s, size_type pos, size_type count)           const noexcept;
		size_type find_last_of(const basic_string& str, size_type pos = npos)             const noexcept;


		// find_last_not_of:��������ַ����е�һ����������ָ�������ַ���λ��
		size_type find_last_not_of(value_type ch, size_type pos = npos)                       const noexcept;
		size_type find_last_not_of(const_pointer s, size_type pos = npos)                     const noexcept;
		size_type find_last_not_of(const_pointer s, size_type pos, size_type count)           const noexcept;
		size_type find_last_not_of(const basic_string& str, size_type pos = npos)             const noexcept;
		
		//// count
		size_type count(value_type ch, size_type pos) const noexcept;
		/*****************************************************************/
		
	public:
		// ���������
		/*****************************************************************/
		//// +=
		basic_string& operator+=(const basic_string& other) { return append(other); }
		basic_string& operator+=(const_iterator s) { return append(s); }
		basic_string& operator+=(value_type ch) { return append(1, ch); }

		//// >>\<<
		friend std::istream& operator>>(std::istream& is, basic_string& str) {
			value_type* buf = new value_type[4096];
			is >> buf;
			basic_string tmp(buf);
			str = esgstl::move(tmp);
			delete[] buf;
			return is;
		}

		friend std::ostream& operator<<(std::ostream& os, const basic_string& str) {
			for (size_t i = 0; i < str.size_; ++i) {
				os << str[i];
			}
			return os;
		}
		/*****************************************************************/

	private:
		// ˽�и�������
		/*****************************************************************/
		
		//// ��ʼ����ʵ�߼�����
		void try_init() noexcept;
		void fill_init(size_type n, value_type ch);
		template<class Iter>
		void copy_init(Iter first, Iter last, esgstl::input_iterator_tag);
		template<class Iter>
		void copy_init(Iter first, Iter last, esgstl::forward_iterator_tag);
		void init_from(const_iterator src, size_type pos, size_type n);

		//// ������ʵ�߼�����
		void destory_buffer();

		//// shrink_to_fit��ʵ�߼�����
		void reinsert(size_type n);

		//// data��cstr��ʵ�߼�����
		const_pointer to_raw_pointer() const;
		
		//// reallocate���·���ռ���ز���
		void			reallocate(size_type need);
		iterator		reallocate_and_fill(iterator pos, size_type n, value_type ch);
		iterator		reallocate_and_copy(iterator pos, const_iterator first, const_iterator last);

		//// append(Iter first, Iter last) ��ʵ�߼�����
		template <class InputIter>
		basic_string& append_range(InputIter first, InputIter last);

		//// replace ��ʵ�߼�����
		basic_string& replace_cstr(const_iterator dst, size_type count1, const_iterator src, size_type count2);
		basic_string& replace_fill(const_iterator dst, size_type count, size_type n, value_type ch);
		template <class Iter>
		basic_string& replace_copy(const_iterator first1, const_iterator last1, Iter first2, Iter last2);

	};


	template<class CharType,class CharTraits>
	void basic_string<CharType,CharTraits>::
		try_init() noexcept {
		try
		{
			buffer_ = data_allocator::allocate(static_cast<int>(STRING_INIT_SIZE));
			size_ = 0;
			capacity_ = 0;
		}
		catch (...)
		{
			buffer_ = nullptr;
			size_ = 0;
			capacity_ = 0;
		}
	}

	template<class CharType, class CharTraits>
	void basic_string<CharType, CharTraits>::fill_init(size_type n, value_type ch){
		const auto init_size = esgstl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
		buffer_ = data_allocator::allocate(init_size);
		char_traits::fill(buffer_, ch, n);
		size_ = n;
		capacity_ = init_size;
	}

	template <class CharType, class CharTraits>
	template <class Iter>
	void basic_string<CharType, CharTraits>::
		copy_init(Iter first,Iter last,esgstl::input_iterator_tag) {
		auto n = esgstl::distance(first, last);
		const auto init_size = esgstl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
		try
		{
			buffer_ = data_allocator::allocate(init_size);
			size_ = n;
			capacity_ = init_size;
		}
		catch (...)
		{
			buffer_ = nullptr;
			size_ = 0;
			capacity_ = 0;
			throw;
		}
		while (n > 0) {
			append(*first++);
			--n;
		}
	}

	template <class CharType, class CharTraits>
	template <class Iter>
	void basic_string<CharType, CharTraits>::
		copy_init(Iter first, Iter last, esgstl::forward_iterator_tag) {
		auto n = esgstl::distance(first, last);
		const auto init_size = esgstl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
		try
		{
			buffer_ = data_allocator::allocate(init_size);
			size_ = n;
			capacity_ = init_size;
			esgstl::uninitialized_copy(first, last, buffer_);	
		}
		catch (...)
		{
			buffer_ = nullptr;
			size_ = 0;
			capacity_ = 0;
			throw;
		}
	}

	template<class CharType, class CharTraits>
	template<class InputIter>
	basic_string<CharType, CharTraits>& 
		basic_string<CharType, CharTraits>::
		append_range(InputIter first, InputIter last) {
		auto n = esgstl::advance(first, last);
		ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - n,
			"basic_string<Char, Tratis>'s size too big");
		if (capacity_ - size_ < n) {
			reallocate(n);
		}
		esgstl::uninitialized_copy(first, last, end());
		size_ += n;
		return *this;
	}

	template<class CharType, class CharTraits>
	template<class Iter>
	basic_string<CharType, CharTraits>& 
		basic_string<CharType, CharTraits>::
		replace_copy(const_iterator first1, const_iterator last1, 
			Iter first2, Iter last2) {
		size_type count1 = static_cast<size_type>(last1 - first1);
		size_type count2 = static_cast<size_type>(esgstl::distance(first2, last2));
		auto p = const_cast<iterator>(first1);
		const size_type len = esgstl::min(count1, count2);
		for (size_type i = 0; i < len; ++i, ++p,++first2) {
			*p = *(first2);
			--count1;
			--count2;
		}
		if (count1 > 0) {
			erase(p, p + count1);
		}
		else {
			insert(p, first2, last2);
		}
		return *this;
	}

	template<class CharType, class CharTraits>
	void basic_string<CharType, CharTraits>::
		init_from(const_iterator src, size_type pos, size_type n) {
		auto init_size = esgstl::max(static_cast<size_type>(STRING_INIT_SIZE), n + 1);
		buffer_ = data_allocator::allocate(init_size);
		char_traits::copy(buffer_, src + pos, n);
		size_ = n;
		capacity_ = init_size;
	}

	template<class CharType, class CharTraits>
	void basic_string<CharType, CharTraits>::
		destory_buffer() {
		if (buffer_ != nullptr) {
			data_allocator::deallocate(buffer_, capacity_);
			buffer_ = nullptr;
			size_ = 0;
			capacity_ = 0;
		}
	}
	template<class CharType, class CharTraits>
	typename basic_string<CharType, CharTraits>&
	basic_string<CharType, CharTraits>::
		operator=(const basic_string& rhs) {
		if (this != &rhs) {
			basic_string tmp(rhs);
			swap(tmp);
		}
		return *this;
	}

	template<class CharType, class CharTraits>
	typename basic_string<CharType, CharTraits>&
		basic_string<CharType, CharTraits>::
		operator=(basic_string&& rhs) noexcept {
		destory_buffer();
		buffer_ = rhs.buffer_;
		size_ = rhs.size_;
		capacity_ = rhs.capacity_;
		rhs.buffer_ = nullptr;
		rhs.size_ = 0;
		rhs.capacity_ = 0;
		return *this;
	}

	template<class CharType, class CharTraits>
	typename basic_string<CharType, CharTraits>&
		basic_string<CharType, CharTraits>::
		operator=(const_pointer str) {
		auto len = char_traits::length(str);
		if (capacity_ < len) {
			destory_buffer();
			init_from(str, 0, len);
		}
		else {
			char_traits::copy(buffer_, str, len);
			size_ = len;
		}
		return *this;
	}


	template<class CharType, class CharTraits>
	typename basic_string<CharType, CharTraits>&
		basic_string<CharType, CharTraits>::
		operator=(value_type ch) {
		if (capacity_ < 1) {
			auto new_buffer = data_allocator::allocate(2);
			destory_buffer();
			buffer_ = new_buffer;
			capacity_ = 2;
		}
		*buffer_ = ch;
		size_ = 1;
	}


	template<class CharType, class CharTraits>
	void basic_string<CharType, CharTraits>::
		reserve(size_type n) {
		if (capacity_ < n) {
			ESGSTL_LENGTH_ERROR_IF(n > max_size(), "n can not larger than max_size()"
				"in basic_string<Char,Traits>::reserve(n)");
			auto new_buffer = data_allocator::allocate(n);
			char_traits::move(new_buffer, buffer_, size_);
			buffer_ = new_buffer;
			capacity_ = n;
		}
	}

	template<class CharType, class CharTraits>
	void basic_string<CharType, CharTraits>::
		shrink_to_fit() {
		if (size_ != capacity_) {
			// ��ʵ�߼�
			reinsert(size_);
		}
	}

	template<class CharType, class CharTraits>
	void basic_string<CharType, CharTraits>::
		reinsert(size_type n) {
		auto new_buffer = data_allocator::allocate(n);
		try
		{
			char_traits::move(new_buffer, buffer_, size_);
		}
		catch (...)
		{
			data_allocator::deallocate(new_buffer);
		}
		buffer_ = new_buffer;
		size_ = n;
		capacity_ = n;
	}

	template<class CharType, class CharTraits>
	typename basic_string<CharType, CharTraits>::const_pointer 
		basic_string<CharType, CharTraits>::
		to_raw_pointer() const {
		// ��Ȼ��const�ģ����ǵײ��Ǹı��˵ģ���ĩ�ַ���Ŀռ��ʼ��Ϊ���ַ���
		*(buffer_ + size_) = value_type();
		return buffer_;
	}

	template<class CharType, class CharTraits>
	void basic_string<CharType, CharTraits>::
		reallocate(size_type need)
	{
		auto newcap = esgstl::max(capacity_ + need, capacity_ + (capacity_ >> 1));
		auto newbuffer = data_allocator::allocate(newcap);
		char_traits::move(newbuffer, buffer_, size_);
		data_allocator::deallocate(buffer_);
		buffer_ = newbuffer;
		capacity_ = newcap;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::iterator 
		basic_string<CharType, CharTraits>::
		reallocate_and_fill(iterator pos, size_type n, value_type ch)
	{
		auto newcap = esgstl::max(capacity_ + n, capacity_ + (capacity_ >> 1));
		auto newbuffer = data_allocator::allocate(newcap);
		auto dis = pos - buffer_;
		auto e1 = char_traits::move(newbuffer, buffer_, dis);
		auto e2 = char_traits::fill(e1, ch, n);
		char_traits::move(e2, pos, end() - pos);
		data_allocator::deallocate(buffer_);
		buffer_ = newbuffer;
		size_ = size_ + n;
		capacity_ = newcap;
		return buffer_ + dis;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::iterator 
		basic_string<CharType, CharTraits>::
		reallocate_and_copy(iterator pos, const_iterator first, const_iterator last)
	{
		auto n = esgstl::distance(first, last);
		auto newcap = esgstl::max(capacity_ + n, capacity_ + (capacity_ >> 1));
		auto newbuffer = data_allocator::allocate(newcap);
		auto dis = pos - buffer_;
		
		auto e1 = char_traits::move(newbuffer, buffer_, dis);
		// ��char_type ��charʱ�� move�������������ƶ��������ǲ�ϣ���ı�first-last�Ķ�������˲�ʹ����������
		// auto e2 = char_traits::move(e1,first,n);
		auto e2 = esgstl::uninitialized_copy_n(first, n, e1);
		char_traits::move(e2, pos, end() - pos);
		data_allocator::deallocate(buffer_);
		buffer_ = newbuffer;
		size_ += n;
		capacity_ = newcap;
		return buffer_ + dis;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>& 
		basic_string<CharType, CharTraits>::
		replace_cstr(const_iterator dst, size_type count1, 
			const_iterator src, size_type count2) {
		
		auto p = const_cast<iterator>(dst);
		const size_type len = esgstl::min(count1, count2);
		for (size_type i = 0; i < len; ++i,++p) {
			*p = *(src + i);
			--count1;
			--count2;
		}
		if (count1 > 0) {
			erase(p, p + count1);
		}
		else {
			insert(p, src + len, src + len + count2);
		}
		return *this;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>& 
		basic_string<CharType, CharTraits>::
		replace_fill(const_iterator dst, size_type count, size_type n, value_type ch) {
		
		auto p = const_cast<iterator>(dst);
		const size_type len = esgstl::min(count, n);
		for (size_type i = 0; i < len; ++i, ++p) {
			*p = ch;
			--count;
			--n;
		}
		if (count > 0) {
			erase(p, p + count);
		}
		else {
			insert(p, n, ch);
		}
		return *this;
	}

	template<class CharType, class CharTraits>
	typename basic_string<CharType, CharTraits>::iterator 
		basic_string<CharType, CharTraits>::
		insert(const_iterator pos, value_type ch) {
		auto r = const_cast<iterator>(pos);
		if (size_ == capacity_) {
			return reallocate_and_fill(r, 1, ch);
		}
		char_traits::move(r + 1, r, 1);
		++size_;
		*r = ch;
		return r;
	}

	template<class CharType, class CharTraits>
	typename basic_string<CharType, CharTraits>::iterator 
		basic_string<CharType, CharTraits>::
		insert(const_iterator pos, size_type n, value_type ch) {
		auto r = const_cast<iterator>(pos);
		if (capacity_ - size_< n) {
			return reallocate_and_fill(r, n, ch);
		}
		if (pos == end()) {
			char_traits::fill(end(), ch, n);
			size_ += n;
			return end() - n;
		}
		char_traits::move(r + n, r, end() - r);
		char_traits::fill(r, ch, n);
		size_ += n;
		return r;
	}

	template <class CharType, class CharTraits>
	template <class Iter>
	basic_string<CharType, CharTraits>::iterator
		basic_string<CharType, CharTraits>::
		insert(const_iterator pos, Iter first, Iter last) {
		auto n = esgstl::distance(first, last);
		auto r = const_cast<iterator>(pos);
		if (capacity_ - size_ < n) {
			return reallocate_and_copy(r, first, last);
		}
		if (pos == end()) {
			esgstl::uninitialized_copy(first, last, end());
			size_ += n;
			return r;
		}
		char_traits::move(r + n, r, end() - r);
		esgstl::uninitialized_copy(first, last, r);
		size_ += n;
		return r;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>& 
		basic_string<CharType, CharTraits>::
		append(size_type n, value_type ch) {
		ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - n,
			"basic_string<Char, Tratis>'s size too big");
		if (capacity_ - size_ < n) {
			reallocate(n);
		}
		char_traits::fill(end(), ch, n);
		size_ += n;
		return *this;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>&
		basic_string<CharType, CharTraits>::
		append(const basic_string& str, size_type pos, size_type len) {
		ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - len,
			"basic_string<Char, Tratis>'s size too big");
		if (capacity_ - size_ < len) {
			reallocate(len);
		}
		char_traits::copy(end(), str.buffer_ + pos, len);
		size_ += len;
		return *this;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>& 
		basic_string<CharType, CharTraits>::
		append(const_pointer s, size_type len) {
		ESGSTL_LENGTH_ERROR_IF(size_ > max_size() - len,
			"basic_string<Char, Tratis>'s size too big");
		if (capacity_ - size_ < len) {
			reallocate(len);
		}
		char_traits::copy(end(), s, len);
		size_ += len;
		return *this;
	}

	template<class CharType, class CharTraits>
	void basic_string<CharType, CharTraits>::
		swap(basic_string& rhs) noexcept{
		if (this != &rhs) {
			esgstl::swap(rhs.buffer_, buffer_);
			esgstl::swap(rhs.size_, size_);
			esgstl::swap(rhs.capacity_, rhs.capacity_);
		}
	}

	template<class CharType, class CharTraits>
	 void basic_string<CharType, CharTraits>::
		 reverse() noexcept {
		 for (auto i = begin(), j = end(); i < j;) {
			 esgstl::iter_swap(i++,--j);
		 }
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::iterator
		basic_string<CharType, CharTraits>::
		erase(const_iterator pos) {
		ESGSTL_DEBUG(pos != end());
		auto r = const_cast<iterator>(pos);
		char_traits::move(r, r + 1, end() - pos - 1);
		--size_;
		return r;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::iterator
		basic_string<CharType, CharTraits>::
		erase(const_iterator first, const_iterator last) {
		if (first == begin() && last == end()) {
			clear();
			return end();
		}
		
		auto r = const_cast<iterator>(first);
		if (last > end()) last = end();
		const size_type n = end() - last;
		char_traits::move(r, last , n);
		size_ -= (last - r);
		return r;
	}

	template<class CharType, class CharTraits>
	inline void basic_string<CharType, CharTraits>::
		resize(size_type n, value_type ch) {
		if (n > size_) {
			append(n - size_, ch);
		}
		else {
			erase(buffer_ + n, end());
		}
	}

	template<class CharType, class CharTraits>
	int basic_string<CharType, CharTraits>::
		compare(const basic_string& rhs) const {
		if (size_ < rhs.size_) return -1;
		else if (rhs.size_ < size_) return 1;
		return char_traits::compare(buffer_, rhs.buffer_, size_);
	}

	template<class CharType, class CharTraits>
	int basic_string<CharType, CharTraits>::
		compare(size_type pos1, size_type count1, const basic_string& rhs) const {
		if (count1 < rhs.size_) return -1;
		else if (rhs.size_ < count1) return 1;
		return char_traits::compare(buffer_+pos1, rhs.buffer_, count1);
	}

	template<class CharType, class CharTraits>
	int basic_string<CharType, CharTraits>::
		compare(size_type pos1, size_type count1, const basic_string& rhs,
			size_type pos2, size_type count2) const {
		if (count2 == npos) count2 = rhs.size_ - pos2;
		if (count1 < count2) return -1;
		else if (count2 < count1) return 1;
		return char_traits::compare(buffer_+pos1, rhs.buffer_+pos2, count1);
	}

	template<class CharType, class CharTraits>
	int basic_string<CharType, CharTraits>::
		compare(const_pointer s) const {
		const size_type len = char_traits::length(s);
		if (size_ < len) return -1;
		else if (len < size_) return 1;
		return char_traits::compare(buffer_, s, size_);
	}

	template<class CharType, class CharTraits>
	int basic_string<CharType, CharTraits>::
		compare(size_type pos1, size_type count1, const_pointer s) const {
		const size_type len = char_traits::length(s);
		if (count1 < len) return -1;
		else if (len < count1) return 1;
		return char_traits::compare(buffer_+pos1, s, count1);
	}

	template<class CharType, class CharTraits>
	int basic_string<CharType, CharTraits>::
		compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const {
		if (count1 < count2) return -1;
		else if (count2 < count1) return 1;
		return char_traits::compare(buffer_ + pos1, s, count1);
	}
	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		find(value_type ch, size_type pos) const noexcept
	{
		for (auto i = pos; i < size_; ++i) {
			if (*(buffer_ + i) == ch) return i;
		}
		return npos;
	}
	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find(const_iterator s, size_type pos) const noexcept
	{
		return find(s,pos,size_);
	}
	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find(const_iterator s, size_type pos, size_type count) const noexcept {
		const auto size = char_traits::length(s);
		auto endIndex = buffer_ + pos + count;
		if (size == 0) return pos;
		for (auto i = buffer_ + pos; i != endIndex; ++i) {
			// ��Ҫƥ����Ӵ����ȴ��ڵ�ǰʣ��δƥ����ַ�������ʱ�����߱�����end()������δ�ҵ�
			if (size > endIndex - i || i == buffer_ + size_) return npos;
			auto j = s;
			for (; j != s + size; ++j) {
				if (*j != *(i+(j-s))) break;
			}
			if (j == s + size) return i - buffer_;
		}
		return npos;
	}
	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find(const basic_string& str, size_type pos) const noexcept {
		const auto size = str.size_;
		auto endIndex = end();
		if (size == 0) return 0;
		for (auto i = buffer_ + pos; i != endIndex; ++i) {
			if (size > endIndex - i) return npos;
			auto j = str.buffer_;
			for (; j != str.end(); ++j) {
				if (*j != *(i+(j-str.buffer_))) break;
			}
			if (j == str.end()) return i - buffer_;
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::rfind(value_type ch, size_type pos) const noexcept {
		pos = esgstl::min(pos, size_ - 1);
		reverse_iterator r(buffer_ + pos + 1);
		reverse_iterator end(buffer_);
		for (; r != end; ++r) {
			if (ch == *r) 
				return r.base() - buffer_ - 1;
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		rfind(const_pointer s, size_type pos) const noexcept {
		return rfind(s, pos, size_ - pos);
	}


	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		rfind(const_pointer s, size_type pos, size_type count) const noexcept {
		pos = esgstl::min(pos, size_);
		const auto size = char_traits::length(s);
		if (size == 0) return pos;
		auto i = buffer_ + pos;
		auto end = buffer_ + esgstl::max(size - 1,size_ - pos - count);
		for (; i >= end; --i) {
			auto j = size;
			for (; j > 0; --j) {
				if (*(i - size + j) != *(s+j-1)) break;
			}
			if (j == 0) return i - size - buffer_ + 1;
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		rfind(const basic_string& str, size_type pos) const noexcept {
		pos = esgstl::min(pos, size_);
		const auto size = str.size_;
		if (size == 0) return pos;
		if (size == 1) return rfind(str[0], pos);
		auto i = buffer_ + pos;
		auto end = buffer_ + size - 1;
		for (; i >= end; --i) {
			auto j = size;
			for (; j > 0; --j) {
				if (*(i - size + j) != str[j - 1]) break;
			}
			if (j == 0) return i - size - buffer_ + 1;
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find_first_of(value_type ch, size_type pos) const noexcept
	{
		return find(ch,pos);
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find_first_of(const_pointer s, size_type pos) const noexcept
	{
		return find_first_of(s, pos, size_);
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find_first_of(const_pointer s, size_type pos, size_type count) const noexcept
	{
		const size_type end = esgstl::min(pos + count, size_);
		const size_type len = char_traits::length(s);
		for (auto i = pos; i < end; ++i) {
			value_type ch = *(buffer_ + i);
			for (size_type j = 0; j < len; ++j) {
				if (ch == *(s + j));
				return i;
			}
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		find_first_of(const basic_string& str, size_type pos) const noexcept {
		const size_type len = str.size_;
		for (auto i = pos; i < size_; ++i) {
			value_type ch = *(buffer_ + i);
			for (size_type j = 0; j < len; ++j) {
				if (ch == str[j]);
				return i;
			}
		}
		return npos;
	}


	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		
		find_first_not_of(value_type ch, size_type pos) const noexcept {
		for (auto i = pos; i < size_; ++i) {
			if (ch != *(buffer_ + i)) return i;
		}
		return npos;
	}
	
	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		find_first_not_of(const_pointer s, size_type pos) const noexcept {
		
		return find_first_not_of(s, pos, size_);
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		find_first_not_of(const_pointer s, size_type pos, size_type count) const noexcept {
		
		const auto len = char_traits::length(s);
		const auto end = esgstl::min(pos + count, size_);
		for (auto i = pos; i < end; ++i) {
			value_type ch = *(buffer_ + i);
			bool flag = false;
			for (size_type j = 0; j < len; ++j) {
				if (s[j] == ch) flag = true;
			}
			if (!flag) return i;
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		find_first_not_of(const basic_string& str, size_type pos) const noexcept {
		
		const auto len = str.size_;
		for (auto i = pos; i < size_; ++i) {
			value_type ch = *(buffer_ + i);
			bool flag = false;
			for (size_type j = 0; j < len; ++j) {
				if (str[j] == ch) flag = true;
			}
			if (!flag) return i;
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find_last_of(value_type ch, size_type pos) const noexcept {
		
		auto i = esgstl::min(pos, size_ - 1);
		for (; i >= 0; --i) {
			if (ch != *(buffer_ + i)) return i;
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find_last_of(const_pointer s, size_type pos) const noexcept {
		
		find_last_of(s, pos, size_);
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find_last_of(const_pointer s, size_type pos, size_type count) const noexcept {
		
		const auto len = char_traits::length(s);
		const auto begin = esgstl::min(pos, size_ - 1);
		size_type end = 0;
		if (pos > count) end = pos - count;
		for (auto i = begin; i >= end; --i) {
			value_type ch = *(buffer_ + i);
			for (auto j = 0; j < len; ++j) {
				if (s[j] != ch) return i;
			}
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		find_last_of(const basic_string& str, size_type pos) const noexcept {

		const auto len = str.size_;
		const auto begin = esgstl::min(pos, size_ - 1);
		for (auto i = begin; i >= 0; --i) {
			value_type ch = *(buffer_ + i);
			for (auto j = 0; j < len; ++j) {
				if (str[j] != ch) return i;
			}
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find_last_not_of(value_type ch, size_type pos) const noexcept {
		
		return find_last_of(ch, pos);
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find_last_not_of(const_pointer s, size_type pos) const noexcept {
		
		pos = esgstl::min(pos, size_ - 1);
		return find_last_not_of(s,pos,size_);
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		find_last_not_of(const_pointer s, size_type pos, size_type count) const noexcept {
		
		const auto len = char_traits::length(s);
		const auto begin = esgstl::min(pos, size_ - 1);
		size_type end = 0;
		if (pos > count) end = pos - count;
		for (auto i = begin; i >= end; --i) {
			bool flag = false;
			value_type ch = *(buffer_ + i);
			for (auto j = 0; j < len; ++j) {
				if (s[j] == ch) flag = true;
			}
			if (!flag) return i;
		}
		return npos;
	}

	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type
		basic_string<CharType, CharTraits>::
		find_last_not_of(const basic_string& str, size_type pos) const noexcept {

		const auto len = str.size_;
		const auto begin = esgstl::min(pos, size_ - 1);
		for (auto i = begin; i >= 0; --i) {
			bool flag = false;
			value_type ch = *(buffer_ + i);
			for (auto j = 0; j < len; ++j) {
				if (str[j] == ch) flag = true;
			}
			if (!flag) return i;
		}
		return npos;
	}
	
	template<class CharType, class CharTraits>
	basic_string<CharType, CharTraits>::size_type 
		basic_string<CharType, CharTraits>::
		count(value_type ch, size_type pos) const noexcept {
		
		value_type n(0);
		for (auto i = pos; i < size_ - 1; ++i) {
			if (*(buffer_ + i) == ch) ++n;
		}
		return n;
	}

	// ��������Ƚϲ�����
	template <class CharType, class CharTraits>
	bool operator==(const basic_string<CharType, CharTraits>& lhs,
		const basic_string<CharType, CharTraits>& rhs) {
		return lhs.compare(rhs) == 0;
	}

	template <class CharType, class CharTraits>
	bool operator!=(const basic_string<CharType, CharTraits>& lhs,
		const basic_string<CharType, CharTraits>& rhs) {
		return lhs.compare(rhs) != 0;
	}

	template <class CharType, class CharTraits>
	bool operator<(const basic_string<CharType, CharTraits>& lhs,
		const basic_string<CharType, CharTraits>& rhs) {
		return lhs.compare(rhs) < 0;
	}

	template <class CharType, class CharTraits>
	bool operator<=(const basic_string<CharType, CharTraits>& lhs,
		const basic_string<CharType, CharTraits>& rhs) {
		return lhs.compare(rhs) <= 0;
	}

	template <class CharType, class CharTraits>
	bool operator>(const basic_string<CharType, CharTraits>& lhs,
		const basic_string<CharType, CharTraits>& rhs) {
		return lhs.compare(rhs) > 0;
	}

	template <class CharType, class CharTraits>
	bool operator>=(const basic_string<CharType, CharTraits>& lhs,
		const basic_string<CharType, CharTraits>& rhs) {
		return lhs.compare(rhs) >= 0;
	}

	// �ػ� esgstl �� swap
	template <class CharType, class CharTraits>
	void swap(basic_string<CharType, CharTraits>& lhs,
		basic_string<CharType, CharTraits>& rhs) noexcept {
		lhs.swap(rhs);
	}

	// �ػ� esgstl::hash
	template <class CharType, class CharTraits>
	struct hash<basic_string<CharType, CharTraits>> {
		size_t operator()(const basic_string<CharType, CharTraits>& str) {
			return bitwise_hash((const unsigned char*)str.c_str(),
				str.size() * sizeof(CharType));
		}
	};
}

