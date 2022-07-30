#include <functional>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <InventoryChanger/Backend/Requestor.h>
#include <InventoryChanger/Backend/Request/RequestTypes.h>
#include <InventoryChanger/Backend/Response/Response.h>

namespace inventory_changer::backend
{
namespace
{

struct MockRequestHandler {
    MOCK_METHOD(Response, handleApplyStickerRequest, (const request::ApplySticker& request), (const));

    template <typename RequestType>
    Response operator()(const RequestType& request) const
    {
        if constexpr (std::is_same_v<RequestType, request::ApplySticker>) {
            return handleApplyStickerRequest(request);
        }
        return {};
    }
};

struct MockResponseQueue {
    MOCK_METHOD(void, add, (const Response& response), ());
};

class InventoryChanger_Backend_RequestorTest : public testing::TestWithParam<Response> {};

TEST_P(InventoryChanger_Backend_RequestorTest, NonEmptyResponseReturnedFromHandlerIsAddedToQueue) {
    MockRequestHandler requestHandler;
    MockResponseQueue responseQueue;

    EXPECT_CALL(requestHandler, handleApplyStickerRequest(testing::_)).WillOnce(testing::Return(GetParam()));
    EXPECT_CALL(responseQueue, add(testing::VariantWith<response::StickerApplied>(testing::_))).Times(isEmptyResponse(GetParam()) ? 0 : 1);

    Requestor{ std::cref(requestHandler), responseQueue }.request<request::ApplySticker>(ItemIterator{}, ItemIterator{}, static_cast<std::uint8_t>(0));
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Backend_RequestorTest, testing::Values(Response{}, response::StickerApplied{ {}, 0 }));

}
}
