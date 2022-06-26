#pragma once

#include <chrono>
#include <deque>
#include <utility>
#include <variant>

#include "ItemInResponse.h"
#include "Response.h"

namespace inventory_changer::backend
{

template <typename Clock = std::chrono::steady_clock>
class ResponseQueue {
public:
    static_assert(Clock::is_steady);

    void add(const Response& response)
    {
        responses.emplace_back(Clock::now(), response);
    }

    void removeResponsesReferencingItem(ItemIterator item)
    {
        std::erase_if(responses, [item](const std::pair<typename Clock::time_point, Response>& pair) { return responseContainsItem(pair.second, item); });
    }

    template <typename Visitor>
    void visit(Visitor&& visitor, std::chrono::milliseconds delay)
    {
        while (!responses.empty()) {
            const auto& [timestamp, response] = responses.front();
            if (Clock::now() - timestamp < delay)
                break;
            std::visit(std::forward<Visitor>(visitor), response);
            responses.pop_front();
        }
    }

private:
    std::deque<std::pair<typename Clock::time_point, Response>> responses;
};

}
