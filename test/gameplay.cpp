#include <memory>
#include <thread>
#include "chess_tty_gui.hpp"
#include "chesstest.hpp"

using namespace chess;

void draw_board(const board_state_t& board) {
    gui::print_board(test_output, board);
}

void print_candidate_moves(const board_state_t& board) {
    auto c_moves_beg = std::make_unique<board_state_t[]>(32);
    auto last_player = last_move_get_player(board_state_meta_get_last_move(board));
    auto next_player = (PLAYER_WHITE == last_player ? PLAYER_BLACK : PLAYER_WHITE);
    auto c_moves_end = fill_candidate_moves(c_moves_beg.get(), board, next_player);
    for (auto it = c_moves_beg.get(); it != c_moves_end; ++it)
        draw_board(*it);
}

game_action_t dummy_move_req(board_state_t& board) {
    return game_action_t::FORFEIT;
}

std::unique_ptr<board_state_t[]> prepare_game_memory() {
    return std::make_unique<board_state_t[]>(128);
}

struct test_logger_t {
    template <typename T>
    test_logger_t& operator<<(const T& stuff) {
        test_output << stuff;
        return *this;
    }
};

game_result_t do_play(
    request_move_f white_move_fn, request_move_f black_move_fn, board_state_t board = START_BOARD) {
    auto memory = prepare_game_memory();
    return play<test_logger_t>(memory.get(), white_move_fn, black_move_fn, board);
}

TEST(Gameplay_Play_ReturnsErrorIfMemoryIsNull) {
    ASSERT(game_result_t::ERROR == play(nullptr, dummy_move_req, dummy_move_req));
}

TEST(Gameplay_Play_ReturnsErrorIfAnyRequestFunctionsAreNull) {
    ASSERT(game_result_t::ERROR == do_play(nullptr, dummy_move_req));
    ASSERT(game_result_t::ERROR == do_play(dummy_move_req, nullptr));
}

board_state_t captured_board;

game_action_t forfeit_on_1st_move(board_state_t& board) {
    captured_board = board;
    return game_action_t::FORFEIT;
}

TEST(Gameplay_Play_GameStartsWithWhitePlayerReqestedToMoveWithInitialBoard) {
    ASSERT(game_result_t::BLACK_WON_FORFEIT == do_play(forfeit_on_1st_move, dummy_move_req));

    auto initial_board = START_BOARD;
    ASSERT(std::equal(initial_board.begin(), initial_board.end(), captured_board.begin()));
}

std::vector<move_s>& white_move_seq() {
    static std::vector<move_s> moves = {};
    return moves;
}
std::size_t white_move_idx = 0;


std::vector<move_s>& black_move_seq() {
    static std::vector<move_s> moves = {};
    return moves;
}
std::size_t black_move_idx = 0;

void fill_white_move_seq(std::vector<move_s> moves) {
    white_move_seq() = std::move(moves);
    white_move_idx = 0;
}

void fill_black_move_seq(std::vector<move_s> moves) {
    black_move_seq() = std::move(moves);
    black_move_idx = 0;
}

game_action_t white_to_play(board_state_t& board) {
    if (white_move_idx >= white_move_seq().size())
        return game_action_t::FORFEIT;

    auto board_ptr = apply_move_if_valid(&board, white_move_seq()[white_move_idx++]);
    ASSERT(board_ptr != &board);
    test_output << "After white's move:\n";
    draw_board(board);
    // test_output << "Black's candidate moves:\n";
    // print_candidate_moves(board);
    return game_action_t::MOVE;
}

game_action_t black_to_play(board_state_t& board) {
    if (black_move_idx >= black_move_seq().size())
        return game_action_t::FORFEIT;

    auto board_ptr = apply_move_if_valid(&board, black_move_seq()[black_move_idx++]);
    ASSERT(board_ptr != &board);
    test_output << "After black's move:\n";
    draw_board(board);
    // test_output << "Whites's candidate moves:\n";
    // print_candidate_moves(board);
    return game_action_t::MOVE;
}

TEST(Gameplay_Play_AfterWhiteMoveBlackIsRequestedToMove) {
    fill_white_move_seq({ { PLAYER_WHITE, PIECE_PAWN, E2, E4 } });
    ASSERT(game_result_t::WHITE_WON_FORFEIT == do_play(white_to_play, forfeit_on_1st_move));
}

board_state_t prepare_board(std::function<void(board_state_t&)> setup_fn) {
    auto board = chess::EMPTY_BOARD;
    setup_fn(board);
    update_fields_under_attack(board);
    draw_board(board);
    return board;
}

TEST(Gameplay_Play_AfterWhitesMoveBlackIsCheckmatedAndWhiteWins) {
    auto board = prepare_board([](auto& board){
        board[A1] = FBK;
        board[C2] = FWQ;
        board[A3] = FWB;
        board[H8] = FWK;
    });
    fill_white_move_seq({ { PLAYER_WHITE, PIECE_QUEEN, C2, B2 } });
    fill_black_move_seq({ { PLAYER_BLACK, PIECE_KING, A1, A7 } });
    ASSERT(game_result_t::WHITE_WON_CHECKMATE == do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_WhitesRidiculousMovesIsNotAllowed) {
    auto board = prepare_board([](auto& board){
        board[E1] = FWK;
        board[E8] = FBK;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KNIGHT, C2, B4 }, // moving non existing piece
        { PLAYER_WHITE, PIECE_KING, E2, E3 },   // moving king from wrong square
        { PLAYER_WHITE, PIECE_KING, E1, A8 }    // flying king
    });
    ASSERT(game_result_t::BLACK_WON_FORFEIT == do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_BlacksRidiculousMovesIsNotAllowed) {
    auto board = prepare_board([](auto& board){
        board[E1] = FWK;
        board[E8] = FBK;
        board[A2] = FWP;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KING, E1, E2 }
    });
    fill_black_move_seq({
        { PLAYER_BLACK, PIECE_KNIGHT, C2, B4 }, // moving non existing piece
        { PLAYER_BLACK, PIECE_KING, E7, E6 },   // moving king from wrong square
        { PLAYER_BLACK, PIECE_KING, E8, H8 }    // flying king
    });
    ASSERT(game_result_t::WHITE_WON_FORFEIT == do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_CheckMateInTwoFromBlack) {
    auto board = prepare_board([](auto& board){
        board[A1] = FWK;
        board[B3] = FBK;
        board[H2] = FBQ;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KING, A1, B1 },   // only allowed move
        { PLAYER_WHITE, PIECE_KING, B1, A1 },   // only allowed move
        { PLAYER_WHITE, PIECE_KING, A1, H8 }    // trying to cheat? (make test fail)
    });
    fill_black_move_seq({
        { PLAYER_BLACK, PIECE_QUEEN, H2, C2 },  // check!
        { PLAYER_BLACK, PIECE_QUEEN, C2, A2 }   // mate!
    });
    ASSERT(game_result_t::BLACK_WON_CHECKMATE == do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_StalemateFromWhite) {
    auto board = prepare_board([](auto& board){
        board[A8] = FBK;
        board[H1] = FWK;
        board[B4] = FWQ;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_QUEEN, B4, B6 }   // oops
    });
    ASSERT(game_result_t::DRAW_STALEMATE == do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_StalemateFromBlack) {
    auto board = prepare_board([](auto& board){
        board[A7] = FWK;
        board[H1] = FBK;
        board[B4] = FBQ;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KING, A7, A8 }   // escape!
    });
    fill_black_move_seq({
        { PLAYER_BLACK, PIECE_QUEEN, B4, B6 }   // oops
    });
    ASSERT(game_result_t::DRAW_STALEMATE == do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_AfterWhitesMove_DrawByInsufficientMaterialOnlyKingsLeft) {
    auto board = prepare_board([](auto& board){
        board[A8] = FBK;
        board[H1] = FWK;
        board[G2] = FBP;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KING, H1, G2 }
    });
    ASSERT(game_result_t::DRAW_INSUFFICIENT_MATERIAL ==
        do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_AfterBlacksMove_DrawByInsufficientMaterialOnlyKingsLeft) {
    auto board = prepare_board([](auto& board){
        board[A8] = FBK;
        board[H1] = FWK;
        board[B7] = FWP;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KING, H1, G2 }
    });
    fill_black_move_seq({
        { PLAYER_BLACK, PIECE_KING, A8, B7 }
    });
    ASSERT(game_result_t::DRAW_INSUFFICIENT_MATERIAL ==
        do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_AfterWhitesMove_DrawByInsufficientMaterialOnlyKingsAndOneBishopLeft) {
    auto board = prepare_board([](auto& board){
        board[A8] = FBK;
        board[H1] = FWK;
        board[G2] = FBP;
        board[F8] = FBB;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KING, H1, G2 }
    });
    ASSERT(game_result_t::DRAW_INSUFFICIENT_MATERIAL ==
        do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_AfterBlacksMove_DrawByInsufficientMaterialOnlyKingsAndOneBishopLeft) {
    auto board = prepare_board([](auto& board){
        board[A8] = FBK;
        board[H1] = FWK;
        board[B7] = FWP;
        board[F1] = FWB;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KING, H1, G2 }
    });
    fill_black_move_seq({
        { PLAYER_BLACK, PIECE_KING, A8, B7 }
    });
    ASSERT(game_result_t::DRAW_INSUFFICIENT_MATERIAL ==
        do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_AfterWhitesMove_DrawByInsufficientMaterialOnlyKingsAndOneKnightLeft) {
    auto board = prepare_board([](auto& board){
        board[A8] = FBK;
        board[H1] = FWK;
        board[G2] = FBP;
        board[F8] = FBN;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KING, H1, G2 }
    });
    ASSERT(game_result_t::DRAW_INSUFFICIENT_MATERIAL ==
        do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_AfterBlacksMove_DrawByInsufficientMaterialOnlyKingsAndOneKnightLeft) {
    auto board = prepare_board([](auto& board){
        board[A8] = FBK;
        board[H1] = FWK;
        board[B7] = FWP;
        board[F1] = FWN;
    });
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KING, H1, G2 }
    });
    fill_black_move_seq({
        { PLAYER_BLACK, PIECE_KING, A8, B7 }
    });
    ASSERT(game_result_t::DRAW_INSUFFICIENT_MATERIAL ==
        do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_AfterWhiteMove_DrawByThreeFoldRepetitionWithPerpetualCheck) {
    auto board = prepare_board([](auto& board){
        board[A8] = FBK;
        board[A6] = FBP;
        board[B6] = FBP;
        board[C5] = FWQ;
        board[E2] = FWK;
    });
    board_state_meta_set_castling_rights(board, 0b1111);
    // REPEATED POSITION: Queen C8 - King A8
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_QUEEN, C5, C8 }, // 1st occurrence of the same position
        { PLAYER_WHITE, PIECE_QUEEN, C8, C7 },
        { PLAYER_WHITE, PIECE_QUEEN, C7, C8 }, // 2nd
        { PLAYER_WHITE, PIECE_QUEEN, C8, C7 },
        { PLAYER_WHITE, PIECE_QUEEN, C7, C8 }  // 3rd
    });
    fill_black_move_seq({
        { PLAYER_BLACK, PIECE_KING, A8, A7 },
        { PLAYER_BLACK, PIECE_KING, A7, A8 },
        { PLAYER_BLACK, PIECE_KING, A8, A7 },
        { PLAYER_BLACK, PIECE_KING, A7, A8 }
    });
    ASSERT(game_result_t::DRAW_REPETITION == do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_AfterBlackMove_DrawByThreeFoldRepetitionDancingKings) {
    auto board = prepare_board([](auto& board){
        board[E2] = FWK;
        board[E6] = FBK;
        board[H4] = FWP;
        board[H5] = FBP;
    });
    board_state_meta_set_castling_rights(board, 0b1111);
    // REPEATED POSITION: Kings on E3 and E5
    fill_white_move_seq({
        { PLAYER_WHITE, PIECE_KING, E2, E3 },
        { PLAYER_WHITE, PIECE_KING, E3, F3 },
        { PLAYER_WHITE, PIECE_KING, F3, E3 },
        { PLAYER_WHITE, PIECE_KING, E3, E2 },
        { PLAYER_WHITE, PIECE_KING, E2, E3 },
        { PLAYER_WHITE, PIECE_KING, E3, F3 },
        { PLAYER_WHITE, PIECE_KING, F3, E3 },
        { PLAYER_WHITE, PIECE_KING, E3, E2 },
        { PLAYER_WHITE, PIECE_KING, E2, E3 }
    });
    fill_black_move_seq({
        { PLAYER_BLACK, PIECE_KING, E6, E5 },  // 1st occurrence of the same position
        { PLAYER_BLACK, PIECE_KING, E5, D5 },
        { PLAYER_BLACK, PIECE_KING, D5, C5 },
        { PLAYER_BLACK, PIECE_KING, C5, D5 },
        { PLAYER_BLACK, PIECE_KING, D5, E5 },  // 2nd
        { PLAYER_BLACK, PIECE_KING, E5, D5 },
        { PLAYER_BLACK, PIECE_KING, D5, C5 },
        { PLAYER_BLACK, PIECE_KING, C5, D5 },
        { PLAYER_BLACK, PIECE_KING, D5, E5 }   // 3rd
    });
    ASSERT(game_result_t::DRAW_REPETITION == do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_AfterWhiteMove_DrawBy50MoveRule) {
    auto board = prepare_board([](auto& board){
        board[B3] = FWK;
        board[B6] = FBK;
        board[H4] = FWP;
        board[H5] = FBP;
        board[H1] = FWB;
        board[A1] = FBB;
    });
    board_state_meta_set_castling_rights(board, 0b1111);
    
    std::vector<move_s> white_loop_1st_half = {
        { PLAYER_WHITE, PIECE_BISHOP, H1, G2 },
        { PLAYER_WHITE, PIECE_BISHOP, G2, F3 },
        { PLAYER_WHITE, PIECE_BISHOP, F3, E4 },
        { PLAYER_WHITE, PIECE_BISHOP, E4, D5 },
        { PLAYER_WHITE, PIECE_BISHOP, D5, C6 },
        { PLAYER_WHITE, PIECE_BISHOP, C6, B7 },
        { PLAYER_WHITE, PIECE_BISHOP, B7, A8 }
    };
    std::vector<move_s> white_loop_2nd_half = {
        { PLAYER_WHITE, PIECE_BISHOP, A8, B7 },
        { PLAYER_WHITE, PIECE_BISHOP, B7, C6 },
        { PLAYER_WHITE, PIECE_BISHOP, C6, D5 },
        { PLAYER_WHITE, PIECE_BISHOP, D5, E4 },
        { PLAYER_WHITE, PIECE_BISHOP, E4, F3 },
        { PLAYER_WHITE, PIECE_BISHOP, F3, G2 },
        { PLAYER_WHITE, PIECE_BISHOP, G2, H1 },
    };
    std::vector<move_s> black_loop_1st_half = {
        { PLAYER_BLACK, PIECE_BISHOP, A1, B2 },
        { PLAYER_BLACK, PIECE_BISHOP, B2, C3 },
        { PLAYER_BLACK, PIECE_BISHOP, C3, D4 },
        { PLAYER_BLACK, PIECE_BISHOP, D4, E5 },
        { PLAYER_BLACK, PIECE_BISHOP, E5, F6 },
        { PLAYER_BLACK, PIECE_BISHOP, F6, G7 },
        { PLAYER_BLACK, PIECE_BISHOP, G7, H8 }
    };
    std::vector<move_s> black_loop_2nd_half = {
        { PLAYER_BLACK, PIECE_BISHOP, H8, G7 },
        { PLAYER_BLACK, PIECE_BISHOP, G7, F6 },
        { PLAYER_BLACK, PIECE_BISHOP, F6, E5 },
        { PLAYER_BLACK, PIECE_BISHOP, E5, D4 },
        { PLAYER_BLACK, PIECE_BISHOP, D4, C3 },
        { PLAYER_BLACK, PIECE_BISHOP, C3, B2 },
        { PLAYER_BLACK, PIECE_BISHOP, B2, A1 },
    };

    std::vector<move_s> white_moves;
    white_moves.insert(white_moves.end(), white_loop_1st_half.begin(), white_loop_1st_half.end());
    white_moves.push_back({ PLAYER_WHITE, PIECE_KING, B3, A2 });
    white_moves.insert(white_moves.end(), white_loop_2nd_half.begin(), white_loop_2nd_half.end());
    white_moves.push_back({ PLAYER_WHITE, PIECE_KING, A2, B1 });
    white_moves.insert(white_moves.end(), white_loop_1st_half.begin(), white_loop_1st_half.end());
    white_moves.push_back({ PLAYER_WHITE, PIECE_KING, B1, C2 });
    white_moves.insert(white_moves.end(), white_loop_2nd_half.begin(), white_loop_2nd_half.end());
    ASSERT(25u < white_moves.size());
    white_moves.erase(white_moves.begin() + 25u, white_moves.end());
    ASSERT(25u == white_moves.size());

    std::vector<move_s> black_moves;
    black_moves.insert(black_moves.end(), black_loop_1st_half.begin(), black_loop_1st_half.end());
    black_moves.push_back({ PLAYER_BLACK, PIECE_KING, B6, A7 });
    black_moves.insert(black_moves.end(), black_loop_2nd_half.begin(), black_loop_2nd_half.end());
    black_moves.push_back({ PLAYER_BLACK, PIECE_KING, A7, B8 });
    black_moves.insert(black_moves.end(), black_loop_1st_half.begin(), black_loop_1st_half.end());
    black_moves.push_back({ PLAYER_BLACK, PIECE_KING, B8, C7 });
    black_moves.insert(black_moves.end(), black_loop_2nd_half.begin(), black_loop_2nd_half.end());
    ASSERT(25u < black_moves.size());
    black_moves.erase(black_moves.begin() + 25u, black_moves.end());
    ASSERT(25u == black_moves.size());

    fill_white_move_seq(white_moves);
    fill_black_move_seq(black_moves);
    ASSERT(game_result_t::DRAW_50_MOVE_RULE == do_play(white_to_play, black_to_play, board));
}

TEST(Gameplay_Play_AfterBlackMove_DrawBy50MoveRule) {
    auto board = prepare_board([](auto& board){
        board[B4] = FWK;
        board[B3] = FBN;
        board[B6] = FBK;
        board[H4] = FWP;
        board[H5] = FBP;
        board[H1] = FWB;
        board[A1] = FBB;
    });
    board_state_meta_set_castling_rights(board, 0b1111);
    
    std::vector<move_s> white_loop_1st_half = {
        { PLAYER_WHITE, PIECE_BISHOP, H1, G2 },
        { PLAYER_WHITE, PIECE_BISHOP, G2, F3 },
        { PLAYER_WHITE, PIECE_BISHOP, F3, E4 },
        { PLAYER_WHITE, PIECE_BISHOP, E4, D5 },
        { PLAYER_WHITE, PIECE_BISHOP, D5, C6 },
        { PLAYER_WHITE, PIECE_BISHOP, C6, B7 },
        { PLAYER_WHITE, PIECE_BISHOP, B7, A8 }
    };
    std::vector<move_s> white_loop_2nd_half = {
        { PLAYER_WHITE, PIECE_BISHOP, A8, B7 },
        { PLAYER_WHITE, PIECE_BISHOP, B7, C6 },
        { PLAYER_WHITE, PIECE_BISHOP, C6, D5 },
        { PLAYER_WHITE, PIECE_BISHOP, D5, E4 },
        { PLAYER_WHITE, PIECE_BISHOP, E4, F3 },
        { PLAYER_WHITE, PIECE_BISHOP, F3, G2 },
        { PLAYER_WHITE, PIECE_BISHOP, G2, H1 },
    };
    std::vector<move_s> black_loop_1st_half = {
        { PLAYER_BLACK, PIECE_BISHOP, A1, B2 },
        { PLAYER_BLACK, PIECE_BISHOP, B2, C3 },
        { PLAYER_BLACK, PIECE_BISHOP, C3, D4 },
        { PLAYER_BLACK, PIECE_BISHOP, D4, E5 },
        { PLAYER_BLACK, PIECE_BISHOP, E5, F6 },
        { PLAYER_BLACK, PIECE_BISHOP, F6, G7 },
        { PLAYER_BLACK, PIECE_BISHOP, G7, H8 }
    };
    std::vector<move_s> black_loop_2nd_half = {
        { PLAYER_BLACK, PIECE_BISHOP, H8, G7 },
        { PLAYER_BLACK, PIECE_BISHOP, G7, F6 },
        { PLAYER_BLACK, PIECE_BISHOP, F6, E5 },
        { PLAYER_BLACK, PIECE_BISHOP, E5, D4 },
        { PLAYER_BLACK, PIECE_BISHOP, D4, C3 },
        { PLAYER_BLACK, PIECE_BISHOP, C3, B2 },
        { PLAYER_BLACK, PIECE_BISHOP, B2, A1 },
    };

    std::vector<move_s> white_moves;
    white_moves.push_back({ PLAYER_WHITE, PIECE_KING, B4, B3 });
    white_moves.insert(white_moves.end(), white_loop_1st_half.begin(), white_loop_1st_half.end());
    white_moves.push_back({ PLAYER_WHITE, PIECE_KING, B3, A2 });
    white_moves.insert(white_moves.end(), white_loop_2nd_half.begin(), white_loop_2nd_half.end());
    white_moves.push_back({ PLAYER_WHITE, PIECE_KING, A2, B1 });
    white_moves.insert(white_moves.end(), white_loop_1st_half.begin(), white_loop_1st_half.end());
    white_moves.push_back({ PLAYER_WHITE, PIECE_KING, B1, C2 });
    white_moves.insert(white_moves.end(), white_loop_2nd_half.begin(), white_loop_2nd_half.end());
    ASSERT(26u < white_moves.size());
    white_moves.erase(white_moves.begin() + 26u, white_moves.end());
    ASSERT(26u == white_moves.size());

    std::vector<move_s> black_moves;
    black_moves.insert(black_moves.end(), black_loop_1st_half.begin(), black_loop_1st_half.end());
    black_moves.push_back({ PLAYER_BLACK, PIECE_KING, B6, A7 });
    black_moves.insert(black_moves.end(), black_loop_2nd_half.begin(), black_loop_2nd_half.end());
    black_moves.push_back({ PLAYER_BLACK, PIECE_KING, A7, B8 });
    black_moves.insert(black_moves.end(), black_loop_1st_half.begin(), black_loop_1st_half.end());
    black_moves.push_back({ PLAYER_BLACK, PIECE_KING, B8, C7 });
    black_moves.insert(black_moves.end(), black_loop_2nd_half.begin(), black_loop_2nd_half.end());
    ASSERT(25u < black_moves.size());
    black_moves.erase(black_moves.begin() + 25u, black_moves.end());
    ASSERT(25u == black_moves.size());

    fill_white_move_seq(white_moves);
    fill_black_move_seq(black_moves);
    ASSERT(game_result_t::DRAW_50_MOVE_RULE == do_play(white_to_play, black_to_play, board));
}