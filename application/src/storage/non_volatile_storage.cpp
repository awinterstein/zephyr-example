#include "non_volatile_storage.hpp"
#include <zephyr/drivers/flash.h>
#include <zephyr/fs/nvs.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/flash_map.h>

LOG_MODULE_REGISTER(non_volatile_storage);

// definition of the flash partition to be used for the storage
#define NVS_PARTITION        storage_partition
#define NVS_PARTITION_DEVICE FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET FIXED_PARTITION_OFFSET(NVS_PARTITION)

namespace
{
struct error_category : public util::error_category {
	[[nodiscard]] const char *name() const noexcept override
	{
		return "non_volatile_storage";
	}

	[[nodiscard]] const char *message(int ev) const override
	{
		switch (static_cast<storage_error_code>(ev)) {
		case storage_error_code::device_not_ready:
			return "Device is not ready";
		case storage_error_code::unable_to_get_page_info:
			return "Unable to get page info";
		case storage_error_code::wrong_data_size:
			return "Wrong data size";
		}

		return "Unknown error";
	}
};

const error_category the_error_category{};

} // namespace

util::error_code make_error_code(storage_error_code code)
{
	return {static_cast<int>(code), the_error_category};
}

util::error_code non_volatile_storage::init()
{
	/* define the nvs file system by settings with:
	 *	sector_size equal to the pagesize,
	 *	2 sectors
	 *	starting at NVS_PARTITION_OFFSET
	 */
	fs.flash_device = NVS_PARTITION_DEVICE;
	if (!device_is_ready(fs.flash_device)) {
		LOG_ERR("Flash device %s is not ready.", fs.flash_device->name);
		return storage_error_code::device_not_ready;
	}

	fs.offset = NVS_PARTITION_OFFSET;
	struct flash_pages_info info;
	auto rc = flash_get_page_info_by_offs(fs.flash_device, fs.offset, &info);
	if (rc) {
		LOG_ERR("%s", "Unable to get page info.");
		return storage_error_code::unable_to_get_page_info;
	}
	fs.sector_size = info.size;
	fs.sector_count = 2U;

	rc = nvs_mount(&fs);
	if (rc < 0) {
		LOG_ERR("%s", "Flash Init failed.");
		return os::result_to_error_code(rc);
	}

	return {};
}
