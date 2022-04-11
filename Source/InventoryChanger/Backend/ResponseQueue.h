#pragma once

#include <chrono>
#include <queue>
#include <variant>

#include "Response.h"

namespace inventory_changer::backend
{

class ResponseQueue {
public:
    void add(const Response& response)
    {
        responses.emplace(Clock::now(), response);
    }

    template <typename Visitor>
    void visit(Visitor visitor, std::chrono::milliseconds delay)
    {
        while (!responses.empty()) {
            const auto& [timestamp, response] = responses.front();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - timestamp) < delay)
                break;
            std::visit(visitor, response.data);
            responses.pop();
        }
    }

private:
    using Clock = std::chrono::steady_clock;
    std::queue<std::pair<Clock::time_point, Response>> responses;
};

}
