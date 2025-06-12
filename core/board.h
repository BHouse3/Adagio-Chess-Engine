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
#include "../util/magicBitboards.h"
#include "../util/kingLookup.h"
#include "../util/knightLookup.h"


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

        inline Bitboard pieces(Side side, pieceType pt) const {return pieceBB[side][pt];}
        inline Bitboard occupied(Side side) const {return occupancy[side];}
        inline Bitboard allOccupied() const {return allOccupiedSquares;}
        inline pieceCode pieceBySquare(Square sq) const {return pieceMailbox[sq];}
        inline Square kingSquare(Side side) const {return kingSq[side];}
        inline Side sideToMove() const {return stm;}
        inline Square enPassantSquare() const {return enPassant;}
        inline int castlingRights() const {return castling;}
        inline int halfmoveClock() const {return halfmoveCounter;}
        inline int fullmoveNumber() const {return fullmoveCounter;}

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

        template<Side color>
        inline Bitboard westPawnAttacksBoard(const Bitboard pawns) const
        {
            return (color == WHITE) ? ((pawns << 7) & 0x7F7F7F7F7F7F7F7F) : ((pawns >> 9) & 0x7F7F7F7F7F7F7F7F); 
        }

        template<Side color>
        inline Bitboard eastPawnAttacksBoard(const Bitboard pawns) const 
        {
            return (color == WHITE) ? ((pawns << 9) & 0xFEFEFEFEFEFEFEFE) : ((pawns >> 7) & 0xFEFEFEFEFEFEFEFE);
        }

};