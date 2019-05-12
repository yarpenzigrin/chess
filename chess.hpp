#ifndef CHESS_HPP_
#define CHESS_HPP_

#include <array>
#include <cstdint>

namespace chess
{

namespace
{

constexpr std::size_t MASK(std::size_t pos, std::size_t size = 0b1) {
    return (size << pos);
}

template <typename T>
constexpr T SET_BIT(T field, std::size_t pos) {
    return field | MASK(pos);
}

template <typename T>
constexpr T CLEAR_BIT(T field, std::size_t pos) {
    return field & ~MASK(pos);
}

template <typename T>
constexpr T SET_VALUE(T field, std::size_t value, std::size_t pos, std::size_t mask) {
    return (field & ~mask) + ((value << pos) & mask);
}

template <typename T>
constexpr T GET_VALUE(T field, std::size_t pos, std::size_t mask) {
    return ((field >> pos) & mask);
}

}  // namespace

using player_t = uint8_t;

constexpr player_t PLAYER_WHITE =    1;
constexpr player_t PLAYER_BLACK =    0;

using piece_t = uint8_t;

constexpr piece_t PIECE_EMPTY =     0b000;
constexpr piece_t PIECE_PAWN =      0b001;
constexpr piece_t PIECE_KNIGHT =    0b010;
constexpr piece_t PIECE_BISHOP =    0b011;
constexpr piece_t PIECE_ROOK =      0b100;
constexpr piece_t PIECE_QUEEN =     0b101;
constexpr piece_t PIECE_KING =      0b110;
constexpr piece_t PIECE_INVALID =   0b111;

using field_meta_bits_t = uint8_t;

using field_state_t = uint8_t;

constexpr std::size_t FIELD_PLAYER_POS = 0;
constexpr std::size_t FIELD_PLAYER_SIZE = 1;
constexpr std::size_t FIELD_PLAYER_WIDTH = 0b1;
constexpr std::size_t FIELD_PLAYER_MASK = MASK(FIELD_PLAYER_POS);
constexpr field_state_t FIELD_SET_PLAYER(field_state_t field, player_t player) {
    return SET_VALUE(field, player, FIELD_PLAYER_POS, FIELD_PLAYER_MASK);
}
constexpr player_t FIELD_GET_PLAYER(field_state_t field) {
    return (field & FIELD_PLAYER_MASK) >> FIELD_PLAYER_POS;
}

constexpr std::size_t FIELD_PIECE_POS = 1;
constexpr std::size_t FIELD_PIECE_SIZE = 3;
constexpr std::size_t FIELD_PIECE_WIDTH = 0b111;
constexpr std::size_t FIELD_PIECE_MASK = MASK(FIELD_PIECE_POS, FIELD_PIECE_WIDTH);
constexpr field_state_t FIELD_SET_PIECE(field_state_t field, piece_t piece) {
    return SET_VALUE(field, piece, FIELD_PIECE_POS, FIELD_PIECE_MASK);
}
constexpr piece_t FIELD_GET_PIECE(field_state_t field) {
    return (field & FIELD_PIECE_MASK) >> FIELD_PIECE_POS;
}

constexpr std::size_t FIELD_META_BITS_POS = 6;
constexpr std::size_t FIELD_META_BITS_SIZE = 2;
constexpr std::size_t FIELD_META_BITS_WIDTH = 0b11;
constexpr std::size_t FIELD_META_BITS_MASK = MASK(FIELD_META_BITS_POS, FIELD_META_BITS_WIDTH);
constexpr field_state_t FIELD_SET_META_BITS(field_state_t field, field_meta_bits_t meta_bits) {
    return SET_VALUE(field, meta_bits, FIELD_META_BITS_POS, FIELD_META_BITS_MASK);
}
constexpr field_meta_bits_t FIELD_GET_META_BITS(field_state_t field) {
    return (field & FIELD_META_BITS_MASK) >> FIELD_META_BITS_POS;
}

constexpr field_state_t FF = 0;

constexpr field_state_t FW = FIELD_SET_PLAYER(FF, PLAYER_WHITE);
constexpr field_state_t FWP = FIELD_SET_PIECE(FW, PIECE_PAWN);
constexpr field_state_t FWN = FIELD_SET_PIECE(FW, PIECE_KNIGHT);
constexpr field_state_t FWB = FIELD_SET_PIECE(FW, PIECE_BISHOP);
constexpr field_state_t FWR = FIELD_SET_PIECE(FW, PIECE_ROOK);
constexpr field_state_t FWQ = FIELD_SET_PIECE(FW, PIECE_QUEEN);
constexpr field_state_t FWK = FIELD_SET_PIECE(FW, PIECE_KING);

constexpr field_state_t FB = FIELD_SET_PLAYER(FF, PLAYER_BLACK);
constexpr field_state_t FBP = FIELD_SET_PIECE(FB, PIECE_PAWN);
constexpr field_state_t FBN = FIELD_SET_PIECE(FB, PIECE_KNIGHT);
constexpr field_state_t FBB = FIELD_SET_PIECE(FB, PIECE_BISHOP);
constexpr field_state_t FBR = FIELD_SET_PIECE(FB, PIECE_ROOK);
constexpr field_state_t FBQ = FIELD_SET_PIECE(FB, PIECE_QUEEN);
constexpr field_state_t FBK = FIELD_SET_PIECE(FB, PIECE_KING);

using board_state_t = std::array<field_state_t, 64>;

constexpr board_state_t START_BOARD =
{
    FWR, FWN, FWB, FWQ, FWK, FWB, FWN, FWR,
    FWP, FWP, FWP, FWP, FWP, FWP, FWP, FWP,
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FBP, FBP, FBP, FBP, FBP, FBP, FBP, FBP,
    FBR, FBN, FBB, FBQ, FBK, FBB, FBN, FBR
};

constexpr board_state_t EMPTY_BOARD =
{
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FF , FF , FF , FF , FF , FF , FF , FF ,
    FF , FF , FF , FF , FF , FF , FF , FF
};

enum field_t
{
    A1 = 0, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    FIELD_INVALID
};

template <typename T>
constexpr void BOARD_STATE_META_SET_BITS(
    board_state_t& board, const T value, const std::size_t start_pos, const std::size_t end_pos) {
    const std::size_t start_field_idx = start_pos / FIELD_META_BITS_SIZE;
    const std::size_t end_field_idx = end_pos / FIELD_META_BITS_SIZE;

    std::size_t field_shift_idx = 0;
    for (std::size_t field_idx = start_field_idx;
         field_idx < end_field_idx;
         ++field_idx, ++field_shift_idx) {
        std::size_t shift = field_shift_idx * FIELD_META_BITS_SIZE;
        field_meta_bits_t meta_bits = GET_VALUE(value, shift, FIELD_META_BITS_WIDTH);
        board[field_idx] = FIELD_SET_META_BITS(board[field_idx], meta_bits);
    }
}

template <typename T>
constexpr T BOARD_STATE_META_GET_BITS(
    const board_state_t& board, const std::size_t start_pos, const std::size_t end_pos) {
    T result = {};
    const std::size_t start_field_idx = start_pos / FIELD_META_BITS_SIZE;
    const std::size_t end_field_idx = end_pos / FIELD_META_BITS_SIZE;

    std::size_t field_shift_idx = 0;
    for (std::size_t field_idx = start_field_idx;
         field_idx < end_field_idx;
         ++field_idx, ++field_shift_idx) {
        field_meta_bits_t meta_bits = FIELD_GET_META_BITS(board[field_idx]);
        std::size_t shift = field_shift_idx * FIELD_META_BITS_SIZE;
        result = SET_VALUE(result, meta_bits, shift, FIELD_META_BITS_WIDTH << shift);
    }
    return result;
}

using last_move_t = uint16_t;

constexpr std::size_t BOARD_STATE_META_LAST_MOVE_POS = 0;
constexpr std::size_t BOARD_STATE_META_LAST_MOVE_BITS = 16;
constexpr void BOARD_STATE_META_SET_LAST_MOVE(board_state_t& board, last_move_t last_move) {
    BOARD_STATE_META_SET_BITS(board, last_move,
        BOARD_STATE_META_LAST_MOVE_POS,
        BOARD_STATE_META_LAST_MOVE_BITS);
}
constexpr last_move_t BOARD_STATE_META_GET_LAST_MOVE(const board_state_t& board) {
    return BOARD_STATE_META_GET_BITS<last_move_t>(board,
        BOARD_STATE_META_LAST_MOVE_POS,
        BOARD_STATE_META_LAST_MOVE_BITS);
}

constexpr std::size_t LAST_MOVE_PLAYER_POS = 0;
constexpr std::size_t LAST_MOVE_PLAYER_WIDTH = 0b1;
constexpr std::size_t LAST_MOVE_PLAYER_SIZE = 1;
constexpr std::size_t LAST_MOVE_PLAYER_MASK = MASK(LAST_MOVE_PLAYER_POS, LAST_MOVE_PLAYER_WIDTH);
constexpr last_move_t LAST_MOVE_SET_PLAYER(last_move_t last_move, player_t player) {
    return SET_VALUE(last_move, player, LAST_MOVE_PLAYER_POS, LAST_MOVE_PLAYER_MASK);
}
constexpr player_t LAST_MOVE_GET_PLAYER(last_move_t last_move) {
    return (last_move & LAST_MOVE_PLAYER_MASK) >> LAST_MOVE_PLAYER_POS;
}

constexpr std::size_t LAST_MOVE_PIECE_POS = 1;
constexpr std::size_t LAST_MOVE_PIECE_WIDTH = 0b111;
constexpr std::size_t LAST_MOVE_PIECE_SIZE = 3;
constexpr std::size_t LAST_MOVE_PIECE_MASK = MASK(LAST_MOVE_PIECE_POS, LAST_MOVE_PIECE_WIDTH);
constexpr last_move_t LAST_MOVE_SET_PIECE(last_move_t last_move, piece_t piece) {
    return SET_VALUE(last_move, piece, LAST_MOVE_PIECE_POS, LAST_MOVE_PIECE_MASK);
}
constexpr piece_t LAST_MOVE_GET_PIECE(last_move_t last_move) {
    return (last_move & LAST_MOVE_PIECE_MASK) >> LAST_MOVE_PIECE_POS;
}

constexpr std::size_t LAST_MOVE_FROM_POS = 4;
constexpr std::size_t LAST_MOVE_FROM_WIDTH = 0b111111;
constexpr std::size_t LAST_MOVE_FROM_SIZE = 6;
constexpr std::size_t LAST_MOVE_FROM_MASK = MASK(LAST_MOVE_FROM_POS, LAST_MOVE_FROM_WIDTH);
constexpr last_move_t LAST_MOVE_SET_FROM(last_move_t last_move, field_t from) {
    return SET_VALUE(last_move, from, LAST_MOVE_FROM_POS, LAST_MOVE_FROM_MASK);
}
constexpr field_t LAST_MOVE_GET_FROM(last_move_t last_move) {
    return static_cast<field_t>((last_move & LAST_MOVE_FROM_MASK) >> LAST_MOVE_FROM_POS);
}

constexpr std::size_t LAST_MOVE_TO_POS = 10;
constexpr std::size_t LAST_MOVE_TO_WIDTH = 0b111111;
constexpr std::size_t LAST_MOVE_TO_SIZE = 6;
constexpr std::size_t LAST_MOVE_TO_MASK = MASK(LAST_MOVE_TO_POS, LAST_MOVE_TO_WIDTH);
constexpr last_move_t LAST_MOVE_SET_TO(last_move_t last_move, field_t to) {
    return SET_VALUE(last_move, to, LAST_MOVE_TO_POS, LAST_MOVE_TO_MASK);
}
constexpr field_t LAST_MOVE_GET_TO(last_move_t last_move) {
    return static_cast<field_t>((last_move & LAST_MOVE_TO_MASK) >> LAST_MOVE_TO_POS);
}

enum class file_t {
    A = 0, B, C, D, E, F, G, H, file_t_max
};

enum class rank_t {
    _1 = 0, _2, _3, _4, _5, _6, _7, _8, rank_t_max
};

constexpr file_t field_file(const field_t field) {
    return field != FIELD_INVALID
        ? static_cast<file_t>(
            static_cast<uint8_t>(field) % static_cast<uint8_t>(file_t::file_t_max))
        : file_t::file_t_max;
}

constexpr rank_t field_rank(const field_t field) {
    return field != FIELD_INVALID
        ? static_cast<rank_t>(
            static_cast<uint8_t>(field) / static_cast<uint8_t>(rank_t::rank_t_max))
        : rank_t::rank_t_max;
}

constexpr field_t make_field(const uint8_t file, const uint8_t rank) {
    return (static_cast<uint8_t>(file_t::file_t_max) <= file or
        static_cast<uint8_t>(rank_t::rank_t_max) <= rank)
        ? FIELD_INVALID
        : static_cast<field_t>(rank * static_cast<uint8_t>(rank_t::rank_t_max) + file);
}

constexpr field_t field_up(const field_t field) {
    return make_field(
        static_cast<uint8_t>(field_file(field)), static_cast<uint8_t>(field_rank(field)) + 1);
}

constexpr field_t field_down(const field_t field) {
    return make_field(
        static_cast<uint8_t>(field_file(field)), static_cast<uint8_t>(field_rank(field)) - 1);
}

constexpr field_t field_left(const field_t field) {
    return make_field(
        static_cast<uint8_t>(field_file(field)) - 1, static_cast<uint8_t>(field_rank(field)));
}

constexpr field_t field_right(const field_t field) {
    return make_field(
        static_cast<uint8_t>(field_file(field)) + 1, static_cast<uint8_t>(field_rank(field)));
}

constexpr field_t field_left_up(const field_t field) {
    return make_field(
        static_cast<uint8_t>(field_file(field)) - 1, static_cast<uint8_t>(field_rank(field)) + 1);
}

constexpr field_t field_right_up(const field_t field) {
    return make_field(
        static_cast<uint8_t>(field_file(field)) + 1, static_cast<uint8_t>(field_rank(field)) + 1);
}

constexpr field_t field_left_down(const field_t field) {
    return make_field(
        static_cast<uint8_t>(field_file(field)) - 1, static_cast<uint8_t>(field_rank(field)) - 1);
}

constexpr field_t field_right_down(const field_t field) {
    return make_field(
        static_cast<uint8_t>(field_file(field)) + 1, static_cast<uint8_t>(field_rank(field)) - 1);
}

struct move_s {
    player_t player;
    piece_t piece;
    field_t from;
    field_t to;
};

bool check_last_move(const board_state_t& board, const move_s& move) {
    last_move_t last_move = BOARD_STATE_META_GET_LAST_MOVE(board);
    piece_t last_move_player = LAST_MOVE_GET_PLAYER(last_move);
    piece_t last_move_piece = LAST_MOVE_GET_PIECE(last_move);
    field_t last_move_from = LAST_MOVE_GET_FROM(last_move);
    field_t last_move_to = LAST_MOVE_GET_TO(last_move);

    return move.player == last_move_player and
        move.piece == last_move_piece and
        move.from == last_move_from and
        move.to == last_move_to;
}

void apply_move(board_state_t& board, const move_s& move) {
    board[move.from] = FIELD_SET_PIECE(board[move.from], PIECE_EMPTY);
    board[move.to] = FIELD_SET_PIECE(FIELD_SET_PLAYER(board[move.to], move.player), move.piece);

    last_move_t last_move = 0;
    last_move = LAST_MOVE_SET_PLAYER(last_move, move.player);
    last_move = LAST_MOVE_SET_PIECE(last_move, move.piece);
    last_move = LAST_MOVE_SET_FROM(last_move, move.from);
    last_move = LAST_MOVE_SET_TO(last_move, move.to);
    BOARD_STATE_META_SET_LAST_MOVE(board, last_move);
}

board_state_t* add_white_pawn_move_up(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_up(field);
    if (FIELD_INVALID == target_field or PIECE_EMPTY != FIELD_GET_PIECE(board[target_field]))
        return moves;

    if (rank_t::_8 == field_rank(target_field)) {
        auto& move1 = moves[0];
        apply_move(move1 = board, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
        move1[target_field] = FIELD_SET_PIECE(move1[target_field], PIECE_KNIGHT);

        auto& move2 = moves[1];
        apply_move(move2 = board, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
        move2[target_field] = FIELD_SET_PIECE(move2[target_field], PIECE_BISHOP);

        auto& move3 = moves[2];
        apply_move(move3 = board, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
        move3[target_field] = FIELD_SET_PIECE(move3[target_field], PIECE_ROOK);

        auto& move4 = moves[3];
        apply_move(move4 = board, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
        move4[target_field] = FIELD_SET_PIECE(move4[target_field], PIECE_QUEEN);
        return moves + 4;
    } else {
        apply_move(*moves = board, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
        return moves + 1;
    }
}

board_state_t* add_white_pawn_move_up_long(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    if (rank_t::_2 != field_rank(field)) return moves;
    field_t target_field = field_up(field);
    if (FIELD_INVALID == target_field or PIECE_EMPTY != FIELD_GET_PIECE(board[target_field]))
        return moves;
    target_field = field_up(target_field);
    if (FIELD_INVALID == target_field or PIECE_EMPTY != FIELD_GET_PIECE(board[target_field]))
        return moves;

    apply_move(*moves = board, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
    return moves + 1;
}

board_state_t* add_white_pawn_capture_left_up(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_left_up(field);
    if (FIELD_INVALID == target_field or
        PIECE_EMPTY == FIELD_GET_PIECE(board[target_field]) or
        PLAYER_BLACK != FIELD_GET_PLAYER(board[target_field]))
        return moves;

    apply_move(*moves = board, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
    return moves + 1;
}

board_state_t* add_white_pawn_capture_right_up(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_right_up(field);
    if (FIELD_INVALID == target_field or
        PIECE_EMPTY == FIELD_GET_PIECE(board[target_field]) or
        PLAYER_BLACK != FIELD_GET_PLAYER(board[target_field]))
        return moves;

    apply_move(*moves = board, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
    return moves + 1;
}

board_state_t* add_white_pawn_capture_enpassant_left(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    if (rank_t::_5 != field_rank(field)) return moves;

    field_t target_field = field_left_up(field);
    field_t opps_move_from = field_up(target_field);
    field_t opps_move_to = field_down(target_field);
    if (!check_last_move(board, { PLAYER_BLACK, PIECE_PAWN, opps_move_from, opps_move_to }))
        return moves;

    *moves = board;
    (*moves)[opps_move_to] = FIELD_SET_PIECE((*moves)[opps_move_to], PIECE_EMPTY);
    apply_move(*moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
    return moves + 1;
}

board_state_t* add_white_pawn_capture_enpassant_right(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    if (rank_t::_5 != field_rank(field)) return moves;

    field_t target_field = field_right_up(field);
    field_t opps_move_from = field_up(target_field);
    field_t opps_move_to = field_down(target_field);
    if (!check_last_move(board, { PLAYER_BLACK, PIECE_PAWN, opps_move_from, opps_move_to }))
        return moves;

    *moves = board;
    (*moves)[opps_move_to] = FIELD_SET_PIECE((*moves)[opps_move_to], PIECE_EMPTY);
    apply_move(*moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
    return moves + 1;
}

board_state_t* fill_white_pawn_candidate_moves(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    moves = add_white_pawn_move_up(moves, board, field);
    moves = add_white_pawn_move_up_long(moves, board, field);
    moves = add_white_pawn_capture_left_up(moves, board, field);
    moves = add_white_pawn_capture_right_up(moves, board, field);
    moves = add_white_pawn_capture_enpassant_left(moves, board, field);
    moves = add_white_pawn_capture_enpassant_right(moves, board, field);
    return moves;
}

board_state_t* add_black_pawn_move_down(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_down(field);
    if (FIELD_INVALID == target_field or PIECE_EMPTY != FIELD_GET_PIECE(board[target_field]))
        return moves;

    if (rank_t::_1 == field_rank(target_field)) {
        auto& move1 = moves[0];
        apply_move(move1 = board, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
        move1[target_field] = FIELD_SET_PIECE(move1[target_field], PIECE_KNIGHT);

        auto& move2 = moves[1];
        apply_move(move2 = board, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
        move2[target_field] = FIELD_SET_PIECE(move2[target_field], PIECE_BISHOP);

        auto& move3 = moves[2];
        apply_move(move3 = board, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
        move3[target_field] = FIELD_SET_PIECE(move3[target_field], PIECE_ROOK);

        auto& move4 = moves[3];
        apply_move(move4 = board, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
        move4[target_field] = FIELD_SET_PIECE(move4[target_field], PIECE_QUEEN);
        return moves + 4;
    } else {
        apply_move(*moves = board, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
        return moves + 1;
    }
}


board_state_t* add_black_pawn_move_down_long(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    if (rank_t::_7 != field_rank(field)) return moves;
    field_t target_field = field_down(field);
    if (FIELD_INVALID == target_field or PIECE_EMPTY != FIELD_GET_PIECE(board[target_field]))
        return moves;
    target_field = field_down(target_field);
    if (FIELD_INVALID == target_field or PIECE_EMPTY != FIELD_GET_PIECE(board[target_field]))
        return moves;

    apply_move(*moves = board, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
    return moves + 1;
}

board_state_t* add_black_pawn_capture_left_down(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_left_down(field);
    if (FIELD_INVALID == target_field or
        PIECE_EMPTY == FIELD_GET_PIECE(board[target_field]) or
        PLAYER_WHITE != FIELD_GET_PLAYER(board[target_field]))
        return moves;

    apply_move(*moves = board, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
    return moves + 1;
}

board_state_t* add_black_pawn_capture_right_down(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_right_down(field);
    if (FIELD_INVALID == target_field or
        PIECE_EMPTY == FIELD_GET_PIECE(board[target_field]) or
        PLAYER_WHITE != FIELD_GET_PLAYER(board[target_field]))
        return moves;

    apply_move(*moves = board, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
    return moves + 1;
}


board_state_t* add_black_pawn_capture_enpassant_left(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    if (rank_t::_4 != field_rank(field)) return moves;

    field_t target_field = field_left_down(field);
    field_t opps_move_from = field_down(target_field);
    field_t opps_move_to = field_up(target_field);
    if (!check_last_move(board, { PLAYER_WHITE, PIECE_PAWN, opps_move_from, opps_move_to }))
        return moves;

    *moves = board;
    (*moves)[opps_move_to] = FIELD_SET_PIECE((*moves)[opps_move_to], PIECE_EMPTY);
    apply_move(*moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
    return moves + 1;
}

board_state_t* add_black_pawn_capture_enpassant_right(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    if (rank_t::_4 != field_rank(field)) return moves;

    field_t target_field = field_right_down(field);
    field_t opps_move_from = field_down(target_field);
    field_t opps_move_to = field_up(target_field);
    if (!check_last_move(board, { PLAYER_WHITE, PIECE_PAWN, opps_move_from, opps_move_to }))
        return moves;

    *moves = board;
    (*moves)[opps_move_to] = FIELD_SET_PIECE((*moves)[opps_move_to], PIECE_EMPTY);
    apply_move(*moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
    return moves + 1;
}

board_state_t* fill_black_pawn_candidate_moves(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    moves = add_black_pawn_move_down(moves, board, field);
    moves = add_black_pawn_move_down_long(moves, board, field);
    moves = add_black_pawn_capture_left_down(moves, board, field);
    moves = add_black_pawn_capture_right_down(moves, board, field);
    moves = add_black_pawn_capture_enpassant_left(moves, board, field);
    moves = add_black_pawn_capture_enpassant_right(moves, board, field);
    return moves;
}

board_state_t* fill_pawn_candidate_moves(
    board_state_t* moves, const board_state_t& board, const player_t player, const field_t field) {
    return PLAYER_WHITE == player
        ? fill_white_pawn_candidate_moves(moves, board, field)
        : fill_black_pawn_candidate_moves(moves, board, field);
}

board_state_t* fill_regular_candidate_move(
    board_state_t* moves, const board_state_t& board, const player_t player, const piece_t piece,
    const field_t field, const field_t target_field) {
    if (FIELD_INVALID == target_field or
        (player == FIELD_GET_PLAYER(board[target_field]) and
         PIECE_EMPTY != FIELD_GET_PIECE(board[target_field])))
        return moves;

    apply_move(*moves = board, { player, piece, field, target_field });
    return moves + 1;
}

board_state_t* fill_knight_candidate_moves(
    board_state_t* moves, const board_state_t& board, const player_t player, const field_t field) {
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KNIGHT, field, field_up(field_left_up(field)));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KNIGHT, field, field_up(field_right_up(field)));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KNIGHT, field, field_left(field_left_up(field)));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KNIGHT, field, field_left(field_left_down(field)));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KNIGHT, field, field_down(field_left_down(field)));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KNIGHT, field, field_down(field_right_down(field)));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KNIGHT, field, field_right(field_right_up(field)));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KNIGHT, field, field_right(field_right_down(field)));
    return moves;
}

board_state_t* fill_ranged_candidate_moves_op(
    board_state_t* moves, const board_state_t& board, const player_t player, const field_t field,
    const piece_t piece, field_t(*operation)(const field_t)) {
    field_t target_field = field;
    uint8_t move_cnt = 0u;
    do {
        target_field = operation(target_field);
        if (FIELD_INVALID == target_field) break;
        if (PIECE_EMPTY == FIELD_GET_PIECE(board[target_field])) {
            apply_move(moves[move_cnt++] = board, { player, piece, field, target_field });
            continue;
        }
        if (player != FIELD_GET_PLAYER(board[target_field])) {
            apply_move(moves[move_cnt++] = board, { player, piece, field, target_field });
            break;
        }
        break;
    } while (true);
    return moves + move_cnt;
}

board_state_t* fill_diagonal_candidate_moves(
    board_state_t* moves, const board_state_t& board, const player_t player, const field_t field,
    const piece_t piece) {
    moves = fill_ranged_candidate_moves_op(moves, board, player, field, piece, field_left_up);
    moves = fill_ranged_candidate_moves_op(moves, board, player, field, piece, field_left_down);
    moves = fill_ranged_candidate_moves_op(moves, board, player, field, piece, field_right_up);
    moves = fill_ranged_candidate_moves_op(moves, board, player, field, piece, field_right_down);
    return moves;
}

board_state_t* fill_cross_candidate_moves(
    board_state_t* moves, const board_state_t& board, const player_t player, const field_t field,
    const piece_t piece) {
    moves = fill_ranged_candidate_moves_op(moves, board, player, field, piece, field_up);
    moves = fill_ranged_candidate_moves_op(moves, board, player, field, piece, field_down);
    moves = fill_ranged_candidate_moves_op(moves, board, player, field, piece, field_right);
    moves = fill_ranged_candidate_moves_op(moves, board, player, field, piece, field_left);
    return moves;
}

board_state_t* fill_white_short_castle(
    board_state_t* moves, const board_state_t& board, const field_t field)
{
    if (E1 != field or
        PLAYER_WHITE != FIELD_GET_PLAYER(board[H1]) or
        PIECE_ROOK != FIELD_GET_PIECE(board[H1]) or
        PIECE_EMPTY != FIELD_GET_PIECE(board[F1]) or
        PIECE_EMPTY != FIELD_GET_PIECE(board[G1]))
        return moves;

    auto& move = *moves = board;
    move[H1] = FIELD_SET_PIECE(move[H1], PIECE_EMPTY);
    move[F1] = FIELD_SET_PIECE(FIELD_SET_PLAYER(move[F1], PLAYER_WHITE), PIECE_ROOK);
    apply_move(move, { PLAYER_WHITE, PIECE_KING, E1, G1 });
    return moves + 1;
}

board_state_t* fill_black_short_castle(
    board_state_t* moves, const board_state_t& board, const field_t field)
{
    if (E8 != field or
        PLAYER_BLACK != FIELD_GET_PLAYER(board[H8]) or
        PIECE_ROOK != FIELD_GET_PIECE(board[H8]) or
        PIECE_EMPTY != FIELD_GET_PIECE(board[F8]) or
        PIECE_EMPTY != FIELD_GET_PIECE(board[G8]))
        return moves;

    auto& move = *moves = board;
    move[H8] = FIELD_SET_PIECE(move[H8], PIECE_EMPTY);
    move[F8] = FIELD_SET_PIECE(FIELD_SET_PLAYER(move[F8], PLAYER_BLACK), PIECE_ROOK);
    apply_move(move, { PLAYER_BLACK, PIECE_KING, E8, G8 });
    return moves + 1;
}

board_state_t* fill_short_castle(
    board_state_t* moves, const board_state_t& board, const player_t player, const field_t field)
{
    return PLAYER_WHITE == player
        ? fill_white_short_castle(moves, board, field)
        : fill_black_short_castle(moves, board, field);
}

board_state_t* fill_white_long_castle(
    board_state_t* moves, const board_state_t& board, const field_t field)
{
    if (E1 != field or
        PLAYER_WHITE != FIELD_GET_PLAYER(board[A1]) or
        PIECE_ROOK != FIELD_GET_PIECE(board[A1]) or
        PIECE_EMPTY != FIELD_GET_PIECE(board[B1]) or
        PIECE_EMPTY != FIELD_GET_PIECE(board[C1]) or
        PIECE_EMPTY != FIELD_GET_PIECE(board[D1]))
        return moves;

    auto& move = *moves = board;
    move[A1] = FIELD_SET_PIECE(move[A1], PIECE_EMPTY);
    move[D1] = FIELD_SET_PIECE(FIELD_SET_PLAYER(move[D1], PLAYER_WHITE), PIECE_ROOK);
    apply_move(move, { PLAYER_WHITE, PIECE_KING, E1, C1 });
    return moves + 1;
}

board_state_t* fill_black_long_castle(
    board_state_t* moves, const board_state_t& board, const field_t field)
{
    if (E8 != field or
        PLAYER_BLACK != FIELD_GET_PLAYER(board[A8]) or
        PIECE_ROOK != FIELD_GET_PIECE(board[A8]) or
        PIECE_EMPTY != FIELD_GET_PIECE(board[B8]) or
        PIECE_EMPTY != FIELD_GET_PIECE(board[C8]) or
        PIECE_EMPTY != FIELD_GET_PIECE(board[D8]))
        return moves;

    auto& move = *moves = board;
    move[A8] = FIELD_SET_PIECE(move[A8], PIECE_EMPTY);
    move[D8] = FIELD_SET_PIECE(FIELD_SET_PLAYER(move[D8], PLAYER_BLACK), PIECE_ROOK);
    apply_move(move, { PLAYER_BLACK, PIECE_KING, E8, C8 });
    return moves + 1;
}

board_state_t* fill_long_castle(
    board_state_t* moves, const board_state_t& board, const player_t player, const field_t field)
{
    return PLAYER_WHITE == player
        ? fill_white_long_castle(moves, board, field)
        : fill_black_long_castle(moves, board, field);
}

board_state_t* fill_king_candidate_moves(
    board_state_t* moves, const board_state_t& board, const player_t player, const field_t field) {
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KING, field, field_up(field));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KING, field, field_left_up(field));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KING, field, field_right_up(field));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KING, field, field_left(field));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KING, field, field_right(field));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KING, field, field_down(field));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KING, field, field_left_down(field));
    moves = fill_regular_candidate_move(
        moves, board, player, PIECE_KING, field, field_right_down(field));
    moves = fill_short_castle(moves, board, player, field);
    moves = fill_long_castle(moves, board, player, field);
    return moves;
}

board_state_t* fill_candidate_moves(
    board_state_t* moves, const board_state_t& board, const player_t player) {
    for (uint8_t field_idx = static_cast<uint8_t>(A1);
         field_idx < static_cast<uint8_t>(FIELD_INVALID);
         ++field_idx) {
        if (player != FIELD_GET_PLAYER(board[field_idx])) continue;

        field_t field = static_cast<field_t>(field_idx);
        piece_t piece = FIELD_GET_PIECE(board[field_idx]);
        if (PIECE_PAWN == piece) {
            moves = fill_pawn_candidate_moves(moves, board, player, field);
            continue;
        }
        if (PIECE_KNIGHT == piece) {
            moves = fill_knight_candidate_moves(moves, board, player, field);
            continue;
        }
        if (PIECE_BISHOP == piece) {
            moves = fill_diagonal_candidate_moves(moves, board, player, field, piece);
            continue;
        }
        if (PIECE_ROOK == piece) {
            moves = fill_cross_candidate_moves(moves, board, player, field, piece);
            continue;
        }
        if (PIECE_QUEEN == piece) {
            moves = fill_diagonal_candidate_moves(moves, board, player, field, piece);
            moves = fill_cross_candidate_moves(moves, board, player, field, piece);
            continue;
        }
        if (PIECE_KING == piece) {
            moves = fill_king_candidate_moves(moves, board, player, field);
        }
    }
    return moves;
}

bool validate_board_state(const board_state_t& board)
{
    last_move_t last_move = BOARD_STATE_META_GET_LAST_MOVE(board);
    player_t last_move_player = LAST_MOVE_GET_PLAYER(last_move);
    piece_t last_move_piece = LAST_MOVE_GET_PIECE(last_move);
    field_t last_move_from = LAST_MOVE_GET_FROM(last_move);
    field_t last_move_to = LAST_MOVE_GET_TO(last_move);

    if (last_move_piece != PIECE_EMPTY and
        last_move_player != FIELD_GET_PLAYER(board[last_move_to]) and
        last_move_piece != FIELD_GET_PIECE(board[last_move_to]) and
        PIECE_EMPTY != FIELD_GET_PIECE(board[last_move_from]))
    {
        return false;
    }
    return true;
}

}  // namespace chess

#endif  // CHESS_HPP_
