#include <chrono>

#include <gmock/gmock.h>

#include <InventoryChanger/Backend/Response/ResponseQueue.h>

namespace inventory_changer::backend
{
namespace
{

using namespace std::chrono_literals;

struct FakeClock {
    using duration = std::chrono::nanoseconds;
    using period = duration::period;
    using rep = duration::rep;
    using time_point = std::chrono::time_point<FakeClock>;

    static constexpr bool is_steady = true;

    [[nodiscard]] static time_point now() noexcept
    {
        return time;
    }

    static void advance(duration a) noexcept
    {
        if (a > duration{ 0 })
            time += a;
    }

private:
    static inline time_point time;
};

struct MockResponseHandler {
    MOCK_METHOD(void, handle, (const response::ItemAdded&), (const));
    MOCK_METHOD(void, handle, (const response::StickerApplied&), (const));
    MOCK_METHOD(void, handleOther, (), (const));

    template <typename ResponseType>
    void operator()(const ResponseType& response) const
    {
        if constexpr (std::is_same_v<ResponseType, response::ItemAdded> || std::is_same_v<ResponseType, response::StickerApplied>)
            handle(response);
        else
            handleOther();
    }
};

class InventoryChanger_Backend_ResponseQueue_InstantResponseTest : public testing::TestWithParam<std::chrono::milliseconds> {};

TEST_P(InventoryChanger_Backend_ResponseQueue_InstantResponseTest, ResponsesAreHandledImmediatelyForDelayNotGreaterThanZero) {
    ResponseQueue<FakeClock> queue;
    queue.add(response::ItemAdded{ {}, false });
    queue.add(response::StickerApplied{ {}, 0 });

    testing::InSequence _;
    testing::StrictMock<MockResponseHandler> mock;
    EXPECT_CALL(mock, handle(testing::An<const response::ItemAdded&>())).Times(GetParam() <= 0ms);
    EXPECT_CALL(mock, handle(testing::An<const response::StickerApplied&>())).Times(GetParam() <= 0ms);

    queue.visit(mock, GetParam());
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Backend_ResponseQueue_InstantResponseTest, testing::Values(-1ms, 0ms, 1ms));

struct ResponseTime {
    std::chrono::microseconds timePassed;
    std::chrono::milliseconds delay;
};

class InventoryChanger_Backend_ResponseQueue_ResponseTimeTest : public testing::TestWithParam<ResponseTime> {};

TEST_P(InventoryChanger_Backend_ResponseQueue_ResponseTimeTest, ResponsesAreHandledWhenDelayHasPassed) {
    ResponseQueue<FakeClock> queue;
    queue.add(response::ItemAdded{ {}, false });
    queue.add(response::StickerApplied{ {}, 0 });

    FakeClock::advance(GetParam().timePassed);

    testing::InSequence _;
    testing::StrictMock<MockResponseHandler> mock;
    EXPECT_CALL(mock, handle(testing::An<const response::ItemAdded&>())).Times(GetParam().timePassed >= GetParam().delay);
    EXPECT_CALL(mock, handle(testing::An<const response::StickerApplied&>())).Times(GetParam().timePassed >= GetParam().delay);

    queue.visit(mock, GetParam().delay);
}

INSTANTIATE_TEST_SUITE_P(, InventoryChanger_Backend_ResponseQueue_ResponseTimeTest,
    testing::Values(ResponseTime{ 999us, 1ms }, ResponseTime{ 1ms, 1ms }, ResponseTime{ 10ms, 1ms })
);

TEST(InventoryChanger_Backend_ResponseQueueTest, ResponsesAreKeptInQueueWhenNotHandled) {
    ResponseQueue<FakeClock> queue;
    queue.add(response::ItemAdded{ {}, false });
    queue.add(response::StickerApplied{ {}, 0 });

    queue.visit([](auto&&){}, 1ms);

    testing::InSequence _;
    testing::StrictMock<MockResponseHandler> mock;
    EXPECT_CALL(mock, handle(testing::An<const response::ItemAdded&>()));
    EXPECT_CALL(mock, handle(testing::An<const response::StickerApplied&>()));

    queue.visit(mock, 0ms);
}

}
}
