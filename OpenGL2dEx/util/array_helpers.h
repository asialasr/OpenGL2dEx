#ifndef COUNT_OF_H
#define COUNT_OF_H

namespace util
{

template <typename T, size_t COUNT>
constexpr size_t count_of(T const (&)[COUNT]) noexcept
{
	return COUNT;
}

template <typename T, size_t COUNT>
void fill(T (&arr)[COUNT], const T &value)
{
	for (auto i = size_t{ 0 }; i < COUNT; ++i)
	{
		arr[i] = value;
	}
}

} // namespace util

#endif // COUNT_OF_H