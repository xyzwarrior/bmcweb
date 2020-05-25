#pragma once
//#ifdef BMCWEB_ENABLE_SDBUS
#include <sdbusplus/asio/connection.hpp>
//#else
#include <internal_system.hpp>
//#endif

namespace crow
{
namespace connections
{
//	#ifdef BMCWEB_ENABLE_SDBUS
static std::shared_ptr<sdbusplus::asio::connection> systemBus;
//	#endif
} // namespace connections
} // namespace crow
