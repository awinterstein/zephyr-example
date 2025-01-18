#include "protobuf_message.hpp"
#include <zephyr/logging/log.h>

namespace protobuf
{

// The registering of a log module must be done only once. Hence, for the template class
// protobuf_message this needs to be done in a separate module file.
LOG_MODULE_REGISTER(protobuf_message, LOG_LEVEL_DBG);

} // namespace protobuf
