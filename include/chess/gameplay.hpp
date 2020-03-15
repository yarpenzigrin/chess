/** chess_gameplay.hpp
 *
 * Chess engine gameplay header-only library.
 */
#ifndef CHESS_GAMEPLAY_HPP_
#define CHESS_GAMEPLAY_HPP_

#include "chess/core.hpp"

namespace chess
{

/** @defgroup gameplay-types Basic types in chess gameplay
 *  @{
 */

/** Type of game action */
enum class game_action_t { MOVE, FORFEIT };

/** Move request type
 *  This function is provided by chess client. Chess game will be calling this function in order
 *  to request current player for a move.
 */
using request_move_f = game_action_t(*)(board_state_t&);

/** Enumeration of possible game results */
enum class game_result_t {
    WHITE_WON_FORFEIT, WHITE_WON_CHECKMATE,
    BLACK_WON_FORFEIT, BLACK_WON_CHECKMATE,
    DRAW_STALEMATE, DRAW_INSUFFICIENT_MATERIAL, DRAW_REPETITION, DRAW_50_MOVE_RULE,
    ERROR
};

/*  @} */ // gameplay-types

/** @defgroup gameplay-api Basic types in chess gameplay
 *  @{
 */

/** Default null-logging logger
 *  If no logger type is provided by the client, this logger will be used in order to silence
 *  log messages.
 */
struct null_log_t {
    template <typename T>
    null_log_t& operator<<(const T&) { return *this; }
};

/** Chess game entry point
 *  Starts and conducts the game querying players for moves. First player queried for a move is a
 *  white player. In order to start from black player, arguments `white_move_fn` and
 *  `black_move_fn` could be switched.
 *
 *  @param log_t - Type of logger to use. By default null-logger is instantiated.
 *  @param memory - Memory that can be used by chess game to allocate move history and candidate
 *                  moves in order to determine whether checkmate occured. Should be enough to
 *                  fit at least 170 `board_state_t`'s (64 bit * 170 = 10 KiB).
 *  @param white_move_fn - Function to handle white player's next move. Function is passed a
 *                         reference to a mutable `board_state_t` representing current position,
 *                         and modification of this data is expected. Function returns type of game
 *                         action (move of forfeit).
 *  @param black_move_fn - as for `white_move_fn` for black player
 *  @param board - starting position on the chessboard
 *
 *  @return Returns `game_result_t` describing game outcome.
 */
template <typename log_t = null_log_t>
game_result_t play(void* memory, request_move_f white_move_fn, request_move_f black_move_fn,
    board_state_t board = START_BOARD);

/*  @} */ // gameplay-api

/** @defgroup gameplay-ringbuf Simple ring buffer implementation
 *  @{
 */

namespace detail
{

template <typename T, std::size_t N>
struct ring_buffer_s {
    static constexpr std::size_t max_size = N;

    T* storage;
    std::size_t size = 0;
    std::size_t start_idx = 0;
};

template <typename T, std::size_t N>
constexpr bool ring_buffer_rolling(const ring_buffer_s<T, N>& buffer) {
    return buffer.size == buffer.max_size;
}

template <typename T, std::size_t N, typename U>
void ring_buffer_add(ring_buffer_s<T, N>& buffer, U&& value) {
    if (ring_buffer_rolling(buffer)) {
        buffer.storage[buffer.start_idx++] = std::forward<U>(value);
        buffer.start_idx = buffer.start_idx % buffer.max_size;
    } else {
        buffer.storage[buffer.size++] = std::forward<U>(value);
    }
}

template <typename T, std::size_t N>
constexpr T* ring_buffer_begin(const ring_buffer_s<T, N>& buffer) {
    return buffer.size ? buffer.storage + buffer.start_idx : nullptr;
}

template <typename T, std::size_t N>
constexpr T* ring_buffer_end(const ring_buffer_s<T, N>& buffer) {
    return nullptr;
}

template <typename T, std::size_t N>
constexpr T* ring_buffer_next(const ring_buffer_s<T, N>& buffer, T* const iter) {
    std::size_t pos = iter - buffer.storage;
    if (ring_buffer_rolling(buffer)) {
        if (0 == buffer.start_idx) {
            return pos < buffer.size - 1 ? iter + 1 : nullptr;
        } else if (pos == buffer.size - 1) {
            return buffer.storage;
        } else {
            return (pos == buffer.start_idx - 1) ? nullptr : iter + 1;
        }
    } else {
        return pos < buffer.size - 1 ? iter + 1 : nullptr;
    }
}

#define RING_BUFFER_FOREACH(buffer, it)       \
    for (auto it = ring_buffer_begin(buffer); \
         it != ring_buffer_end(buffer);       \
         it = ring_buffer_next(buffer, it))

}  // detail

/*  @} */ // gameplay-ringbuf

/** @defgroup gameplay-private-impl Private implementation
 *  @{
 */
namespace
{

bool check_draw_by_insufficient_material(const board_state_t& board) {
    bool white_bishop_found = false;
    bool white_knight_found = false;
    bool black_bishop_found = false;
    bool black_knight_found = false;
    for (const auto field : board) {
        switch (field_get_piece(field)) {
            case PIECE_EMPTY:
            case PIECE_KING: break;
            case PIECE_BISHOP: {
                if (PLAYER_WHITE == field_get_player(field)) {
                    if (white_bishop_found or white_knight_found) {
                        return false;
                    } else {
                        white_bishop_found = true;
                    }
                } else {
                    if (black_bishop_found or black_knight_found) {
                        return false;
                    } else {
                        black_bishop_found = true;
                    }
                }
                break;
            }
            case PIECE_KNIGHT: {
                if (PLAYER_WHITE == field_get_player(field)) {
                    if (white_knight_found or white_bishop_found) {
                        return false;
                    } else {
                        white_knight_found = true;
                    }
                } else {
                    if (black_knight_found or black_bishop_found) {
                        return false;
                    } else {
                        black_knight_found = true;
                    }
                }
                break;
            }
            default: return false;
        }
    }
    return true;
}

constexpr std::size_t MOVE_HISTORY_SIZE = 50;
using move_history_t = detail::ring_buffer_s<board_state_t, MOVE_HISTORY_SIZE>;

bool check_draw_by_threefold_repetition(const board_state_t& board, move_history_t& history) {
    std::size_t same_position_count = 0u;
    RING_BUFFER_FOREACH(history, move_ptr) {
        if (compare_simple_position(*move_ptr, board) and ++same_position_count > 1 )
            return true;
    }
    ring_buffer_add(history, board);
    return false;
}

void update_insignificant_move_cnt(std::size_t& insignificant_move_cnt,
    const board_state_t& new_position, const board_state_t& previous_position) {
    last_move_t last_move = board_state_meta_get_last_move(new_position);
    if (PIECE_PAWN == last_move_get_piece(last_move)) {
        insignificant_move_cnt = 0;
    } else {
        player_t player = last_move_get_player(last_move);
        field_t target_field = last_move_get_to(last_move);
        if (PIECE_EMPTY != field_get_piece(previous_position[target_field]) and
            player != field_get_player(previous_position[target_field])) {
            insignificant_move_cnt = 0;
        }
    }
    ++insignificant_move_cnt;
}

}  // namespace

/*  @} */ // gameplay-private-impl

/** @defgroup gameplay-impl Implementation of public functions
 *  @{
 */

template <typename log_t = null_log_t>
game_result_t play(void* memory, request_move_f white_move_fn, request_move_f black_move_fn,
    board_state_t board) {
    log_t log;
    if (nullptr == memory or nullptr == white_move_fn or nullptr == black_move_fn) {
        log << "Game ended with error.";
        return game_result_t::ERROR;
    }

    board_state_t* move_storage = static_cast<board_state_t*>(memory);

    move_history_t move_history;
    move_history.storage = move_storage;

    board_state_t* candidate_moves_beg = move_storage + move_history.max_size;
    board_state_t* candidate_moves_end = candidate_moves_beg;
    board_state_t saved_board = board;
    game_action_t last_action = game_action_t::MOVE;
    std::size_t insignificant_move_cnt = 0;
    std::size_t move_cnt = 0;
    log << "Game started.\n";
    do {
        candidate_moves_end = fill_candidate_moves(candidate_moves_beg, board, PLAYER_WHITE);
        if (candidate_moves_end == candidate_moves_beg)
            return is_king_under_attack(board, PLAYER_WHITE)
                ? game_result_t::BLACK_WON_CHECKMATE
                : game_result_t::DRAW_STALEMATE;

        bool white_move_valid = false;
        ++move_cnt;
        do {
            log << move_cnt << ". White to move.\n";
            last_action = white_move_fn(board);
            if (game_action_t::FORFEIT == last_action)
                return game_result_t::BLACK_WON_FORFEIT;
            for (auto it = candidate_moves_beg; it != candidate_moves_end; ++it) {
                if (std::equal(board.begin(), board.end(), it->begin())) {
                    white_move_valid = true;
                    break;
                }
            }
            if (!white_move_valid) {
                log << "Illegal move from white rejected.\n";
                board = saved_board;
                // return game_result_t::ERROR;
            }
        } while (!white_move_valid);
        if (check_draw_by_insufficient_material(board))
            return game_result_t::DRAW_INSUFFICIENT_MATERIAL;
        if (check_draw_by_threefold_repetition(board, move_history))
            return game_result_t::DRAW_REPETITION;
        update_insignificant_move_cnt(insignificant_move_cnt, board, saved_board);
        if (insignificant_move_cnt >= 50)
            return game_result_t::DRAW_50_MOVE_RULE;

        saved_board = board;

        candidate_moves_end = fill_candidate_moves(candidate_moves_beg, board, PLAYER_BLACK);
        if (candidate_moves_end == candidate_moves_beg)
            return is_king_under_attack(board, PLAYER_BLACK)
                ? game_result_t::WHITE_WON_CHECKMATE
                : game_result_t::DRAW_STALEMATE;
        bool black_move_valid = false;
        ++move_cnt;
        do {
            log << move_cnt << ". Black to move.\n";
            last_action = black_move_fn(board);
            if (game_action_t::FORFEIT == last_action)
                return game_result_t::WHITE_WON_FORFEIT;
            for (auto it = candidate_moves_beg; it != candidate_moves_end; ++it) {
                if (std::equal(board.begin(), board.end(), it->begin())) {
                    black_move_valid = true;
                    break;
                }
            }
            if (!black_move_valid) {
                log << "Illegal move from black rejected.\n";
                board = saved_board;
                // return game_result_t::ERROR;
            }
        } while (!black_move_valid);

        if (check_draw_by_insufficient_material(board))
            return game_result_t::DRAW_INSUFFICIENT_MATERIAL;
        if (check_draw_by_threefold_repetition(board, move_history))
            return game_result_t::DRAW_REPETITION;
        update_insignificant_move_cnt(insignificant_move_cnt, board, saved_board);
        if (insignificant_move_cnt >= 50)
            return game_result_t::DRAW_50_MOVE_RULE;
        
        saved_board = board;
    } while (move_cnt < 500);

    log << "Game ended with weird error.\n";
    return game_result_t::ERROR;
}

/*  @} */ // gameplay-impl

}  // namespace chess

#endif  // CHESS_GAMEPLAY_HPP_
