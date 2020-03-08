/** chess.hpp
 *
 * Chess engine core header-only library.
 */
#ifndef CHESS_HPP_
#define CHESS_HPP_

#include <array>
#include <cstdint>

namespace chess
{

/** @defgroup core-types Basic types in chess engine
 *  @{
 */

/** Basic field type
 *  Bits contain metadata about field on the chessboard.
 */
using field_state_t = uint8_t;

/** Field property - player
 *  Describes whether the field is occupied by white or black player's piece. If field has not
 *  piece on it, this property should be ignored.
 */
using player_t = field_state_t;

/** Field property - piece
 *  Describes the piece that the field is occupied by or if the field is empty.
 */
using piece_t = field_state_t;

/** Field property - under white's attack
 *  Describes whether the field is attached by any white player's piece.
 */
using under_white_attack_t = field_state_t;

/** Field property - under black's attack
 *  Describes whether the field is attached by any black player's piece.
 */
using under_black_attack_t = field_state_t;

/** Field property - meta bits
 *  Metabits from all fields of board combined together describe meta-state of the game
 *  (not a field) - for example: castling rights.
 */
using field_meta_bits_t = field_state_t;

/** Basic board type
 *  Array of 64 fields from A1, B1 ... H8.
 */
using board_state_t = std::array<field_state_t, 64>;

/** Field enumeration
 *  Convenient type for enumerating fields of the chess board. Can be casted to integrat type to
 *  index into `board_state_t`.
 */
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
    INVALID,
    BEGIN = A1,
    END = INVALID
};

/** Last move basic type
 *  Describes last move. Stored in meta bits of `board_state_t`
 */
using last_move_t = uint16_t;

/** Describes castling rights of current game. Stored in meta bits of `board_state_t`
  * bit 0: white short castling right
  * bit 1: white long castling right
  * bit 2: black short castling right
  * bit 3: black long castling right
  * Value 0 on given bit position means that given castling type is ALLOWED
  */
using castling_rights_t = uint8_t;

/** Basic enumeration of file (column) on a chessboard */
enum class file_t {
    A = 0, B, C, D, E, F, G, H,
    BEGIN = A,
    LAST = H,
    MAX = 8,
    END = MAX
};

/** Basic enumeration of rank (row) on a chessboard */
enum class rank_t {
    _1 = 0, _2, _3, _4, _5, _6, _7, _8,
    BEGIN = _1,
    LAST = _8,
    MAX = 8,
    END = MAX
};

/** Basic type describing a move on a chessboard */
struct move_s {
    /** Which player made a move */
    player_t player;
    /** What piece was moved */
    piece_t piece;
    /** From which field move was made */
    field_t from;
    /** Onto which field move was made */
    field_t to;
};

/*  @} */ // core-types

/** @defgroup helpers Helper functions
 *  Allow to read and manipulate data based on basic types.
 *  @{
 */

/** Returns opposite player's property value */
constexpr player_t opponent(const player_t player);

/** Field state property getters and setters */
constexpr field_state_t field_get_player(const field_state_t field);
constexpr field_state_t field_set_player(const field_state_t field, const player_t player);

constexpr field_state_t field_get_piece(const field_state_t field);
constexpr field_state_t field_set_piece(const field_state_t field, const piece_t piece);

constexpr field_state_t field_set_under_white_attack(const field_state_t field);
constexpr field_state_t field_clear_under_white_attack(const field_state_t field);
constexpr bool field_under_white_attack(const field_state_t field);

constexpr field_state_t field_set_under_black_attack(const field_state_t field);
constexpr field_state_t field_clear_under_black_attack(const field_state_t field);
constexpr bool field_under_black_attack(const field_state_t field);

constexpr field_state_t field_get_meta_bits(const field_state_t field);
constexpr field_state_t field_set_meta_bits(const field_state_t field,
    const field_meta_bits_t meta_bits);

/** Last move getters and setters */
constexpr last_move_t last_move_set_player(last_move_t last_move, player_t player);
constexpr player_t last_move_get_player(last_move_t last_move);

constexpr last_move_t last_move_set_piece(last_move_t last_move, piece_t piece);
constexpr piece_t last_move_get_piece(last_move_t last_move);

constexpr last_move_t last_move_set_from(last_move_t last_move, field_t from);
constexpr field_t last_move_get_from(last_move_t last_move);

constexpr last_move_t last_move_set_to(last_move_t last_move, field_t to);
constexpr field_t last_move_get_to(last_move_t last_move);

/** Castling rights manipulation */
constexpr castling_rights_t castling_rights_remove_white_short(const castling_rights_t rights);
constexpr bool castling_rights_white_short(const castling_rights_t rights);

constexpr castling_rights_t castling_rights_remove_white_long(const castling_rights_t rights);
constexpr bool castling_rights_white_long(const castling_rights_t rights);

constexpr castling_rights_t castling_rights_remove_black_short(const castling_rights_t rights);
constexpr bool castling_rights_black_short(const castling_rights_t rights);

constexpr castling_rights_t castling_rights_remove_black_long(const castling_rights_t rights);
constexpr bool castling_rights_black_long(const castling_rights_t rights);

/** Meta bits property getters and setters */
constexpr void board_state_meta_set_last_move(board_state_t& board, const last_move_t last_move);
constexpr last_move_t board_state_meta_get_last_move(const board_state_t& board);

constexpr void board_state_meta_set_castling_rights(board_state_t& board,
    const castling_rights_t rights);
constexpr castling_rights_t board_state_meta_get_castling_rights(const board_state_t& board);

/** Returns `file_t` of a field */
constexpr file_t field_file(const field_t field);

/** Returns `rank_t` of a field */
constexpr rank_t field_rank(const field_t field);

/** Makes a `field_t` from file and rank
 *  If requested field would exceed a border of chessboard, field on the border is returned.
 */
constexpr field_t make_field(const uint8_t file, const uint8_t rank);

/** Returns a `field_t` above field
 *  If requested field would exceed a border of chessboard, original field is returned.
 */
constexpr field_t field_up(const field_t field);

/** Returns a `field_t` below field
 *  If requested field would exceed a border of chessboard, original field is returned.
 */
constexpr field_t field_down(const field_t field);

/** Returns a `field_t` left to a field
 *  If requested field would exceed a border of chessboard, original field is returned.
 */
constexpr field_t field_left(const field_t field);

/** Returns a `field_t` right to a field
 *  If requested field would exceed a border of chessboard, original field is returned.
 */
constexpr field_t field_right(const field_t field);

/** Returns a `field_t` left and up to a field
 *  If requested field would exceed a border of chessboard, original field is returned.
 */
constexpr field_t field_left_up(const field_t field);

/** Returns a `field_t` right and up to a field
 *  If requested field would exceed a border of chessboard, original field is returned.
 */
constexpr field_t field_right_up(const field_t field);

/** Returns a `field_t` left and down to a field
 *  If requested field would exceed a border of chessboard, original field is returned.
 */
constexpr field_t field_left_down(const field_t field);

/** Returns a `field_t` right and down to a field
 *  If requested field would exceed a border of chessboard, original field is returned.
 */
constexpr field_t field_right_down(const field_t field);

/*  @} */ // helpers

/** @defgroup core-defs Basic definitions in chess engine
 *  @{
 */

/** White player property value */
constexpr player_t PLAYER_WHITE = 1;

/** Black player property value */
constexpr player_t PLAYER_BLACK = 0;

/** Pieces property values */
constexpr piece_t PIECE_EMPTY =     0b000;
constexpr piece_t PIECE_PAWN =      0b001;
constexpr piece_t PIECE_KNIGHT =    0b010;
constexpr piece_t PIECE_BISHOP =    0b011;
constexpr piece_t PIECE_ROOK =      0b100;
constexpr piece_t PIECE_QUEEN =     0b101;
constexpr piece_t PIECE_KING =      0b110;
constexpr piece_t PIECE_INVALID =   0b111;

/** Castling rights property values */
constexpr castling_rights_t CASTLING_RIGHTS_WHITE_SHORT = 0b0001;
constexpr castling_rights_t CASTLING_RIGHTS_WHITE_LONG = 0b0010;
constexpr castling_rights_t CASTLING_RIGHTS_BLACK_SHORT = 0b0100;
constexpr castling_rights_t CASTLING_RIGHTS_BLACK_LONG = 0b1000;

/*  @} */ // core-defs

/** @defgroup core-api Core API functions
 *  @{
 */

/** Fills board states with possible candidate moves in current postion for given player
 *
 * @param moves - Pointer to an array of `board_state_t` elements to be written to. Available
 *                memory has to be sufficient to store at least 120 candidate moves.
 * @param board - `board_state_t` which represents current position on the board.
 * @param player - Player to make one of the candidate moves.
 *
 * @return Pointer to element past the last filled out candidate move. Number of generated
 *         candidate moves can be calculated with the pointer difference between passed `moves`
 *         argument and the return value.
 */
board_state_t* fill_candidate_moves(board_state_t* moves, const board_state_t& board,
    const player_t player);

/** Checks whether current `board_state_t` is valid in terms of `last_move_t` stored in metabits.
 *
 *  @param board - `board_state_t` which represents current position on the board.
 *
 *  @return - `true` if last move stored in the board state is represented correctly on the fields
 *            of the board, otherwise `false`.
 */
bool validate_board_state(const board_state_t& board);

/** Compares fields of two `board_state_t`'s and their castling rights
 *  This function is useful to determine in two abstract chess positions are the same. For that
 *  reason castling rights are taken into account, but not last move.
 *
 *  @param board - `board_state_t` which represents current position on the board.
 *
 *  @return - `true` if field pieces and playes are equal on each position and castling rights are
 *            the same, `false` otherwise.
 *
 *  @note - Assumption in this function is not 100% correct. Last move made also matters when
 *          determining if two positions are the same, due to en-passant pawn moves, when are
 *          allowed only after specific move has been done by the opponent. For that reason, two
 *          board states that are compared equal by this function, may generate different candidate
 *          moves based on the stored `last_move_t` in the metabits.
 */
bool compare_simple_position(const board_state_t& lhs, const board_state_t& rhs);

/*  @} */ // core-api

/** @defgroup private-impl Private implementation
 *  @{
 */
namespace
{

/** @defgroup private-bitfield Bitfield helpers
 *  @{
 */
namespace bitfield
{

template <typename T>
struct property_descriptor_s
{
    std::size_t bit_pos;
    std::size_t bit_width;
    std::size_t mask;

    constexpr property_descriptor_s(std::size_t pos, std::size_t width);
};

template <typename T>
constexpr T property_mask(const property_descriptor_s<T> desc)
{
    // Example:
    // For descriptor = { bit_pos = 4, bit_width = 3 }
    // Mask = ~( 0b10000000 | 0b00001111 ) = ~ 0b10001111 = 0b01110000
    return ~(0xFF << (desc.bit_width + desc.bit_pos) | ~(0xFF << desc.bit_pos));
}

template <typename T>
constexpr T get_property(const T field, const property_descriptor_s<T> desc)
{
    return (field & desc.mask) >> desc.bit_pos;
}

template <typename T, typename U>
constexpr T set_property(const T field, const U prop, const property_descriptor_s<T> desc)
{
    return (field & ~desc.mask) | (prop << desc.bit_pos);
}

template <typename T>
constexpr property_descriptor_s<T>::property_descriptor_s(std::size_t pos, std::size_t width)
: bit_pos{ pos }, bit_width{ width }, mask{ bitfield::property_mask<T>(*this) }
{
}

}  // namespace bitfield

/*  @} */ // private-bitfield

/** @defgroup private-desc Private bitfield descriptors
 *  @{
 */

/** Player property descriptor
  * Occupies bit 0 of `field_state_t`
  */
constexpr bitfield::property_descriptor_s<field_state_t> FIELD_PLAYER_DESC = { 0, 1 };

/** Piece property descriptor
  * Occupies bits 1-3 of `field_state_t`
  */
constexpr bitfield::property_descriptor_s<field_state_t> FIELD_PIECE_DESC = { 1, 3 };

/** Under white attack property descriptor
  * Occupies bit 4 of `field_state_t`
  */
constexpr bitfield::property_descriptor_s<field_state_t> FIELD_UNDER_WHITE_ATTACK_DESC = { 4, 1 };

/** Under black attack property descriptor
  * Occupies bit 5 of `field_state_t`
  */
constexpr bitfield::property_descriptor_s<field_state_t> FIELD_UNDER_BLACK_ATTACK_DESC = { 5, 1 };

/** Meta bits property descriptor
  * Occupies bits 6-7 of `field_state_t`
  */
constexpr bitfield::property_descriptor_s<field_state_t> FIELD_META_BITS_DESC = { 6, 2 };

/** Player property descriptor of last move struct
  * Occupies bit 0 of `last_move_t`
  */
constexpr bitfield::property_descriptor_s<last_move_t> LAST_MOVE_PLAYER_DESC = { 0, 1 };

/** Piece property descriptor of last move struct
  * Occupies bits 1-3 of `last_move_t`
  */
constexpr bitfield::property_descriptor_s<last_move_t> LAST_MOVE_PIECE_DESC = { 1, 3 };

/** From property descriptor of last move struct
  * Occupies bits 4-9 of `last_move_t`
  */
constexpr bitfield::property_descriptor_s<last_move_t> LAST_MOVE_FROM_DESC = { 4, 6 };

/** To property descriptor of last move struct
  * Occupies bits 10-15 of `last_move_t`
  */
constexpr bitfield::property_descriptor_s<last_move_t> LAST_MOVE_TO_DESC = { 10, 6 };

/** Last move property descriptor
  * Occupies bits 0-15 of combined meta bits
  */
constexpr bitfield::property_descriptor_s<field_meta_bits_t> META_BITS_LAST_MOVE_DESC = { 0, 16 };

/** Last move property descriptor
  * Occupies bits 16-19 of combined meta bits
  */
constexpr bitfield::property_descriptor_s<field_meta_bits_t> META_BITS_CASTLING_DESC = { 16, 4 };

/*  @} */ // private-desc

/** Sets value of a meta bits property in `board_state_t` */
template <typename T, typename U>
constexpr void board_state_meta_set_bits(
    board_state_t& board, T value, const bitfield::property_descriptor_s<U> desc) {

    const auto field_meta_bits_width = FIELD_META_BITS_DESC.bit_width;
    auto consume = [&](const auto bit_pos, const auto bit_width, const auto field_idx)
    {
        auto meta_bits = field_get_meta_bits(board[field_idx]);
        auto value_to_set = bitfield::get_property(value, { 0u, bit_width });
        meta_bits = bitfield::set_property(meta_bits, value_to_set, { bit_pos, bit_width });
        board[field_idx] = field_set_meta_bits(board[field_idx], meta_bits);
        value >>= bit_width;
    };

    const auto start_field_idx = desc.bit_pos / field_meta_bits_width;
    const auto end_field_idx = (desc.bit_pos + desc.bit_width) / field_meta_bits_width;

    consume(desc.bit_pos % field_meta_bits_width,
        field_meta_bits_width - (desc.bit_pos % field_meta_bits_width), start_field_idx);
    for (auto field_idx = start_field_idx + 1; field_idx < end_field_idx - 1; ++field_idx) {
        consume(0u, field_meta_bits_width, field_idx);
    }
    consume(0u, (end_field_idx - 1) * field_meta_bits_width - (desc.bit_pos + desc.bit_width),
        end_field_idx - 1);
}

/** Gets value of a meta bits property in `board_state_t` */
template <typename T, typename U>
constexpr T board_state_meta_get_bits(
    const board_state_t& board, const bitfield::property_descriptor_s<U> desc) {
    T result = {};
    const auto field_meta_bits_width = FIELD_META_BITS_DESC.bit_width;
    const auto start_field_idx = desc.bit_pos / field_meta_bits_width;
    const auto end_field_idx = (desc.bit_pos + desc.bit_width) / field_meta_bits_width;

    auto field_shift_idx = 0u;
    for (std::size_t field_idx = start_field_idx;
         field_idx < end_field_idx;
         ++field_idx, ++field_shift_idx) {
        auto shift = field_shift_idx * field_meta_bits_width;
        field_meta_bits_t meta_bits = field_get_meta_bits(board[field_idx]);
        result |= meta_bits << shift;
    }
    return result;
}

bool check_last_move(const board_state_t& board, const move_s& move) {
    last_move_t last_move = board_state_meta_get_last_move(board);
    player_t last_move_player = last_move_get_player(last_move);
    piece_t last_move_piece = last_move_get_piece(last_move);
    field_t last_move_from = last_move_get_from(last_move);
    field_t last_move_to = last_move_get_to(last_move);

    return move.player == last_move_player and
        move.piece == last_move_piece and
        move.from == last_move_from and
        move.to == last_move_to;
}

bool is_king_under_attack(const board_state_t& board, const player_t player) {
    for (const auto field : board) {
        if (PIECE_KING == field_get_piece(field) and player == field_get_player(field)) {
            return PLAYER_WHITE == player
                ? field_under_black_attack(field)
                : field_under_white_attack(field);
        }
    }
    return false;
}

void clear_fields_under_attack(board_state_t& board) {
    for (auto& field : board) {
        field = field_clear_under_white_attack(field);
        field = field_clear_under_black_attack(field);
    }
}

void update_field_under_attack(board_state_t& board, const field_t field, const player_t player) {
    if (field_t::INVALID != field) {
        if (PLAYER_WHITE == player) {
            board[field] = field_set_under_white_attack(board[field]);
        } else {
            board[field] = field_set_under_black_attack(board[field]);
        }
    }
}

void update_pawn_fields_under_attack(
    board_state_t& board, const field_t field, const player_t player) {
    if (PLAYER_WHITE == player) {
        update_field_under_attack(board, field_left_up(field), player);
        update_field_under_attack(board, field_right_up(field), player);
    } else {
        update_field_under_attack(board, field_left_down(field), player);
        update_field_under_attack(board, field_right_down(field), player);
    }
}

void update_knight_fields_under_attack(
    board_state_t& board, const field_t field, const player_t player) {
    update_field_under_attack(board, field_up(field_left_up(field)), player);
    update_field_under_attack(board, field_up(field_right_up(field)), player);
    update_field_under_attack(board, field_left(field_left_up(field)), player);
    update_field_under_attack(board, field_left(field_left_down(field)), player);
    update_field_under_attack(board, field_down(field_left_down(field)), player);
    update_field_under_attack(board, field_down(field_right_down(field)), player);
    update_field_under_attack(board, field_right(field_right_up(field)), player);
    update_field_under_attack(board, field_right(field_right_down(field)), player);
}

void update_ranged_fields_under_attack_op(
    board_state_t& board, const field_t field, const player_t player,
    field_t(*operation)(const field_t)) {
    field_t target_field = field;
    do {
        target_field = operation(target_field);
        if (field_t::INVALID == target_field) break;
        if (PIECE_EMPTY == field_get_piece(board[target_field])) {
            update_field_under_attack(board, target_field, player);
            continue;
        }
        if (player != field_get_player(board[target_field])) {
            update_field_under_attack(board, target_field, player);
            break;
        }
        break;
    } while (true);
}

void update_diagonal_fields_under_attack(
    board_state_t& board, const field_t field, const player_t player) {
    update_ranged_fields_under_attack_op(board, field, player, field_left_up);
    update_ranged_fields_under_attack_op(board, field, player, field_left_down);
    update_ranged_fields_under_attack_op(board, field, player, field_right_up);
    update_ranged_fields_under_attack_op(board, field, player, field_right_down);
}

void update_cross_fields_under_attack(
    board_state_t& board, const field_t field, const player_t player) {
    update_ranged_fields_under_attack_op(board, field, player, field_up);
    update_ranged_fields_under_attack_op(board, field, player, field_down);
    update_ranged_fields_under_attack_op(board, field, player, field_right);
    update_ranged_fields_under_attack_op(board, field, player, field_left);
}

void update_king_fields_under_attack(
    board_state_t& board, const field_t field, const player_t player) {
    update_field_under_attack(board, field_up(field), player);
    update_field_under_attack(board, field_right_up(field), player);
    update_field_under_attack(board, field_right(field), player);
    update_field_under_attack(board, field_right_down(field), player);
    update_field_under_attack(board, field_down(field), player);
    update_field_under_attack(board, field_left_down(field), player);
    update_field_under_attack(board, field_left(field), player);
    update_field_under_attack(board, field_left_up(field), player);
}

void update_fields_under_attack(board_state_t& board) {
    clear_fields_under_attack(board);
    for (uint8_t field_idx = static_cast<uint8_t>(field_t::BEGIN);
         field_idx < static_cast<uint8_t>(field_t::END);
         ++field_idx) {
        field_t field = static_cast<field_t>(field_idx);
        piece_t piece = field_get_piece(board[field]);
        player_t player = field_get_player(board[field]);
        switch (piece) {
            case PIECE_EMPTY: break;
            case PIECE_PAWN: update_pawn_fields_under_attack(board, field, player); break;
            case PIECE_KNIGHT: update_knight_fields_under_attack(board, field, player); break;
            case PIECE_BISHOP: update_diagonal_fields_under_attack(board, field, player); break;
            case PIECE_ROOK: update_cross_fields_under_attack(board, field, player); break;
            case PIECE_QUEEN:
                update_diagonal_fields_under_attack(board, field, player);
                update_cross_fields_under_attack(board, field, player);
                break;
            case PIECE_KING: update_king_fields_under_attack(board, field, player); break;
        }
    }
}

void update_last_move(board_state_t& board, const move_s& move) {
    last_move_t last_move = {};
    last_move = last_move_set_player(last_move, move.player);
    last_move = last_move_set_piece(last_move, move.piece);
    last_move = last_move_set_from(last_move, move.from);
    last_move = last_move_set_to(last_move, move.to);
    board_state_meta_set_last_move(board, last_move);
}

void update_castling_rights(board_state_t& board, const move_s& move) {
    if (PLAYER_WHITE == move.player) {
        if (PIECE_KING == move.piece) {
            castling_rights_t rights = board_state_meta_get_castling_rights(board);
            rights = castling_rights_remove_white_long(rights);
            rights = castling_rights_remove_white_short(rights);
            board_state_meta_set_castling_rights(board, rights);
        } else if (PIECE_ROOK == move.piece and rank_t::_1 == field_rank(move.from) and
            (file_t::A == field_file(move.from) or file_t::H == field_file(move.from))) {
            castling_rights_t rights = board_state_meta_get_castling_rights(board);
            rights = (file_t::A == field_file(move.from)
                ? castling_rights_remove_white_long(rights)
                : castling_rights_remove_white_short(rights));
            board_state_meta_set_castling_rights(board, rights);
        }
    } else {
        if (PIECE_KING == move.piece) {
            castling_rights_t rights = board_state_meta_get_castling_rights(board);
            rights = castling_rights_remove_black_long(rights);
            rights = castling_rights_remove_black_short(rights);
            board_state_meta_set_castling_rights(board, rights);
        } else if (PIECE_ROOK == move.piece and rank_t::_8 == field_rank(move.from) and
            (file_t::A == field_file(move.from) or file_t::H == field_file(move.from))) {
            castling_rights_t rights = board_state_meta_get_castling_rights(board);
            rights = (file_t::A == field_file(move.from)
                ? castling_rights_remove_black_long(rights)
                : castling_rights_remove_black_short(rights));
            board_state_meta_set_castling_rights(board, rights);
        }
    }
}

board_state_t* apply_move_if_valid(board_state_t* moves, const move_s& move) {
    auto& board = *moves;
    board[move.from] = field_set_piece(board[move.from], PIECE_EMPTY);
    board[move.to] = field_set_piece(field_set_player(board[move.to], move.player), move.piece);

    update_fields_under_attack(board);
    if (not is_king_under_attack(board, move.player)) {
        update_last_move(board, move);
        update_castling_rights(board, move);
        return moves + 1;
    }
    return moves;
}

board_state_t* add_white_pawn_move_up(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_up(field);
    if (field_t::INVALID == target_field or PIECE_EMPTY != field_get_piece(board[target_field]))
        return moves;

    if (rank_t::_8 == field_rank(target_field)) {
        auto& move1 = *moves;
        move1 = board;
        moves = apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
        move1[target_field] = field_set_piece(move1[target_field], PIECE_KNIGHT);

        auto& move2 = *moves;
        move2 = board;
        moves = apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
        move2[target_field] = field_set_piece(move2[target_field], PIECE_BISHOP);

        auto& move3 = *moves;
        move3 = board;
        moves = apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
        move3[target_field] = field_set_piece(move3[target_field], PIECE_ROOK);

        auto& move4 = *moves;
        move4 = board;
        moves = apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
        move4[target_field] = field_set_piece(move4[target_field], PIECE_QUEEN);
        return moves;
    } else {
        *moves = board;
        return apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
    }
}

board_state_t* add_white_pawn_move_up_long(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    if (rank_t::_2 != field_rank(field)) return moves;
    field_t target_field = field_up(field);
    if (field_t::INVALID == target_field or PIECE_EMPTY != field_get_piece(board[target_field]))
        return moves;
    target_field = field_up(target_field);
    if (field_t::INVALID == target_field or PIECE_EMPTY != field_get_piece(board[target_field]))
        return moves;

    *moves = board;
    return apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
}

board_state_t* add_white_pawn_capture_left_up(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_left_up(field);
    if (field_t::INVALID == target_field or
        PIECE_EMPTY == field_get_piece(board[target_field]) or
        PLAYER_BLACK != field_get_player(board[target_field]))
        return moves;

    *moves = board;
    return apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
}

board_state_t* add_white_pawn_capture_right_up(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_right_up(field);
    if (field_t::INVALID == target_field or
        PIECE_EMPTY == field_get_piece(board[target_field]) or
        PLAYER_BLACK != field_get_player(board[target_field]))
        return moves;

    *moves = board;
    return apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
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
    (*moves)[opps_move_to] = field_set_piece((*moves)[opps_move_to], PIECE_EMPTY);
    return apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
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
    (*moves)[opps_move_to] = field_set_piece((*moves)[opps_move_to], PIECE_EMPTY);
    return apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_PAWN, field, target_field });
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
    if (field_t::INVALID == target_field or PIECE_EMPTY != field_get_piece(board[target_field]))
        return moves;

    if (rank_t::_1 == field_rank(target_field)) {
        auto& move1 = *moves;
        move1 = board;
        moves = apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
        move1[target_field] = field_set_piece(move1[target_field], PIECE_KNIGHT);

        auto& move2 = *moves;
        move2 = board;
        moves = apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
        move2[target_field] = field_set_piece(move2[target_field], PIECE_BISHOP);

        auto& move3 = *moves;
        move3 = board;
        moves = apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
        move3[target_field] = field_set_piece(move3[target_field], PIECE_ROOK);

        auto& move4 = *moves;
        move4 = board;
        moves = apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
        move4[target_field] = field_set_piece(move4[target_field], PIECE_QUEEN);
        return moves + 4;
    } else {
        *moves = board;
        return apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
    }
}

board_state_t* add_black_pawn_move_down_long(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    if (rank_t::_7 != field_rank(field)) return moves;
    field_t target_field = field_down(field);
    if (field_t::INVALID == target_field or PIECE_EMPTY != field_get_piece(board[target_field]))
        return moves;
    target_field = field_down(target_field);
    if (field_t::INVALID == target_field or PIECE_EMPTY != field_get_piece(board[target_field]))
        return moves;

    *moves = board;
    return apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
}

board_state_t* add_black_pawn_capture_left_down(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_left_down(field);
    if (field_t::INVALID == target_field or
        PIECE_EMPTY == field_get_piece(board[target_field]) or
        PLAYER_WHITE != field_get_player(board[target_field]))
        return moves;

    *moves = board;
    return apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
}

board_state_t* add_black_pawn_capture_right_down(
    board_state_t* moves, const board_state_t& board, const field_t field) {
    field_t target_field = field_right_down(field);
    if (field_t::INVALID == target_field or
        PIECE_EMPTY == field_get_piece(board[target_field]) or
        PLAYER_WHITE != field_get_player(board[target_field]))
        return moves;

    *moves = board;
    return apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
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
    (*moves)[opps_move_to] = field_set_piece((*moves)[opps_move_to], PIECE_EMPTY);
    return apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
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
    (*moves)[opps_move_to] = field_set_piece((*moves)[opps_move_to], PIECE_EMPTY);
    return apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_PAWN, field, target_field });
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
    if (field_t::INVALID == target_field or
        (PIECE_EMPTY != field_get_piece(board[target_field]) and 
         player == field_get_player(board[target_field])) or
        (PIECE_KING == piece and (
            (PLAYER_WHITE == player and field_under_black_attack(board[target_field])) or
            (PLAYER_BLACK == player and field_under_white_attack(board[target_field])))))
        return moves;

    *moves = board;
    return apply_move_if_valid(moves, { player, piece, field, target_field });
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
        if (field_t::INVALID == target_field) break;
        if (PIECE_EMPTY == field_get_piece(board[target_field])) {
            *moves = board;
            moves = apply_move_if_valid(moves, { player, piece, field, target_field });
            continue;
        }
        if (player != field_get_player(board[target_field])) {
            *moves = board;
            moves = apply_move_if_valid(moves, { player, piece, field, target_field });
            break;
        }
        break;
    } while (true);
    return moves;
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
        PLAYER_WHITE != field_get_player(board[H1]) or
        PIECE_ROOK != field_get_piece(board[H1]) or
        PIECE_EMPTY != field_get_piece(board[F1]) or
        PIECE_EMPTY != field_get_piece(board[G1]) or
        !castling_rights_white_short(board_state_meta_get_castling_rights(board)) or
        field_under_black_attack(board[E1]) or
        field_under_black_attack(board[F1]) or
        field_under_black_attack(board[G1]))
        return moves;

    auto& move = *moves = board;
    move[H1] = field_set_piece(move[H1], PIECE_EMPTY);
    move[F1] = field_set_piece(field_set_player(move[F1], PLAYER_WHITE), PIECE_ROOK);
    return apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_KING, E1, G1 });
}

board_state_t* fill_black_short_castle(
    board_state_t* moves, const board_state_t& board, const field_t field)
{
    if (E8 != field or
        PLAYER_BLACK != field_get_player(board[H8]) or
        PIECE_ROOK != field_get_piece(board[H8]) or
        PIECE_EMPTY != field_get_piece(board[F8]) or
        PIECE_EMPTY != field_get_piece(board[G8]) or
        !castling_rights_black_short(board_state_meta_get_castling_rights(board)) or
        field_under_white_attack(board[E8]) or
        field_under_white_attack(board[F8]) or
        field_under_white_attack(board[G8]))
        return moves;

    auto& move = *moves = board;
    move[H8] = field_set_piece(move[H8], PIECE_EMPTY);
    move[F8] = field_set_piece(field_set_player(move[F8], PLAYER_BLACK), PIECE_ROOK);
    return apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_KING, E8, G8 });
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
        PLAYER_WHITE != field_get_player(board[A1]) or
        PIECE_ROOK != field_get_piece(board[A1]) or
        PIECE_EMPTY != field_get_piece(board[B1]) or
        PIECE_EMPTY != field_get_piece(board[C1]) or
        PIECE_EMPTY != field_get_piece(board[D1]) or
        !castling_rights_white_long(board_state_meta_get_castling_rights(board)) or
        field_under_black_attack(board[C1]) or
        field_under_black_attack(board[D1]) or
        field_under_black_attack(board[E1]))
        return moves;

    auto& move = *moves = board;
    move[A1] = field_set_piece(move[A1], PIECE_EMPTY);
    move[D1] = field_set_piece(field_set_player(move[D1], PLAYER_WHITE), PIECE_ROOK);
    return apply_move_if_valid(moves, { PLAYER_WHITE, PIECE_KING, E1, C1 });
}

board_state_t* fill_black_long_castle(
    board_state_t* moves, const board_state_t& board, const field_t field)
{
    if (E8 != field or
        PLAYER_BLACK != field_get_player(board[A8]) or
        PIECE_ROOK != field_get_piece(board[A8]) or
        PIECE_EMPTY != field_get_piece(board[B8]) or
        PIECE_EMPTY != field_get_piece(board[C8]) or
        PIECE_EMPTY != field_get_piece(board[D8]) or
        !castling_rights_black_long(board_state_meta_get_castling_rights(board)))
        return moves;

    auto& move = *moves = board;
    move[A8] = field_set_piece(move[A8], PIECE_EMPTY);
    move[D8] = field_set_piece(field_set_player(move[D8], PLAYER_BLACK), PIECE_ROOK);
    return apply_move_if_valid(moves, { PLAYER_BLACK, PIECE_KING, E8, C8 });
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

}  // namespace

/*  @} */ // private-impl

/** @defgroup impl Implementation of public functions
 *  @{
 */

constexpr player_t opponent(const player_t player) { return ~player & 0b1; }

constexpr field_state_t field_get_player(const field_state_t field) {
    return bitfield::get_property(field, FIELD_PLAYER_DESC);
}

constexpr field_state_t field_set_player(const field_state_t field, const player_t player) {
    return bitfield::set_property(field, player, FIELD_PLAYER_DESC);
}

constexpr field_state_t field_get_piece(const field_state_t field) {
    return bitfield::get_property(field, FIELD_PIECE_DESC);
}

constexpr field_state_t field_set_piece(const field_state_t field, const piece_t piece) {
    return bitfield::set_property(field, piece, FIELD_PIECE_DESC);
}

constexpr field_state_t field_set_under_white_attack(const field_state_t field) {
    return bitfield::set_property(field, 1, FIELD_UNDER_WHITE_ATTACK_DESC);
}

constexpr field_state_t field_clear_under_white_attack(const field_state_t field) {
    return bitfield::set_property(field, 0, FIELD_UNDER_WHITE_ATTACK_DESC);
}

constexpr bool field_under_white_attack(const field_state_t field) {
    return bitfield::get_property(field, FIELD_UNDER_WHITE_ATTACK_DESC);
}

constexpr field_state_t field_set_under_black_attack(const field_state_t field) {
    return bitfield::set_property(field, 1, FIELD_UNDER_BLACK_ATTACK_DESC);
}

constexpr field_state_t field_clear_under_black_attack(const field_state_t field) {
    return bitfield::set_property(field, 0, FIELD_UNDER_BLACK_ATTACK_DESC);
}

constexpr bool field_under_black_attack(const field_state_t field) {
    return bitfield::get_property(field, FIELD_UNDER_BLACK_ATTACK_DESC);
}

constexpr field_state_t field_get_meta_bits(const field_state_t field) {
    return bitfield::get_property(field, FIELD_META_BITS_DESC);
}

constexpr field_state_t field_set_meta_bits(const field_state_t field,
    const field_meta_bits_t meta_bits) {
    return bitfield::set_property(field, meta_bits, FIELD_META_BITS_DESC);
}

constexpr last_move_t last_move_set_player(last_move_t last_move, player_t player) {
    return bitfield::set_property(last_move, player, LAST_MOVE_PLAYER_DESC);
}

constexpr player_t last_move_get_player(last_move_t last_move) {
    return static_cast<player_t>(bitfield::get_property(last_move, LAST_MOVE_PLAYER_DESC));
}

constexpr last_move_t last_move_set_piece(last_move_t last_move, piece_t piece) {
    return bitfield::set_property(last_move, piece, LAST_MOVE_PIECE_DESC);
}

constexpr piece_t last_move_get_piece(last_move_t last_move) {
    return static_cast<piece_t>(bitfield::get_property(last_move, LAST_MOVE_PIECE_DESC));
}

constexpr last_move_t last_move_set_from(last_move_t last_move, field_t from) {
    return bitfield::set_property(last_move, from, LAST_MOVE_FROM_DESC);
}

constexpr field_t last_move_get_from(last_move_t last_move) {
    return static_cast<field_t>(bitfield::get_property(last_move, LAST_MOVE_FROM_DESC));
}

constexpr last_move_t last_move_set_to(last_move_t last_move, field_t to) {
    return bitfield::set_property(last_move, to, LAST_MOVE_TO_DESC);
}

constexpr field_t last_move_get_to(last_move_t last_move) {
    return static_cast<field_t>(bitfield::get_property(last_move, LAST_MOVE_TO_DESC));
}

constexpr castling_rights_t castling_rights_remove_white_short(const castling_rights_t rights) {
    return rights | CASTLING_RIGHTS_WHITE_SHORT;
}

constexpr bool castling_rights_white_short(const castling_rights_t rights) {
    return !(rights & CASTLING_RIGHTS_WHITE_SHORT);
}

constexpr castling_rights_t castling_rights_remove_white_long(const castling_rights_t rights) {
    return rights | CASTLING_RIGHTS_WHITE_LONG;
}

constexpr bool castling_rights_white_long(const castling_rights_t rights) {
    return !(rights & CASTLING_RIGHTS_WHITE_LONG);
}

constexpr castling_rights_t castling_rights_remove_black_short(const castling_rights_t rights) {
    return rights | CASTLING_RIGHTS_BLACK_SHORT;
}

constexpr bool castling_rights_black_short(const castling_rights_t rights) {
    return !(rights & CASTLING_RIGHTS_BLACK_SHORT);
}

constexpr castling_rights_t castling_rights_remove_black_long(const castling_rights_t rights) {
    return rights | CASTLING_RIGHTS_BLACK_LONG;
}

constexpr bool castling_rights_black_long(const castling_rights_t rights) {
    return !(rights & CASTLING_RIGHTS_BLACK_LONG);
}

constexpr void board_state_meta_set_last_move(board_state_t& board, const last_move_t last_move) {
    board_state_meta_set_bits(board, last_move, META_BITS_LAST_MOVE_DESC);
}

constexpr last_move_t board_state_meta_get_last_move(const board_state_t& board) {
    return board_state_meta_get_bits<last_move_t>(board, META_BITS_LAST_MOVE_DESC);
}

constexpr void board_state_meta_set_castling_rights(
    board_state_t& board, const castling_rights_t rights) {
    board_state_meta_set_bits(board, rights, META_BITS_CASTLING_DESC);
}

constexpr castling_rights_t board_state_meta_get_castling_rights(const board_state_t& board) {
    return board_state_meta_get_bits<castling_rights_t>(board, META_BITS_CASTLING_DESC);
}

constexpr file_t field_file(const field_t field) {
    return field != field_t::INVALID
        ? static_cast<file_t>(
            static_cast<uint8_t>(field) % static_cast<uint8_t>(file_t::MAX))
        : file_t::MAX;
}

constexpr rank_t field_rank(const field_t field) {
    return field != field_t::INVALID
        ? static_cast<rank_t>(
            static_cast<uint8_t>(field) / static_cast<uint8_t>(rank_t::MAX))
        : rank_t::MAX;
}

constexpr field_t make_field(const uint8_t file, const uint8_t rank) {
    return (static_cast<uint8_t>(file_t::MAX) <= file or
        static_cast<uint8_t>(rank_t::MAX) <= rank)
        ? field_t::INVALID
        : static_cast<field_t>(rank * static_cast<uint8_t>(rank_t::MAX) + file);
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

board_state_t* fill_candidate_moves(
    board_state_t* moves, const board_state_t& board, const player_t player) {
    auto temp_moves = moves;
    for (uint8_t field_idx = static_cast<uint8_t>(field_t::BEGIN);
         field_idx < static_cast<uint8_t>(field_t::END);
         ++field_idx) {
        if (player != field_get_player(board[field_idx])) continue;

        field_t field = static_cast<field_t>(field_idx);
        piece_t piece = field_get_piece(board[field_idx]);
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

bool validate_board_state(const board_state_t& board) {
    last_move_t last_move = board_state_meta_get_last_move(board);
    player_t last_move_player = last_move_get_player(last_move);
    piece_t last_move_piece = last_move_get_piece(last_move);
    field_t last_move_from = last_move_get_from(last_move);
    field_t last_move_to = last_move_get_to(last_move);

    return last_move_piece == PIECE_EMPTY or
        last_move_player == field_get_player(board[last_move_to]) or
        last_move_piece == field_get_piece(board[last_move_to]) or
        PIECE_EMPTY == field_get_piece(board[last_move_from]);
}

bool compare_simple_position(const board_state_t& lhs, const board_state_t& rhs) {
    for (uint8_t field_idx = static_cast<uint8_t>(field_t::BEGIN);
         field_idx < static_cast<uint8_t>(field_t::END);
         ++field_idx) {
        auto left_field = lhs[field_idx];
        auto right_field = rhs[field_idx];
        if (field_get_piece(left_field) != field_get_piece(right_field) or
            (PIECE_EMPTY != field_get_piece(left_field) and
            field_get_player(left_field) != field_get_player(right_field))) {
            return false;
        }
    }
    return board_state_meta_get_castling_rights(lhs) == board_state_meta_get_castling_rights(rhs);
}

/*  @} */ // impl

/** @defgroup extra-defs Extra definitions for user's convenience
 *  @{
 */

/** Basic field constants */
namespace {
constexpr field_state_t FF = 0;
constexpr field_state_t FW = field_set_player(FF, PLAYER_WHITE);
constexpr field_state_t FB = field_set_player(FF, PLAYER_BLACK);
}  // namespace

/** White player's pieces constants */
constexpr field_state_t FWP = field_set_piece(FW, PIECE_PAWN);
constexpr field_state_t FWN = field_set_piece(FW, PIECE_KNIGHT);
constexpr field_state_t FWB = field_set_piece(FW, PIECE_BISHOP);
constexpr field_state_t FWR = field_set_piece(FW, PIECE_ROOK);
constexpr field_state_t FWQ = field_set_piece(FW, PIECE_QUEEN);
constexpr field_state_t FWK = field_set_piece(FW, PIECE_KING);

/** Black player's pieces constants */
constexpr field_state_t FBP = field_set_piece(FB, PIECE_PAWN);
constexpr field_state_t FBN = field_set_piece(FB, PIECE_KNIGHT);
constexpr field_state_t FBB = field_set_piece(FB, PIECE_BISHOP);
constexpr field_state_t FBR = field_set_piece(FB, PIECE_ROOK);
constexpr field_state_t FBQ = field_set_piece(FB, PIECE_QUEEN);
constexpr field_state_t FBK = field_set_piece(FB, PIECE_KING);

/** Convenient definition of starting chess board */
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

/** Convenient definition of empty chess board */
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

/*  @} */ // extra-defs

}  // namespace chess

#endif  // CHESS_HPP_
