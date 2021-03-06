#include <lp3/sdl.hpp>

namespace lp3::sdl {

LP3_SDL_API
SDL2::SDL2(Uint32 flags) { SDL_Init(flags); }

LP3_SDL_API
SDL2::~SDL2() { SDL_Quit(); }

LP3_SDL_API
SdlAssertToExceptionConverter::SdlAssertToExceptionConverter()
    : old_handler(SDL_GetAssertionHandler(nullptr)) {
    auto throw_except = [](const SDL_AssertData *, void *) -> SDL_AssertState {
        throw SdlAssertCalled();
    };
    SDL_SetAssertionHandler(throw_except, nullptr);
}

LP3_SDL_API
SdlAssertToExceptionConverter::~SdlAssertToExceptionConverter() {
    SDL_SetAssertionHandler(old_handler, nullptr);
    SDL_ResetAssertionReport();
}

LP3_SDL_API
RWops::RWops() : ops(nullptr) {}

LP3_SDL_API
RWops::RWops(SDL_RWops * _ops) : ops(_ops) { SDL_assert(nullptr != _ops); }

LP3_SDL_API
RWops::~RWops() {
    if (nullptr != ops) {
        auto result = ops->close(ops);
        SDL_assert(0 == result);
    }
    ops = nullptr;
}

LP3_SDL_API
RWops::RWops(RWops && rhs) : ops(rhs.ops) { rhs.ops = nullptr; }

LP3_SDL_API
RWops & RWops::operator=(RWops && rvalue) {
    SDL_assert(nullptr == ops);
    if (this->ops != rvalue.ops) {
        this->ops = rvalue.ops;
        rvalue.ops = nullptr;
    }
    return *this;
}

} // namespace lp3::sdl
