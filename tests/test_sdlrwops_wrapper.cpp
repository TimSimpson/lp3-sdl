#include <algorithm>
#include <lp3/sdl.hpp>
#include <string>
#include <vector>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("RWops read", "[read_a_file]") {
    const std::string text("Hi, how's it going?");

    lp3::sdl::RWops rwops(SDL_RWFromConstMem(text.data(), static_cast<int>(text.size())));

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
        struct Hows {
            char text[5];
        } hows;

        const auto result = rwops.read(&hows, sizeof(hows));
        REQUIRE(1 == result);
        std::string expected("how's");
        REQUIRE(std::string_view(hows.text, 5) == expected);
    }

    {
        struct Rest {
            char text[10];
        };

        const auto result = rwops.read_optional<Rest>();
        REQUIRE((bool)result);
        std::string expected(" it going?");
        REQUIRE(std::string_view(result->text, 10) == expected);

        // Won't work a second time
        const auto result2 = rwops.read_optional<Rest>();
        REQUIRE(!(bool)result2);
    }

    {
        rwops.seek_from_beginning(1);
        const auto i = rwops.read_optional<char>();
        REQUIRE((bool)i);
        REQUIRE('i' == *i);
    }

    {
        rwops.seek_from_end(-1);
        const auto q = rwops.read_optional<char>();
        REQUIRE((bool)q);
        REQUIRE('?' == *q);
    }
}

TEST_CASE("RWops write", "[write_a_file]") {
    std::string text(40, '*');

    lp3::sdl::RWops rwops(SDL_RWFromMem(text.data(), static_cast<int>(text.size())));

    {
        std::string new_text("Good day.");
        const auto result = rwops.write(new_text.data(), new_text.size());
        REQUIRE(1 == result);
        REQUIRE(text == "Good day.*******************************");
    }

    {
#pragma pack(1)
        struct WeirdStruct {
            std::int8_t number;
            char word[5];
            std::int64_t big_number;
        };
#pragma pack()

        WeirdStruct weird_struct;
        weird_struct.number = 65; // 'a'
        std::string hello("HELLO");
        std::copy(hello.begin(), hello.end(), weird_struct.word);
        weird_struct.big_number = 0x21756f7920656573; // "see you!"

        const auto result = rwops.write(weird_struct);
        REQUIRE(1 == result);
        REQUIRE(text == "Good day.AHELLOsee you!*****************");
    }
}
// ~end-doc
