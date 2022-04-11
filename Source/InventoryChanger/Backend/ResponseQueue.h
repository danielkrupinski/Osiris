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
        responses.emplace(std::chrono::high_resolution_clock::now(), response);
    }

    template <typename Visitor>
    void visit(Visitor visitor, std::chrono::milliseconds delay)
    {
        while (!responses.empty()) {
            const auto& [timestamp, response] = responses.front();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - timestamp) < delay)
                break;
            std::visit(visitor, response.data);
            responses.pop();
        }
    }

private:
    std::queue<std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>, Response>> responses;
};

}
