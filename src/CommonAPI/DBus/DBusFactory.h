/* Copyright (C) 2013 BMW Group
 * Author: Manfred Bathelt (manfred.bathelt@bmw.de)
 * Author: Juergen Gehring (juergen.gehring@bmw.de)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef COMMONAPI_DBUS_DBUS_FACTORY_H_
#define COMMONAPI_DBUS_DBUS_FACTORY_H_

#include <thread>

#include <CommonAPI/Factory.h>

#include "CommonAPI/DBus/DBusStubAdapter.h"
#include "DBusConnection.h"

namespace CommonAPI {
namespace DBus {

typedef std::shared_ptr<DBusProxy> (*DBusProxyFactoryFunction) (const char* busName, const char* objectPath, std::shared_ptr<DBusProxyConnection> dbusProxyConnection);
typedef std::shared_ptr<DBusStubAdapter> (*DBusAdapterFactoryFunction) (std::string busName, std::string objectPath, std::shared_ptr<DBusProxyConnection> dbusProxyConnection, std::shared_ptr<StubBase> stubBase);

class DBusFactory: public Factory {
 public:
    DBusFactory(std::shared_ptr<Runtime> runtime, const MiddlewareInfo* middlewareInfo);
    virtual ~DBusFactory();

    static void registerProxyFactoryMethod(std::string interfaceName, DBusProxyFactoryFunction proxyFactoryFunction);
    static void registerAdapterFactoryMethod(std::string interfaceName, DBusAdapterFactoryFunction adapterFactoryMethod);

    virtual std::vector<std::string> getAvailableServiceInstances(const std::string& serviceInterfaceName, const std::string& serviceDomainName = "local");

    virtual bool isServiceInstanceAlive(const std::string& serviceInstanceID, const std::string& serviceInterfaceName, const std::string& serviceDomainName = "local");

    virtual std::shared_ptr<Proxy> createProxy(const char* interfaceName, const std::string& participantId, const std::string& domain);

    virtual std::shared_ptr<StubAdapter> createAdapter(std::shared_ptr<StubBase> stubBase, const char* interfaceName, const std::string& participantId, const std::string& domain);

 private:
    std::shared_ptr<CommonAPI::DBus::DBusConnection> dbusConnection_;
    std::string acquiredConnectionName_;
};

} // namespace DBus
} // namespace CommonAPI

#endif // COMMONAPI_DBUS_DBUS_FACTORY_H_