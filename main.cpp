#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <exception>
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
void name();                                                                        \
static bool name##_init = add_test(name, #name);                                    \
void name()                                                                         \

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

bool check_candidate_move(
    const board_state_t* moves, std::size_t moves_cnt, const last_move_s& last_move)
{
    return std::any_of(moves, moves + moves_cnt, [&last_move](const auto& board) {
            bool success = check_last_move(board, last_move);
            if (success) {
                printf("\nFound requested candidate move:\n\n");
                gui::print_board(std::cout, board);
            }
            return success;
        });
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

TEST(Pawn_CandidateMoves_White_MoveForward) {
    auto board = chess::EMPTY_BOARD;
    board[E1] = FWK;
    board[E8] = FBK;
    board[E2] = FWP;
    gui::print_board(std::cout, board);

    auto c_moves = std::make_unique<board_state_t[]>(32);
    auto c_moves_cnt = fill_candidate_moves(c_moves.get(), board);

    ASSERT(check_candidate_move(c_moves.get(), c_moves_cnt, { PLAYER_WHITE, PIECE_PAWN, E2, E3 }));
}