#pragma once

#include <cstdint>
#include <string>

typedef uint64_t Bitboard;
typedef int Square;

/*
 * move enconding 32 bit int 
 * (16 bit move ordering heuristics, 6-bit Source square, 6-bit Destination Square, 4-bit special move info)
 * 0x0000003F for destination square
 * 0x00000FC0 for Source Square
 * 0x0000F000 for special move info
    0: quiet move
    1: double pawn push
    2: king castle
    3: queen castle
    4: captures
    5: enPassant capture
    8: knight promotion
    9: bishop promotion
    10: rook promotion
    11: queen promotion
    12: knight-promotion capture
    13: bishop promotion capture
    14: rook-Promo capture
    15: queen promo capture 

 * 0xFFFF0000 will be used for a move ordering score later on.
    The point being that we would like to evaluate the best moves first to 
    maximize the advantages of search optimizations like alpha beta pruning.
    We can give each move a score based on features of the resulting position
    such as a capture, check, pin, etc. This will give a rough approximation of the 
    eval without having to use the actual evaluation function
*/
typedef uint32_t Move;

enum Side : int
{
    WHITE = 0,
    BLACK = 1,
};

enum pieceType : int8_t 
{
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5,
    NO_PIECE = -1
};

enum pieceCode : int8_t
{
    EMPTY = -1,

    W_PAWN = 0,
    W_KNIGHT = 1,
    W_BISHOP = 2,
    W_ROOK = 3,
    W_QUEEN = 4,
    W_KING = 5,

    B_PAWN = 6,
    B_KNIGHT = 7,
    B_BISHOP = 8,
    B_ROOK = 9,
    B_QUEEN = 10,
    B_KING = 11
};


enum castling : int
{
    whiteKingSide = 0b0001,
    whiteQueenSide = 0b0010,
    blackKingSide = 0b0100,
    blackQueenSide = 0b1000
};

enum moveFlags : int
{
    QUIET = 0,
    DOUBLE_PAWN_PUSH = 1,
    KING_CASTLE = 2,
    QUEEN_CASTLE = 3,
    CAPTURE = 4,
    EN_PASSANT = 5,
    KNIGHT_PROMOTION = 8,
    BISHOP_PROMOTION = 9,
    ROOK_PROMOTION = 10,
    QUEEN_PROMOTION = 11,
    KNIGHT_PROMO_CAPTURE = 12,
    BISHOP_PROMO_CAPTURE = 13,
    ROOK_PROMO_CAPTURE = 14,
    QUEEN_PROMO_CAPTURE = 15
};

inline Side oppositeSide(const Side side) {return side == Side::WHITE ? Side::BLACK : Side::WHITE;}

inline Move encodeMove(const Square source, const Square dest, const int specInfo) 
{
    return (specInfo << 12) | (source << 6) | dest;
}

inline Square sourceSquare(const Move move) {return move & 0x3F;}
inline Square destSquare(const Move move) {return (move >> 6) & 0x3F;}
inline int moveSpecInfo(const Move move) {return static_cast<int> (move>>12) & 0xF;}

inline pieceType getPieceType(const pieceCode pc) {
    if (pc == EMPTY) {return NO_PIECE;}
    return static_cast<pieceType>(pc % 6);
}

inline Bitboard fenMask(const int x, const int y) {return ((1ULL << ((8*x) + y)));}