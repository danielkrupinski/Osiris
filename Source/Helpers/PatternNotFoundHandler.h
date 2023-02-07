#pragma once

class BytePattern;

struct PatternNotFoundHandler {
    void operator()(BytePattern pattern) const;
};
