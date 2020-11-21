#ifndef TEMPLATE_HELPERS_H
#define TEMPLATE_HELPERS_H

#include <type_traits>

namespace util {
	// logical operations
	template<typename ...T>
	struct and;

	template<>
	struct and<>
	{
		static constexpr bool value = true;
	};

	template<typename T0, typename ...Types>
	struct and<T0, Types...>
	{
		static constexpr bool value = T0::value && and<Types...>::value;
	};

	template<typename ...Types>
	struct or;

	template<>
	struct or<>
	{
		static constexpr bool value = false;
	};

	template<typename T0, typename ...Types>
	struct or<T0, Types...>
	{
		static constexpr bool value = T0::value || or<Types...>::value;
	};

	template<typename T>
	struct not
	{
		static constexpr bool value = !T::value;
	};

	// maximum values
	template<typename T, T ...vals>
	struct max;

	// TODO(sasiala): I don't like this specialization, but it gets run in the conditional assignment even when the
	// condition is false, it appears.  The problem (in general) with this specialization is that: A) it doesn't make
	// much sense to take the max of nothing; B) if the only values are negative and we don't put in a defense (I have)
	// in the other specializations of this template, default construction could result in the wrong max.  For example,
	// if you're looking for the max of -3, -2, and -1, but this is evaluated & used at some point, the 0 would be the max
	template<typename T>
	struct max<T>
	{
		static constexpr T value = {};
	};

	template<typename T, T val0, T ...vals>
	struct max<T, val0, vals...>
	{
		static constexpr auto num_vals = sizeof...(vals);
		static constexpr bool use_max_from_vals = (num_vals > 0);
		static constexpr T max_from_vals = (use_max_from_vals) 
										   ? max<T, vals...>::value 
										   : T{};
		static constexpr T max_val0_and_vals = (max_from_vals > val0) 
											   ? max_from_vals 
											   : val0;
		static constexpr T value = (use_max_from_vals) 
								   ? max_val0_and_vals 
								   : val0;
	};

	template<typename T>
	struct strip_const_from_pointed_to_type
	{
		using type = T;
	};

	template<typename T>
	struct strip_const_from_pointed_to_type<const T*>
	{
		using type = std::remove_const_t<T>*;
	};

	template<typename T>
	using strip_const_from_pointed_to_type_t = typename strip_const_from_pointed_to_type<T>::type;

	// parameter pack common traits
	template<typename ...Types>
	struct are_trivially_copyable
	{
		static constexpr bool value = and<std::is_trivially_copyable<Types>...>::value;
	};

	template<typename ...Types>
	struct are_non_reference_types
	{
		static constexpr bool value = and<not<std::is_reference<Types>>...>::value;
	};

	template<typename ...Types>
	struct are_non_const_types
	{
		static constexpr bool value = and<not<std::is_const<Types>>...>::value;
	};
} // namespace util

#endif // TEMPLATE_HELPERS_H