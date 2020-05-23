/*
// Copyright (c) 2018 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/
#pragma once

#include "health.hpp"
#include "led.hpp"
#include "node.hpp"

#include <boost/container/flat_map.hpp>
#include <variant>

namespace redfish
{

/**
 * @brief Retrieves chassis state properties over dbus
 *
 * @param[in] aResp - Shared pointer for completing asynchronous calls.
 *
 * @return None.
 */
void getChassisState(std::shared_ptr<AsyncResp> aResp)
{
    //crow::connections::systemBus->async_method_call(
    //    [aResp{std::move(aResp)}](
    //        const boost::system::error_code ec,
    //        const std::variant<std::string> &chassisState) {
    //        if (ec)
    //        {
    //            BMCWEB_LOG_DEBUG << "DBUS response error " << ec;
    //            messages::internalError(aResp->res);
    //            return;
    //        }

    //        const std::string *s = std::get_if<std::string>(&chassisState);
    //        BMCWEB_LOG_DEBUG << "Chassis state: " << *s;
    //        if (s != nullptr)
    //        {
                // Verify Chassis State
    //            if (*s == "xyz.openbmc_project.State.Chassis.PowerState.On")
    //            {
                    aResp->res.jsonValue["PowerState"] = "On";
                    aResp->res.jsonValue["Status"]["State"] = "Enabled";
    //            }
    //            else if (*s ==
    //                     "xyz.openbmc_project.State.Chassis.PowerState.Off")
    //            {
    //                aResp->res.jsonValue["PowerState"] = "Off";
    //                aResp->res.jsonValue["Status"]["State"] = "StandbyOffline";
    //            }
    //        }
    //   },
    //   "xyz.openbmc_project.State.Chassis",
    //    "/xyz/openbmc_project/state/chassis0",
    //    "org.freedesktop.DBus.Properties", "Get",
    //    "xyz.openbmc_project.State.Chassis", "CurrentPowerState");
}

/**
 * DBus types primitives for several generic DBus interfaces
 * TODO(Pawel) consider move this to separate file into boost::dbus
 */
// Note, this is not a very useful Variant, but because it isn't used to get
// values, it should be as simple as possible
// TODO(ed) invent a nullvariant type
using VariantType = std::variant<bool, std::string, uint64_t, uint32_t>;
using ManagedObjectsType = std::vector<std::pair<
    sdbusplus::message::object_path,
    std::vector<std::pair<std::string,
                          std::vector<std::pair<std::string, VariantType>>>>>>;

using PropertiesType = boost::container::flat_map<std::string, VariantType>;

void getIntrusionByService(std::shared_ptr<AsyncResp> aResp,
                           const std::string &service,
                           const std::string &objPath)
{
    BMCWEB_LOG_DEBUG << "Get intrusion status by service \n";
    /*
    crow::connections::systemBus->async_method_call(
        [aResp{std::move(aResp)}](const boost::system::error_code ec,
                                  const std::variant<std::string> &value) {
            if (ec)
            {
                // do not add err msg in redfish response, becaues this is not
                //     mandatory property
                BMCWEB_LOG_ERROR << "DBUS response error " << ec << "\n";
                return;
            }

            const std::string *status = std::get_if<std::string>(&value);

            if (status == nullptr)
            {
                BMCWEB_LOG_ERROR << "intrusion status read error \n";
                return;
            }

            aResp->res.jsonValue["PhysicalSecurity"] = {
                {"IntrusionSensorNumber", 1}, {"IntrusionSensor", *status}};
        },
        service, objPath, "org.freedesktop.DBus.Properties", "Get",
        "xyz.openbmc_project.Chassis.Intrusion", "Status");
    */
    aResp->res.jsonValue["PhysicalSecurity"] = {
                {"IntrusionSensorNumber", 1}, {"IntrusionSensor", "Normal"}};
}

/**
 * Retrieves physical security properties over dbus
 */
void getPhysicalSecurityData(std::shared_ptr<AsyncResp> aResp)
{
/*
    crow::connections::systemBus->async_method_call(
        [aResp{std::move(aResp)}](
            const boost::system::error_code ec,
            const std::vector<std::pair<
                std::string,
                std::vector<std::pair<std::string, std::vector<std::string>>>>>
                &subtree) {
            if (ec)
            {
                // do not add err msg in redfish response, becaues this is not
                //     mandatory property
                BMCWEB_LOG_ERROR << "DBUS error: no matched iface " << ec
                                 << "\n";
                return;
            }
            // Iterate over all retrieved ObjectPaths.
            for (const auto &object : subtree)
            {
                for (const auto &service : object.second)
                {
                    getIntrusionByService(aResp, service.first, object.first);
                    return;
                }
            }
        },
        "xyz.openbmc_project.ObjectMapper",
        "/xyz/openbmc_project/object_mapper",
        "xyz.openbmc_project.ObjectMapper", "GetSubTree",
        "/xyz/openbmc_project/Intrusion", 1,
        std::array<const char *, 1>{"xyz.openbmc_project.Chassis.Intrusion"});
*/
}

/**
 * ChassisCollection derived class for delivering Chassis Collection Schema
 */
class ChassisCollection : public Node
{
  public:
    ChassisCollection(CrowApp &app) : Node(app, "/redfish/v1/Chassis/")
    {
        entityPrivileges = {
            {boost::beast::http::verb::get, {{"Login"}}},
            {boost::beast::http::verb::head, {{"Login"}}},
            {boost::beast::http::verb::patch, {{"ConfigureComponents"}}},
            {boost::beast::http::verb::put, {{"ConfigureComponents"}}},
            {boost::beast::http::verb::delete_, {{"ConfigureComponents"}}},
            {boost::beast::http::verb::post, {{"ConfigureComponents"}}}};
    }

  private:
    void getChassisList(std::vector<std::string> &chassisList)
    {
         chassisList.emplace_back("xyz/openbmc_project/inventory/chassis0");
    }

    /**
     * Functions triggers appropriate requests on DBus
     */
    void doGet(crow::Response &res, const crow::Request &req,
               const std::vector<std::string> &params) override
    {
        res.jsonValue["@odata.type"] = "#ChassisCollection.ChassisCollection";
        res.jsonValue["@odata.id"] = "/redfish/v1/Chassis";
        res.jsonValue["Name"] = "Chassis Collection";

        //const std::array<const char *, 2> interfaces = {
        //    "xyz.openbmc_project.Inventory.Item.Board",
        //    "xyz.openbmc_project.Inventory.Item.Chassis"};

        auto asyncResp = std::make_shared<AsyncResp>(res);
        //crow::connections::systemBus->async_method_call(
        //    [asyncResp](const boost::system::error_code ec,
        //                const std::vector<std::string> &chassisList) {
        //        if (ec)
        //        {
        //            messages::internalError(asyncResp->res);
        //            return;
        //        }

	std::vector<std::string> chassisList;
        getChassisList(chassisList);

                nlohmann::json &chassisArray =
                    asyncResp->res.jsonValue["Members"];
                chassisArray = nlohmann::json::array();
                for (const std::string &objpath : chassisList)
                {
                    std::size_t lastPos = objpath.rfind("/");
                    if (lastPos == std::string::npos)
                    {
                        BMCWEB_LOG_ERROR << "Failed to find '/' in " << objpath;
                        continue;
                    }
                    chassisArray.push_back(
                        {{"@odata.id", "/redfish/v1/Chassis/" +
                                           objpath.substr(lastPos + 1)}});
                }

                asyncResp->res.jsonValue["Members@odata.count"] =
                    chassisArray.size();
        //    },
        //    "xyz.openbmc_project.ObjectMapper",
        //    "/xyz/openbmc_project/object_mapper",
        //    "xyz.openbmc_project.ObjectMapper", "GetSubTreePaths",
        //    "/xyz/openbmc_project/inventory", 0, interfaces);
    }
};

/**
 * Chassis override class for delivering Chassis Schema
 */
class Chassis : public Node
{
  public:
    Chassis(CrowApp &app) :
        Node(app, "/redfish/v1/Chassis/<str>/", std::string())
    {
        entityPrivileges = {
            {boost::beast::http::verb::get, {{"Login"}}},
            {boost::beast::http::verb::head, {{"Login"}}},
            {boost::beast::http::verb::patch, {{"ConfigureComponents"}}},
            {boost::beast::http::verb::put, {{"ConfigureComponents"}}},
            {boost::beast::http::verb::delete_, {{"ConfigureComponents"}}},
            {boost::beast::http::verb::post, {{"ConfigureComponents"}}}};
    }

  private:

    void getChassisSubTree(const std::string &chassisId, crow::openbmc_mapper::GetSubTreeType &subtree)
    {

        std::vector<std::string> interfaces{
                std::string("org.freedesktop.DBus.Introspectable"),
                        std::string("org.freedesktop.DBus.Peer"),
                        std::string("org.freedesktop.DBus.Properties"),
                        std::string("xyz.openbmc_project.Inventory.Item.Chassis")
        };

        std::pair<std::string, std::vector<std::string>> connections(std::string("xyz.openbmc_project.Inventory.Manager"), interfaces);

        subtree.emplace_back(std::pair<std::string,
              std::vector<std::pair<std::string, std::vector<std::string>>>>(
                std::string("/xyz/openbmc_project/inventory/system/chassis0"), {connections})
            );
    }

    void getChassisSensors(const std::string &chassisId, std::variant<std::vector<std::string>> &resp)
    {
        resp = std::vector<std::string>{
            std::string("/xyz/openbmc_project/sensors/fan_tach/System_Fan01"),
            std::string("/xyz/openbmc_project/sensors/voltage/volt0"),
            std::string("/xyz/openbmc_project/sensors/temperature/temp0"),
            std::string("/xyz/openbmc_project/sensors/power/psu0")
        };
    }

    void getChassisPropertiesList(const std::string &chassisId, std::vector<std::pair<std::string, VariantType>> &propertiesList)
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

    /**
     * Functions triggers appropriate requests on DBus
     */
    void doGet(crow::Response &res, const crow::Request &req,
               const std::vector<std::string> &params) override
    {
        //const std::array<const char *, 2> interfaces = {
        //    "xyz.openbmc_project.Inventory.Item.Board",
        //    "xyz.openbmc_project.Inventory.Item.Chassis"};

        // Check if there is required param, truly entering this shall be
        // impossible.
        if (params.size() != 1)
        {
            messages::internalError(res);
            res.end();
            return;
        }
        const std::string &chassisId = params[0];

        auto asyncResp = std::make_shared<AsyncResp>(res);

        crow::openbmc_mapper::GetSubTreeType subtree;

        getChassisSubTree(chassisId, subtree);

        //crow::connections::systemBus->async_method_call(
        //    [asyncResp, chassisId(std::string(chassisId))](
        //        const boost::system::error_code ec,
        //        const crow::openbmc_mapper::GetSubTreeType &subtree) {
        //        if (ec)
        //        {
        //            messages::internalError(asyncResp->res);
        //            return;
        //        }
                // Iterate over all retrieved ObjectPaths.
                for (const std::pair<
                         std::string,
                         std::vector<
                             std::pair<std::string, std::vector<std::string>>>>
                         &object : subtree)
                {
                    const std::string &path = object.first;
                    const std::vector<
                        std::pair<std::string, std::vector<std::string>>>
                        &connectionNames = object.second;

                    if (!boost::ends_with(path, chassisId))
                    {
                        continue;
                    }

                    auto health = std::make_shared<HealthPopulate>(asyncResp);

                    std::variant<std::vector<std::string>> resp;

                    //crow::connections::systemBus->async_method_call(
                    //    [health](const boost::system::error_code ec,
                    //             std::variant<std::vector<std::string>> &resp) {
                    //        if (ec)
                    //        {
                    //            return; // no sensors = no failures
                    //        }
                            std::vector<std::string> *data =
                                std::get_if<std::vector<std::string>>(&resp);
                            if (data == nullptr)
                            {
                                return;
                            }
                            health->inventory = std::move(*data);
                     //   },
                     //   "xyz.openbmc_project.ObjectMapper",
                     //   path + "/all_sensors",
                     //   "org.freedesktop.DBus.Properties", "Get",
                     //   "xyz.openbmc_project.Association", "endpoints");

                    health->populate();

                    if (connectionNames.size() < 1)
                    {
                        BMCWEB_LOG_ERROR << "Got 0 Connection names";
                        continue;
                    }

                    asyncResp->res.jsonValue["@odata.type"] =
                        "#Chassis.v1_10_0.Chassis";
                    asyncResp->res.jsonValue["@odata.id"] =
                        "/redfish/v1/Chassis/" + chassisId;
                    asyncResp->res.jsonValue["Name"] = "Chassis Collection";
                    asyncResp->res.jsonValue["ChassisType"] = "RackMount";
                    asyncResp->res.jsonValue["PCIeDevices"] = {
                        {"@odata.id",
                         "/redfish/v1/Systems/system/PCIeDevices"}};

                    const std::string &connectionName =
                        connectionNames[0].first;

                    BMCWEB_LOG_DEBUG << "connectionName: " << connectionName.c_str();

                    const std::vector<std::string> &interfaces =
                        connectionNames[0].second;

                    for (const std::string inf : interfaces)
                        BMCWEB_LOG_DEBUG << "|--interfaces: " << inf;

                    const std::array<const char *, 2> hasIndicatorLed = {
                        "xyz.openbmc_project.Inventory.Item.Panel",
                        "xyz.openbmc_project.Inventory.Item.Board.Motherboard"};

                    for (const char *interface : hasIndicatorLed)
                    {
                        if (std::find(interfaces.begin(), interfaces.end(),
                                      interface) != interfaces.end())
                        {
                            getIndicatorLedState(asyncResp);
                            break;
                        }
                    }

                    std::vector<std::pair<std::string, VariantType>> propertiesList;

                    getChassisPropertiesList(chassisId, propertiesList);

                    //crow::connections::systemBus->async_method_call(
                    //    [asyncResp, chassisId(std::string(chassisId))](
                    //        const boost::system::error_code ec,
                    //        const std::vector<std::pair<
                    //            std::string, VariantType>> &propertiesList) {
                            for (const std::pair<std::string, VariantType>
                                     &property : propertiesList)
                            {
                                // Store DBus properties that are also Redfish
                                // properties with same name and a string value
                                const std::string &propertyName =
                                    property.first;
                                if ((propertyName == "PartNumber") ||
                                    (propertyName == "SerialNumber") ||
                                    (propertyName == "Manufacturer") ||
                                    (propertyName == "Model"))
                                {
                                    const std::string *value =
                                        std::get_if<std::string>(
                                            &property.second);
                                    if (value != nullptr)
                                    {
                                        asyncResp->res.jsonValue[propertyName] =
                                            *value;
                                    }
                                }
                            }
                            asyncResp->res.jsonValue["Name"] = chassisId;
                            asyncResp->res.jsonValue["Id"] = chassisId;
                            asyncResp->res.jsonValue["Thermal"] = {
                                {"@odata.id", "/redfish/v1/Chassis/" +
                                                  chassisId + "/Thermal"}};
                            // Power object
                            asyncResp->res.jsonValue["Power"] = {
                                {"@odata.id", "/redfish/v1/Chassis/" +
                                                  chassisId + "/Power"}};
                            // SensorCollection
                            asyncResp->res.jsonValue["Sensors"] = {
                                {"@odata.id", "/redfish/v1/Chassis/" +
                                                  chassisId + "/Sensors"}};
                            asyncResp->res.jsonValue["Status"] = {
                                {"State", "Enabled"},
                            };

                            asyncResp->res
                                .jsonValue["Links"]["ComputerSystems"] = {
                                {{"@odata.id", "/redfish/v1/Systems/system"}}};
                            asyncResp->res.jsonValue["Links"]["ManagedBy"] = {
                                {{"@odata.id", "/redfish/v1/Managers/bmc"}}};
                            getChassisState(asyncResp);
                    //    },
                    //    connectionName, path, "org.freedesktop.DBus.Properties",
                    //    "GetAll",
                    //    "xyz.openbmc_project.Inventory.Decorator.Asset");
                    return;
                }

                // Couldn't find an object with that name.  return an error
                messages::resourceNotFound(
                    asyncResp->res, "#Chassis.v1_10_0.Chassis", chassisId);
            //},
            //"xyz.openbmc_project.ObjectMapper",
            //"/xyz/openbmc_project/object_mapper",
            //"xyz.openbmc_project.ObjectMapper", "GetSubTree",
            //"/xyz/openbmc_project/inventory", 0, interfaces);

        getPhysicalSecurityData(asyncResp);
    }

    void doPatch(crow::Response &res, const crow::Request &req,
                 const std::vector<std::string> &params) override
    {
        std::optional<std::string> indicatorLed;
        auto asyncResp = std::make_shared<AsyncResp>(res);

        if (params.size() != 1)
        {
            return;
        }

        if (!json_util::readJson(req, res, "IndicatorLED", indicatorLed))
        {
            return;
        }

        if (!indicatorLed)
        {
            return; // delete this when we support more patch properties
        }

        const std::array<const char *, 2> interfaces = {
            "xyz.openbmc_project.Inventory.Item.Board",
            "xyz.openbmc_project.Inventory.Item.Chassis"};

        const std::string &chassisId = params[0];

        crow::connections::systemBus->async_method_call(
            [asyncResp, chassisId, indicatorLed](
                const boost::system::error_code ec,
                const crow::openbmc_mapper::GetSubTreeType &subtree) {
                if (ec)
                {
                    messages::internalError(asyncResp->res);
                    return;
                }

                // Iterate over all retrieved ObjectPaths.
                for (const std::pair<
                         std::string,
                         std::vector<
                             std::pair<std::string, std::vector<std::string>>>>
                         &object : subtree)
                {
                    const std::string &path = object.first;
                    const std::vector<
                        std::pair<std::string, std::vector<std::string>>>
                        &connectionNames = object.second;

                    if (!boost::ends_with(path, chassisId))
                    {
                        continue;
                    }

                    if (connectionNames.size() < 1)
                    {
                        BMCWEB_LOG_ERROR << "Got 0 Connection names";
                        continue;
                    }

                    const std::vector<std::string> &interfaces =
                        connectionNames[0].second;

                    if (indicatorLed)
                    {
                        const std::array<const char *, 2> hasIndicatorLed = {
                            "xyz.openbmc_project.Inventory.Item.Panel",
                            "xyz.openbmc_project.Inventory.Item.Board."
                            "Motherboard"};
                        bool indicatorChassis = false;
                        for (const char *interface : hasIndicatorLed)
                        {
                            if (std::find(interfaces.begin(), interfaces.end(),
                                          interface) != interfaces.end())
                            {
                                indicatorChassis = true;
                                break;
                            }
                        }
                        if (indicatorChassis)
                        {
                            setIndicatorLedState(asyncResp,
                                                 std::move(*indicatorLed));
                        }
                        else
                        {
                            messages::propertyUnknown(asyncResp->res,
                                                      "IndicatorLED");
                        }
                    }
                    return;
                }

                messages::resourceNotFound(
                    asyncResp->res, "#Chassis.v1_10_0.Chassis", chassisId);
            },
            "xyz.openbmc_project.ObjectMapper",
            "/xyz/openbmc_project/object_mapper",
            "xyz.openbmc_project.ObjectMapper", "GetSubTree",
            "/xyz/openbmc_project/inventory", 0, interfaces);
    }
};
} // namespace redfish
