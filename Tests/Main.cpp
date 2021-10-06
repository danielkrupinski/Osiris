#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#ifdef _WIN32
int __cdecl main(int argc, char* argv[])
#else
int main(int argc, char* argv[])
#endif
{
    return Catch::Session().run(argc, argv);
}
