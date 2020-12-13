#ifndef UNION_H
#define UNION_H

#include "array_helpers.h"
#include "template_helpers.h"

#include <type_traits>

namespace util {

	namespace impl {
		template<typename ...Types>
		struct max_size
		{
			static constexpr auto value = max<size_t, sizeof(Types)...>::value;
		};

		template<>
		struct max_size<>
		{
			static constexpr auto value = size_t{ 0 };
		};

		template<typename ...Types>
		constexpr bool are_types_valid_for_union()
		{
			return and<are_trivially_copyable<Types...>, 
					   are_non_reference_types<Types...>, 
				       are_non_const_types<Types...>>::value;
		}

		template<typename CHECK_TYPE, typename ...Types>
		struct is_type_in_pack
		{
			static constexpr bool value = or<std::is_same<CHECK_TYPE, Types>...>::value;
		};

		template<typename CHECK_TYPE, typename ...Types>
		struct enable_get
		{
			// strip constness from CHECK_TYPE, because Types won't contain const types
			// ensure that CHECK_TYPE pointed to is const if the type in Types is const, otherwise ignore it
			// don't need to worry about references, as Types will not contain any and CHECK_TYPE shouldn't be a ref
			static constexpr bool value = or<is_type_in_pack<std::remove_const_t<CHECK_TYPE>, Types...>,
											 is_type_in_pack<strip_const_from_pointed_to_type_t<std::remove_const_t<CHECK_TYPE>>, Types...>>::value;
		};

		template<typename CHECK_TYPE, typename ...Types>
		static constexpr bool enable_get_v = enable_get<CHECK_TYPE, Types...>::value;

		template<typename CHECK_TYPE, typename ...Types>
		struct enable_set
		{
			// strip constness from CHECK_TYPE, because Types won't contain const types
			// ensure that type pointed to in Types is const if CHECK_TYPE pointed to is const, otherwise ignore pointed-to constness
			// don't need to worry about references, as Types will not contain any and CHECK_TYPE shouldn't be a ref
			static constexpr bool value = or<is_type_in_pack<std::remove_const_t<CHECK_TYPE>, Types...>,
											 is_type_in_pack<std::remove_const_t<CHECK_TYPE>, strip_const_from_pointed_to_type_t<Types>...>>::value;
		};

		template<typename CHECK_TYPE, typename ...Types>
		static constexpr bool enable_set_v = enable_get<CHECK_TYPE, Types...>::value;
	} // namespace impl

	template<typename ...Types>
	class Union {
	private:
		static constexpr size_t kDataSize = impl::max_size<Types...>::value;
		using StorageType = unsigned char;
		static constexpr size_t kArraySize = kDataSize / sizeof(StorageType);

	public:
		template<typename = std::enable_if_t<impl::are_types_valid_for_union<Types...>()>>
		Union()
			: data_{}
		{
		}

		template<typename T,
			     typename = std::enable_if_t<impl::are_types_valid_for_union<Types...>()>,
				 typename = std::enable_if_t<impl::enable_set_v<T, Types...>>>
		Union(const T &data)
			: data_{}
		{
			set(data);
		}

		template <typename T,
				  typename = std::enable_if_t<impl::enable_get_v<T, Types...>>>
		T& get()
		{
			return reinterpret_cast<T&>(*data_);
		}

		template <typename T,
				  typename = std::enable_if_t<impl::enable_set_v<T, Types...>>>
		void set(const T &data)
		{
			copy(reinterpret_cast<const StorageType (&)[sizeof(T)]>(data), reinterpret_cast<StorageType (&)[sizeof(T)]>(data_));
			//reinterpret_cast<T&>(*data_) = data;
		}

	private:
		StorageType data_[kArraySize];
	};

} // namespace util

#endif // UNION_H