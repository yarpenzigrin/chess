#ifndef CHESS_TTY_GUI_HPP_
#define CHESS_TTY_GUI_HPP_

#include "chess.hpp"

namespace chess
{
namespace gui
{

constexpr std::array<char, 8> PIECE_TO_CHAR = { '.', 'P', 'N', 'B', 'R', 'Q', 'K', '*' };

const char* COLOR_BLACK = "\e]P0222222";
const char* COLOR_DARKGREY = "\e]P8222222";
const char* COLOR_DARKRED = "\e]P1803232";
const char* COLOR_RED = "\e]P9982b2b";
const char* COLOR_DARKGREEN = "\e]P25b762f";
const char* COLOR_GREEN = "\e[0;32m";
const char* COLOR_BROWN = "\e[0;33m";
const char* COLOR_YELLOW = "\e]PBefef60";
const char* COLOR_DARKBLUE = "\e]P4324c80";
const char* COLOR_BLUE = "\e[0;34m";
const char* COLOR_DARKMAGENTA = "\e]P5706c9a";
const char* COLOR_MAGENTA = "\e]PD826ab1";
const char* COLOR_DARKCYAN = "\e]P692b19e";
const char* COLOR_CYAN = "\e]PEa1cdcd";
const char* COLOR_LIGHTGREY = "\e]P7ffffff";
const char* COLOR_WHITE = "\e]PFdedede";

const char* COLOR_PLAYER_WHITE = COLOR_GREEN;
const char* COLOR_PLAYER_BLACK = COLOR_BLUE;
const char* COLOR_NO_PLAYER = COLOR_BROWN;
const char* COLOR_STOP = "\e[m";

template <typename T>
void print_field(T& stream, const field_state_t field) {
    player_t player = field_get_player(field);
    piece_t piece = field_get_piece(field);
    bool occupied = piece != PIECE_EMPTY;

    const char* color = !occupied ? COLOR_NO_PLAYER :
        (PLAYER_WHITE == player ? COLOR_PLAYER_WHITE : COLOR_PLAYER_BLACK);
    stream << color << PIECE_TO_CHAR[piece] << COLOR_STOP;
}

template <typename T>
void print_board(T& stream, const board_state_t& board) {
    stream << '\n';

    for (uint8_t j = 0; j < 8; ++j) {
        stream << static_cast<char>(7 - j + '1') << " ";
        for (uint8_t i = 0; i < 8; ++i) {
            stream << ' ';
            print_field(stream, board[make_field(i, 7 - j)]);
            stream << ' ';
        }
        stream << '\n' << " \n";
    }
    stream << "   A  B  C  D  E  F  G  H\n\n";
}

}  // namespace gui
}  // namespace chess

#endif  // CHESS_TTY_GUI_HPP_