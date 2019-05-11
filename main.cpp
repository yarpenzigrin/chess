#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <exception>
#include <algorithm>
#include "chess_tty_gui.hpp"

using namespace chess;

#define ASSERT(cond) if (!(cond)) { printf("\n!!! %s:%d Condition:\n  \"%s\"\nfailed.\n\n", \
    __FILE__, __LINE__, #cond);                                                             \
    throw std::runtime_error("Assertion failed."); }

using Test = std::function<void(void)>;
std::vector<Test> tests;
std::vector<const char*> failures;

static bool add_test(void (*fn)(void), const char* test_name)
{
    tests.push_back([fn, test_name](){
        printf("***** %s *****\n", test_name);
        try {
            fn();
        } catch (const std::runtime_error& e) {
            failures.push_back(test_name);
        }
        printf("^^^^^ %s ^^^^^\n", test_name);
    });
    return true;
}

#define TEST(name)                                                                  \
    void name();                                                                    \
    static bool name##_init = add_test(name, #name);                                \
    void name()                                                                     \

int main() {
    for (auto test : tests)
        test();
    if (!failures.empty())
    {
        printf("\n\nFAILURES:\n");
        for (auto failed_test : failures)
            printf(" * %s\n", failed_test);
    }
    return 0;
}

TEST(Internal_StaticEvaluation_FieldProperties) {
    static_assert(file_t::file_t_max == field_file(FIELD_INVALID),
        "Unknown file for invalid field");
    static_assert(rank_t::rank_t_max == field_rank(FIELD_INVALID),
        "Unknown rank for invalid field");
    static_assert(file_t::A == field_file(A4), "A-file for field A4");
    static_assert(file_t::H == field_file(H8), "H-file for field H8");
    static_assert(rank_t::_4 == field_rank(A4), "4th-rank for field A4");
    static_assert(rank_t::_8 == field_rank(H8), "8th-rank for field H8");
    static_assert(FIELD_INVALID == make_field(150, 0), "(150,0) results in invalid field");
    static_assert(FIELD_INVALID == make_field(3, 69), "(3,69) results in invalid field");
    static_assert(A4 == make_field(0, 3), "(0,3) results in field A4");
    static_assert(H8 == make_field(7, 7), "(7,7) results in field H8");
}

TEST(Internal_StaticEvaluation_FieldGeneration) {
    static_assert(D5 == field_up(D4), "D4 --UP--> D5");
    static_assert(D3 == field_down(D4), "D4 --DOWN--> D3");
    static_assert(C4 == field_left(D4), "D4 --LEFT--> C4");
    static_assert(E4 == field_right(D4), "D4 --RIGHT--> E4");
    static_assert(FIELD_INVALID == field_up(H8), "H8 --UP--> invalid field");
    static_assert(FIELD_INVALID == field_right(H8), "H8 --RIGHT--> invalid field");
    static_assert(FIELD_INVALID == field_down(A1), "A1 --DOWN--> invalid field");
    static_assert(FIELD_INVALID == field_left(A1), "A1 --LEFT--> invalid field");
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

auto one_pawn_board(const field_t pawn_position)
{
    auto board = chess::EMPTY_BOARD;
    board[E1] = FWK;
    board[E8] = FBK;
    board[pawn_position] = FWP;
    gui::print_board(std::cout, board);
    return board;
}

bool check_candidate_move(
    const board_state_t* moves, const board_state_t* moves_end, const move_s& last_move)
{
    return std::any_of(moves, moves_end, [&last_move](const auto& board) {
            bool success = check_last_move(board, last_move);
            if (success) {
                printf("\nFound requested candidate move:\n\n");
                gui::print_board(std::cout, board);
            }
            return success;
        });
}

bool all_candidate_moves_are_valid(const board_state_t* moves, const board_state_t* moves_end)
{
    return std::all_of(moves, moves_end, [](const auto& board) {
            return validate_board_state(board);
        });
}

TEST(Pawn_CandidateMoves_White_MoveForward_E2E3) {
    auto board = one_pawn_board(E2);
    auto c_moves = std::make_unique<board_state_t[]>(32);
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, E2, E3 }));
}

TEST(Pawn_CandidateMoves_White_MoveLongForward_E2E4) {
    auto board = one_pawn_board(E2);

    auto c_moves = std::make_unique<board_state_t[]>(32);
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, E2, E4 }));
}

TEST(Pawn_CandidateMoves_White_MoveForward_D2D3) {
    auto board = one_pawn_board(D2);
    auto c_moves = std::make_unique<board_state_t[]>(32);
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, D2, D3 }));
}

TEST(Pawn_CandidateMoves_White_MoveForward_D2D4) {
    auto board = one_pawn_board(D2);
    auto c_moves = std::make_unique<board_state_t[]>(32);
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, D2, D4 }));
}

TEST(Pawn_CandidateMoves_White_NoMoveForward_D3D5) {
    auto board = one_pawn_board(D3);
    auto c_moves = std::make_unique<board_state_t[]>(32);
    auto c_moves_end = fill_candidate_moves(c_moves.get(), board, PLAYER_WHITE);

    ASSERT(all_candidate_moves_are_valid(c_moves.get(), c_moves_end));
    ASSERT(!check_candidate_move(c_moves.get(), c_moves_end, { PLAYER_WHITE, PIECE_PAWN, D3, D5 }));
}