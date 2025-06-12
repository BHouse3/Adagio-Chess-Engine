#pragma once
#include "board.h"
#include "types.h"
#include "../util/kingLookup.h"
#include "../util/knightLookup.h"
#include "../util/magicBitboards.h"

class moveGenerator
{
public: 
    inline int generateMoves(const cBoard& cb, Move *moveList)
    {
        if (cb.sideToMove() == WHITE) {enemyAttacks = generateEnemyAttacks<BLACK>(cb); return generateAllMoves<WHITE>(cb, moveList);}
        else                          {enemyAttacks = generateEnemyAttacks<WHITE>(cb); return generateAllMoves<BLACK>(cb, moveList);}
    }

private:
    template<Side color>
    int generateAllMoves(const cBoard& cb, Move *moveList)
    {
        Move *movePtr = moveList;

        generatePawnMoves<color>(cb,movePtr);
        generateKnightMoves<color>(cb, movePtr);
        generateKingMoves<color>(cb, movePtr);
        generateBishopMoves<color>(cb, movePtr);
        generateRookMoves<color>(cb, movePtr);
        generateQueenMoves<color>(cb, movePtr);

        return movePtr - moveList;
    }

    template<Side color>
    void generatePawnMoves(const cBoard& cb, Move*& moveList)
    {
        constexpr int singlePush = (color == WHITE) ? 8 : -8;
        constexpr int doublePush = (color == WHITE) ? 16 : -16;
        constexpr Bitboard startRank = (color == WHITE) ? 0x000000000000FF00 : 0x00FF000000000000;
        constexpr int promoRank = (color == WHITE) ? 7 : 0;
        constexpr int westCapSource = (color == WHITE) ? 7 : -9;
        constexpr int eastCapSource = (color == WHITE) ? 9 : -7;
        constexpr Side enemySide = (color == WHITE) ? BLACK : WHITE;
        
        const Bitboard pawns = cb.pieces(color, PAWN);
        const Bitboard empty = ~cb.allOccupied();
        const Bitboard enemyPieces = cb.occupied(enemySide);
        const Bitboard enPassTarget = cb.enPassantSquare() == -1 ? 0 : (1ULL << cb.enPassantSquare());

        //single pushes (can move forward one square if square is empty)
        //check for pawns that end up on promotion rank after double push
        Bitboard validSinglePushes = singlePawnPush<color>(pawns) & empty;
        while (validSinglePushes)
        {
            const int destSq = __builtin_ctzll(validSinglePushes);
            validSinglePushes &= validSinglePushes-1;
            const int sourceSq = destSq - singlePush;
            const int rank = destSq/8;

            //promotion case
            if (rank == promoRank)
            {
                *moveList++ = encodeMove(sourceSq, destSq, 11); //queen
                *moveList++ = encodeMove(sourceSq, destSq, 10); //rook
                *moveList++ = encodeMove(sourceSq, destSq, 9);  //bishop
                *moveList++ = encodeMove(sourceSq, destSq, 8);  //knight
            }
            else 
            {
                *moveList++ = encodeMove(sourceSq, destSq, 0); //quiet single pawn push
            }
        }

        //double pushes (can move forward two squares if on starting rank and the two squares are empty)
        Bitboard validDoublePushes = doublePawnPush<color>(pawns, empty);
        while (validDoublePushes)
        {
            const int destSq = __builtin_ctzll(validDoublePushes);
            validDoublePushes &= validDoublePushes-1;
            const int sourceSq = destSq - doublePush;

            *moveList++ = encodeMove(sourceSq, destSq, 1); //double pawn push
        }

        //west captures (important to seperate east and west attacks to sort out source square)
        Bitboard validWestCaptures = westPawnAttacks<color>(pawns) & (enemyPieces | enPassTarget);
        while (validWestCaptures)
        {
            const int destSq = __builtin_ctzll(validWestCaptures);
            validWestCaptures &= validWestCaptures-1;
            const int sourceSq = destSq - westCapSource;

            if ((destSq/8) == promoRank)
            {
                *moveList++ = encodeMove(sourceSq, destSq, 15); //queen cap
                *moveList++ = encodeMove(sourceSq, destSq, 14); //rook cap
                *moveList++ = encodeMove(sourceSq, destSq, 13); //bishop cap
                *moveList++ = encodeMove(sourceSq, destSq, 12); //knight cap
            }
            else
            {
                if (destSq == cb.enPassantSquare()) {*moveList++ = encodeMove(sourceSq, destSq, 5);} //enPassant capture
                else {*moveList++ = encodeMove(sourceSq, destSq, 4);} //regular capture
            }
        }

        //east captures
        Bitboard validEastCaptures = eastPawnAttacks<color>(pawns) & (enemyPieces | enPassTarget);
        while (validEastCaptures)
        {
            const int destSq = __builtin_ctzll(validEastCaptures);
            validEastCaptures &= validEastCaptures-1;
            const int sourceSq = destSq - eastCapSource;

            if ((destSq/8)==promoRank)
            {
                *moveList++ = encodeMove(sourceSq, destSq, 15); //queen cap
                *moveList++ = encodeMove(sourceSq, destSq, 14); //rook cap
                *moveList++ = encodeMove(sourceSq, destSq, 13); //bishop cap
                *moveList++ = encodeMove(sourceSq, destSq, 12); //knight cap
            }
            else
            {
                if (destSq == cb.enPassantSquare()) {*moveList++ = encodeMove(sourceSq, destSq, 5);} //enPassant capture
                else {*moveList++ = encodeMove(sourceSq, destSq, 4);} //regular capture
            }
        }
    }

    template<Side color>
    void generateKnightMoves(const cBoard& cb, Move*& moveList)
    {
        constexpr Side enemySide = (color == WHITE) ? BLACK : WHITE;

        const Bitboard enemyPieces = cb.occupied(enemySide);
        const Bitboard emptySquares = ~cb.allOccupied();

        Bitboard knights = cb.pieces(color, KNIGHT);
        
        //knights will only capture enemy pieces or have quiet moves
        //for each knight find all possible moves through a lookup table by source square
        //do bitwise ands with the LUT bitboard and empty and enemy square bitboards
        while (knights)
        {
            const int sourceSq = __builtin_ctzll(knights);
            knights &= knights-1;

            Bitboard validKnightQuiets = knightAttacks[sourceSq] & emptySquares;
            Bitboard validKnightCaptures = knightAttacks[sourceSq] & enemyPieces;

            while (validKnightCaptures)
            {
                const int destSq = __builtin_ctzll(validKnightCaptures);
                validKnightCaptures &= validKnightCaptures-1;

                *moveList++ = encodeMove(sourceSq, destSq, 4);
            }
            
            while (validKnightQuiets)
            {
                const int destSq = __builtin_ctzll(validKnightQuiets);
                validKnightQuiets &= validKnightQuiets-1;

                *moveList++ = encodeMove(sourceSq, destSq, 0);
            }
        }
    }

    template<Side color>
    void generateBishopMoves(const cBoard& cb, Move*& moveList)
    {
        constexpr Side enemySide = (color == WHITE) ? BLACK : WHITE;

        const Bitboard enemySquares = cb.occupied(enemySide);
        const Bitboard occupiedSquares = cb.allOccupied();
        const Bitboard emptySquares = ~occupiedSquares;

        Bitboard bishops = cb.pieces(color, BISHOP);

        while (bishops)
        {
            const int sourceSq = __builtin_ctzll(bishops);
            bishops &= bishops-1;

            const Bitboard bishopAttacks = bishop_magic_numbers[sourceSq].attacks[(bishop_magic_numbers[sourceSq].magicNumber * (occupiedSquares & bishop_magic_numbers[sourceSq].mask)) >> (64-bishop_magic_numbers[sourceSq].shift)];
            Bitboard validBishopCaptures = bishopAttacks & enemySquares;
            Bitboard validBishopQuiets = bishopAttacks & emptySquares;

            while (validBishopCaptures)
            {
                const int destSq = __builtin_ctzll(validBishopCaptures);
                validBishopCaptures &= validBishopCaptures-1;

                *moveList++ = encodeMove(sourceSq, destSq, 4);
            }

            while (validBishopQuiets)
            {
                const int destSq = __builtin_ctzll(validBishopQuiets);
                validBishopQuiets &= validBishopQuiets-1;

                *moveList++ = encodeMove(sourceSq, destSq, 0);
            }
        }
    }

    template<Side color>
    void generateRookMoves(const cBoard& cb, Move*& moveList)
    {
        constexpr Side enemySide = (color == WHITE) ? BLACK : WHITE;

        const Bitboard enemySquares = cb.occupied(enemySide);
        const Bitboard occupiedSquares = cb.allOccupied();
        const Bitboard emptySquares = ~occupiedSquares;

        Bitboard rooks = cb.pieces(color, ROOK);

        while (rooks)
        {
            const int sourceSq = __builtin_ctzll(rooks);
            rooks &= rooks-1;

            const Bitboard rookAttacks = rook_magic_numbers[sourceSq].attacks[(rook_magic_numbers[sourceSq].magicNumber * (occupiedSquares & rook_magic_numbers[sourceSq].mask)) >> (64-rook_magic_numbers[sourceSq].shift)];
            Bitboard validRookCaptures = rookAttacks & enemySquares;
            Bitboard validRookQuiets = rookAttacks & emptySquares;

            while (validRookCaptures)
            {
                const int destSq = __builtin_ctzll(validRookCaptures);
                validRookCaptures &= validRookCaptures-1;

                *moveList++ = encodeMove(sourceSq, destSq, 4);
            }

            while (validRookQuiets)
            {
                const int destSq = __builtin_ctzll(validRookQuiets);
                validRookQuiets &= validRookQuiets-1;

                *moveList++ = encodeMove(sourceSq, destSq, 0);
            }
        }
    }

    template<Side color>
    void generateQueenMoves(const cBoard& cb, Move*& moveList)
    {
        constexpr Side enemySide = (color == WHITE) ? BLACK : WHITE;

        const Bitboard enemySquares = cb.occupied(enemySide);
        const Bitboard occupiedSquares = cb.allOccupied();
        const Bitboard emptySquares = ~occupiedSquares;

        Bitboard queens = cb.pieces(color, QUEEN);

        while (queens)
        {
            const int sourceSq = __builtin_ctzll(queens);
            queens &= queens-1;

            const Bitboard bishopAttacks = bishop_magic_numbers[sourceSq].attacks[(bishop_magic_numbers[sourceSq].magicNumber * (occupiedSquares & bishop_magic_numbers[sourceSq].mask)) >> (64-bishop_magic_numbers[sourceSq].shift)];
            const Bitboard rookAttacks = rook_magic_numbers[sourceSq].attacks[(rook_magic_numbers[sourceSq].magicNumber * (occupiedSquares & rook_magic_numbers[sourceSq].mask)) >> (64-rook_magic_numbers[sourceSq].shift)];
            const Bitboard queenAttacks = bishopAttacks | rookAttacks;
            Bitboard validQueenCaptures = queenAttacks & enemySquares;
            Bitboard validQueenQuiets = queenAttacks & emptySquares;

            while (validQueenCaptures)
            {
                const int destSq = __builtin_ctzll(validQueenCaptures);
                validQueenCaptures &= validQueenCaptures-1;

                *moveList++ = encodeMove(sourceSq, destSq, 4);
            }
            while (validQueenQuiets)
            {
                const int destSq = __builtin_ctzll(validQueenQuiets);
                validQueenQuiets &= validQueenQuiets-1;

                *moveList++ = encodeMove(sourceSq, destSq, 0);
            }
        }
    }

    template<Side color>
    void generateKingMoves(const cBoard& cb, Move*& moveList)
    {
        constexpr Side enemySide = (color == WHITE) ? BLACK : WHITE;
        constexpr Bitboard kingSideEmptySquares = (color == WHITE) ? 0x60ULL : 0x6000000000000000ULL;
        constexpr Bitboard queenSideEmptySquares = (color == WHITE) ?  0x0EULL : 0x0E00000000000000ULL;
        constexpr Bitboard kingSideSquares = (color == WHITE) ? 0x70ULL : 0x7000000000000000ULL;
        constexpr Bitboard queenSideSquares = (color == WHITE) ? 0x1CULL : 0x1C00000000000000ULL;
        constexpr int castSource = (color == WHITE) ? 4 : 60;
        constexpr int kingSideCastDest = (color == WHITE) ? 6 : 62;
        constexpr int queenSideCastDest = (color == WHITE) ? 2 : 58;

        const Bitboard emptySquares = ~cb.allOccupied();
        const Bitboard enemySquares = cb.occupied(enemySide);

        const Square king = cb.kingSquare(color);
        Bitboard validKingCaptures = kingAttacks[king] & enemySquares;
        Bitboard validKingQuiets = kingAttacks[king] & emptySquares;

        while (validKingCaptures)
        {
            const int destSq = __builtin_ctzll(validKingCaptures);
            validKingCaptures &= validKingCaptures-1;

            *moveList++ = encodeMove(king, destSq, 4);
        }
        while (validKingQuiets)
        {
            const int destSq = __builtin_ctzll(validKingQuiets);
            validKingQuiets &= validKingQuiets-1;

            *moveList++ = encodeMove(king, destSq, 0);
        }

        const int castRights = castlingRights<color>(cb);
        if ((castRights & 0b01) && ((kingSideEmptySquares & emptySquares) == kingSideEmptySquares) && !(kingSideSquares & enemyAttacks))
        {
            *moveList++ = encodeMove(castSource, kingSideCastDest, 2);
        }
        if ((castRights & 0b10) && ((queenSideEmptySquares & emptySquares) == queenSideEmptySquares) && !(queenSideSquares & enemyAttacks))
        {
            *moveList++ = encodeMove(castSource, queenSideCastDest, 3);
        }
    }

     template<Side color>
    inline Bitboard westPawnAttacks(const Bitboard pawns)
    {
        return (color == WHITE) ? ((pawns << 7) & 0x7F7F7F7F7F7F7F7F) : ((pawns >> 9) & 0x7F7F7F7F7F7F7F7F); 
    }

    template<Side color>
    inline Bitboard eastPawnAttacks(const Bitboard pawns)
    {
        return (color == WHITE) ? ((pawns << 9) & 0xFEFEFEFEFEFEFEFE) : ((pawns >> 7) & 0xFEFEFEFEFEFEFEFE);
    }

    template<Side color>
    inline Bitboard singlePawnPush(const Bitboard pawns)
    {
        return (color == WHITE) ? (pawns << 8) : (pawns >> 8);
    }

    template<Side color>
    inline Bitboard doublePawnPush(const Bitboard pawns, const Bitboard emptySquares)
    {
        return (color == WHITE) ? (((pawns & 0x000000000000FF00) << 16) & emptySquares & (emptySquares << 8)) : (((pawns & 0x00FF000000000000) >> 16) & emptySquares & (emptySquares >> 8));
    }

    template<Side color>
    inline int castlingRights(const cBoard& cb)
    {
        constexpr int castlingMask = (color == WHITE) ? 0b0011 : 0b1100;
        constexpr int castlingShift = (color == WHITE) ? 0 : 2;
        return (cb.castlingRights() & castlingMask) >> castlingShift;
    }

    template<Side color>
    Bitboard generateEnemyAttacks(const cBoard& cb)
    {
        const Bitboard occupied = cb.allOccupied();

        Bitboard pawnAttack = 0;
        Bitboard bishopAttack = 0;
        Bitboard knightAttack = 0;
        Bitboard rookAttack = 0;
        Bitboard queenAttack = 0;
        Bitboard kingAttack = 0;

        const Bitboard pawns = cb.pieces(color, PAWN);
        Bitboard knights = cb.pieces(color, KNIGHT);
        Bitboard bishops = cb.pieces(color, BISHOP);
        Bitboard rooks = cb.pieces(color, ROOK);
        Bitboard queens = cb.pieces(color, QUEEN);
        Bitboard kings = cb.pieces(color, KING);

        while (bishops)
        {
            const int sq = __builtin_ctzll(bishops);
            bishops &= bishops - 1;
            bishopAttack |= bishop_magic_numbers[sq].attacks[(bishop_magic_numbers[sq].magicNumber * (occupied & bishop_magic_numbers[sq].mask)) >> (64-bishop_magic_numbers[sq].shift)];
        }
        while (rooks)
        {
            const int sq = __builtin_ctzll(rooks);
            rooks &= rooks - 1;
            rookAttack |= rook_magic_numbers[sq].attacks[(rook_magic_numbers[sq].magicNumber * (occupied & rook_magic_numbers[sq].mask)) >> (64-rook_magic_numbers[sq].shift)];
        }
        while (queens)
        {
            const int sq = __builtin_ctzll(queens);
            queens &= queens - 1;
            Bitboard diagonalQueenAttack = bishop_magic_numbers[sq].attacks[(bishop_magic_numbers[sq].magicNumber * (occupied & bishop_magic_numbers[sq].mask)) >> (64-bishop_magic_numbers[sq].shift)];
            Bitboard perpendicularQueenAttack = rook_magic_numbers[sq].attacks[(rook_magic_numbers[sq].magicNumber * (occupied & rook_magic_numbers[sq].mask)) >> (64-rook_magic_numbers[sq].shift)];
            queenAttack |= diagonalQueenAttack | perpendicularQueenAttack;
        }
        while (knights)
        {
            const int sq = __builtin_ctzll(knights);
            knights &= knights-1;
            knightAttack |= knightAttacks[sq];
        }
        pawnAttack = eastPawnAttacks<color>(pawns) | westPawnAttacks<color>(pawns);
        kingAttack = kingAttacks[cb.kingSquare(color)];
        return pawnAttack | knightAttack | bishopAttack | rookAttack | queenAttack | kingAttack;
    }

    Bitboard enemyAttacks;
};
