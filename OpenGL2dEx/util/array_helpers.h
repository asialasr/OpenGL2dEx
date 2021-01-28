#ifndef ARRAY_HELPERS_H
#define ARRAY_HELPERS_H

#include "logging.h"

namespace util
{

	template <typename T, size_t COUNT>
	constexpr size_t count_of(T const (&)[COUNT]) noexcept
	{
		return COUNT;
	}

	template <typename T, size_t COUNT>
	void fill(T(&arr)[COUNT], const T &value)
	{
		for (auto i = size_t{ 0 }; i < COUNT; ++i)
		{
			arr[i] = value;
		}
	}


	template <typename T>
	void copy_ptr_to_ptr(const T *origin, T *dest, size_t num_to_copy)
	{
		for (auto i = size_t{ 0 }; i < num_to_copy; ++i)
		{
			dest[i] = origin[i];
		}
	}

	template <typename T, size_t COUNT_DEST>
	void copy_ptr_to_arr(const T *origin, T(&dest)[COUNT_DEST], size_t num_to_copy)
	{
		ASSERT(num_to_copy <= COUNT_DEST, "Attempting to copy too many items");

		copy_ptr_to_ptr(origin, dest, num_to_copy);
	}

	template <typename T, size_t COUNT_ORIG>
	void copy_arr_to_ptr(const T(&origin)[COUNT_ORIG], T *dest, size_t num_to_copy)
	{
		ASSERT(num_to_copy <= COUNT_ORIG, "Attempting to copy too many items");

		copy_ptr_to_ptr(origin, dest, num_to_copy);
	}

	template <typename T, size_t COUNT_ORIG, size_t COUNT_DEST>
	void copy(const T(&origin)[COUNT_ORIG], T(&dest)[COUNT_DEST], size_t num_to_copy)
	{
		ASSERT(num_to_copy <= COUNT_ORIG && num_to_copy <= COUNT_DEST, "Attempting to copy too many items");

		copy_ptr_to_ptr(origin, dest, num_to_copy);
	}

	template <typename T, size_t COUNT>
	void copy(const T(&origin)[COUNT], T(&dest)[COUNT])
	{
		copy_ptr_to_ptr(origin, dest, COUNT);
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
	void apply_with_count(T(&arr)[COUNT], size_t count_to_apply, Func function)
	{
		ASSERT(count_to_apply <= COUNT, "Net enough elements for given count");

		for (auto i = size_t{ 0 }; i < count_to_apply; ++i)
		{
			function(arr[i]);
		}
	}

	template <typename T, typename Func = FunctionApplier<T>, size_t COUNT>
	void apply(T(&arr)[COUNT], Func function)
	{
		apply_with_count(arr, COUNT, function);
	}

	template<typename T, typename INPUT_TYPE1>
	using SingleInputFunctionApplier = void(*)(T, INPUT_TYPE1);

	template <typename T, typename INPUT_TYPE, typename SingleInputFunc = SingleInputFunctionApplier<T, INPUT_TYPE>, size_t COUNT>
	void apply_with_count(T(&arr)[COUNT], size_t count_to_apply, SingleInputFunc function, INPUT_TYPE input1)
	{
		ASSERT(count_to_apply <= COUNT, "Net enough elements for given count");

		for (auto i = size_t{ 0 }; i < count_to_apply; ++i)
		{
			function(arr[i], input1);
		}
	}

	template <typename T, typename INPUT_TYPE, typename SingleInputFunc = SingleInputFunctionApplier<T, INPUT_TYPE>, size_t COUNT>
	void apply(T(&arr)[COUNT], SingleInputFunc function, INPUT_TYPE input1)
	{
		apply_with_count(arr, COUNT, function, input1);
	}

	template<typename T, typename INPUT_TYPE1, typename INPUT_TYPE2>
	using DualInputFunctionApplier = void(*)(T&, INPUT_TYPE1, INPUT_TYPE2);

	template <typename T, typename INPUT_TYPE1, typename INPUT_TYPE2, typename DualInputFunc = DualInputFunctionApplier<T, INPUT_TYPE1, INPUT_TYPE2>, size_t COUNT>
	void apply_with_count(T(&arr)[COUNT], size_t count_to_apply, DualInputFunc function, INPUT_TYPE1 input1, INPUT_TYPE2 input2)
	{
		ASSERT(count_to_apply <= COUNT, "Net enough elements for given count");
		
		for (auto i = size_t{ 0 }; i < count_to_apply; ++i)
		{
			function(arr[i], input1, input2);
		}
	}

	template <typename T, typename INPUT_TYPE1, typename INPUT_TYPE2, typename DualInputFunc = DualInputFunctionApplier<T, INPUT_TYPE1, INPUT_TYPE2>, size_t COUNT>
	void apply(T(&arr)[COUNT], DualInputFunc function, INPUT_TYPE1 input1, INPUT_TYPE2 input2)
	{
		apply_with_count(arr, COUNT, function, input1, input2);
	}
} // namespace util

#endif // ARRAY_HELPERS_H