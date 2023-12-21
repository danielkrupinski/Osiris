#pragma once

struct LinuxVmt {
    static constexpr auto offsetOfOffsetFromTop{0};
    static constexpr auto offsetOfTypeDescriptorPointer{8};
    static constexpr auto offsetOfFirstMethodPointer{16};
};
