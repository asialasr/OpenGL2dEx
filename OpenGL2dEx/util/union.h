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
			return and<are_non_reference_types<Types...>,
				are_non_const_types<Types...>>::value;
		}

		template<typename CHECK_TYPE, typename ...Types>
		struct is_type_in_pack
		{
			static constexpr bool value = or <std::is_same<CHECK_TYPE, Types>...>::value;
		};

		template<typename CHECK_TYPE, typename ...Types>
		struct enable_get
		{
			// strip constness from CHECK_TYPE, because Types won't contain const types
			// ensure that CHECK_TYPE pointed to is const if the type in Types is const, otherwise ignore it
			// don't need to worry about references, as Types will not contain any and CHECK_TYPE shouldn't be a ref
			static constexpr bool value = or <is_type_in_pack<std::remove_const_t<CHECK_TYPE>, Types...>,
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
			static constexpr bool value = or <is_type_in_pack<std::remove_const_t<CHECK_TYPE>, Types...>,
				is_type_in_pack<std::remove_const_t<CHECK_TYPE>, strip_const_from_pointed_to_type_t<Types>...>>::value;
		};

		template<typename CHECK_TYPE, typename ...Types>
		static constexpr bool enable_set_v = enable_get<CHECK_TYPE, Types...>::value;
	} // namespace impl

	template<typename ...Types>
	class UnionBase {
	protected:
		static constexpr size_t kDataSize = impl::max_size<Types...>::value;
		using StorageType = unsigned char;
		static constexpr size_t kArraySize = kDataSize / sizeof(StorageType);
		using StorageArray = StorageType[kArraySize];

	private:
		struct Destroyer {
			virtual void destroy(StorageArray data) = 0;
		};

		template<typename T>
		class DestroyerImpl : public Destroyer {
		public:
			void destroy(StorageArray data) override
			{
				destroy_impl(reinterpret_cast<T&>(*data));
			}

		private:
			template<typename T2, std::enable_if_t<std::is_destructible<T2>::value, bool> = true>
			void destroy_impl(T2 &obj)
			{
				obj.~T2();
			}

			template<typename T2, std::enable_if_t<not<std::is_destructible<T2>>::value, bool> = true>
			void destroy_impl(T2 &/*obj*/)
			{
			}
		};

		template<typename T>
		static Destroyer *destroyer_dispatch()
		{
			static DestroyerImpl<T> destroyer{};
			return &destroyer;
		}

		struct Copier {
			virtual void copy(const StorageArray origin, StorageArray destination) = 0;
		};

		template<typename T>
		class CopierImpl : public Copier {
		public:
			void copy(const StorageArray origin, StorageArray destination) override
			{
				copy_obj(reinterpret_cast<const T&>(*origin), destination);
			}

			void copy_obj(const T &origin, StorageArray destination)
			{
				new(destination) T(origin);
			}
		};

		template<typename T>
		static Copier *copier_dispatch()
		{
			static CopierImpl<T> copier{};
			return &copier;
		}

	public:
		template<typename = std::enable_if_t<impl::are_types_valid_for_union<Types...>()>>
		UnionBase()
			: data_{}
			, destroyer_{ nullptr }
			, copier_{ nullptr }
			, initialized_{ false }
		{
		}

		template<typename T,
			typename = std::enable_if_t<impl::are_types_valid_for_union<Types...>()>,
			typename = std::enable_if_t<impl::enable_set_v<T, Types...>>>
			UnionBase(const T &data)
			: data_{}
			, destroyer_{ nullptr }
			, copier_{ nullptr }
			, initialized_{ false }
		{
			set(data);
		}

		UnionBase(const UnionBase<Types...> &other)
			: data_{}
			, destroyer_{ other.destroyer_ }
			, copier_{ other.copier_ }
			, initialized_{ other.initialized_ }
		{
			if (initialized_)
			{
				copier_->copy(other.data_, data_);
			}
		}

		virtual ~UnionBase()
		{
			if (initialized_)
			{
				destroyer_->destroy(data_);
			}
		}

		template <typename T,
			typename = std::enable_if_t<impl::enable_get_v<T, Types...>>>
			T& get()
		{
			ASSERT(initialized_, "Union currently contains no value");
			return reinterpret_cast<T&>(*data_);
		}

		template <typename T,
			typename = std::enable_if_t<impl::enable_set_v<T, Types...>>>
			void set(const T &data)
		{
			// destroy & copy-construct using placement new
			if (initialized_)
			{
				ASSERT(destroyer_, "No defined destroyer");
				destroyer_->destroy(data_);
			}

			copier_ = copier_dispatch<T>();
			CopierImpl<T> copier{};
			copier.copy_obj(data, data_);

			destroyer_ = destroyer_dispatch<T>();
			initialized_ = true;
		}
	protected:
		bool data_valid() const
		{
			return initialized_;
		}

		StorageArray& data()
		{
			return data_;
		}
	private:
		StorageArray alignas(Types...) data_;
		Destroyer    *destroyer_;
		Copier       *copier_;
		bool          initialized_;
	};

	template<typename ...Types>
	using Union = UnionBase<Types...>;

	template<typename PTR_TYPE, typename ...Types>
	class UnionWithDataPtr : public UnionBase<Types...>
	{
	public:
		using BaseType = UnionBase<Types...>;
		using PointerType = PTR_TYPE;
		using BaseType::StorageType;
		using BaseType::StorageArray;

	private:
		struct DataPointerProducer {
			virtual PointerType *make_data_pointer(StorageArray &data) const = 0;
			virtual const PointerType *make_const_data_pointer(const StorageArray &data) const = 0;
		};

		template<typename T>
		class DataPointerProducerImpl : public DataPointerProducer {
		public:
			PointerType *make_data_pointer(StorageArray &data) const override
			{
				return &static_cast<PointerType&>(reinterpret_cast<T&>(*data));
			}

			const PointerType *make_const_data_pointer(const StorageArray &data) const override
			{
				return &static_cast<const PointerType&>(reinterpret_cast<const T&>(*data));
			}
		};

		template<typename T>
		static DataPointerProducer *data_pointer_producer_dispatch()
		{
			static DataPointerProducerImpl<T> data_pointer_producer{};
			return &data_pointer_producer;
		}

	public:
		template<typename = std::enable_if_t<impl::are_types_valid_for_union<Types...>()>>
		UnionWithDataPtr()
			: BaseType{}
			, data_pointer_producer_{nullptr}
		{
		}

		template<typename T,
			typename = std::enable_if_t<impl::are_types_valid_for_union<Types...>()>,
			typename = std::enable_if_t<impl::enable_set_v<T, Types...>>>
			UnionWithDataPtr(const T &data)
			: BaseType{ data }
			, data_pointer_producer_{data_pointer_producer_dispatch<T>()}
		{
		}

		UnionWithDataPtr(const UnionWithDataPtr<PointerType, Types...> &other)
			: BaseType{ other }
			, data_pointer_producer_{other.data_pointer_producer_}
		{
		}

		virtual ~UnionWithDataPtr()
		{
		}

		template <typename T,
			typename = std::enable_if_t<impl::enable_set_v<T, Types...>>>
			void set(const T &data)
		{
			data_pointer_producer_ = data_pointer_producer_dispatch<T>();
			ParentType::set(data);
		}

		PointerType *get_data_ptr()
		{
			if (!data_valid())
			{
				return nullptr;
			}


			ASSERT(data_pointer_producer_, "No data pointer producer defined");
			return data_pointer_producer_->make_data_pointer(data());
		}

		const PointerType *get_const_data_ptr() const
		{
			if (!data_valid())
			{
				return nullptr;
			}


			ASSERT(data_pointer_producer_, "No data pointer producer defined");
			return data_pointer_producer_->make_const_data_pointer(data());
		}

	private:
		DataPointerProducer *data_pointer_producer_;
	}; // class UnionWithDataPtr
} // namespace util

#endif // UNION_H