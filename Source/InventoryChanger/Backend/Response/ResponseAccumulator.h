#pragma once

#include "Response.h"
#include "ResponseQueue.h"

namespace inventory_changer::backend
{

struct ResponseAccumulator {
    explicit ResponseAccumulator(ResponseQueue<>& responseQueue)
        : responseQueue{ responseQueue } {}

    void operator()(const Response& response) const
    {
        responseQueue.add(response);
    }

private:
    ResponseQueue<>& responseQueue;
};

}
