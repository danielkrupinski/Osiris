#pragma once

#include "Backend/BackendSimulator.h"
#include "Backend/Response.h"

namespace inventory_changer
{

struct BackendResponseHandler {
    explicit BackendResponseHandler(backend::BackendSimulator& backend) : backend{ backend } {}

    void operator()(const backend::Response::ItemAdded& response) const;
    void operator()(const backend::Response::ItemMovedToFront& response) const;
    void operator()(const backend::Response::ItemRemoved& response) const;
    void operator()(const backend::Response::StickerApplied& response) const;
    void operator()(const backend::Response::StickerScraped& response) const {}
    void operator()(const backend::Response::StickerRemoved& response) const;
    void operator()(const backend::Response::StatTrakUpdated& response) const;
    void operator()(const backend::Response::ViewerPassActivated& response) const;
    void operator()(const backend::Response::NameTagAdded& response) const;
    void operator()(const backend::Response::NameTagRemoved& response) const;
    void operator()(const backend::Response::ContainerOpened& response) const;
    void operator()(const backend::Response::PatchApplied& response) const;
    void operator()(const backend::Response::PatchRemoved& response) const;
    void operator()(const backend::Response::SouvenirTokenActivated& response) const;
    void operator()(const backend::Response::GraffitiUnsealed& response) const;
    void operator()(const backend::Response::StatTrakSwapped& response) const;

private:
    backend::BackendSimulator& backend;
};

}
