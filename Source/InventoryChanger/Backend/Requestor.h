#pragma once

#include <utility>

#include "Response/Response.h"

namespace inventory_changer::backend
{

template <typename RequestHandler, typename ResponseQueue>
class Requestor {
public:
    Requestor(RequestHandler requestHandler, ResponseQueue& responseQueue)
        : requestHandler{ requestHandler }, responseQueue{ responseQueue } {}

    template <typename RequestType, typename... Args>
    void request(Args&&... args)
    {
        requestHandler(RequestType{ std::forward<Args>(args)... });
    }

private:
    RequestHandler requestHandler;
    ResponseQueue& responseQueue;
};

}
