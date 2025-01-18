#ifndef STORAGE_NON_VOLATILE_STORAGE_HPP
#define STORAGE_NON_VOLATILE_STORAGE_HPP

#include <zephyr/fs/nvs.h>
#include "os/kernel.hpp"
#include <expected>
#include <span>

#ifdef CONFIG_NANOPB
#include "protobuf/protobuf_message.hpp"
#endif

enum class storage_error_code : uint8_t {
	device_not_ready = 1,
	unable_to_get_page_info = 2,
	wrong_data_size = 3,
};

util::error_code make_error_code(storage_error_code code);

// the concrete error code type needs to be made known to the 'system_error'
// implementation that resides in the 'util' namespace
namespace util
{
template <>
struct is_error_code_enum<storage_error_code> : public std::true_type {
};
} // namespace util

/**
 * @brief Non-volatile storage that can store fixed data types (templated), binary buffers and
 *        protobuf message via the 'nvs' module of Zephyr.
 */
class non_volatile_storage
{
public:
	[[nodiscard]] util::error_code init();

	[[nodiscard]] util::error_code clear()
	{
		const auto result = nvs_clear(&fs);
		return os::result_to_error_code(result);
	}

	/**
	 * @brief Reading of a fixed data type.
	 */
	template <typename T>
	[[nodiscard]] std::expected<T, util::error_code> read(uint16_t id)
	{
		T data{};

		const auto result = nvs_read(&fs, id, &data, sizeof(data));
		const auto error = os::result_to_error_code(result);
		if (error) {
			return std::unexpected{error};
		}

		if (result != sizeof(data)) {
			return std::unexpected{storage_error_code::wrong_data_size};
		}

		return data;
	}

	/**
	 * @brief Reading of stored data into a provided buffer.
	 */
	[[nodiscard]] std::expected<std::span<uint8_t>, util::error_code>
	read(uint16_t id, std::span<uint8_t> buffer)
	{
		const auto result = nvs_read(&fs, id, buffer.data(), buffer.size());
		const auto error = os::result_to_error_code(result);
		if (error) {
			return std::unexpected{error};
		}

		// if the result was not an error (was not < 0), then it indicates the read length
		// (which is >= 0)
		return std::span<uint8_t>{buffer.data(), static_cast<size_t>(result)};
	}

	/**
	 * @brief Writing of a fixed data type.
	 */
	template <typename T>
	[[nodiscard]] util::error_code write(uint16_t id, T const &data)
	{
		const auto result = nvs_write(&fs, id, &data, sizeof(data));
		return os::result_to_error_code(result);
	}

	/**
	 * @brief Writing of data from a provided buffer into the storage.
	 */
	[[nodiscard]] util::error_code write(uint16_t id, std::span<uint8_t> buffer)
	{
		const auto result = nvs_write(&fs, id, buffer.data(), buffer.size());
		return os::result_to_error_code(result);
	}

#ifdef CONFIG_NANOPB
	// Reading and writing of protobuf messages.
	template <typename type, size_t max_size>
	[[nodiscard]] util::error_code read(uint16_t id, protobuf::message<type, max_size> &message)
	{
		uint8_t buffer[protobuf::message<type, max_size>::maximum_encoded_size];

		const auto buffer_view = read(id, std::span<uint8_t>{buffer, sizeof(buffer)});
		if (!buffer_view) {
			return buffer_view.error();
		}

		return message.decode(buffer_view.value());
	}

	template <typename type, size_t max_size>
	[[nodiscard]] util::error_code write(uint16_t id,
					     protobuf::message<type, max_size> const &message)
	{
		std::array<uint8_t, protobuf::message<type, max_size>::maximum_encoded_size> buffer;

		const auto encode_result =
			message.encode(std::span<uint8_t>{buffer.data(), buffer.size()});
		if (!encode_result) {
			return encode_result.error();
		}

		return write(id, encode_result.value());
	}
#endif

private:
	struct nvs_fs fs;
};

#endif /* STORAGE_NON_VOLATILE_STORAGE_HPP */
