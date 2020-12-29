#include "array_helpers.h"

namespace util {

template<typename STORAGE_TYPE, STORAGE_TYPE terminating_character, typename INDEX_TYPE>
INDEX_TYPE generic_strlen(const STORAGE_TYPE *str)
{
	auto len = INDEX_TYPE{0};

	while (true)
	{
		if (str[len] == terminating_character)
		{
			break;
		}
		++len;
	}

	return len;
}

template <typename STORAGE_TYPE, STORAGE_TYPE terminating_character, typename INDEX_TYPE, INDEX_TYPE N>
class string_base {
public:
	template <size_t LENGTH>
	using string_type = string_base<STORAGE_TYPE, terminating_character, INDEX_TYPE, LENGTH>;

	using Index = INDEX_TYPE;
	static constexpr auto kMaxLength = N;

	using StorageType = STORAGE_TYPE;
	static constexpr auto kTerminator = terminating_character;
	static constexpr auto kStorageLength = kMaxLength + 1;

private:
	auto strlen_local(const StorageType *str)
	{
		return generic_strlen<StorageType, kTerminator, Index>(str);
	}

public:
	string_base()
		: length_{ 0 }
		, string_{}
	{
	}

	string_base(const StorageType *str)
		: length_{}
		, string_{}
	{
		set_string(str);
	}

	template <Index OTHER_N>
	string_base(const string_type<OTHER_N> &other)
		: length_{}
		, string_{}
	{
		*this = other;
	}

	string_type<kMaxLength>& operator=(const StorageType *str)
	{
		set_string(str);

		return *this;
	}

	template <Index OTHER_N>
	string_type<kMaxLength>& operator=(const string_type<OTHER_N> &other)
	{
		set_string(other.c_str());

		return *this;
	}

	string_type<kMaxLength> operator+(const StorageType *str)
	{
		const auto other_length = strlen_local(str);

		ASSERT(other_length + length_ <= kMaxLength, "Combined string would be too large");

		string_base<StorageType, kTerminator, Index, kMaxLength> ret{ *this };
		// include terminating character
		copy_ptr_to_ptr(str, &ret.string_[ret.length_], other_length + 1);
		ret.length_ += other_length;

		return ret;
	}

	template<Index OTHER_N>
	string_type<kMaxLength + OTHER_N> operator+(const string_type<OTHER_N>& other)
	{
		string_type<kMaxLength + OTHER_N> ret{ *this };
		return ret + other.c_str();
	}

	string_type<kMaxLength>& operator+=(const StorageType *str)
	{
		const auto other_length = strlen_local(str);

		ASSERT(length_ + other_length <= kMaxLength, "Combined string too long");

		// include terminating character
		copy_ptr_to_ptr(str, string_ + length_, other_length + 1);
		length_ += other_length;

		return *this;
	}

	template<Index OTHER_N>
	string_type<kMaxLength>& operator+=(const string_type<OTHER_N> &other)
	{
		ASSERT(length_ + other.length() <= kMaxLength, "Combined string too long");

		// include terminating character
		copy_ptr_to_ptr(other.c_str(), string_ + length_, other.length() + 1);
		length_ += other.length();

		return *this;
	}

	bool operator==(const StorageType *str) const
	{
		// expects that str is of size length_ + 1 or greater
		for (Index i = 0; i < length_ + 1; ++i)
		{
			if (string_[i] != str[i])
				return false;
		}

		return true;
	}

	template<Index OTHER_N>
	bool operator==(const string_type<OTHER_N> &str) const
	{
		if (str.length() != length_)
			return false;

		return (*this == str.c_str());
	}

	bool operator!=(const StorageType *str) const
	{
		return !(*this == str);
	}

	template<Index OTHER_N>
	bool operator!=(const string_type<OTHER_N> &str) const
	{
		return !(*this == str);
	}

	void push_back(const StorageType &c)
	{
		ASSERT(length_ < kMaxLength, "No room left");

		string_[length_] = c;
		++length_;
		string_[length_] = kTerminator;
	}

	void append(const StorageType &c)
	{
		push_back(c);
	}

	void append(const StorageType *str)
	{
		*this += str;
	}

	template <Index OTHER_N>
	void append(const string_type<OTHER_N> &other)
	{
		*this += other;
	}

	// TODO(sasiala): front() and pop_front()?

	StorageType back() const
	{
		ASSERT(!empty(), "String is empty");

		return string_[length_ - 1];
	}

	void pop_back()
	{
		ASSERT(!empty(), "No character to pop");

		string_[length_ - 1] = kTerminator;
		--length_;
	}

	void clear()
	{
		length_ = 0;
		string_[0] = kTerminator;
	}

	const StorageType *c_str() const
	{
		return static_cast<const StorageType *>(string_);
	}

	static constexpr auto max_size()
	{
		return kMaxLength;
	}

	auto size() const
	{
		return length_;
	}

	auto length() const
	{
		return length_;
	}

	bool empty() const
	{
		return (length_ == 0);
	}

private:
	void set_string(const StorageType *str)
	{
		length_ = strlen_local(str);
		if (length_ > 0)
		{
			copy_ptr_to_arr(str, string_, length_ + 1);
		}
		else
		{
			string_[0] = kTerminator;
		}
	}

	Index length_;
	StorageType string_[kStorageLength];
}; // class string

template<size_t N>
using string = string_base<char, '\0', size_t, N>;

} // namespace util