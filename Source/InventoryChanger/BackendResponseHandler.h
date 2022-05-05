#pragma once

#include <variant>

#include "Backend/BackendSimulator.h"
#include "Backend/Response/ResponseTypes.h"

namespace inventory_changer
{

struct BackendResponseHandler {
    explicit BackendResponseHandler(backend::BackendSimulator& backend) : backend{ backend } {}

    void operator()(std::monostate) const { /* Empty response, this should never be called */ }
    void operator()(const backend::response::ItemAdded& response) const;
    void operator()(const backend::response::ItemMovedToFront& response) const;
    void operator()(const backend::response::ItemUpdated& response) const;
    void operator()(const backend::response::ItemEquipped& response) const;
    void operator()(const backend::response::ItemRemoved& response) const;
    void operator()(const backend::response::StickerApplied& response) const;
    void operator()(const backend::response::StickerScraped& response) const;
    void operator()(const backend::response::StickerRemoved& response) const;
    void operator()(const backend::response::StatTrakUpdated& response) const;
    void operator()(const backend::response::ViewerPassActivated& response) const;
    void operator()(const backend::response::NameTagAdded& response) const;
    void operator()(const backend::response::NameTagRemoved& response) const;
    void operator()(const backend::response::ContainerOpened& response) const;
    void operator()(const backend::response::PatchApplied& response) const;
    void operator()(const backend::response::PatchRemoved& response) const;
    void operator()(const backend::response::SouvenirTokenActivated& response) const;
    void operator()(const backend::response::GraffitiUnsealed& response) const;
    void operator()(const backend::response::StatTrakSwapped& response) const;
    void operator()(const backend::response::TeamGraffitiSelected& response) const;

private:
    backend::BackendSimulator& backend;
};

}
