#pragma once
#include <sdbusplus/asio/connection.hpp>

#include <internal_system.hpp>

namespace crow
{
namespace connections
{
static std::shared_ptr<sdbusplus::asio::connection> systemBus;

} // namespace connections
} // namespace crow
