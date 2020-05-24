#pragma once

#include <vector>
#include <variant>

using GetSubTreeType = std::vector<
    std::pair<std::string,
              std::vector<std::pair<std::string, std::vector<std::string>>>>>;
using VariantType = std::variant<bool, std::string, uint64_t, uint32_t>;

namespace internal
{
namespace system
{

inline void getChassisList(std::vector<std::string> &chassisList)
{
    chassisList.emplace_back("xyz/openbmc_project/inventory/chassis0");
}

inline void getChassisSubTree(const std::string &chassisId, /*crow::openbmc_mapper::*/GetSubTreeType &subtree)
{
    std::vector<std::string> interfaces{
        std::string("org.freedesktop.DBus.Introspectable"),
        std::string("org.freedesktop.DBus.Peer"),
        std::string("org.freedesktop.DBus.Properties"),
        std::string("xyz.openbmc_project.Inventory.Item.Chassis")
    };

    std::pair<std::string, std::vector<std::string>> connections(
        std::string("xyz.openbmc_project.Inventory.Manager"), interfaces);

    subtree.emplace_back(std::pair<std::string,
        std::vector<std::pair<std::string, std::vector<std::string>>>>(
        std::string("/xyz/openbmc_project/inventory/system/chassis0"), {connections})
    );
}

inline void getChassisSensors(const std::string &chassisId,
    std::variant<std::vector<std::string>> &resp)
{
    resp = std::vector<std::string>{
        std::string("/xyz/openbmc_project/sensors/fan_tach/System_Fan01"),
        std::string("/xyz/openbmc_project/sensors/voltage/volt0"),
        std::string("/xyz/openbmc_project/sensors/temperature/temp0"),
        std::string("/xyz/openbmc_project/sensors/power/psu0")
    };
}

inline void getChassisPropertiesList(const std::string &chassisId,
    std::vector<std::pair<std::string, VariantType>> &propertiesList)
{
    propertiesList.emplace_back(
        std::make_pair(std::string("PartNumber"), VariantType(std::string("PN1234")))
    );
    propertiesList.emplace_back(
        std::make_pair(std::string("SerialNumber"), VariantType(std::string("SN1234")))
    );
    propertiesList.emplace_back(
        std::make_pair(std::string("Manufacturer"), VariantType(std::string("MF1234")))
    );
    propertiesList.emplace_back(
        std::make_pair(std::string("Model"), VariantType(std::string("MD1234")))
    );
}

} //namespace system
} //namespace internal
