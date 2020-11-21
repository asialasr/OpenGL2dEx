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

template <typename T, size_t COUNT>
void copy(const T(&origin)[COUNT], T(&dest)[COUNT])
{
	for (auto i = size_t{ 0 }; i < COUNT; ++i)
	{
		dest[i] = origin[i];
	}
}

// TODO(sasiala): on one hand I'd like a more general implementation in terms of
// the number of inputs, but on the other you can only have so many inputs to a 
// function before it gets a little ridiculous, so may be OK to leave like this

// TODO(sasiala): I'm not 100% sure this handles lambdas that accept references
// correctly, but when stepping through in the debugger it appeared to.  Double
// check that it is working for that.
template <typename T>
using FunctionApplier = void(*)(T&);

template <typename T, typename Func = FunctionApplier<T>, size_t COUNT>
void apply(T (&arr)[COUNT], Func function)
{
	for (auto i = size_t{ 0 }; i < COUNT; ++i)
	{
		function(arr[i]);
	}
}

template<typename T, typename INPUT_TYPE1>
using SingleInputFunctionApplier = void(*)(T, INPUT_TYPE1);

template <typename T, typename INPUT_TYPE, typename SingleInputFunc = SingleInputFunctionApplier<T, INPUT_TYPE>, size_t COUNT>
void apply(T (&arr)[COUNT], SingleInputFunc function, INPUT_TYPE input1)
{
	for (auto i = size_t{ 0 }; i < COUNT; ++i)
	{
		function(arr[i], input1);
	}
}

template<typename T, typename INPUT_TYPE1, typename INPUT_TYPE2>
using DualInputFunctionApplier = void(*)(T&, INPUT_TYPE1, INPUT_TYPE2);

template <typename T, typename INPUT_TYPE1, typename INPUT_TYPE2, typename DualInputFunc = DualInputFunctionApplier<T, INPUT_TYPE1, INPUT_TYPE2>, size_t COUNT>
void apply(T (&arr)[COUNT], DualInputFunc function, INPUT_TYPE1 input1, INPUT_TYPE2 input2)
{
	for (auto i = size_t{ 0 }; i < COUNT; ++i)
	{
		function(arr[i], input1, input2);
	}
}
} // namespace util

#endif // COUNT_OF_H