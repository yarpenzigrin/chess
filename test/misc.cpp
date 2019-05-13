#include <memory>
#include "chess_tty_gui.hpp"
#include "chesstest.hpp"

using namespace chess::detail;

using buffer_type = ring_buffer_s<std::string, 5>;

std::string sum_strings(const buffer_type& buffer) {
    std::string result;

    RING_BUFFER_FOREACH(buffer, str_it){
        result += *str_it;
    }
    return result;
}

TEST(Misc_RingBufferStringContainerOfSize5) {
    auto storage = std::make_unique<std::string[]>(5);
    buffer_type buffer;
    buffer.storage = storage.get();

    using namespace std::string_literals;
    ASSERT(sum_strings(buffer) == "");
    ring_buffer_add(buffer, "DO"s);
    ASSERT(sum_strings(buffer) == "DO");
    ring_buffer_add(buffer, "RE"s);
    ASSERT(sum_strings(buffer) == "DORE");
    ring_buffer_add(buffer, "MI"s);
    ASSERT(sum_strings(buffer) == "DOREMI");
    ring_buffer_add(buffer, "FA"s);
    ASSERT(sum_strings(buffer) == "DOREMIFA");
    ring_buffer_add(buffer, "SO"s);
    ASSERT(sum_strings(buffer) == "DOREMIFASO");
    ring_buffer_add(buffer, "LA"s);
    ASSERT(sum_strings(buffer) == "REMIFASOLA");
    ring_buffer_add(buffer, "SI"s);
    ASSERT(sum_strings(buffer) == "MIFASOLASI");
    ring_buffer_add(buffer, "DO"s);
    ASSERT(sum_strings(buffer) == "FASOLASIDO");
}
