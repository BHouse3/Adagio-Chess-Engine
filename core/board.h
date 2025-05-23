/*
 * Board Representation Class Header
 * Board representation based on bitboards: 64 bit integers where each bit represents a square of the chess board
*/
#pragma once

#include <vector>
#include <stack>
#include <string>
#include <cassert>
#include <sstream>
#include <iostream>
#include <cstring>
#include <iomanip>
#include "types.h"
#include "zobrist.h"


class cBoard 
{
    public:
        void setFromFEN(const std::string& FEN);
        
        std::string getFEN() const;
        
        void resetBoard();


        void makeMove(Move move);
        void undoMove();

        bool isSquareAttacked(Square sq, Side attackingSide) const;
        bool inCheck(Side side) const;

        Bitboard pieces(Side side, pieceType pt) const;
        Bitboard occupied (Side side) const;
        Bitboard allOccupied() const;
        pieceCode pieceBySquare(Square sq) const;
        Square kingSquare(Side side) const;

        Side sideToMove() const;
        Square enPassantSquare() const;
        int castlingRights() const;
        int halfmoveClock() const;
        int fullmoveNumber() const;
        uint64_t hash() const;

    private:
        Bitboard pieceBB[2][6];
        Bitboard occupancy[2];
        Bitboard allOccupiedSquares;
        
        Square kingSq[2];
        pieceCode pieceMailbox[64];

        Side stm;
        Square enPassant;
        int castling;
        int halfmoveCounter;
        int fullmoveCounter;
        uint64_t zobristKey;

        struct Undo 
        {
            uint64_t hash;
            Move move;
            int castling;
            Square enPassant;
            pieceType capturedPiece;
            int halfmove;
        };

        std::stack<Undo> history;
};