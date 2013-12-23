/* Copyright (C) 2013 BMW Group
 * Author: Manfred Bathelt (manfred.bathelt@bmw.de)
 * Author: Juergen Gehring (juergen.gehring@bmw.de)
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef COMMONAPI_FACTORY_H_
#define COMMONAPI_FACTORY_H_

#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "MiddlewareInfo.h"
#include "Proxy.h"
#include "Runtime.h"
#include "Stub.h"
#include "AttributeExtension.h"


namespace CommonAPI {


class Factory;
class Runtime;
class MiddlewareInfo;


template<template<typename ...> class _ProxyType, template<typename> class _AttributeExtension>
struct DefaultAttributeProxyFactoryHelper;


template<template<typename ...> class _ProxyClass, template<typename> class _AttributeExtension>
std::shared_ptr<typename DefaultAttributeProxyFactoryHelper<_ProxyClass, _AttributeExtension>::class_t> createProxyWithDefaultAttributeExtension(Factory* specificFactory, const std::string& participantId, const std::string& domain);

/**
 * \brief The main CommonAPI access class. A factory is responsible for creation and destruction of service objects.
 *
 * The main CommonAPI access class. A factory is responsible for creation and destruction of service objects.
 * This includes proxies and stubs. It also provides service discovery methods.
 */
class Factory {
 public:

    /**
     * \brief Creates factory. Don't call manually.
     *
     * Creates factory. Don't call manually.
     */
    Factory(const std::shared_ptr<Runtime> runtime,
            const MiddlewareInfo* middlewareInfo):
                runtime_(runtime),
                middlewareInfo_(middlewareInfo) {
    }

    virtual ~Factory() {}

    /**
     * \brief Build a proxy for the specified address
     *
     * Build a proxy for the specified address.
     * Template this method call for the desired proxy type and attribute extension.
     *
     * @param participantId The participant ID of the common API address (last part)
     * @param serviceName The service name of the common API address (middle part)
     * @param domain The domain of the common API address (first part)
     * @return a shared pointer to the constructed proxy
     */
    template<template<typename ...> class _ProxyClass, typename ... _AttributeExtensions>
    std::shared_ptr<
        _ProxyClass<
#ifdef WIN32
            CommonAPI::WINDummyAttributeExtension<WINDummyAttribute>,
#endif
            _AttributeExtensions...>
            > buildProxy(const std::string& participantId,
                         const std::string& serviceName,
                         const std::string& domain) {
            std::shared_ptr<Proxy> abstractMiddlewareProxy = createProxy(_ProxyClass<_AttributeExtensions...>::getInterfaceId(), participantId, serviceName, domain);
            auto returnProxy = std::make_shared<
                                _ProxyClass<
#ifdef WIN32
                                    CommonAPI::WINDummyAttributeExtension<WINDummyAttribute>,
#endif
                                    _AttributeExtensions...>
                                >(abstractMiddlewareProxy);
            return returnProxy;
    }

    /**
     * \brief Build a proxy for the specified address
     *
     * Build a proxy for the specified address.
     * Template this method call for the desired proxy type and attribute extension.
     *
     * @param serviceAddress The common API address
     * @return a shared pointer to the constructed proxy
     */
    template<template<typename ...> class _ProxyClass, typename ... _AttributeExtensions >
    std::shared_ptr<
        _ProxyClass<
#ifdef WIN32
            CommonAPI::WINDummyAttributeExtension<WINDummyAttribute>,
#endif
            _AttributeExtensions...>
        > buildProxy(const std::string& serviceAddress) {
        std::string domain;
        std::string serviceName;
        std::string participantId;
        if(!splitValidAddress(serviceAddress, domain, serviceName, participantId)) {
            return false;
        }

        return buildProxy<_ProxyClass, _AttributeExtensions...>(participantId, serviceName, domain);
    }

    /**
     * \brief Build a proxy for the specified address with one extension for all attributes
     *
     * Build a proxy for the specified address with one extension for all attributes
     * Template this method call for the desired proxy type attribute extension.
     *
     * @param participantId The participant ID of the common API address (last part)
     * @param serviceName The service name of the common API address (middle part)
     * @param domain The domain of the common API address (first part)
     * @return a shared pointer to the constructed proxy
     */
    template <template<typename ...> class _ProxyClass, template<typename> class _AttributeExtension>
    std::shared_ptr<typename DefaultAttributeProxyFactoryHelper<_ProxyClass, _AttributeExtension>::class_t>
    buildProxyWithDefaultAttributeExtension(const std::string& participantId,
                                            const std::string& serviceName,
                                            const std::string& domain) {

    	std::shared_ptr<Proxy> abstractMiddlewareProxy = createProxy(DefaultAttributeProxyFactoryHelper<_ProxyClass, _AttributeExtension>::class_t::getInterfaceId(), participantId, serviceName, domain);
    	return std::make_shared<typename DefaultAttributeProxyFactoryHelper<_ProxyClass, _AttributeExtension>::class_t>(abstractMiddlewareProxy);
    }

    /**
     * \brief Build a proxy for the specified address with one extension for all attributes
     *
     * Build a proxy for the specified address with one extension for all attributes
     * Template this method call for the desired proxy type attribute extension.
     *
     * @param serviceAddress The common API address
     * @return a shared pointer to the constructed proxy
     */
    template <template<typename ...> class _ProxyClass, template<typename> class _AttributeExtension>
    std::shared_ptr<typename DefaultAttributeProxyFactoryHelper<_ProxyClass, _AttributeExtension>::class_t>
    buildProxyWithDefaultAttributeExtension(const std::string& serviceAddress) {

		std::string domain;
		std::string serviceName;
		std::string participantId;
		if(!splitValidAddress(serviceAddress, domain, serviceName, participantId)) {
			return false;
		}

		return buildProxyWithDefaultAttributeExtension<_ProxyClass, _AttributeExtension>(participantId, serviceName, domain);
    }

    /**
     * \brief Get a pointer to the runtime of this factory.
     *
     * Get a pointer to the runtime of this factory.
     *
     * @return the Runtime
     */
    inline std::shared_ptr<Runtime> getRuntime() {
        return runtime_;
    }

    /**
     * \brief Register a service stub under a specified address
     *
     * Register a service stub under a specified address
     *
     * @param stub The stub pointer
     * @param participantId The participant ID of the common API address (last part)
     * @param serviceName The service name of the common API address (middle part)
     * @param domain The domain of the common API address (first part)
     * @return Was the registration successful
     */
    template<typename _Stub>
    bool registerService(std::shared_ptr<_Stub> stub,
    				     const std::string& participantId,
    				     const std::string& serviceName,
            			 const std::string& domain) {

    	std::shared_ptr<StubBase> stubBase = std::dynamic_pointer_cast<StubBase>(stub);
		return registerAdapter(stubBase, _Stub::StubAdapterType::getInterfaceId(), participantId, serviceName, domain);
    }

    /**
     * \brief Register a service stub under a specified address
     *
     * Register a service stub under a specified address
     *
     * @param stub The stub pointer
     * @param serviceAddress The common API address
     * @return Was the registration successful
     */
    template<typename _Stub>
    bool registerService(std::shared_ptr<_Stub> stub, const std::string& serviceAddress) {
		std::string domain;
		std::string serviceName;
		std::string participantId;
		if(!splitValidAddress(serviceAddress, domain, serviceName, participantId)) {
			return false;
		}

		return registerService<_Stub>(stub, participantId, serviceName, domain);
    }

    /**
     * \brief Unregister a service stub associated with a specified address
     *
     * Unregister a service stub associated with a specified address
     *
     * @param participantId The participant ID of the common API address (last part)
     * @param serviceName The service name of the common API address (middle part)
     * @param domain The domain of the common API address (first part)
     * @return Was the deregistration successful
     */
    virtual bool unregisterService(const std::string& participantId, const std::string& serviceName, const std::string& domain) = 0;

    /**
     * \brief Unregister a service stub associated with a specified address
     *
     * Unregister a service stub associated with a specified address
     *
     * @param serviceAddress The common API address
     * @return Was the deregistration successful
     */
    inline bool unregisterService(const std::string& serviceAddress) {
		std::string domain;
		std::string serviceName;
		std::string participantId;
		if(!splitValidAddress(serviceAddress, domain, serviceName, participantId)) {
			return false;
		}
		return unregisterService(participantId, serviceName, domain);
    }

    /**
     * \brief Get all instances of a specific service name available. Synchronous call.
     *
     * Get all instances of a specific service name available. Synchronous call.
     *
     * @param serviceName The service name of the common API address (middle part)
     * @param serviceDomainName The domain of the common API address (first part)
     * @return A vector of strings containing the available complete common api addresses.
     */
    virtual std::vector<std::string> getAvailableServiceInstances(const std::string& serviceName, const std::string& serviceDomainName = "local") = 0;

    /**
     * \brief Is a particular complete common api address available. Synchronous call.
     *
     * Is a particular complete common api address available. Synchronous call.
     *
     * @param serviceAddress The common API address
     * @return Is alive
     */
    virtual bool isServiceInstanceAlive(const std::string& serviceAddress) = 0;

    /**
     * \brief Is a particular complete common api address available. Synchronous call.
     *
     * Is a particular complete common api address available. Synchronous call.
     *
     * @param serviceInstanceID The participant ID of the common API address (last part)
     * @param serviceName The service name of the common API address (middle part)
     * @param serviceDomainName The domain of the common API address (first part)
     * @return Is alive
     */
    virtual bool isServiceInstanceAlive(const std::string& serviceInstanceID, const std::string& serviceName, const std::string& serviceDomainName = "local") = 0;

 protected:
    virtual std::shared_ptr<Proxy> createProxy(const char* interfaceId, const std::string& participantId, const std::string& serviceName, const std::string& domain) = 0;
    virtual bool registerAdapter(std::shared_ptr<StubBase> stubBase, const char* interfaceId, const std::string& participantId, const std::string& serivceName, const std::string& domain) = 0;

 private:
    std::shared_ptr<Runtime> runtime_;

    const MiddlewareInfo* middlewareInfo_;

    inline bool splitValidAddress(const std::string& serviceAddress, std::string& domain, std::string& serviceName, std::string& participantId) {
    	std::istringstream addressStream(serviceAddress);
		if(!std::getline(addressStream, domain, ':')) {
			return false;
		}
		if(!std::getline(addressStream, serviceName, ':')) {
			return false;
		}
		if(!std::getline(addressStream, participantId, ':')) {
			return false;
		}
		if(std::getline(addressStream, participantId)) {
			return false;
		}
		return true;
    }
};


} // namespace CommonAPI

#endif // COMMONAPI_FACTORY_H_
