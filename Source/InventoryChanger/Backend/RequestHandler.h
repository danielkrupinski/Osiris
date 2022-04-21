#pragma once

#include "RequestTypes.h"
#include "Response.h"

namespace game_items
{
    class Lookup;
}

namespace inventory_changer::backend
{

class BackendSimulator;

struct RequestHandler {
    RequestHandler(BackendSimulator& backend, const game_items::Lookup& gameItemLookup, ItemConstRemover constRemover) : backend{ backend }, gameItemLookup{ gameItemLookup }, constRemover{ constRemover } {}

private:
    BackendSimulator& backend;
    const game_items::Lookup& gameItemLookup;
    ItemConstRemover constRemover;
};

}
