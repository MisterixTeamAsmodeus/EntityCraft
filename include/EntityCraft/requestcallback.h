#pragma once

#include "requestcallbacktype.h"

#include <DatabaseAdapter/itransaction.h>

#include <memory>

namespace entity_craft {

template<typename T>
class IRequestCallback
{
public:
    virtual ~IRequestCallback() = default;

    virtual void pre_request_callback(T& value, request_callback_type type, const std::shared_ptr<database_adapter::ITransaction>& transaction) = 0;
    virtual void post_request_callback(T& value, request_callback_type type, const std::shared_ptr<database_adapter::ITransaction>& transaction) = 0;
};

} // namespace entity_craft