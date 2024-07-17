#pragma once

template <typename T>
class PrivateTag {
    PrivateTag() = default;
    friend T;
};
