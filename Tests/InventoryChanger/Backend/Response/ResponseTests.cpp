#include <variant>

#include <gtest/gtest.h>

#include <InventoryChanger/Backend/Response/Response.h>
#include <InventoryChanger/Backend/Response/ResponseTypes.h>

namespace inventory_changer::backend
{
namespace
{

TEST(InventoryChanger_Backend_ResponseTest, DefaultConstructedResponseIsEmpty) {
    ASSERT_TRUE(isEmptyResponse(Response{}));
}

TEST(InventoryChanger_Backend_ResponseTest, ResponseFromMonostateIsEmpty) {
    ASSERT_TRUE(isEmptyResponse(std::monostate{}));
}

TEST(InventoryChanger_Backend_ResponseTest, ResponseNotFromMonostateIsNotEmpty) {
    ASSERT_FALSE(isEmptyResponse(response::PickEmUpdated{}));
}

}
}
