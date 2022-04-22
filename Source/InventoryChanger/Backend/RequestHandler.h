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

    Response operator()(const request::ApplySticker& request);
    Response operator()(const request::WearSticker& request);
    Response operator()(const request::SwapStatTrak& request);

private:
    BackendSimulator& backend;
    const game_items::Lookup& gameItemLookup;
    ItemConstRemover constRemover;
};

}
