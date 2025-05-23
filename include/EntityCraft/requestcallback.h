#pragma once
#include "DatabaseAdapter/itransaction.h"
#include "requestcallbacktype.h"

#include <memory>

namespace entity_craft {

template<typename T>
class request_callback
{
public:
    virtual ~request_callback() = default;

    virtual void pre_request_callback(T& value, request_callback_type type, const std::shared_ptr<database_adapter::ITransaction>& transaction) = 0;
    virtual void post_request_callback(T& value, request_callback_type type, const std::shared_ptr<database_adapter::ITransaction>& transaction) = 0;
};

} // namespace entity_craft