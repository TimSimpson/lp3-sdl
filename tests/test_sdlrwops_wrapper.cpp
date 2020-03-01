#include <lp3/sdl.hpp>
#include <string>
#include <vector>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("Test RWOPs wrappers", "[read_a_file]") {
    const std::string text("Hi, how's it going?");

    lp3::sdl::RWops rwops(SDL_RWFromConstMem(text.data(), text.size()));

    {
        char hi[2];
        const std::size_t read_size = rwops.read(hi, 1, 2);
        REQUIRE(2 == read_size);
        REQUIRE('H' == hi[0]);
        REQUIRE('i' == hi[1]);
    }

    {
        const std::int64_t tell = rwops.seek(2);
        REQUIRE(4 == tell);
    }

    {
        struct Hows{
            char text[5];
        } hows;

        const auto result = rwops.read(&hows, sizeof(hows));
        REQUIRE(1 == result);
        std::string expected("how's");
        REQUIRE(std::string_view(hows.text, 5) == expected);
    }

    {
        struct Rest{
            char text[10];
        };

        const auto result = rwops.read_optional<Rest>();
        REQUIRE((bool) result);
        std::string expected(" it going?");
        REQUIRE(std::string_view(result->text, 10) == expected);

        // Won't work a second time
        const auto result2 = rwops.read_optional<Rest>();
        REQUIRE(! (bool) result2);
    }
}
// ~end-doc
