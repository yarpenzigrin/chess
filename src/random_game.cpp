#include <random>
#include <memory>
#include <iostream>
#include <thread>
#include "chess_gameplay.hpp"
#include "chess_tty_gui.hpp"

using namespace chess;
using namespace std::chrono_literals;

std::unique_ptr<board_state_t[]> prepare_game_memory(const std::size_t size = 254) {
    return std::make_unique<board_state_t[]>(size);
}

board_state_t* player_cm_storage;
board_state_t* evaluation_storage;
std::size_t move_cnt = 0;
auto timeout = 50ms;

struct stdout_logger_t {
    template <typename T>
    stdout_logger_t& operator<<(const T& stuff) {
        std::cout << stuff;
        return *this;
    }
};

int gen_random_num(int max_num, int min_num = 0) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    std::uniform_int_distribution<> dis(min_num, max_num);
    return dis(gen);
}

std::size_t intpow(const std::size_t num, const std::size_t exp) {
    return exp ? num * pow(num, exp - 1) : 1;
}

using stream_t = stdout_logger_t;
// using stream_t = chess::null_log_t;
auto stream = stream_t{};

using score_t = int;
static constexpr score_t MAX_SCORE = 100000;
static constexpr score_t MIN_SCORE = -100000;

score_t score_position(const board_state_t& board) {
    constexpr score_t PAWN_SCORE = 1;
    constexpr score_t KNIGHT_SCORE = 3;
    constexpr score_t BISHOP_SCORE = 3;
    constexpr score_t ROOK_SCORE = 5;
    constexpr score_t QUEEN_SCORE = 9;

    score_t total_score = 0;
    for (const auto field : board) {
        score_t sign = (PLAYER_WHITE == field_get_player(field) ? 1 : -1 );
        switch (field_get_piece(field)) {
            case PIECE_PAWN: total_score += PAWN_SCORE * sign; break;
            case PIECE_KNIGHT: total_score += KNIGHT_SCORE * sign; break;
            case PIECE_BISHOP: total_score += BISHOP_SCORE * sign; break;
            case PIECE_ROOK: total_score += ROOK_SCORE * sign; break;
            case PIECE_QUEEN: total_score += QUEEN_SCORE * sign; break;
        }
    }
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

evaluation_s evaluate_position_min_AB(
    const board_state_t* moves_beg, board_state_t* moves_end, const player_t player,
    const int depth, const score_t beta);

evaluation_s evaluate_position_max_AB(
    const board_state_t* moves_beg, board_state_t* moves_end, const player_t player,
    const int depth, const score_t alpha) {
    auto score_f =
        [depth, player](const board_state_t& board, board_state_t* moves_beg, const score_t beta) {
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

            return evaluate_position_min_AB(
                moves_beg, moves_end, opponent(player), depth - 1, beta).score;
        } else {
            return score_position(board);
        }
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
        if (score > best_score) {
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
            auto moves_end = fill_candidate_moves(moves_beg, board, opponent(player));
            if (moves_beg == moves_end) {
                if (is_king_under_attack(board, player))
                    return 100;
                else if (is_king_under_attack(board, opponent(player)))
                    return -100;
                else
                    return 0;
            }

            return evaluate_position_max_AB(
                moves_beg, moves_end, opponent(player), depth - 1, alpha).score;
        } else {
            return score_position(board);
        }
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
        if (score < best_score) {
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
    board = minimax(board, PLAYER_WHITE, DEPTH);

    stream << "After whites's move " << ++move_cnt << ":\n";
    gui::print_board(stream, board);
    return game_action_t::MOVE;
}

template <std::size_t DEPTH>
game_action_t black_minimax(board_state_t& board) {
    board = minimax(board, PLAYER_BLACK, DEPTH);

    stream << "After black's move " << ++move_cnt << ":\n";
    gui::print_board(stream, board);
    return game_action_t::MOVE;
}

game_action_t white_random(board_state_t& board) {
    auto cm_moves_beg = player_cm_storage;
    auto cm_moves_end = fill_candidate_moves(cm_moves_beg, board, PLAYER_WHITE);
    if (cm_moves_beg == cm_moves_end)
        return game_action_t::FORFEIT;

    auto cm_moves_cnt = cm_moves_end - cm_moves_beg;
    auto chosen_number = gen_random_num(cm_moves_cnt - 1);

    stream << "After whites's move " << ++move_cnt << " (chosen: " << chosen_number << " / "
        << cm_moves_cnt << " ):\n";
    board = cm_moves_beg[chosen_number];
    gui::print_board(stream, board);
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

    stream << "After black's move " << ++move_cnt << " (chosen: " << chosen_number << " / "
        << cm_moves_cnt << " ):\n";
    board = cm_moves_beg[chosen_number];
    gui::print_board(stream, board);
    std::this_thread::sleep_for(timeout);
    return game_action_t::MOVE;
}

game_action_t white_human(board_state_t& board) {
    auto cm_moves_beg = player_cm_storage;
    auto cm_moves_end = fill_candidate_moves(cm_moves_beg, board, PLAYER_WHITE);
    if (cm_moves_beg == cm_moves_end)
        return game_action_t::FORFEIT;

    stream << "Possible moves:\n";
    int idx = 0;
    for (auto it = cm_moves_beg; it != cm_moves_end; ++it) {
        stream << '[' << idx++ << "] ";
        last_move_t move = board_state_meta_get_last_move(*it);
        player_t move_player = last_move_get_player(move);
        piece_t move_piece = last_move_get_piece(move);
        field_t move_from = last_move_get_from(move);
        field_t move_to = last_move_get_to(move);
        gui::print_move(stream, { move_player, move_piece, move_from, move_to });
        stream << '\n';
    }
    stream << "[-1] FORFEIT\n";
    int choice = 0;
    stream << "\nChoice [-1 .. " << idx - 1 << "]: ";
    std::cin >> choice;
    while (idx <= choice or choice < -1) {
        stream << "\nWrong choice.";
        stream << "\nChoice [-1 .. " << idx - 1 << "]: ";
        std::cin >> choice;
    }
    if (choice == -1)
    {
        return game_action_t::FORFEIT;
    }
    board = cm_moves_beg[choice];
    stream << "\n";
    gui::print_board(stream, board);
    return game_action_t::MOVE;
}

void announce_result(const game_result_t result) {
    stream << "Game ended with result: ";
    switch (result) {
        case game_result_t::WHITE_WON_FORFEIT: {
            stream << "WHITE_WON_FORFEIT";
            break;
        }
        case game_result_t::WHITE_WON_CHECKMATE: {
            stream << "WHITE_WON_CHECKMATE";
            break;
        }
        case game_result_t::BLACK_WON_FORFEIT: {
            stream << "BLACK_WON_FORFEIT";
            break;
        }
        case game_result_t::BLACK_WON_CHECKMATE: {
            stream << "BLACK_WON_CHECKMATE";
            break;
        }
        case game_result_t::DRAW_STALEMATE: {
            stream << "DRAW_STALEMATE";
            break;
        }
        case game_result_t::DRAW_INSUFFICIENT_MATERIAL: {
            stream << "DRAW_INSUFFICIENT_MATERIAL";
            break;
        }
        case game_result_t::DRAW_REPETITION: {
            stream << "DRAW_REPETITION";
            break;
        }
        case game_result_t::DRAW_50_MOVE_RULE: {
            stream << "DRAW_50_MOVE_RULE";
            break;
        }
        case game_result_t::ERROR: {
            stream << "ERROR";
            break;
        }
    }
    stream << '\n';
}

int main() {
    auto game_memory = prepare_game_memory();
    auto player_memory = prepare_game_memory();
    auto minimax_memory = prepare_game_memory(intpow(64, 4));
    player_cm_storage = player_memory.get();
    minimax_storage = minimax_memory.get();

    auto result = play<stream_t>(game_memory.get(), white_human, black_minimax<5>);
    announce_result(result);
}