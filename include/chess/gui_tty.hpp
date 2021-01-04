#ifndef CHESS_GUI_TTY_HPP_
#define CHESS_GUI_TTY_HPP_

#include "chess/core.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <deque>
#include <sys/ioctl.h>
#include <unistd.h>

namespace chess
{
namespace gui
{

constexpr std::array<char, 8> PIECE_TO_CHAR = { '.', 'P', 'N', 'B', 'R', 'Q', 'K', '*' };
constexpr std::array<char, 8> RANK_TO_CHAR = { '1', '2', '3', '4', '5', '6', '7', '8' };
constexpr std::array<char, 8> FILE_TO_CHAR = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' };

const char* COLOR_BLACK       = "\e]P0222222";
const char* COLOR_DARKGREY    = "\e]P8222222";
const char* COLOR_DARKRED     = "\e]P1803232";
const char* COLOR_RED         = "\e]P9982b2b";
const char* COLOR_DARKGREEN   = "\e]P25b762f";
const char* COLOR_GREEN       = "\e[0;32m";
const char* COLOR_BROWN       = "\e[0;33m";
const char* COLOR_YELLOW      = "\e]PBefef60";
const char* COLOR_DARKBLUE    = "\e]P4324c80";
const char* COLOR_BLUE        = "\e[0;34m";
const char* COLOR_DARKMAGENTA = "\e]P5706c9a";
const char* COLOR_MAGENTA     = "\e]PD826ab1";
const char* COLOR_DARKCYAN    = "\e]P692b19e";
const char* COLOR_CYAN        = "\e]PEa1cdcd";
const char* COLOR_LIGHTGREY   = "\e]P7ffffff";
const char* COLOR_WHITE       = "\e]PFdedede";

const char* COLOR_PLAYER_WHITE = COLOR_GREEN;
const char* COLOR_PLAYER_BLACK = COLOR_BLUE;
const char* COLOR_NO_PLAYER = COLOR_BROWN;
const char* COLOR_STOP = "\e[m";

struct layout_t {
    std::unique_ptr<char[]> buffer;
    uint16_t rows = 0;
    uint16_t cols = 0;

    struct frame_t {
        enum class color_code {
            GREEN = 0, BLUE, BROWN, NONE
        };
        struct color_pos {
            std::size_t pos = 0;
            color_code color = color_code::NONE;
        };

        std::unique_ptr<char[]> buffer;
        std::vector<color_pos> colors;
        uint16_t written_chars = 0;
        uint16_t origin_x = 0;
        uint16_t origin_y = 0;
        uint16_t width = 0;
        uint16_t height = 0;
        uint8_t margin = 0;
        uint8_t padding = 0;
        bool draw_border = false;
        bool reset_after_display = false;
    };

    std::array<frame_t, 10> frames;
};

std::size_t frame_line_size(const layout_t::frame_t& frame) {
    auto padding_lines =  2u * frame.padding;
    auto margin_lines = 2u * frame.margin;
    auto border_lines = frame.draw_border ? 2u : 0u;
    return frame.width - padding_lines - margin_lines - border_lines;
}

std::size_t frame_buffer_size(const layout_t::frame_t& frame) {
    auto padding_lines =  2u * frame.padding;
    auto margin_lines = 2u * frame.margin;
    auto border_lines = frame.draw_border ? 2u : 0u;
    auto total_lines_consumed = padding_lines + margin_lines + border_lines;
    return (frame.width - total_lines_consumed) * (frame.height - total_lines_consumed);
}

void reset_frame(layout_t::frame_t& frame) {
    frame.written_chars = 0;
    frame.colors.clear();
    auto buffer_size = frame_buffer_size(frame);
    std::fill_n(frame.buffer.get(), buffer_size, ' ');
}

void reset_frame_after_display(layout_t::frame_t& frame) {
    frame.reset_after_display = true;
}

layout_t::frame_t chessboard_frame() {
    layout_t::frame_t frame;
    frame.origin_x = 0;
    frame.origin_y = 0;
    frame.width = 49;
    frame.height = 21;
    frame.margin = 0;
    frame.padding = 1;
    frame.draw_border = true;
    auto buffer_size = frame_buffer_size(frame);
    frame.buffer = std::make_unique<char[]>(buffer_size);
    reset_frame(frame);
    return frame;
}

layout_t::frame_t last_move_frame() {
    layout_t::frame_t frame;
    frame.origin_x = 49;
    frame.origin_y = 0;
    frame.width = 30;
    frame.height = 5;
    frame.margin = 0;
    frame.padding = 0;
    frame.draw_border = true;
    auto buffer_size = frame_buffer_size(frame);
    frame.buffer = std::make_unique<char[]>(buffer_size);
    reset_frame(frame);
    return frame;
}

layout_t::frame_t castling_frame() {
    layout_t::frame_t frame;
    frame.origin_x = 79;
    frame.origin_y = 0;
    frame.width = 30;
    frame.height = 5;
    frame.margin = 0;
    frame.padding = 0;
    frame.draw_border = true;
    auto buffer_size = frame_buffer_size(frame);
    frame.buffer = std::make_unique<char[]>(buffer_size);
    reset_frame(frame);
    return frame;
}

layout_t::frame_t dialog_frame() {
    layout_t::frame_t frame;
    frame.origin_x = 0;
    frame.origin_y = 21;
    frame.width = 49;
    frame.height = 4;
    frame.margin = 0;
    frame.padding = 0;
    frame.draw_border = true;
    auto buffer_size = frame_buffer_size(frame);
    frame.buffer = std::make_unique<char[]>(buffer_size);
    reset_frame(frame);
    return frame;
}

layout_t::frame_t custom_frame() {
    layout_t::frame_t frame;
    frame.origin_x = 49;
    frame.origin_y = 5;
    frame.width = 60;
    frame.height = 20;
    frame.margin = 0;
    frame.padding = 0;
    frame.draw_border = true;
    auto buffer_size = frame_buffer_size(frame);
    frame.buffer = std::make_unique<char[]>(buffer_size);
    reset_frame(frame);
    return frame;
}

void reset_layout_buffer(layout_t& layout) {
    constexpr auto BUFFER_SIZE = 30u*1024u;
    for (auto r = 0u; r < layout.rows; ++r)
    {
        std::fill_n(layout.buffer.get() + (r * layout.cols), layout.cols - 1, ' ');
        layout.buffer[(r + 1) * layout.cols - 1] = '\n';
    }
    std::fill_n(layout.buffer.get() + layout.rows * layout.cols,
        BUFFER_SIZE - layout.rows * layout.cols, '\0');
}

layout_t game_layout() {
    static winsize terminal_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_size);

    if (terminal_size.ws_row < 30 or terminal_size.ws_col < 120)
    {
        std::cerr << "Terminal size " << terminal_size.ws_row << '/' << terminal_size.ws_col
            << " is too small. At least 30/100 is required.";
        std::exit(1);
    }
    layout_t layout;
    constexpr auto BUFFER_SIZE = 30u*1024u;
    layout.buffer = std::make_unique<char[]>(BUFFER_SIZE);
    layout.rows = 26;
    layout.cols = 120;
    layout.frames[0] = chessboard_frame();
    layout.frames[1] = last_move_frame();
    layout.frames[2] = castling_frame();
    layout.frames[3] = dialog_frame();
    layout.frames[4] = custom_frame();
    reset_layout_buffer(layout);
    return layout;
}

void display(layout_t& layout) {
    auto set_cursor = [&](const auto& frame, const auto line)
    {
        return layout.buffer.get() + layout.cols * (frame.origin_y + line) + frame.origin_x;
    };
    auto draw_hor_margin_lines = [&](const auto& frame, auto& cursor, auto& line)
    {
        for (auto m = 0u; m < frame.margin; ++m)
        {
            std::fill_n(cursor, frame.width, ' ');
            cursor = set_cursor(frame, ++line);
        }
    };
    auto draw_hor_border = [&](const auto& frame, auto& cursor, auto& line)
    {
        if (frame.draw_border)
        {
            cursor = std::fill_n(cursor, frame.margin, ' ');
            *cursor++ = '*';
            cursor = std::fill_n(cursor, frame.width - 2 * frame.margin - 2, '-');
            *cursor++ = '*';
            cursor = std::fill_n(cursor, frame.margin, ' ');
            cursor = set_cursor(frame, ++line);
        }
    };
    auto draw_hor_padding_lines = [&](const auto& frame, auto& cursor, auto& line)
    {
        for (auto p = 0u; p < frame.padding; ++p)
        {
            cursor = std::fill_n(cursor, frame.margin, ' ');
            if (frame.draw_border)
            {
                *cursor++ = '|';
            }
            cursor = std::fill_n(cursor,
                frame.width - 2 * frame.margin - (frame.draw_border ? 2 : 0), ' ');
            if (frame.draw_border)
            {
                *cursor++ = '|';
            }
            cursor = std::fill_n(cursor, frame.margin, ' ');
            cursor = set_cursor(frame, ++line);
        }
    };
    auto draw_line = [&](const auto& frame, auto& cursor, auto& line, auto& frame_cursor)
    {
        cursor = std::fill_n(cursor, frame.margin, ' ');
        if (frame.draw_border){
            *cursor++ = '|';
        }
        cursor = std::fill_n(cursor, frame.padding, ' ');

        auto chars_to_copy = frame_line_size(frame);
        while (chars_to_copy) {
            const auto c = *frame_cursor++;
            if (c != '\n') {
                *cursor++ = c;
                --chars_to_copy;
            } else {
                break;
            }
        }
        cursor = std::fill_n(cursor, chars_to_copy + frame.padding, ' ');
        if (frame.draw_border)
        {
            *cursor++ = '|';
        }
        cursor = std::fill_n(cursor, frame.margin, ' ');
        cursor = set_cursor(frame, ++line);
    };
    for (const auto& f : layout.frames) {
        auto line = 0u;
        auto cursor = set_cursor(f, line);
        draw_hor_margin_lines(f, cursor, line);
        draw_hor_border(f, cursor, line);
        draw_hor_padding_lines(f, cursor, line);
        for (auto frame_cursor = f.buffer.get();
            line < (f.height - f.margin - f.padding - (f.draw_border ? 1u : 0u));) {
            draw_line(f, cursor, line, frame_cursor);
        }
        draw_hor_padding_lines(f, cursor, line);
        draw_hor_border(f, cursor, line);
        draw_hor_margin_lines(f, cursor, line);
    }

    std::deque<layout_t::frame_t::color_pos> color_positions;
    for (auto& f : layout.frames) {
        std::stable_sort(std::begin(f.colors), std::end(f.colors),
            [](const auto& lhs, const auto& rhs){ return lhs.pos < rhs.pos; });
        std::size_t chars_per_line = frame_line_size(f);
        for (const auto& cp : f.colors) {
            std::size_t line = cp.pos / chars_per_line;
            std::size_t pos_in_line = cp.pos % chars_per_line;
            std::size_t layout_line =
                f.origin_y + f.margin + f.padding + (f.draw_border ? 1u : 0u) + line;
            std::size_t layout_pos_in_line =
                f.origin_x + f.margin + f.padding + (f.draw_border ? 1u : 0u) + pos_in_line;
            color_positions.push_back({ layout_line * layout.cols + layout_pos_in_line, cp.color });
        }
    }
    if (color_positions.empty()) {
        std::cout << layout.buffer.get();
    } else {
        constexpr auto BUFFER_SIZE = 30u*1024u;
        auto secondary_buffer = std::make_unique<char[]>(BUFFER_SIZE);
        auto write_cursor = secondary_buffer.get();
        auto read_pos = 0u;

        std::stable_sort(std::begin(color_positions), std::end(color_positions),
            [](const auto& lhs, const auto& rhs){ return lhs.pos < rhs.pos; });

        auto cp = color_positions.front();
        color_positions.pop_front();
        do {
            *write_cursor++ = layout.buffer[read_pos++];
            if (read_pos == cp.pos) {
                const char* cc = nullptr;
                switch (cp.color) {
                    case layout_t::frame_t::color_code::GREEN: { cc = COLOR_GREEN; break; }
                    case layout_t::frame_t::color_code::BLUE: { cc = COLOR_BLUE; break; }
                    case layout_t::frame_t::color_code::BROWN: { cc = COLOR_BROWN; break; }
                    case layout_t::frame_t::color_code::NONE: { cc = COLOR_STOP; break; }
                }
                std::strcpy(write_cursor, cc);
                write_cursor += std::strlen(cc);
                if (color_positions.empty()) {
                    break;
                }
                cp = color_positions.front();
                color_positions.pop_front();
            }
        } while (layout.buffer[read_pos] != '\0');
        std::copy_n(layout.buffer.get() + read_pos,
            BUFFER_SIZE - (write_cursor - secondary_buffer.get()), write_cursor);
        std::cout << secondary_buffer.get();
    }

    for (auto& f : layout.frames) {
        if (f.reset_after_display) {
            reset_frame(f);
            f.reset_after_display = false;
        }
    }
}

struct frame_stream
{
    frame_stream(layout_t::frame_t* frame) : frame_{ frame } {}

    template <typename T>
    frame_stream& operator<<(T&& value) {
        auto value_str = [v = std::forward<T>(value)]{
            std::stringstream ss;
            ss << v;
            return ss.str();
        }();
        auto read_cursor = value_str.c_str();
        auto chars_written = 0u;
        const auto chars_per_line = frame_line_size(*frame_);

        auto consume_until_eol = [&]{
            auto chars_to_write = std::min(
                value_str.size() - chars_written,
                frame_buffer_size(*frame_) - frame_->written_chars);
            while (chars_to_write) {
                if (*read_cursor != '\n') {
                    frame_->buffer[frame_->written_chars++] = *read_cursor++;
                    ++chars_written;
                    --chars_to_write;
                } else {
                    auto fill = chars_per_line - (frame_->written_chars % chars_per_line);
                    std::fill_n(frame_->buffer.get() + frame_->written_chars, fill, ' ');
                    frame_->written_chars += fill;
                    ++chars_written;
                    ++read_cursor;
                    return true;
                }
            }
            return false;
        };
        while (consume_until_eol()) {}
        return *this;
    }

private:
    layout_t::frame_t* frame_;
};

template <>
frame_stream& frame_stream::operator<<(const layout_t::frame_t::color_code& color) {
    frame_->colors.push_back({ frame_->written_chars, color });
    return *this;
}

template <>
frame_stream& frame_stream::operator<<(layout_t::frame_t::color_code&& color) {
    frame_->colors.push_back({ frame_->written_chars, color });
    return *this;
}

layout_t::frame_t::color_code decide_color(piece_t piece, player_t player) {
    return piece == PIECE_EMPTY
        ? layout_t::frame_t::color_code::BROWN
        : (PLAYER_WHITE == player
            ? layout_t::frame_t::color_code::GREEN
            : layout_t::frame_t::color_code::BLUE);
}

template <typename T>
void print_move(T& stream, const move_s& move) {
    auto file_str = [&](auto f){ return FILE_TO_CHAR[static_cast<int>(field_file(f))]; };
    auto rank_str = [&](auto f){ return RANK_TO_CHAR[static_cast<int>(field_rank(f))]; };
    stream << decide_color(move.piece, move.player) << PIECE_TO_CHAR[move.piece]
        << ' ' << file_str(move.from) << rank_str(move.from)
        << " -> " << file_str(move.to) << rank_str(move.to) << layout_t::frame_t::color_code::NONE;
}

void print_board(layout_t& layout, const board_state_t& board) {
    chess::gui::reset_frame(layout.frames[0]);
    auto chessboard_frame = frame_stream(&layout.frames[0]);
    for (uint8_t j = 0; j < 8; ++j) {
        chessboard_frame << "  " << static_cast<char>(7 - j + '1') << ' ';
        for (uint8_t i = 0; i < 8; ++i) {
            auto field = board[make_field(i, 7 - j)];
            player_t player = field_get_player(field);
            piece_t piece = field_get_piece(field);
            chessboard_frame << "  " << decide_color(piece, player) << PIECE_TO_CHAR[piece]
                << layout_t::frame_t::color_code::NONE << ((7 - i) ? "  " : "");
        }
        chessboard_frame << "\n\n";
    }
    chessboard_frame << "      A    B    C    D    E    F    G    H";

    chess::gui::reset_frame(layout.frames[2]);
    auto castling_frame = frame_stream(&layout.frames[2]);
    auto castling = board_state_meta_get_castling_rights(board);
    castling_frame << "Castling:\n"
        << layout_t::frame_t::color_code::GREEN << "  SHORT: "
        << (castling_rights_white_short(castling) ? "YES," : "NO,")
        << " LONG: "
        << (castling_rights_white_long(castling) ? "YES" : "NO")
        << layout_t::frame_t::color_code::NONE
        << '\n'
        << layout_t::frame_t::color_code::BLUE << "  SHORT: "
        << (castling_rights_black_short(castling) ? "YES," : "NO,")
        << " LONG: "
        << (castling_rights_black_long(castling) ? "YES" : "NO")
        << layout_t::frame_t::color_code::NONE
        << "\n\n";

    chess::gui::reset_frame(layout.frames[1]);
    auto last_move_frame = frame_stream(&layout.frames[1]);
    last_move_t last_move = board_state_meta_get_last_move(board);
    player_t last_move_player = last_move_get_player(last_move);
    piece_t last_move_piece = last_move_get_piece(last_move);
    field_t last_move_from = last_move_get_from(last_move);
    field_t last_move_to = last_move_get_to(last_move);
    last_move_frame << "Last move:\n";
    if (PIECE_EMPTY == last_move_piece or PIECE_INVALID == last_move_piece) {
        last_move_frame << "<none>\n";
    } else {
        print_move(last_move_frame,
            { last_move_player, last_move_piece, last_move_from, last_move_to });
        last_move_frame << '\n';
    }
}

}  // namespace gui
}  // namespace chess

#endif  // CHESS_GUI_TTY_HPP_