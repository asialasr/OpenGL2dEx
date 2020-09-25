#ifndef COUNT_OF_H
#define COUNT_OF_H

namespace util
{

template <typename T, size_t COUNT>
constexpr size_t count_of(T const (&)[COUNT]) noexcept
{
	return COUNT;
}

} // namespace util

#endif // COUNT_OF_H