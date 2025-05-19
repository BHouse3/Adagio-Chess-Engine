/*
 * Board Representation Class Header
 * Board representation based on bitboards: 64 bit integers where each bit represents a square of the chess board
*/
#pragma once

#include <vector>
#include <string>
#include <cassert>
#include <sstream>
#include "types.h"
#include "zobrist.h"


class cBoard {
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

        Side stm;
        Square enPassant;
        int castling;
        int halfmoveCounter;
        int fullmoveCounter;
        uint64_t zobristKey;

        struct Undo 
        {
            Move move;
            Square capturedSq;
            int castling;
            Square enPassant;
            int halfmove;
            uint64_t hash;
        };

        std::vector<Undo> history;
};