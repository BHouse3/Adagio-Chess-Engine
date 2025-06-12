#pragma once

#include "../core/types.h"
#include "../core/board.h"
#include "../core/movegen.h"
#include "../util/debug.h"

Bitboard perft(int depth, cBoard& cb, bool root, std::ostream& out = std::cout)
{
    Move move_list[256];
    int i;
    uint64_t nodes = 0;
    moveGenerator mg;

    int moveListIndex = mg.generateMoves(cb, move_list);

    if (depth == 1) 
    {
        uint64_t legalMoves = 0;
        for (int j = 0; j < moveListIndex; j++) 
        {
            cb.makeMove(move_list[j]); 
            if (!cb.inCheck(static_cast<Side>(oppositeSide(cb.sideToMove())))) 
            {
                legalMoves++;
                if (root) {out << moveToString(move_list[j]) << ": 1\n";}
            } 
            cb.undoMove();
        }
        return legalMoves;
    }

    for (i = 0; i < moveListIndex; i++) 
    {
        cb.makeMove(move_list[i]);
        if (!cb.inCheck(static_cast<Side>(oppositeSide(cb.sideToMove()))))
        {
            uint64_t childNodes = perft(depth - 1, cb, false);//, out);
            if (root) {out << moveToString(move_list[i]) << ": " << childNodes << '\n';}
            nodes += childNodes;
        }
        cb.undoMove();
    }


    if (root) 
    {
        out << '\n';
        out << "Nodes searched: " << nodes << '\n';
    }

    return nodes;
}