#ifndef CHESS_GUI_NCURSES_HPP_
#define CHESS_GUI_NCURSES_HPP_

#include <ncurses.h>
#include <array>

namespace chess {
namespace ncurses {

namespace {

const wchar_t* PIECE_TO_EMOJI[2][8] = {
    { L" ", L"♟", L"♞", L"♝", L"♜", L"♛", L"♚", L"!" },
    { L" ", L"♙", L"♘", L"♗", L"♖", L"♕", L"♔", L"!" }
};
constexpr std::array<char, 8> RANK_TO_CHAR = { '1', '2', '3', '4', '5', '6', '7', '8' };
constexpr std::array<char, 8> FILE_TO_CHAR = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H' };

constexpr int COLOR_PAIR_DESC[2][2] = { { 1, 2 }, { 3, 4 } };
constexpr int INFO_COLOR_PAIR = 5;
constexpr int INV_COLOR_PAIR = 6;

struct preset_1 {
    static constexpr int SQUARE_WIDTH = 5;
    static constexpr int SQUARE_HEIGHT = 3;
    static constexpr bool CHESSBOARD_BORDER = true;
    static constexpr int CHESSBOARD_OFFSET_X = 1;
    static constexpr int CHESSBOARD_OFFSET_Y = 1;
    static constexpr int RANK_DESC_WIDTH = 3;
    static constexpr int FILE_DESC_HEIGHT = 2;
    static constexpr int GAME_STATUS_OFFSET_Y = 3;
    static constexpr int GAME_STATUS_HEIGHT = 3;
    static constexpr int LAST_MOVE_WIDTH = 30;
    static constexpr int LAST_MOVE_HEIGHT = 5;
    static constexpr int LAST_MOVE_OFFSET_X = 3;
    static constexpr int CASTLING_WIDTH = 30;
    static constexpr int CASTLING_HEIGHT = 5;
    static constexpr int CASTLING_OFFSET_X = 2;
};

struct preset_2 {
    static constexpr int SQUARE_WIDTH = 3;
    static constexpr int SQUARE_HEIGHT = 1;
    static constexpr bool CHESSBOARD_BORDER = true;
    static constexpr int CHESSBOARD_OFFSET_X = 1;
    static constexpr int CHESSBOARD_OFFSET_Y = 1;
    static constexpr int RANK_DESC_WIDTH = 1;
    static constexpr int FILE_DESC_HEIGHT = 1;
    static constexpr int GAME_STATUS_OFFSET_Y = 2;
    static constexpr int GAME_STATUS_HEIGHT = 3;
    static constexpr int LAST_MOVE_WIDTH = 30;
    static constexpr int LAST_MOVE_HEIGHT = 5;
    static constexpr int LAST_MOVE_OFFSET_X = 1;
    static constexpr int CASTLING_WIDTH = 30;
    static constexpr int CASTLING_HEIGHT = 5;
    static constexpr int CASTLING_OFFSET_X = 0;
};

using DEFAULTS = preset_2;

constexpr int SQUARE_WIDTH = DEFAULTS::SQUARE_WIDTH;
constexpr int SQUARE_HEIGHT = DEFAULTS::SQUARE_HEIGHT;
constexpr bool CHESSBOARD_BORDER = DEFAULTS::CHESSBOARD_BORDER;
constexpr int CHESSBOARD_OFFSET_X = DEFAULTS::CHESSBOARD_OFFSET_X;
constexpr int CHESSBOARD_OFFSET_Y = DEFAULTS::CHESSBOARD_OFFSET_Y;
constexpr int RANK_DESC_WIDTH = DEFAULTS::RANK_DESC_WIDTH;
constexpr int FILE_DESC_HEIGHT = DEFAULTS::FILE_DESC_HEIGHT;
constexpr int GAME_STATUS_OFFSET_Y = DEFAULTS::GAME_STATUS_OFFSET_Y;
constexpr int GAME_STATUS_HEIGHT = DEFAULTS::GAME_STATUS_HEIGHT;
constexpr int LAST_MOVE_WIDTH = DEFAULTS::LAST_MOVE_WIDTH;
constexpr int LAST_MOVE_HEIGHT = DEFAULTS::LAST_MOVE_HEIGHT;
constexpr int LAST_MOVE_OFFSET_X = DEFAULTS::LAST_MOVE_OFFSET_X;
constexpr int CASTLING_WIDTH = DEFAULTS::CASTLING_WIDTH;
constexpr int CASTLING_HEIGHT = DEFAULTS::CASTLING_HEIGHT;
constexpr int CASTLING_OFFSET_X = DEFAULTS::CASTLING_OFFSET_X;
}  // namespace

struct board_rect_t {
    WINDOW* board_win = nullptr;
    WINDOW* board_border_win = nullptr;
    WINDOW* rank_desc_win = nullptr;
    WINDOW* file_desc_win = nullptr;
};

struct game_status_rect_t {
    WINDOW* game_status_win = nullptr;
    WINDOW* game_status_border_win = nullptr;
};

struct last_move_rect_t {
    WINDOW* last_move_win = nullptr;
    WINDOW* last_move_border_win = nullptr;
};

struct castling_rect_t {
    WINDOW* castling_win = nullptr;
    WINDOW* castling_border_win = nullptr;
};

struct layout_t {
    board_rect_t board_rect;
    game_status_rect_t game_status_rect;
    last_move_rect_t last_move_rect;
    castling_rect_t castling_rect;
};

namespace {

void init_rank_desc(WINDOW* w) {
    for (int row = 0; row < 8; ++row) {
        for (int nl_bef = 0; nl_bef < (SQUARE_HEIGHT - 1) / 2; ++nl_bef) { waddch(w, '\n'); }

        for (int sp_bef = 0; sp_bef < (RANK_DESC_WIDTH - 1) / 2; ++sp_bef) { waddch(w, ' '); }
        waddch(w, static_cast<char>(7 - row + '1'));
        if (RANK_DESC_WIDTH % 2 == 0) { waddch(w, ' '); }
        for (int sp_after = 0; sp_after < (RANK_DESC_WIDTH - 1) / 2; ++sp_after) { waddch(w, ' '); }

        for (int nl_after = 0; nl_after < (SQUARE_HEIGHT - 1) / 2; ++nl_after) { waddch(w, '\n'); }
    }
}

void init_file_desc(WINDOW* w) {
    if (FILE_DESC_HEIGHT % 2 == 0) { waddch(w, '\n'); }
    for (int nl_bef = 0; nl_bef < (FILE_DESC_HEIGHT - 1) / 2; ++nl_bef) { waddch(w, '\n'); }

    for (int col = 0; col < 8; ++col) {
        for (int sp_bef = 0; sp_bef < (SQUARE_WIDTH - 1) / 2; ++sp_bef) { waddch(w, ' '); }
        waddch(w, static_cast<char>('A' + col));
        for (int sp_after = 0; sp_after < (SQUARE_WIDTH - 1) / 2; ++sp_after) { waddch(w, ' '); }
    }

    for (int nl_after = 0; nl_after < (FILE_DESC_HEIGHT - 1) / 2; ++nl_after) { waddch(w, '\n'); }
}

void init_board_rect(layout_t& layout) {
    auto board_win_h = SQUARE_HEIGHT * 8;
    auto board_win_w = SQUARE_WIDTH * 8;
    auto board_win_y = CHESSBOARD_OFFSET_Y + CHESSBOARD_BORDER;
    auto board_win_x = CHESSBOARD_OFFSET_X + RANK_DESC_WIDTH + CHESSBOARD_BORDER;
    layout.board_rect.board_win = newwin(board_win_h, board_win_w, board_win_y, board_win_x);

    auto rank_desc_h = board_win_h;
    auto rank_desc_w = RANK_DESC_WIDTH;
    auto rank_desc_y = CHESSBOARD_OFFSET_Y + CHESSBOARD_BORDER;
    auto rank_desc_x = CHESSBOARD_OFFSET_X;
    layout.board_rect.rank_desc_win = newwin(rank_desc_h, rank_desc_w, rank_desc_y, rank_desc_x);

    auto file_desc_h = FILE_DESC_HEIGHT;
    auto file_desc_w = board_win_w;
    auto file_desc_y = CHESSBOARD_OFFSET_Y + board_win_h + 2 * CHESSBOARD_BORDER;
    auto file_desc_x = CHESSBOARD_OFFSET_X + RANK_DESC_WIDTH + CHESSBOARD_BORDER;
    layout.board_rect.file_desc_win = newwin(file_desc_h, file_desc_w, file_desc_y, file_desc_x);

    auto border_h = board_win_h + 2 * CHESSBOARD_BORDER;
    auto border_w = board_win_w + 2 * CHESSBOARD_BORDER;
    auto border_y = board_win_y - CHESSBOARD_BORDER;
    auto border_x = board_win_x - CHESSBOARD_BORDER;
    layout.board_rect.board_border_win = newwin(border_h, border_w, border_y, border_x);
    if (CHESSBOARD_BORDER) {
        wborder(layout.board_rect.board_border_win, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    init_rank_desc(layout.board_rect.rank_desc_win);
    init_file_desc(layout.board_rect.file_desc_win);
}

void init_game_status_rect(layout_t& layout) {
    auto game_status_h = GAME_STATUS_HEIGHT;
    auto game_status_w = SQUARE_WIDTH * 8 + 2 * CHESSBOARD_BORDER + RANK_DESC_WIDTH - 2;
    auto game_status_y = CHESSBOARD_OFFSET_Y + SQUARE_HEIGHT * 8 + 2 * CHESSBOARD_BORDER +
        FILE_DESC_HEIGHT + GAME_STATUS_OFFSET_Y;
    auto game_status_x = CHESSBOARD_OFFSET_X + 1;
    layout.game_status_rect.game_status_win = newwin(
        game_status_h, game_status_w, game_status_y, game_status_x);

    auto border_h = game_status_h + 2;
    auto border_w = game_status_w + 2;
    auto border_y = game_status_y - 1;
    auto border_x = game_status_x - 1;
    layout.game_status_rect.game_status_border_win = newwin(border_h, border_w, border_y, border_x);
    wborder(layout.game_status_rect.game_status_border_win, 0, 0, 0, 0, 0, 0, 0, 0);
}

void init_last_move_rect(layout_t& layout) {
    auto last_move_h = LAST_MOVE_HEIGHT - 2;
    auto last_move_w = LAST_MOVE_WIDTH - 2;
    auto last_move_y = CHESSBOARD_OFFSET_Y + 1;
    auto last_move_x = CHESSBOARD_OFFSET_X + RANK_DESC_WIDTH + CHESSBOARD_BORDER * 2 +
        SQUARE_WIDTH * 8 + LAST_MOVE_OFFSET_X;
    layout.last_move_rect.last_move_win = newwin(
        last_move_h, last_move_w, last_move_y, last_move_x);

    auto border_h = last_move_h + 2;
    auto border_w = last_move_w + 2;
    auto border_y = last_move_y - 1;
    auto border_x = last_move_x - 1;
    layout.last_move_rect.last_move_border_win = newwin(border_h, border_w, border_y, border_x);
    wborder(layout.last_move_rect.last_move_border_win, 0, 0, 0, 0, 0, 0, 0, 0);
}

void init_castling_rect(layout_t& layout) {
    auto castling_h = CASTLING_HEIGHT - 2;
    auto castling_w = CASTLING_WIDTH - 2;
    auto castling_y = CHESSBOARD_OFFSET_Y + 1;
    auto castling_x = CHESSBOARD_OFFSET_X + RANK_DESC_WIDTH + CHESSBOARD_BORDER * 2 +
        SQUARE_WIDTH * 8 + LAST_MOVE_OFFSET_X + LAST_MOVE_WIDTH + CASTLING_OFFSET_X;
    layout.castling_rect.castling_win = newwin(castling_h, castling_w, castling_y, castling_x);

    auto border_h = castling_h + 2;
    auto border_w = castling_w + 2;
    auto border_y = castling_y - 1;
    auto border_x = castling_x - 1;
    layout.castling_rect.castling_border_win = newwin(border_h, border_w, border_y, border_x);
    wborder(layout.castling_rect.castling_border_win, 0, 0, 0, 0, 0, 0, 0, 0);
}

void update_board_win(WINDOW* w, const board_state_t& board) {
    auto active_pair = INFO_COLOR_PAIR;
    auto update_color = [&, char_count = 0](const auto player) mutable {
        wattroff(w, COLOR_PAIR(active_pair));
        auto square = (char_count++ / SQUARE_WIDTH) % 2 == 0;
        active_pair = COLOR_PAIR_DESC[square][player];
        wattron(w, COLOR_PAIR(active_pair));
    };
    auto print_board_char = [&](const auto c) {
        if (c != '\n') { update_color(PLAYER_BLACK); }
        waddch(w, c);
    };
    auto print_board_str = [&](const char* str) {
        while (*str != '\0') {
            print_board_char(*str);
            ++str;
        }
    };
    auto print_board_piece = [&](const auto player, const auto piece) {
        update_color(player);
        wattron(w, A_BOLD);
        waddwstr(w, PIECE_TO_EMOJI[player][piece]);
        wattroff(w, A_BOLD);
    };

    wmove(w, 0, 0);
    for (uint8_t j = 0; j < 8; ++j) {
        for (int i = 0; i < (SQUARE_HEIGHT - 1) / 2; ++i) {
            for (int j = 0; j < SQUARE_WIDTH; ++j) { print_board_str("        "); }
        }
        for (uint8_t i = 0; i < 8; ++i) {
            for (int i = 0; i < (SQUARE_WIDTH - 1) / 2; ++i) { print_board_char(' '); }

            auto field = board[make_field(i, 7 - j)];
            player_t player = field_get_player(field);
            piece_t piece = field_get_piece(field);
            print_board_piece(player, piece);

            for (int i = 0; i < (SQUARE_WIDTH - 1) / 2; ++i) { print_board_char(' '); }
        }
        for (int i = 0; i < (SQUARE_HEIGHT - 1) / 2; ++i) {
            for (int j = 0; j < SQUARE_WIDTH; ++j) { print_board_str("        "); }
        }

        for (int i = 0; i < SQUARE_WIDTH; ++i) { update_color(PLAYER_BLACK); }
    }
}

void update_last_move_win(WINDOW* w, const board_state_t& board) {
    last_move_t last_move = board_state_meta_get_last_move(board);
    player_t last_move_player = last_move_get_player(last_move);
    piece_t last_move_piece = last_move_get_piece(last_move);
    field_t last_move_from = last_move_get_from(last_move);
    field_t last_move_to = last_move_get_to(last_move);

    werase(w);
    wmove(w, 0, 0);
    waddstr(w, " Last move:\n");

    auto file_ch = [&](auto f){ return FILE_TO_CHAR[static_cast<int>(field_file(f))]; };
    auto rank_ch = [&](auto f){ return RANK_TO_CHAR[static_cast<int>(field_rank(f))]; };
    if (PIECE_EMPTY == last_move_piece or PIECE_INVALID == last_move_piece) {
        waddstr(w, " <none>\n");
    } else {
        waddch(w, ' ');
        wattron(w, A_BOLD | COLOR_PAIR(INV_COLOR_PAIR));
        waddwstr(w, PIECE_TO_EMOJI[last_move_player][last_move_piece]);
        wattroff(w, A_BOLD | COLOR_PAIR(INV_COLOR_PAIR));
        waddch(w, ' ');
        waddch(w, file_ch(last_move_from));
        waddch(w, rank_ch(last_move_from));
        waddstr(w, " -> ");
        waddch(w, file_ch(last_move_to));
        waddch(w, rank_ch(last_move_to));
        waddch(w, '\n');
    }
}

void update_castling_win(WINDOW* w, const board_state_t& board) {
    auto castling = board_state_meta_get_castling_rights(board);

    werase(w);
    wmove(w, 0, 0);
    waddstr(w, " Castling:\n");

    waddch(w, ' ');
    wattron(w, A_BOLD | COLOR_PAIR(INV_COLOR_PAIR));
    waddwstr(w, PIECE_TO_EMOJI[PLAYER_WHITE][PIECE_KING]);
    waddwstr(w, PIECE_TO_EMOJI[PLAYER_WHITE][PIECE_ROOK]);
    wattroff(w, A_BOLD | COLOR_PAIR(INV_COLOR_PAIR));
    waddstr(w, " SHORT: ");
    waddstr(w, (castling_rights_white_short(castling) ? "YES" : "NO "));
    waddstr(w, " | LONG: ");
    waddstr(w, (castling_rights_white_long(castling) ? "YES" : "NO"));
    waddch(w, '\n');

    waddch(w, ' ');
    wattron(w, A_BOLD | COLOR_PAIR(INV_COLOR_PAIR));
    waddwstr(w, PIECE_TO_EMOJI[PLAYER_BLACK][PIECE_KING]);
    waddwstr(w, PIECE_TO_EMOJI[PLAYER_BLACK][PIECE_ROOK]);
    wattroff(w, A_BOLD | COLOR_PAIR(INV_COLOR_PAIR));
    waddstr(w, " SHORT: ");
    waddstr(w, (castling_rights_black_short(castling) ? "YES" : "NO "));
    waddstr(w, " | LONG: ");
    waddstr(w, (castling_rights_black_long(castling) ? "YES" : "NO"));
    waddch(w, '\n');
}

void update_game_status_win(WINDOW* w, const std::string& str) {
    werase(w);
    waddstr(w, str.c_str());
}

}  // namespace

layout_t init() {
    setlocale(LC_ALL, "");
    initscr();
    curs_set(0);

    start_color();
    init_pair(COLOR_PAIR_DESC[0][PLAYER_BLACK], COLOR_BLACK, 6);
    init_pair(COLOR_PAIR_DESC[0][PLAYER_WHITE], COLOR_BLACK, 6);
    init_pair(COLOR_PAIR_DESC[1][PLAYER_BLACK], COLOR_BLACK, 7);
    init_pair(COLOR_PAIR_DESC[1][PLAYER_WHITE], COLOR_BLACK, 7);
    init_pair(INFO_COLOR_PAIR, COLOR_WHITE, COLOR_BLACK);
    init_pair(INV_COLOR_PAIR, COLOR_BLACK, COLOR_WHITE);

    layout_t layout;
    init_board_rect(layout);
    init_game_status_rect(layout);
    init_last_move_rect(layout);
    init_castling_rect(layout);

    wrefresh(layout.board_rect.rank_desc_win);
    wrefresh(layout.board_rect.file_desc_win);
    wrefresh(layout.board_rect.board_border_win);
    wrefresh(layout.game_status_rect.game_status_border_win);
    wrefresh(layout.last_move_rect.last_move_border_win);
    wrefresh(layout.castling_rect.castling_border_win);
    return layout;
}

void update_board(const layout_t& layout, const board_state_t& board) {
    update_board_win(layout.board_rect.board_win, board);
    update_last_move_win(layout.last_move_rect.last_move_win, board);
    update_castling_win(layout.castling_rect.castling_win, board);
}

void update_game_status(const layout_t& layout, const std::string& str) {
    update_game_status_win(layout.game_status_rect.game_status_win, str);
}

void update(const layout_t& layout) {
    wrefresh(layout.board_rect.board_win);
    wrefresh(layout.game_status_rect.game_status_win);
    wrefresh(layout.last_move_rect.last_move_win);
    wrefresh(layout.castling_rect.castling_win);
}

void finish(layout_t& layout) {
    delwin(layout.board_rect.board_win);
    delwin(layout.board_rect.rank_desc_win);
    delwin(layout.board_rect.file_desc_win);
    delwin(layout.board_rect.board_border_win);
    delwin(layout.game_status_rect.game_status_win);
    delwin(layout.game_status_rect.game_status_border_win);
    delwin(layout.last_move_rect.last_move_win);
    delwin(layout.last_move_rect.last_move_border_win);
    delwin(layout.castling_rect.castling_win);
    delwin(layout.castling_rect.castling_border_win);
    endwin();
}

}  // namespace ncurses
}  // namespace chess

#endif  // CHESS_GUI_NCURSES_HPP_