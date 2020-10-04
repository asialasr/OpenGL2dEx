#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <cassert>

// TODO(sasiala): this optional ws just an initial thing to 
// get things working.  It needs a lot of work.
namespace util {
	struct nullopt_t {
		constexpr nullopt_t(int) {}
	};
	constexpr nullopt_t nullopt{0};

	template<typename T>
	class Optional {
	public:
		constexpr Optional() noexcept
			: data_{}
			, has_data_{ false }
		{
		}

		constexpr Optional(nullopt_t) noexcept
			: data_{}
			, has_data_{ false }
		{
		}

		constexpr Optional(const Optional &other)
			: data_{ other.data_ }
			, has_data_{ other.has_data_ }
		{
		}

		constexpr Optional(Optional &&other) noexcept
			: data_{ std::move(other.data_)}
			, has_data_{ std::move(other.has_data_) }
		{
		}

		template<typename T>
		Optional(const T &data) noexcept
			: data_{data}
			, has_data_{true}
		{
		}

		template<typename T>
		Optional(const Optional<T> &other)
			: data_{ other.data_ }
			, has_data_{ other.has_data_ }
		{
		}

		// TODO(sasiala): implement comparison operators

		constexpr Optional& operator=(const Optional &other)
		{
			if (!has_data_ && !other.has_data_)
			{
				return *this;
			}
			
			if (has_data_ && !other.has_data_)
			{
				~data_();
				has_data_ = false;
			}
			else if (has_data_ && other.has_data_)
			{
				data_ = other.data_;
			}

			has_data_ = other->has_data_;
			data_ = other->has_data_;
			return *this;
		}

		operator bool() const
		{
			return has_data_;
		}

		T& operator*()
		{
			assert(has_data_);
			return data_;
		}

		const T& operator*() const
		{
			assert(has_data_);
			return data_;
		}

		T *operator->()
		{
			assert(has_data_);
			return &data_;
		}

		const T *operator->() const
		{
			assert(has_data_);
			return &data_;
		}

		~Optional()
		{
		}
		
		// TODO(sasiala): there are a ton more members to implement

	private:
		T	 data_;
		bool has_data_;
	};

} // namespace util

#endif // !OPTIONAL_H
