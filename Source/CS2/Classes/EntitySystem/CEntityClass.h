#pragma once

namespace cs2
{

struct CEntityClass {};
// The engine's entity-class registration tree uses an intermediate node. Its
// layout is intentionally opaque; its two links are resolved from client.dll.
struct CEntityClassParentNode {};

}
