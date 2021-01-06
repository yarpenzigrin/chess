#include <random>
#include <memory>
#include <iostream>
#include <thread>
#include <unordered_map>
#include "chess/gameplay.hpp"
#include "chess/gui_tty.hpp"
#include "chess/gui_ncurses.hpp"

using namespace std::chrono_literals;

namespace chess {
namespace example {

std::unique_ptr<board_state_t[]> prepare_game_memory(const std::size_t size = 254) {
    return std::make_unique<board_state_t[]>(size);
}

board_state_t* player_cm_storage;
board_state_t* evaluation_storage;
std::size_t move_cnt = 0;
auto timeout = 50ms;
chess::ncurses::layout_t layout;

// struct game_status_t {
//     template <typename T>
//     game_status_t& operator<<(T&& stuff) {
//         std::stringstream ss;
//         ss << std::forward<T>(stuff);
//         auto s = ss.str();
//         if (std::end(s) == std::find(std::begin(s), std::end(s), '\n')) {
//             chess::gui::frame_stream(&layout.frames[3]) << s;
//         } else {
//             chess::gui::frame_stream(&layout.frames[3]) << s;
//             // chess::gui::reset_frame_after_display(layout.frames[3]);
//         }
//         chess::gui::reset_frame_after_display(layout.frames[3]);
//         return *this;
//     }

//     game_status_t& operator<<(const chess::gui::layout_t::frame_t::color_code color) {
//         chess::gui::frame_stream(&layout.frames[3]) << color;
//         return *this;
//     }
// };

// game_status_t game_status;

int gen_random_num(int max_num, int min_num = 0) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min_num, max_num);
    return dis(gen);
}

std::size_t intpow(const std::size_t num, const std::size_t exp) {
    return exp ? num * pow(num, exp - 1) : 1;
}

using score_t = int;
static constexpr score_t MAX_SCORE = 100000;
static constexpr score_t MIN_SCORE = -100000;

std::random_device score_rd;
std::mt19937 score_gen(score_rd());
std::uniform_int_distribution<> score_distr(-2, 2);

score_t score_position(const board_state_t& board) {
    constexpr score_t KNIGHT_SCORE = 3;
    constexpr score_t BISHOP_SCORE = 3;
    constexpr score_t ROOK_SCORE = 5;
    constexpr score_t QUEEN_SCORE = 9;

    auto score_pawn = [](const auto field) -> double {
        constexpr score_t PAWN_SCORE = 1;
        score_t sign = (PLAYER_WHITE == field_get_player(field) ? 1 : -1 );
        return sign * PAWN_SCORE * (field_get_player(field) == PLAYER_WHITE
            ? static_cast<float>(field_rank(field)) / static_cast<float>(rank_t::_8)
            : static_cast<float>(
                static_cast<int>(rank_t::_8) - static_cast<int>(field_rank(field)))
                / static_cast<float>(rank_t::_8));
    };

    float total_score = 0;
    for (uint8_t field_idx = static_cast<uint8_t>(field_t::BEGIN);
         field_idx < static_cast<uint8_t>(field_t::END);
         ++field_idx) {
        const auto field = board[field_idx];
        score_t sign = (PLAYER_WHITE == field_get_player(field) ? 1 : -1 );
        switch (field_get_piece(field)) {
            case PIECE_PAWN: total_score += score_pawn(static_cast<field_t>(field_idx)); break;
            case PIECE_KNIGHT: total_score += KNIGHT_SCORE * sign; break;
            case PIECE_BISHOP: total_score += BISHOP_SCORE * sign; break;
            case PIECE_ROOK: total_score += ROOK_SCORE * sign; break;
            case PIECE_QUEEN: total_score += QUEEN_SCORE * sign; break;
        }
    }
    total_score -= is_king_under_attack(board, PLAYER_WHITE) * 10;
    total_score += is_king_under_attack(board, PLAYER_BLACK) * 10;
    total_score += score_distr(score_gen);
    return total_score;
}

struct evaluation_s {
    std::size_t index;
    score_t score;
};

auto minimax_stream = null_log_t{};
// auto minimax_stream = stdout_logger_t{};

void print_tab(const int depth) {
    for (int i = 0; i < 4 - depth; ++i) {
        // minimax_stream << "  ";
    }
}

bool compare_score(const score_t score, const score_t best_score) {
    return score > best_score;
}

struct cache_entry_t {
    int depth = 0;
    score_t score = 0;
    score_t alpha = MIN_SCORE;
    score_t beta = MAX_SCORE;
};

std::unordered_map<board_state_t, cache_entry_t> cache;
int cache_hits = 0;

struct cache_entry_2_t {
    std::vector<board_state_t> candidates;
};

std::unordered_map<board_state_t, cache_entry_2_t> cache_2;

board_state_t* gen_moves(
    board_state_t* moves_beg, const board_state_t& board, const player_t player) {
    if (cache_2.size() > 20'000'000) {
        cache_2.clear();
    }
    auto& entry = cache_2[board];
    if (entry.candidates.size()) {
        ++cache_hits;
        return std::copy(std::begin(entry.candidates), std::end(entry.candidates), moves_beg);
    }
    auto moves_end = fill_candidate_moves(moves_beg, board, opponent(player));
    entry.candidates.reserve(moves_end - moves_beg);
    std::copy(moves_beg, moves_end, std::back_inserter(entry.candidates));
    return moves_end;
}

evaluation_s evaluate_position_min_AB(
    const board_state_t* moves_beg, board_state_t* moves_end, const player_t player,
    const int depth, const score_t beta);

evaluation_s evaluate_position_max_AB(
    const board_state_t* moves_beg, board_state_t* moves_end, const player_t player,
    const int depth, const score_t alpha) {
    auto score_f =
        [depth, player](const board_state_t& board, board_state_t* moves_beg, const score_t beta) {
        if (depth > 0) {
            auto moves_end = gen_moves(moves_beg, board, opponent(player));

            if (moves_beg == moves_end) {
                if (is_king_under_attack(board, player))
                    return -1000;
                else if (is_king_under_attack(board, opponent(player)))
                    return 1000;
                else
                    return 0;
            }

            return evaluate_position_min_AB(
                moves_beg, moves_end, opponent(player), depth - 1, beta).score;
        } else {
            return score_position(board);
        }
    };
    auto cached_score_f =
        [&](const board_state_t& board, board_state_t* moves_beg, const score_t beta) {
        auto& entry = cache[board];
        if (entry.depth >= depth and entry.beta < beta) {
            ++cache_hits;
            return entry.score;
        }
        entry.depth = depth;
        entry.alpha = MIN_SCORE;
        entry.beta = beta;
        return entry.score = score_f(board, moves_beg, beta);
    };

    std::size_t moves_cnt = moves_end - moves_beg;
    std::size_t best_move_idx = 0;
    print_tab(depth);
    // minimax_stream << "MAX: " << moves_cnt << " candidate moves at depth " << depth << "\n";
    score_t best_score = MIN_SCORE;
    for (std::size_t idx = 0; idx < moves_cnt; ++idx) {
        auto score = score_f(moves_beg[idx], moves_end, best_score);
        print_tab(depth);
        // minimax_stream << "MAX: move " << idx << " -> score " << score << '\n';
        if (depth and score >= alpha) {
            print_tab(depth);
            minimax_stream << "MAX: pruning on alpha = " << alpha << " at depth = " << depth << '\n';
            return { idx, score };
        }
        if (compare_score(score, best_score)) {
            best_score = score;
            best_move_idx = idx;
        }
    }
    print_tab(depth);
    // minimax_stream << "MAX: chose move " << best_move_idx << " with score " << best_score << '\n';
    return { best_move_idx, best_score };
}

evaluation_s evaluate_position_min_AB(
    const board_state_t* moves_beg, board_state_t* moves_end, const player_t player,
    const int depth, const score_t beta) {
    auto score_f =
        [depth, player](const board_state_t& board, board_state_t* moves_beg, const score_t alpha) {
        if (depth > 0) {
            auto moves_end = gen_moves(moves_beg, board, opponent(player));
            if (moves_beg == moves_end) {
                if (is_king_under_attack(board, player))
                    return 1000;
                else if (is_king_under_attack(board, opponent(player)))
                    return -1000;
                else
                    return 0;
            }

            return evaluate_position_max_AB(
                moves_beg, moves_end, opponent(player), depth - 1, alpha).score;
        } else {
            return score_position(board);
        }
    };
    auto cached_score_f =
        [&](const board_state_t& board, board_state_t* moves_beg, const score_t alpha) {
        auto& entry = cache[board];
        if (entry.depth >= depth and entry.alpha > alpha) {
            ++cache_hits;
            return entry.score;
        }
        entry.depth = depth;
        entry.alpha = alpha;
        entry.beta = MAX_SCORE;
        return entry.score = score_f(board, moves_beg, alpha);
    };

    std::size_t moves_cnt = moves_end - moves_beg;
    std::size_t best_move_idx = 0;
    print_tab(depth);
    // minimax_stream << "MIN: " << moves_cnt << " candidate moves at depth " << depth << "\n";
    score_t best_score = MAX_SCORE;
    for (std::size_t idx = 0; idx < moves_cnt; ++idx) {
        auto score = score_f(moves_beg[idx], moves_end, best_score);
        print_tab(depth);
        // minimax_stream << "MIN: move " << idx << " -> score " << score << '\n';
        if (depth and score <= beta) {
            print_tab(depth);
            minimax_stream << "MIN: pruning on beta = " << beta << " at depth = " << depth <<  '\n';
            return { idx, score };
        }
        if (compare_score(best_score, score)) {
            best_score = score;
            best_move_idx = idx;
        }
    }
    print_tab(depth);
    // minimax_stream << "MIN: chose move " << best_move_idx << " with score " << best_score << '\n';
    return { best_move_idx, best_score };
}

evaluation_s evaluate_position_min(
    const board_state_t* moves_beg, board_state_t* moves_end, const player_t player,
    const int depth);

evaluation_s evaluate_position_max(
    const board_state_t* moves_beg, board_state_t* moves_end, const player_t player,
    const int depth) {
    auto score_f = [depth, player](const board_state_t& board, board_state_t* moves_beg) {
        if (depth > 0) {
            auto moves_end = fill_candidate_moves(moves_beg, board, opponent(player));

            if (moves_beg == moves_end) {
                if (is_king_under_attack(board, player))
                    return -100;
                else if (is_king_under_attack(board, opponent(player)))
                    return 100;
                else
                    return 0;
            }

            return evaluate_position_min(moves_beg, moves_end, opponent(player), depth - 1).score;
        } else {
            return score_position(board);
        }
    };

    std::size_t moves_cnt = moves_end - moves_beg;
    std::size_t best_move_idx = 0;
    print_tab(depth);
    printf("MAX: %ld candidate moves at depth %ld\n", moves_cnt, depth);
    score_t best_score = score_f(moves_beg[0], moves_end);
    for (std::size_t idx = 1; idx < moves_cnt; ++idx) {
        auto score = score_f(moves_beg[idx], moves_end);
        print_tab(depth);
        printf("MAX: move %ld -> score %d\n", idx, score);
        if (score > best_score) {
            best_score = score;
            best_move_idx = idx;
        }
    }
    print_tab(depth);
    printf("MAX: chose move %ld with score %d\n", best_move_idx, best_score);
    return { best_move_idx, best_score };
}

evaluation_s evaluate_position_min(
    const board_state_t* moves_beg, board_state_t* moves_end, const player_t player,
    const int depth) {
    auto score_f = [depth, player](const board_state_t& board, board_state_t* moves_beg) {
        if (depth > 0) {
            auto moves_end = fill_candidate_moves(moves_beg, board, opponent(player));
            if (moves_beg == moves_end) {
                if (is_king_under_attack(board, player))
                    return 100;
                else if (is_king_under_attack(board, opponent(player)))
                    return -100;
                else
                    return 0;
            }

            return evaluate_position_max(moves_beg, moves_end, opponent(player), depth - 1).score;
        } else {
            return score_position(board);
        }
    };

    std::size_t moves_cnt = moves_end - moves_beg;
    std::size_t best_move_idx = 0;
    print_tab(depth);
    printf("MIN: %ld candidate moves at depth %ld\n", moves_cnt, depth);
    score_t best_score = score_f(moves_beg[0], moves_end);
    for (std::size_t idx = 1; idx < moves_cnt; ++idx) {
        auto score = score_f(moves_beg[idx], moves_end);
        print_tab(depth);
        printf("MIN: move %ld -> score %d\n", idx, score);
        if (score < best_score) {
            best_score = score;
            best_move_idx = idx;
        }
    }
    print_tab(depth);
    printf("MIN: chose move %ld with score %d\n", best_move_idx, best_score);
    return { best_move_idx, best_score };
}

board_state_t* minimax_storage = nullptr;

board_state_t minimax(
    const board_state_t& board, const player_t player, const int depth) {
    cache_2.clear();
    cache_hits = 0;
    auto moves_beg = minimax_storage;
    auto moves_end = fill_candidate_moves(moves_beg, board, player);

    if (PLAYER_WHITE == player) {
        auto idx = evaluate_position_max_AB(moves_beg, moves_end, player, depth, MAX_SCORE).index;
        return moves_beg[idx];
    } else {
        auto idx = evaluate_position_min_AB(moves_beg, moves_end, player, depth, MIN_SCORE).index;
        return moves_beg[idx];
    }
}

template <std::size_t DEPTH>
game_action_t white_minimax(board_state_t& board) {
    // game_status << "Cache size: " << cache_2.size() << " | hits: " << cache_hits;
    chess::ncurses::update_board(layout, board);
    chess::ncurses::update(layout);

    board = minimax(board, PLAYER_WHITE, DEPTH);
    std::this_thread::sleep_for(500ms);

    return game_action_t::MOVE;
}

template <std::size_t DEPTH>
game_action_t black_minimax(board_state_t& board) {
    // game_status << "Cache size: " << cache_2.size() << " | hits: " << cache_hits;
    chess::ncurses::update_board(layout, board);
    chess::ncurses::update(layout);

    board = minimax(board, PLAYER_BLACK, DEPTH);
    std::this_thread::sleep_for(500ms);
    
    return game_action_t::MOVE;
}

game_action_t white_random(board_state_t& board) {
    auto cm_moves_beg = player_cm_storage;
    auto cm_moves_end = fill_candidate_moves(cm_moves_beg, board, PLAYER_WHITE);
    if (cm_moves_beg == cm_moves_end)
        return game_action_t::FORFEIT;

    auto cm_moves_cnt = cm_moves_end - cm_moves_beg;
    auto chosen_number = gen_random_num(cm_moves_cnt - 1);

    board = cm_moves_beg[chosen_number];
    chess::ncurses::update_board(layout, board);
    chess::ncurses::update(layout);
    std::this_thread::sleep_for(timeout);
    return game_action_t::MOVE;
}

game_action_t black_random(board_state_t& board) {
    auto cm_moves_beg = player_cm_storage;
    auto cm_moves_end = fill_candidate_moves(cm_moves_beg, board, PLAYER_BLACK);
    if (cm_moves_beg == cm_moves_end)
        return game_action_t::FORFEIT;

    auto cm_moves_cnt = cm_moves_end - cm_moves_beg;
    auto chosen_number = gen_random_num(cm_moves_cnt - 1);

    board = cm_moves_beg[chosen_number];
    chess::ncurses::update_board(layout, board);
    chess::ncurses::update(layout);
    std::this_thread::sleep_for(timeout);
    return game_action_t::MOVE;
}

// game_action_t white_human(board_state_t& board) {
//     auto cm_moves_beg = player_cm_storage;
//     auto cm_moves_end = fill_candidate_moves(cm_moves_beg, board, PLAYER_WHITE);
//     if (cm_moves_beg == cm_moves_end)
//         return game_action_t::FORFEIT;

//     chess::gui::reset_frame(layout.frames[4]);
//     auto choice_stream = chess::gui::frame_stream(&layout.frames[4]);
//     choice_stream << "Possible moves:\n";
//     auto print_choice = [&](const auto idx)
//     {
//         choice_stream << '[' << idx << "] ";
//         if (idx < 10) {
//             choice_stream << ' ';
//         }
//         last_move_t move = board_state_meta_get_last_move(cm_moves_beg[idx]);
//         player_t move_player = last_move_get_player(move);
//         piece_t move_piece = last_move_get_piece(move);
//         field_t move_from = last_move_get_from(move);
//         field_t move_to = last_move_get_to(move);
//         chess::gui::print_move(choice_stream, { move_player, move_piece, move_from, move_to });
//     };
//     const int moves_cnt = cm_moves_end - cm_moves_beg;
//     const char* separator = " | ";
//     const int COLUMNS_CNT = 3;
//     const int rows_cnt = (moves_cnt - 1) / COLUMNS_CNT + 1;

//     for (int row = 0; row != rows_cnt; ++row)
//     {
//         for (int col = 0; col != COLUMNS_CNT; ++col)
//         {
//             auto idx = row + col * rows_cnt;
//             if (idx < moves_cnt)
//             {
//                 print_choice(idx);
//                 if (col < COLUMNS_CNT - 1) {
//                     choice_stream << separator;
//                 }
//             }
//         }
//         choice_stream << '\n';
//     }

//     choice_stream << "[-1] FORFEIT\n";
//     chess::gui::print_board(layout, board);
//     chess::gui::display(layout);

//     int choice = 0;
//     std::cout << "> Choice [-1 .. " << moves_cnt - 1 << "]: ";
//     std::cin >> choice;
//     while (moves_cnt <= choice or choice < -1) {
//         std::cout << "\nWrong choice.\n> Choice [-1 .. " << moves_cnt - 1 << "]: ";
//         std::cin >> choice;
//     }
//     if (choice == -1)
//     {
//         return game_action_t::FORFEIT;
//     }
//     board = cm_moves_beg[choice];
//     std::cout << "\n";
//     chess::gui::print_board(layout, board);
//     chess::gui::display(layout);
//     return game_action_t::MOVE;
// }

// void announce_result(const game_result_t result) {
//     game_status << "Game ended with result: ";
//     switch (result) {
//         case game_result_t::WHITE_WON_FORFEIT: {
//             game_status << "WHITE_WON_FORFEIT\n";
//             break;
//         }
//         case game_result_t::WHITE_WON_CHECKMATE: {
//             game_status << "WHITE_WON_CHECKMATE\n";
//             break;
//         }
//         case game_result_t::BLACK_WON_FORFEIT: {
//             game_status << "BLACK_WON_FORFEIT\n";
//             break;
//         }
//         case game_result_t::BLACK_WON_CHECKMATE: {
//             game_status << "BLACK_WON_CHECKMATE\n";
//             break;
//         }
//         case game_result_t::DRAW_STALEMATE: {
//             game_status << "DRAW_STALEMATE\n";
//             break;
//         }
//         case game_result_t::DRAW_INSUFFICIENT_MATERIAL: {
//             game_status << "DRAW_INSUFFICIENT_MATERIAL\n";
//             break;
//         }
//         case game_result_t::DRAW_REPETITION: {
//             game_status << "DRAW_REPETITION\n";
//             break;
//         }
//         case game_result_t::DRAW_50_MOVE_RULE: {
//             game_status << "DRAW_50_MOVE_RULE\n";
//             break;
//         }
//         case game_result_t::ERROR: {
//             game_status << "ERROR\n";
//             break;
//         }
//     }
// }

}  // namespace example
}  // namespace chess

int main() {
    // chess::gui::display(chess::example::layout);
    // std::this_thread::sleep_for(500ms);

    // auto game_memory = chess::example::prepare_game_memory();
    // auto player_memory = chess::example::prepare_game_memory();
    // auto minimax_memory = chess::example::prepare_game_memory(chess::example::intpow(64, 4));
    // chess::example::player_cm_storage = player_memory.get();
    // chess::example::minimax_storage = minimax_memory.get();

    // auto board = chess::START_BOARD;
    // auto result = chess::play(
    //     game_memory.get(),
    //     chess::example::white_minimax<5>,
    //     chess::example::black_minimax<6>,
    //     board,
    //     chess::example::game_status
    // );
    // chess::example::announce_result(result);
    // chess::gui::print_board(chess::example::layout, board);
    // chess::gui::display(chess::example::layout);

    chess::example::layout = chess::ncurses::init();
    auto board = chess::START_BOARD;

    auto game_memory = chess::example::prepare_game_memory();
    auto player_memory = chess::example::prepare_game_memory();
    auto minimax_memory = chess::example::prepare_game_memory(chess::example::intpow(64, 4));
    chess::example::player_cm_storage = player_memory.get();
    chess::example::minimax_storage = minimax_memory.get();

    chess::ncurses::update_board(chess::example::layout, board);
    chess::ncurses::update(chess::example::layout);
    std::this_thread::sleep_for(500ms);

    auto result = chess::play(
        game_memory.get(),
        chess::example::white_minimax<4>,
        chess::example::black_minimax<4>,
        board
    );

    chess::ncurses::update_board(chess::example::layout, board);
    chess::ncurses::update(chess::example::layout);

    char c;
    std::cin >> c;

    chess::ncurses::finish(chess::example::layout);
}