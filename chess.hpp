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
    A8, B8, C8, D8, E8, F8, G8, H8
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

constexpr std::size_t LAST_MOVE_PIECE_POS = 0;
constexpr std::size_t LAST_MOVE_PIECE_WIDTH = 0b111;
constexpr std::size_t LAST_MOVE_PIECE_SIZE = 4;
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

std::size_t fill_candidate_moves(board_state_t* moves, const board_state_t& board) {
    *moves = board;
    last_move_t last_move = 0;
    last_move = LAST_MOVE_SET_PIECE(last_move, PIECE_PAWN);
    last_move = LAST_MOVE_SET_FROM(last_move, E2);
    last_move = LAST_MOVE_SET_TO(last_move, E3);
    BOARD_STATE_META_SET_LAST_MOVE(*moves, last_move);
    return 1;
}

bool check_last_move(const board_state_t& board, piece_t piece, field_t from, field_t to) {
    last_move_t last_move = BOARD_STATE_META_GET_LAST_MOVE(board);
    piece_t last_move_piece = LAST_MOVE_GET_PIECE(last_move);
    field_t last_move_from = LAST_MOVE_GET_FROM(last_move);
    field_t last_move_to = LAST_MOVE_GET_TO(last_move);

    // printf("last_move: %x\n", last_move);
    // printf("last_move_piece: %x | piece: %x\n", last_move_piece, piece);
    // printf("last_move_from: %x | from: %x\n", last_move_from, from);
    // printf("last_move_to: %x | to: %x\n", last_move_to, to);

    // printf("meta_bits[0]: %x\n", FIELD_GET_META_BITS(board[0]));
    // printf("meta_bits[1]: %x\n", FIELD_GET_META_BITS(board[1]));
    // printf("meta_bits[2]: %x\n", FIELD_GET_META_BITS(board[2]));
    // printf("meta_bits[3]: %x\n", FIELD_GET_META_BITS(board[3]));
    // printf("meta_bits[4]: %x\n", FIELD_GET_META_BITS(board[4]));
    // printf("meta_bits[5]: %x\n", FIELD_GET_META_BITS(board[5]));
    // printf("meta_bits[6]: %x\n", FIELD_GET_META_BITS(board[6]));
    // printf("meta_bits[7]: %x\n", FIELD_GET_META_BITS(board[7]));
    return piece == last_move_piece and from == last_move_from and to == last_move_to;
}

}  // namespace chess

#endif  // CHESS_HPP_
