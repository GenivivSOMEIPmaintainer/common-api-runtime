// Copyright (C) 2014, 2015 BMW Group
// Author: Manfred Bathelt (manfred.bathelt@bmw.de)
// Author: Juergen Gehring (juergen.gehring@bmw.de)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#if !defined (COMMONAPI_INTERNAL_COMPILATION)
#error "Only <CommonAPI/CommonAPI.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef COMMONAPI_TYPES_HPP_
#define COMMONAPI_TYPES_HPP_

#include <cstdint>
#include <functional>
#include <unordered_set>
#include <memory>
#include <tuple>

#include <CommonAPI/ContainerUtils.hpp>
#include <CommonAPI/Event.hpp>
#include <CommonAPI/Version.hpp>

#if  __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#  define COMMONAPI_DEPRECATED __attribute__ ((__deprecated__))
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
#  define COMMONAPI_DEPRECATED __declspec(deprecated)
#else
#  define COMMONAPI_DEPRECATED
#endif


#ifdef WIN32
#define CCALL __cdecl
#pragma section(".CRT$XCU",read)
#define INITIALIZER(f) \
    static void __cdecl f(void); \
    __declspec(allocate(".CRT$XCU")) void(__cdecl*f##_)(void) = f; \
    static void __cdecl f(void)
#else
#define CCALL
#define INITIALIZER(f) \
    static void f(void) __attribute__((constructor)); \
    static void f(void)
#endif

#ifdef WIN32
#define usleep(micSec) \
    std::this_thread::sleep_for(std::chrono::microseconds(micSec))
#endif

namespace CommonAPI {

enum class AvailabilityStatus {
    UNKNOWN,
    AVAILABLE,
    NOT_AVAILABLE
};

enum class CallStatus {
    SUCCESS,
    OUT_OF_MEMORY,
    NOT_AVAILABLE,
    CONNECTION_FAILED,
    REMOTE_ERROR
};

typedef uint32_t CallId;

enum class OperationStatus {
	PENDING,
	DONE
};

/**
 * \brief Identifies a client sending a call to a stub.
 *
 * The ClientId is used to identify the caller within a stub.
 * The ClientId is supposed to be added by the middleware and can be compared using the == operator.
 */
class ClientId {
public:
    virtual ~ClientId() { }
    virtual bool operator==(ClientId& clientIdToCompare) = 0;
    virtual std::size_t hashCode() = 0;
};

template <typename ... Args>
struct SelectiveBroadcastFunctorHelper {
    typedef std::function<void(Args...)> SelectiveBroadcastFunctor;
};

typedef std::unordered_set<
	std::shared_ptr<CommonAPI::ClientId>,
	SharedPointerClientIdContentHash,
	SharedPointerClientIdContentEqual
> ClientIdList;

template<typename _EnumType>
class EnumHasher {
public:
    size_t operator()(const _EnumType& testEnum) const {
        return static_cast<int32_t>(testEnum);
    }
};

// Type identifier for polymorphic structs
typedef uint32_t Serial;

} // namespace CommonAPI

#endif // COMMONAPI_TYPES_HPP_