#include <memory>
#include <algorithm>
#include "chess/core.hpp"
#include "chesstest.hpp"

using namespace chess;

static void temp_print_c_moves(const board_state_t* c_moves_beg, const board_state_t* c_moves_end) {
    // for (auto it = c_moves_beg; it != c_moves_end; ++it)
    //     draw_board(*it);

    test_output << c_moves_end - c_moves_beg << " candidate moves.\n";
}

board_state_t prepare_board(std::function<void(board_state_t&)> setup_fn) {
    auto board = chess::EMPTY_BOARD;
    setup_fn(board);
    update_fields_under_attack(board);
    // draw_board(board);
    return board;
}

std::unique_ptr<board_state_t[]> prepare_moves() {
    return std::make_unique<board_state_t[]>(64);
}

TEST(Internal_StaticEvaluation_FieldProperties) {
    static_assert(file_t::MAX == field_file(field_t::INVALID),
        "Unknown file for invalid field");
    static_assert(rank_t::MAX == field_rank(field_t::INVALID),
        "Unknown rank for invalid field");
    static_assert(file_t::A == field_file(A4), "A-file for field A4");
    static_assert(file_t::H == field_file(H8), "H-file for field H8");
    static_assert(rank_t::_4 == field_rank(A4), "4th-rank for field A4");
    static_assert(rank_t::_8 == field_rank(H8), "8th-rank for field H8");
    static_assert(field_t::INVALID == make_field(150, 0), "(150,0) results in invalid field");
    static_assert(field_t::INVALID == make_field(3, 69), "(3,69) results in invalid field");
    static_assert(A4 == make_field(0, 3), "(0,3) results in field A4");
    static_assert(H8 == make_field(7, 7), "(7,7) results in field H8");
}

TEST(Internal_StaticEvaluation_FieldGeneration) {
    static_assert(D5 == field_up(D4), "D4 --UP--> D5");
    static_assert(D3 == field_down(D4), "D4 --DOWN--> D3");
    static_assert(C4 == field_left(D4), "D4 --LEFT--> C4");
    static_assert(E4 == field_right(D4), "D4 --RIGHT--> E4");
    static_assert(C5 == field_left_up(D4), "D4 --LEFT--UP--> C5");
    static_assert(E5 == field_right_up(D4), "D4 --RIGHT--UP--> E5");
    static_assert(C3 == field_left_down(D4), "D4 --LEFT--DOWN--> C3");
    static_assert(E3 == field_right_down(D4), "D4 --RIGHT--DOWN--> E3");
    static_assert(field_t::INVALID == field_up(H8), "H8 --UP--> invalid field");
    static_assert(field_t::INVALID == field_right(H8), "H8 --RIGHT--> invalid field");
    static_assert(field_t::INVALID == field_left_up(H8), "H8 --LEFT--UP--> invalid field");
    static_assert(field_t::INVALID == field_right_up(H8), "H8 --RIGHT--UP--> invalid field");
    static_assert(field_t::INVALID == field_right_down(H8), "H8 --RIGHT--DOWN--> invalid field");
    static_assert(field_t::INVALID == field_down(A1), "A1 --DOWN--> invalid field");
    static_assert(field_t::INVALID == field_left(A1), "A1 --LEFT--> invalid field");
    static_assert(field_t::INVALID == field_left_up(A1), "A1 --LEFT--UP--> invalid field");
    static_assert(field_t::INVALID == field_left_down(A1), "A1 --LEFT--DOWN--> invalid field");
    static_assert(field_t::INVALID == field_right_down(A1), "A1 --RIGHT--DOWN--> invalid field");
}

TEST(Internal_Meta_CheckLastMove_WhitePawnE2E3) {
    auto board = chess::EMPTY_BOARD;
    // first 2 bytest of meta = last move
    // 1 - white
    // 001 - pawn
    // 001100 - (12) E2
    // 010100 - (20) E3
    board[0] = 0b11000000;
    board[1] = 0b00000000;
    board[2] = 0b00000000;
    board[3] = 0b11000000;
    board[4] = 0b00000000;
    board[5] = 0b00000000;
    board[6] = 0b01000000;
    board[7] = 0b01000000;

    board[E3] = 0b0000011; // white pawn on E3

    ASSERT(check_last_move(board, { PLAYER_WHITE, PIECE_PAWN, E2, E3 }));
    ASSERT(!check_last_move(board, { PLAYER_BLACK, PIECE_PAWN, E2, E3 }));
}

TEST(Internal_Meta_CheckLastMove_BlackRookE3E2) {
    auto board = chess::EMPTY_BOARD;
    // first 2 bytest of meta = last move
    // 0 - black
    // 100 - rook
    // 010100 - (20) E3
    // 001100 - (12) E2
    board[0] = 0b00000000;
    board[1] = 0b10000000;
    board[2] = 0b00000000;
    board[3] = 0b01000000;
    board[4] = 0b01000000;
    board[5] = 0b00000000;
    board[6] = 0b11000000;
    board[7] = 0b00000000;

    board[E2] = 0b00001000; // black rook on E2

    ASSERT(!check_last_move(board, { PLAYER_WHITE, PIECE_ROOK, E2, E3 }));
    ASSERT(!check_last_move(board, { PLAYER_WHITE, PIECE_ROOK, E3, E2 }));
    ASSERT(check_last_move(board, { PLAYER_BLACK, PIECE_ROOK, E3, E2 }));
}

auto one_pawn_board(const field_t pawn_position, const field_state_t pawn_type = FWP) {
    return prepare_board([=](auto& board) {
        board[E1] = FWK;
        board[E8] = FBK;
        board[pawn_position] = pawn_type;
    });
}

const board_state_t* find_candidate_move(
    const board_state_t* moves, const board_state_t* moves_end, const move_s& last_move) {
    temp_print_c_moves(moves, moves_end);
    return std::find_if(moves, moves_end, [&last_move](const auto& board) {
            bool success = check_last_move(board, last_move);
            if (success) {
                test_output << "\nFound requested candidate move:\n\n";
                // draw_board(board);
            }
            return success;
        });
}

bool check_candidate_move(
    const board_state_t* moves, const board_state_t* moves_end, const move_s& last_move) {
    return moves_end != find_candidate_move(moves, moves_end, last_move);
}

bool all_candidate_moves_are_valid(const board_state_t* moves, const board_state_t* moves_end) {
    return std::all_of(moves, moves_end, [](const auto& board) {
            return validate_board_state(board);
        });
}

TEST(CandidateMoves_Pawn_White_MoveForward_E2E3) {
    auto board = one_pawn_board(E2);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, E2, E3 }));
}

TEST(CandidateMoves_Pawn_Black_MoveForward_E7E6) {
    auto board = one_pawn_board(E7, FBP);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, E7, E6 }));
}

TEST(CandidateMoves_Pawn_White_MoveLongForward_E2E4) {
    auto board = one_pawn_board(E2);

    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, E2, E4 }));
}

TEST(CandidateMoves_Pawn_Black_MoveLongForward_E7E5) {
    auto board = one_pawn_board(E7, FBP);

    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, E7, E5 }));
}

TEST(CandidateMoves_Pawn_White_MoveForward_D2D3) {
    auto board = one_pawn_board(D2);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, D2, D3 }));
}

TEST(CandidateMoves_Pawn_Black_MoveForward_D7DD6) {
    auto board = one_pawn_board(D7, FBP);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, D7, D6 }));
}

TEST(CandidateMoves_Pawn_White_MoveForward_D2D4) {
    auto board = one_pawn_board(D2);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, D2, D4 }));
}

TEST(CandidateMoves_Pawn_Black_MoveForward_D7D5) {
    auto board = one_pawn_board(D7, FBP);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, D7, D5 }));
}

TEST(CandidateMoves_Pawn_White_NoMoveForwardLong_D3D5) {
    auto board = one_pawn_board(D3);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, D3, D5 }));
}

TEST(CandidateMoves_Pawn_Black_NoMoveForwardLong_D6D4) {
    auto board = one_pawn_board(D6, FBP);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, D6, D4 }));
}

auto two_pawn_board(const field_t white_pawn_position, const field_t black_pawn_position) {
    return prepare_board([=](auto& board) {
        board[E1] = FWK;
        board[E8] = FBK;
        board[white_pawn_position] = FWP;
        board[black_pawn_position] = FBP;
    });
}

auto two_pawn_board(const field_t white_pawn_position, const field_t black_pawn_position,
    std::function<void(board_state_t&)> setup_fn) {
    return prepare_board([=](auto& board) {
        board[E1] = FWK;
        board[E8] = FBK;
        board[white_pawn_position] = FWP;
        board[black_pawn_position] = FBP;
        setup_fn(board);
    });
}

TEST(CandidateMoves_Pawn_White_NoMoveForwardBlocked_D2D3) {
    auto board = two_pawn_board(D2, D3);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, D2, D3 }));
}

TEST(CandidateMoves_Pawn_Black_NoMoveForwardBlocked_D7D6) {
    auto board = two_pawn_board(D6, D7);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, D7, D6 }));
}

TEST(CandidateMoves_Pawn_White_NoMoveForwardLongBlockedClose_D2D3) {
    auto board = two_pawn_board(D2, D3);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, D2, D4 }));
}

TEST(CandidateMoves_Pawn_Black_NoMoveForwardLongBlockedClose_D7D5) {
    auto board = two_pawn_board(D6, D7);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, D7, D5 }));
}

TEST(CandidateMoves_Pawn_White_NoMoveForwardLongBlockedFar_D2D3) {
    auto board = two_pawn_board(D2, D4);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, D2, D4 }));
}

TEST(CandidateMoves_Pawn_Black_NoMoveForwardLongBlockedFar_D7D5) {
    auto board = two_pawn_board(D5, D7);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, D7, D5 }));
}

TEST(CandidateMoves_Pawn_White_CaptureLeftUp) {
    auto board = two_pawn_board(E2, D3);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, E2, D3 }));
}

TEST(CandidateMoves_Pawn_White_CaptureRightUp) {
    auto board = two_pawn_board(E2, F3);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, E2, F3 }));
}

TEST(CandidateMoves_Pawn_Black_CaptureLeftDown) {
    auto board = two_pawn_board(E2, F3);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, F3, E2 }));
}

TEST(CandidateMoves_Pawn_Black_CaptureRightDown) {
    auto board = two_pawn_board(E2, D3);
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, D3, E2 }));
}

TEST(CandidateMoves_Pawn_White_CaptureEnPassantLeft) {
    auto board = two_pawn_board(E5, D7, [](auto& board) {
            apply_move_if_valid(&board, { PLAYER_BLACK, PIECE_PAWN, D7, D5 });
        });
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto found_move = find_candidate_move(
        c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, E5, D6 });
    ASSERT(c_moves_end != found_move);
    ASSERT(PIECE_EMPTY == field_get_piece((*found_move)[D5]));
}

TEST(CandidateMoves_Pawn_White_CaptureEnPassantRight) {
    auto board = two_pawn_board(E5, D7, [](auto& board) {
            apply_move_if_valid(&board, { PLAYER_BLACK, PIECE_PAWN, F7, F5 });
        });
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto found_move = find_candidate_move(
        c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, E5, F6 });
    ASSERT(c_moves_end != found_move);
    ASSERT(PIECE_EMPTY == field_get_piece((*found_move)[F5])); 
}

TEST(CandidateMoves_Pawn_Black_CaptureEnPassantLeft) {
    auto board = two_pawn_board(E2, F4, [](auto& board) {
            apply_move_if_valid(&board, { PLAYER_WHITE, PIECE_PAWN, E2, E4 });
        });
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto found_move = find_candidate_move(
        c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, F4, E3 });
    ASSERT(c_moves_end != found_move);
    ASSERT(PIECE_EMPTY == field_get_piece((*found_move)[E4]));
}

TEST(CandidateMoves_Pawn_Black_CaptureEnPassantRight) {
    auto board = two_pawn_board(E2, D4, [](auto& board) {
            apply_move_if_valid(&board, { PLAYER_WHITE, PIECE_PAWN, E2, E4 });
        });
    auto c_moves = prepare_moves();
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto found_move = find_candidate_move(
        c_moves.get(), c_moves_end, { PLAYER_BLACK, PIECE_PAWN, D4, E3 });
    ASSERT(c_moves_end != found_move);
    ASSERT(PIECE_EMPTY == field_get_piece((*found_move)[E4]));
}

TEST(CandidateMoves_Pawn_White_MoveForward_Queening) {
    auto board = one_pawn_board(A7);
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();
    std::vector<piece_t> transformed_pieces;
    do {
        auto found_move = find_candidate_move(
            c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_PAWN, A7, A8 });
        if (c_moves_end != found_move) {
            transformed_pieces.push_back(field_get_piece((*found_move)[A8]));
            c_moves_beg = found_move + 1;
        } else {
            c_moves_beg = found_move;
        }
    } while (c_moves_end != c_moves_beg);

    ASSERT(4u == transformed_pieces.size());
    ASSERT(transformed_pieces.end() !=
        std::find(transformed_pieces.begin(), transformed_pieces.end(), PIECE_KNIGHT));
    ASSERT(transformed_pieces.end() !=
        std::find(transformed_pieces.begin(), transformed_pieces.end(), PIECE_BISHOP));
    ASSERT(transformed_pieces.end() !=
        std::find(transformed_pieces.begin(), transformed_pieces.end(), PIECE_ROOK));
    ASSERT(transformed_pieces.end() !=
        std::find(transformed_pieces.begin(), transformed_pieces.end(), PIECE_QUEEN));
}

TEST(CandidateMoves_Pawn_Black_MoveForward_Queening) {
    auto board = one_pawn_board(A2, FBP);
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();
    std::vector<piece_t> transformed_pieces;
    do {
        auto found_move = find_candidate_move(
            c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_PAWN, A2, A1 });
        if (c_moves_end != found_move) {
            transformed_pieces.push_back(field_get_piece((*found_move)[A1]));
            c_moves_beg = found_move + 1;
        } else {
            c_moves_beg = found_move;
        }
    } while (c_moves_end != c_moves_beg);

    ASSERT(4u == transformed_pieces.size());
    ASSERT(transformed_pieces.end() !=
        std::find(transformed_pieces.begin(), transformed_pieces.end(), PIECE_KNIGHT));
    ASSERT(transformed_pieces.end() !=
        std::find(transformed_pieces.begin(), transformed_pieces.end(), PIECE_BISHOP));
    ASSERT(transformed_pieces.end() !=
        std::find(transformed_pieces.begin(), transformed_pieces.end(), PIECE_ROOK));
    ASSERT(transformed_pieces.end() !=
        std::find(transformed_pieces.begin(), transformed_pieces.end(), PIECE_QUEEN));
}

auto three_piece_board(
    const field_t piece1_pos, const field_state_t piece1,
    const field_t piece2_pos, const field_state_t piece2,
    const field_t piece3_pos, const field_state_t piece3) {
    return prepare_board([=](auto& board) {
        board[H1] = FWK;
        board[H8] = FBK;
        board[piece1_pos] = piece1;
        board[piece2_pos] = piece2;
        board[piece3_pos] = piece3;
    });
}

TEST(CandidateMoves_Knight_White_InvalidMoves_ValidMoves_Captures) {
    auto board = three_piece_board(B3, FWN, C5, FBP, D4, FWP);
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto white_knigth_moves = std::count_if(c_moves_beg, c_moves_end, [](const auto& board)
        {
            last_move_t last_move = board_state_meta_get_last_move(board);
            player_t last_move_player = last_move_get_player(last_move);
            piece_t last_move_piece = last_move_get_piece(last_move);
            return PLAYER_WHITE == last_move_player and PIECE_KNIGHT == last_move_piece;
        });
    ASSERT(5u == white_knigth_moves);
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KNIGHT, B3, C1 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KNIGHT, B3, D2 }));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KNIGHT, B3, D4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KNIGHT, B3, C5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KNIGHT, B3, A5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KNIGHT, B3, A1 }));
}

TEST(CandidateMoves_Knight_Black_InvalidMoves_ValidMoves_Captures) {
    auto board = three_piece_board(B3, FBN, C5, FBP, D4, FWP);
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto black_knigth_moves = std::count_if(c_moves_beg, c_moves_end, [](const auto& board)
        {
            last_move_t last_move = board_state_meta_get_last_move(board);
            player_t last_move_player = last_move_get_player(last_move);
            piece_t last_move_piece = last_move_get_piece(last_move);
            return PLAYER_BLACK == last_move_player and PIECE_KNIGHT == last_move_piece;
        });

    ASSERT(5u == black_knigth_moves);
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KNIGHT, B3, C1 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KNIGHT, B3, D2 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KNIGHT, B3, D4 }));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KNIGHT, B3, C5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KNIGHT, B3, A5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KNIGHT, B3, A1 }));
}

TEST(CandidateMoves_Bishop_White_InvalidMoves_ValidMoves_Captures) {
    auto board = three_piece_board(B3, FWB, F7, FBP, C2, FWP);
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto white_bishop_moves = std::count_if(c_moves_beg, c_moves_end, [](const auto& board)
        {
            last_move_t last_move = board_state_meta_get_last_move(board);
            player_t last_move_player = last_move_get_player(last_move);
            piece_t last_move_piece = last_move_get_piece(last_move);
            return PLAYER_WHITE == last_move_player and PIECE_BISHOP == last_move_piece;
        });

    temp_print_c_moves(c_moves_beg, c_moves_end);

    ASSERT(6u == white_bishop_moves);
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_BISHOP, B3, A2 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_BISHOP, B3, A4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_BISHOP, B3, C4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_BISHOP, B3, D5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_BISHOP, B3, E6 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_BISHOP, B3, F7 }));
}

TEST(CandidateMoves_Bishop_Black_InvalidMoves_ValidMoves_Captures) {
    auto board = three_piece_board(B3, FBB, F7, FWP, C2, FBP);
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto black_bishop_moves = std::count_if(c_moves_beg, c_moves_end, [](const auto& board)
        {
            last_move_t last_move = board_state_meta_get_last_move(board);
            player_t last_move_player = last_move_get_player(last_move);
            piece_t last_move_piece = last_move_get_piece(last_move);
            return PLAYER_BLACK == last_move_player and PIECE_BISHOP == last_move_piece;
        });

    temp_print_c_moves(c_moves_beg, c_moves_end);

    ASSERT(6u == black_bishop_moves);
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_BISHOP, B3, A2 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_BISHOP, B3, A4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_BISHOP, B3, C4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_BISHOP, B3, D5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_BISHOP, B3, E6 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_BISHOP, B3, F7 }));
}

TEST(CandidateMoves_Rook_White_InvalidMoves_ValidMoves_Captures) {
    auto board = three_piece_board(C5, FWR, C7, FWP, F5, FBP);
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto white_rook_moves = std::count_if(c_moves_beg, c_moves_end, [](const auto& board)
        {
            last_move_t last_move = board_state_meta_get_last_move(board);
            player_t last_move_player = last_move_get_player(last_move);
            piece_t last_move_piece = last_move_get_piece(last_move);
            return PLAYER_WHITE == last_move_player and PIECE_ROOK == last_move_piece;
        });

    ASSERT(10u == white_rook_moves);
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_ROOK, C5, B5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_ROOK, C5, A5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_ROOK, C5, C4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_ROOK, C5, C3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_ROOK, C5, C2 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_ROOK, C5, C1 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_ROOK, C5, C6 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_ROOK, C5, D5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_ROOK, C5, E5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_ROOK, C5, F5 }));
}

TEST(CandidateMoves_Rook_Black_InvalidMoves_ValidMoves_Captures) {
    auto board = three_piece_board(C5, FBR, C7, FBP, F5, FWP);
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto black_rook_moves = std::count_if(c_moves_beg, c_moves_end, [](const auto& board)
        {
            last_move_t last_move = board_state_meta_get_last_move(board);
            player_t last_move_player = last_move_get_player(last_move);
            piece_t last_move_piece = last_move_get_piece(last_move);
            return PLAYER_BLACK == last_move_player and PIECE_ROOK == last_move_piece;
        });

    ASSERT(10u == black_rook_moves);
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_ROOK, C5, B5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_ROOK, C5, A5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_ROOK, C5, C4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_ROOK, C5, C3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_ROOK, C5, C2 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_ROOK, C5, C1 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_ROOK, C5, C6 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_ROOK, C5, D5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_ROOK, C5, E5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_ROOK, C5, F5 }));
}

TEST(CandidateMoves_Queen_White_InvalidMoves_ValidMoves_Captures) {
    auto board = three_piece_board(B3, FWQ, F7, FBP, C2, FWP);
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto white_queen_moves = std::count_if(c_moves_beg, c_moves_end, [](const auto& board)
        {
            last_move_t last_move = board_state_meta_get_last_move(board);
            player_t last_move_player = last_move_get_player(last_move);
            piece_t last_move_piece = last_move_get_piece(last_move);
            return PLAYER_WHITE == last_move_player and PIECE_QUEEN == last_move_piece;
        });

    ASSERT(20u == white_queen_moves);
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, B1 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, B2 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, B4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, B5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, B6 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, B7 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, B8 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, A2 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, A3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, A4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, C4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, D5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, E6 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, F7 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, C3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, D3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, E3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, F3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, G3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, B3, H3 }));
}

TEST(CandidateMoves_Queen_Black_InvalidMoves_ValidMoves_Captures) {
    auto board = three_piece_board(B3, FBQ, F7, FWP, C2, FBP);
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto black_queen_moves = std::count_if(c_moves_beg, c_moves_end, [](const auto& board)
        {
            last_move_t last_move = board_state_meta_get_last_move(board);
            player_t last_move_player = last_move_get_player(last_move);
            piece_t last_move_piece = last_move_get_piece(last_move);
            return PLAYER_BLACK == last_move_player and PIECE_QUEEN == last_move_piece;
        });

    ASSERT(20u == black_queen_moves);
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, B1 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, B2 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, B4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, B5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, B6 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, B7 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, B8 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, A2 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, A3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, A4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, C4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, D5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, E6 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, F7 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, C3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, D3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, E3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, F3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, G3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, B3, H3 }));
}

TEST(CandidateMoves_King_White_InvalidMoves_ValidMoves_Captures) {
    auto board = prepare_board([](auto& board) {
        board[H8] = FBK;
        board[E4] = FWK;
        board[E3] = FBP;
        board[E5] = FWP;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto white_king_moves = std::count_if(c_moves_beg, c_moves_end, [](const auto& board)
        {
            last_move_t last_move = board_state_meta_get_last_move(board);
            player_t last_move_player = last_move_get_player(last_move);
            piece_t last_move_piece = last_move_get_piece(last_move);
            return PLAYER_WHITE == last_move_player and PIECE_KING == last_move_piece;
        });

    ASSERT(7u == white_king_moves);
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, D3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, E3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, F3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, D4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, F4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, D5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, F5 }));
}

TEST(CandidateMoves_King_Black_InvalidMoves_ValidMoves_Captures) {
    auto board = prepare_board([](auto& board) {
        board[H8] = FWK;
        board[E4] = FBK;
        board[E3] = FWP;
        board[E5] = FBP;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto black_king_moves = std::count_if(c_moves_beg, c_moves_end, [](const auto& board)
        {
            last_move_t last_move = board_state_meta_get_last_move(board);
            player_t last_move_player = last_move_get_player(last_move);
            piece_t last_move_piece = last_move_get_piece(last_move);
            return PLAYER_BLACK == last_move_player and PIECE_KING == last_move_piece;
        });

    ASSERT(5u == black_king_moves);
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E4, D3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E4, E3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E4, F3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E4, D5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E4, F5 }));
}

TEST(CandidateMoves_King_White_ValidShortCastle) {
    auto board = prepare_board([](auto& board) {
        board[A8] = FBK;
        board[E1] = FWK;
        board[H1] = FWR;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto found_move = find_candidate_move(
        c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, G1 });
    ASSERT(c_moves_end != found_move);
    ASSERT(PIECE_EMPTY == field_get_piece((*found_move)[H1])); 
    ASSERT(PLAYER_WHITE == field_get_player((*found_move)[F1])); 
    ASSERT(PIECE_ROOK == field_get_piece((*found_move)[F1])); 
}

TEST(CandidateMoves_King_Black_ValidShortCastle) {
    auto board = prepare_board([](auto& board) {
        board[A1] = FWK;
        board[E8] = FBK;
        board[H8] = FBR;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto found_move = find_candidate_move(
        c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E8, G8 });
    ASSERT(c_moves_end != found_move);
    ASSERT(PIECE_EMPTY == field_get_piece((*found_move)[H8])); 
    ASSERT(PLAYER_BLACK == field_get_player((*found_move)[F8])); 
    ASSERT(PIECE_ROOK == field_get_piece((*found_move)[F8])); 
}

TEST(CandidateMoves_King_White_ValidLongCastle) {
    auto board = prepare_board([](auto& board) {
        board[H8] = FBK;
        board[E1] = FWK;
        board[A1] = FWR;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto found_move = find_candidate_move(
        c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, C1 });
    ASSERT(c_moves_end != found_move);
    ASSERT(PIECE_EMPTY == field_get_piece((*found_move)[A1])); 
    ASSERT(PLAYER_WHITE == field_get_player((*found_move)[D1])); 
    ASSERT(PIECE_ROOK == field_get_piece((*found_move)[D1])); 
}

TEST(CandidateMoves_King_Black_ValidLongCastle) {
    auto board = prepare_board([](auto& board) {
        board[H1] = FWK;
        board[E8] = FBK;
        board[A8] = FBR;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    auto found_move = find_candidate_move(
        c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E8, C8 });
    ASSERT(c_moves_end != found_move);
    ASSERT(PIECE_EMPTY == field_get_piece((*found_move)[A8])); 
    ASSERT(PLAYER_BLACK == field_get_player((*found_move)[D8])); 
    ASSERT(PIECE_ROOK == field_get_piece((*found_move)[D8])); 
}

TEST(CandidateMoves_King_White_CastlingRightsLostByKing) {
    auto board = prepare_board([](auto& board) {
        board[E8] = FBK;
        board[E2] = FWK;
        board[A1] = FWR;
        board[H1] = FWR;
        apply_move_if_valid(&board, { PLAYER_WHITE, PIECE_KING, E2, E1 });
    });

    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, G1 }));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, C1 }));
}

TEST(CandidateMoves_King_Black_CastlingRightsLostByKing) {
    auto board = prepare_board([](auto& board) {
        board[E1] = FWK;
        board[E7] = FBK;
        board[A8] = FBR;
        board[H8] = FBR;
        apply_move_if_valid(&board, { PLAYER_BLACK, PIECE_KING, E7, E8 });
    });

    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E8, G8 }));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E8, C8 }));
}

TEST(CandidateMoves_King_White_ShortCastlingRightsLostByRook) {
    auto board = prepare_board([](auto& board) {
        board[E8] = FBK;
        board[E1] = FWK;
        board[A1] = FWR;
        board[H1] = FWR;
        apply_move_if_valid(&board, { PLAYER_WHITE, PIECE_ROOK, H1, G1 });
        apply_move_if_valid(&board, { PLAYER_WHITE, PIECE_ROOK, G1, H1 });
    });

    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, G1 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, C1 }));
}

TEST(CandidateMoves_King_Black_ShortCastlingRightsLostByRook) {
    auto board = prepare_board([](auto& board) {
        board[E1] = FWK;
        board[E8] = FBK;
        board[A8] = FBR;
        board[H8] = FBR;
        apply_move_if_valid(&board, { PLAYER_BLACK, PIECE_ROOK, H8, H7 });
        apply_move_if_valid(&board, { PLAYER_BLACK, PIECE_ROOK, H7, H8 });
    });

    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E8, G8 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E8, C8 }));
}

TEST(CandidateMoves_King_White_LongCastlingRightsLostByRook) {
    auto board = prepare_board([](auto& board) {
        board[E8] = FBK;
        board[E1] = FWK;
        board[A1] = FWR;
        board[H1] = FWR;
        apply_move_if_valid(&board, { PLAYER_WHITE, PIECE_ROOK, A1, B1 });
        apply_move_if_valid(&board, { PLAYER_WHITE, PIECE_ROOK, B1, A1 });
    });

    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, G1 }));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, C1 }));
}

TEST(CandidateMoves_King_Black_LongCastlingRightsLostByRook) {
    auto board = prepare_board([](auto& board) {
        board[E1] = FWK;
        board[E8] = FBK;
        board[A8] = FBR;
        board[H8] = FBR;
        apply_move_if_valid(&board, { PLAYER_BLACK, PIECE_ROOK, A8, A7 });
        apply_move_if_valid(&board, { PLAYER_BLACK, PIECE_ROOK, A7, A8 });
    });

    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E8, G8 }));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E8, C8 }));
}

TEST(CandidateMoves_King_White_ShortCastleNotPermittedDueToF1Attacked) {
    auto board = prepare_board([](auto& board) {
        board[A8] = FBK;
        board[F8] = FBR;
        board[E1] = FWK;
        board[H1] = FWR;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, G1 }));
}

TEST(CandidateMoves_King_White_ShortCastleNotPermittedDueToG1Attacked) {
    auto board = prepare_board([](auto& board) {
        board[A8] = FBK;
        board[G8] = FBQ;
        board[E1] = FWK;
        board[H1] = FWR;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, G1 }));
}

TEST(CandidateMoves_King_White_CastleNotPermittedDueToKingInCheck) {
    auto board = prepare_board([](auto& board) {
        board[A8] = FBK;
        board[D2] = FBP;
        board[E1] = FWK;
        board[H1] = FWR;
        board[A1] = FWR;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, G1 }));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, C1 }));
}

TEST(CandidateMoves_King_White_LongCastleNotPermittedDueToD1Attacked) {
    auto board = prepare_board([](auto& board) {
        board[A8] = FBK;
        board[A4] = FBB;
        board[E1] = FWK;
        board[H1] = FWR;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, C1 }));
}

TEST(CandidateMoves_King_White_LongCastleNotPermittedDueToC1Attacked) {
    auto board = prepare_board([](auto& board) {
        board[A8] = FBK;
        board[A2] = FBN;
        board[E1] = FWK;
        board[H1] = FWR;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, C1 }));
}

TEST(CandidateMoves_King_Black_ShortCastleNotPermittedDueToF8Attacked) {
    auto board = prepare_board([](auto& board) {
        board[E8] = FBK;
        board[H8] = FBR;
        board[A1] = FWK;
        board[F1] = FWR;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, G1 }));
}

TEST(CandidateMoves_King_Black_ShortCastleNotPermittedDueToG8Attacked) {
    auto board = prepare_board([](auto& board) {
        board[E8] = FBK;
        board[H8] = FBR;
        board[A1] = FWK;
        board[H7] = FWP;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, G1 }));
}

TEST(CandidateMoves_King_Black_CastleNotPermittedDueToKingInCheck) {
    auto board = prepare_board([](auto& board) {
        board[E8] = FBK;
        board[H8] = FBR;
        board[A1] = FBR;
        board[B1] = FWK;
        board[H4] = FWQ;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, G1 }));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, C1 }));
}

TEST(CandidateMoves_King_Black_LongCastleNotPermittedDueToD8Attacked) {
    auto board = prepare_board([](auto& board) {
        board[E8] = FBK;
        board[A8] = FBR;
        board[H1] = FWK;
        board[G5] = FWB;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, C1 }));
}

TEST(CandidateMoves_King_Black_LongCastleNotPermittedDueToC8Attacked) {
    auto board = prepare_board([](auto& board) {
        board[E8] = FBK;
        board[A8] = FBR;
        board[B7] = FWK;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E1, C1 }));
}

TEST(Validation_White_Checkmate_ShouldResultIn0CandidateMoves) {
    auto board = prepare_board([](auto& board) {
        board[A1] = FWK;
        board[A2] = FBR;
        board[B2] = FWP;
        board[B3] = FBB;
        board[C2] = FBP;
        board[B8] = FWQ;
        board[H8] = FWB;
        board[E5] = FWN;
        board[H1] = FBK;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    temp_print_c_moves(c_moves_beg, c_moves_end);
    ASSERT(c_moves_beg == c_moves_end);
}

TEST(Validation_White_Check_OnlyWayToStopMate_ShouldResultIn1CandidateMove) {
    auto board = prepare_board([](auto& board) {
        board[B1] = FWK;
        board[A2] = FWP;
        board[B2] = FWP;
        board[C2] = FWP;
        board[D7] = FWQ;
        board[H1] = FBR;
        board[H8] = FBK;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    temp_print_c_moves(c_moves_beg, c_moves_end);
    ASSERT(1u == (c_moves_end - c_moves_beg));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, D7, D1 }));
}

TEST(Validation_White_Check_TwoWaysToStopMate_ShouldResultIn2CandidateMoves) {
    auto board = prepare_board([](auto& board) {
        board[B1] = FWK;
        board[A2] = FWP;
        board[B2] = FWP;
        board[C2] = FWP;
        board[E4] = FWQ;
        board[H1] = FBR;
        board[H8] = FBK;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    temp_print_c_moves(c_moves_beg, c_moves_end);
    ASSERT(2u == (c_moves_end - c_moves_beg));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, E4, E1 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_QUEEN, E4, H1 }));
}

TEST(Validation_White_CannotMovePinnedPiece) {
    auto board = prepare_board([](auto& board) {
        board[E8] = FBK;
        board[E4] = FWK;
        board[D4] = FWP;
        board[F5] = FWN;
        board[A4] = FBR;
        board[H7] = FBB;
        board[C5] = FBP;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    temp_print_c_moves(c_moves_beg, c_moves_end);
    ASSERT(6u == (c_moves_end - c_moves_beg));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, E5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, D5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, E3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, D3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, F3 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, E4, F4 }));
}

TEST(Validation_Black_Checkmate_ShouldResultIn0CandidateMoves) {
    auto board = prepare_board([](auto& board) {
        board[A8] = FBK;
        board[A7] = FWR;
        board[B7] = FBP;
        board[B6] = FWB;
        board[C7] = FWP;
        board[B1] = FBQ;
        board[H1] = FBB;
        board[E5] = FBN;
        board[H8] = FWK;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    temp_print_c_moves(c_moves_beg, c_moves_end);
    ASSERT(c_moves_beg == c_moves_end);
}

TEST(Validation_Black_Check_OnlyWayToStopMate_ShouldResultIn1CandidateMove) {
    auto board = prepare_board([](auto& board) {
        board[B8] = FBK;
        board[A7] = FBP;
        board[B7] = FBP;
        board[C7] = FBP;
        board[D2] = FBQ;
        board[H8] = FWR;
        board[H1] = FWK;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    temp_print_c_moves(c_moves_beg, c_moves_end);
    ASSERT(1u == (c_moves_end - c_moves_beg));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, D2, D8 }));
}

TEST(Validation_Black_Check_TwoWaysToStopMate_ShouldResultIn2CandidateMoves) {
    auto board = prepare_board([](auto& board) {
        board[B8] = FBK;
        board[A7] = FBP;
        board[B7] = FBP;
        board[C7] = FBP;
        board[E5] = FBQ;
        board[H8] = FWR;
        board[H1] = FWK;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    temp_print_c_moves(c_moves_beg, c_moves_end);
    ASSERT(2u == (c_moves_end - c_moves_beg));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, E5, E8 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_QUEEN, E5, H8 }));
}

TEST(Validation_Black_CannotMovePinnedPiece) {
    auto board = prepare_board([](auto& board) {
        board[E1] = FWK;
        board[E5] = FBK;
        board[D5] = FBP;
        board[F4] = FBN;
        board[A5] = FWR;
        board[H2] = FWB;
        board[C4] = FWP;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    temp_print_c_moves(c_moves_beg, c_moves_end);
    ASSERT(6u == (c_moves_end - c_moves_beg));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E5, E4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E5, D4 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E5, E6 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E5, D6 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E5, F5 }));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, E5, F6 }));
}


TEST(Validation_White_CannotMoveKingNearKing) {
    auto board = prepare_board([](auto& board) {
        board[A1] = FWK;
        board[B3] = FBK;
        board[D4] = FWP;
        board[D5] = FBP;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);
    const auto* c_moves_beg = c_moves.get();

    temp_print_c_moves(c_moves_beg, c_moves_end);
    ASSERT(1u == (c_moves_end - c_moves_beg));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_WHITE, PIECE_KING, A1, B1 }));
}

TEST(Validation_Black_CannotMoveKingNearKing) {
    auto board = prepare_board([](auto& board) {
        board[A8] = FBK;
        board[B6] = FWK;
        board[D4] = FWP;
        board[D5] = FBP;
    });
    auto c_moves = prepare_moves();
    const auto* c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_BLACK);
    const auto* c_moves_beg = c_moves.get();

    temp_print_c_moves(c_moves_beg, c_moves_end);
    ASSERT(1u == (c_moves_end - c_moves_beg));
    ASSERT(check_candidate_move(c_moves_beg, c_moves_end, { PLAYER_BLACK, PIECE_KING, A8, B8 }));
}