#ifndef CHESS_GUI_NCURSES_HPP_
#define CHESS_GUI_NCURSES_HPP_

#include <ncurses.h>

namespace chess {
namespace ncurses {

namespace {

const wchar_t* PIECE_TO_EMOJI[2][8] = {
    { L" ", L"♟", L"♞", L"♝", L"♜", L"♛", L"♚", L"!" },
    { L" ", L"♙", L"♘", L"♗", L"♖", L"♕", L"♔", L"!" }
};

constexpr int COLOR_PAIR_DESC[2][2] = { { 1, 2 }, { 3, 4 } };
constexpr int INFO_COLOR_PAIR = 5;

struct preset_1 {
    static constexpr int SQUARE_WIDTH = 5;
    static constexpr int SQUARE_HEIGHT = 3;
    static constexpr bool CHESSBOARD_BORDER = true;
    static constexpr int CHESSBOARD_OFFSET_X = 1;
    static constexpr int CHESSBOARD_OFFSET_Y = 1;
    static constexpr int RANK_DESC_WIDTH = 3;
    static constexpr int FILE_DESC_HEIGHT = 2;
};

struct preset_2 {
    static constexpr int SQUARE_WIDTH = 3;
    static constexpr int SQUARE_HEIGHT = 1;
    static constexpr bool CHESSBOARD_BORDER = true;
    static constexpr int CHESSBOARD_OFFSET_X = 1;
    static constexpr int CHESSBOARD_OFFSET_Y = 1;
    static constexpr int RANK_DESC_WIDTH = 1;
    static constexpr int FILE_DESC_HEIGHT = 1;
};

using DEFAULTS = preset_2;

constexpr int SQUARE_WIDTH = DEFAULTS::SQUARE_WIDTH;
constexpr int SQUARE_HEIGHT = DEFAULTS::SQUARE_HEIGHT;
constexpr bool CHESSBOARD_BORDER = DEFAULTS::CHESSBOARD_BORDER;
constexpr int CHESSBOARD_OFFSET_X = DEFAULTS::CHESSBOARD_OFFSET_X;
constexpr int CHESSBOARD_OFFSET_Y = DEFAULTS::CHESSBOARD_OFFSET_Y;
constexpr int RANK_DESC_WIDTH = DEFAULTS::RANK_DESC_WIDTH;
constexpr int FILE_DESC_HEIGHT = DEFAULTS::FILE_DESC_HEIGHT;

}  // namespace

struct board_rect_t {
    WINDOW* board_win = nullptr;
    WINDOW* board_border_win = nullptr;
    WINDOW* rank_desc_win = nullptr;
    WINDOW* file_desc_win = nullptr;
};

struct layout_t {
    board_rect_t board_rect;
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
    init_pair(5, COLOR_WHITE, COLOR_BLACK);

    layout_t layout;
    init_board_rect(layout);

    wrefresh(layout.board_rect.rank_desc_win);
    wrefresh(layout.board_rect.file_desc_win);
    wrefresh(layout.board_rect.board_border_win);
    return layout;
}

void update_board(const layout_t& layout, const board_state_t& board) {
    wrefresh(layout.board_rect.board_win);
    update_board_win(layout.board_rect.board_win, board);
    wrefresh(layout.board_rect.board_win);
}

void update(const layout_t& layout) {
    // wrefresh(layout.board_rect.board_win);
}

void finish(layout_t& layout) {
    delwin(layout.board_rect.board_win);
    delwin(layout.board_rect.rank_desc_win);
    delwin(layout.board_rect.file_desc_win);
    delwin(layout.board_rect.board_border_win);
    endwin();
}

}  // namespace ncurses
}  // namespace chess

#endif  // CHESS_GUI_NCURSES_HPP_