#ifndef CHESS_TTY_GUI_HPP_
#define CHESS_TTY_GUI_HPP_

#include "chess.hpp"

namespace chess
{
namespace gui
{

constexpr std::array<char, 8> PIECE_TO_CHAR = { '.', 'P', 'N', 'B', 'R', 'Q', 'K', '*' };
constexpr std::array<char, 8> RANK_TO_CHAR = { '1', '2', '3', '4', '5', '6', '7', '8' };
constexpr std::array<char, 8> FILE_TO_CHAR = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' };

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

}  // namespace gui
}  // namespace chess

template <typename T>
struct colored_manip {
    const T& value;
    const char* color = chess::gui::COLOR_NO_PLAYER;
};

template <typename T, typename U>
U& operator<<(U& stream, const colored_manip<T>& colored_value) {
    stream << colored_value.color << colored_value.value << chess::gui::COLOR_STOP;
    return stream;
}

namespace chess
{
namespace gui
{

template <typename T>
colored_manip<T> colored(const T& value, player_t player, piece_t piece) {
    return colored_manip<T>{ value, piece == PIECE_EMPTY
        ? COLOR_NO_PLAYER
        : (PLAYER_WHITE == player ? COLOR_PLAYER_WHITE : COLOR_PLAYER_BLACK) };
}

template <typename T>
void print_field(T& stream, const field_state_t field) {
    player_t player = field_get_player(field);
    piece_t piece = field_get_piece(field);
    stream << colored(PIECE_TO_CHAR[piece], player, piece);
}

template <typename T>
void print_move(T& stream, const move_s& move) {
    auto file_str = [&](auto f){ return FILE_TO_CHAR[static_cast<int>(field_file(f))]; };
    auto rank_str = [&](auto f){ return RANK_TO_CHAR[static_cast<int>(field_rank(f))]; };
    stream << colored(PIECE_TO_CHAR[move.piece], move.player, move.piece)
        << ' ' << file_str(move.from) << rank_str(move.from)
        << " -> " << file_str(move.to) << rank_str(move.to);
}

template <typename T>
void print_board(T& stream, const board_state_t& board) {
    stream << '\n';

    for (uint8_t j = 0; j < 8; ++j) {
        stream << static_cast<char>(7 - j + '1') << " ";
        for (uint8_t i = 0; i < 8; ++i) {
            stream << ' ';
            print_field(stream, board[make_field(i, 7 - j)]);
            stream << ' ' << ' ';
        }
        stream << '\n' << " \n";
    }
    stream << "   A   B   C   D   E   F   G   H\n\n";
    auto castling = board_state_meta_get_castling_rights(board);
    stream << "Castling: "
        << colored("WHITE{ SHORT: ", PLAYER_WHITE, PIECE_INVALID)
        << colored(
            castling_rights_white_short(castling) ? "YES," : "NO,", PLAYER_WHITE, PIECE_INVALID)
        << colored(" LONG: ", PLAYER_WHITE, PIECE_INVALID)
        << colored(
            castling_rights_white_long(castling) ? "YES }" : "NO }", PLAYER_WHITE, PIECE_INVALID)
        << colored(" BLACK{ SHORT: ", PLAYER_BLACK, PIECE_INVALID)
        << colored(
            castling_rights_black_short(castling) ? "YES," : "NO,", PLAYER_BLACK, PIECE_INVALID)
        << colored(" LONG: ", PLAYER_BLACK, PIECE_INVALID)
        << colored(
            castling_rights_black_long(castling) ? "YES }" : "NO }", PLAYER_BLACK, PIECE_INVALID)
        << '\n';

    last_move_t last_move = board_state_meta_get_last_move(board);
    player_t last_move_player = last_move_get_player(last_move);
    piece_t last_move_piece = last_move_get_piece(last_move);
    field_t last_move_from = last_move_get_from(last_move);
    field_t last_move_to = last_move_get_to(last_move);
    stream << "Last move: ";
    if (PIECE_EMPTY == last_move_piece or PIECE_INVALID == last_move_piece)
    {
        stream << "<none>\n\n";
    }
    else
    {
        print_move(stream, { last_move_player, last_move_piece, last_move_from, last_move_to });
        stream << '\n' << '\n';
    }
}

}  // namespace gui
}  // namespace chess

#endif  // CHESS_TTY_GUI_HPP_