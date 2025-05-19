
#include "debug.h"


void printBitboard(const Bitboard& board, std::ostream& out) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;
            Bitboard mask = 1ULL << square;
            out << (board & mask ? "1 " : ". ");
        }
        out << std::endl;
    }
    out << std::endl;
}

void verifyBoardState(const cBoard& cb, std::ostream& out)
{
    out << "White Pawns\n\n";
    printBitboard(cb.pieces(WHITE, PAWN), out); 
    out << "\nBlack Pawns\n\n";
    printBitboard(cb.pieces(BLACK, PAWN), out);

    out << "\nWhite KNIGHTs\n\n";
    printBitboard(cb.pieces(WHITE, KNIGHT), out); 
    out << "\nBlack KNIGHTs\n\n";
    printBitboard(cb.pieces(BLACK, KNIGHT), out);

    out << "\nWhite BISHOPS\n\n";
    printBitboard(cb.pieces(WHITE, BISHOP), out); 
    out << "\nBlack BISHOPS\n\n";
    printBitboard(cb.pieces(BLACK, BISHOP), out);

    out << "\nWhite ROOKs\n\n";
    printBitboard(cb.pieces(WHITE, ROOK), out); 
    out << "\nBlack ROOKs\n\n";
    printBitboard(cb.pieces(BLACK, ROOK), out);

    out << "\nWhite QUEENs\n\n";
    printBitboard(cb.pieces(WHITE, QUEEN), out); 
    out << "\nBlack QUEENs\n\n";
    printBitboard(cb.pieces(BLACK, QUEEN), out);

    out << "\nWhite KINGs\n\n";
    printBitboard(cb.pieces(WHITE, KING), out); 
    out << "\nBlack KINGs\n\n";
    printBitboard(cb.pieces(BLACK, KING), out);

    out << "\nWhite occupancy\n\n";
    printBitboard(cb.occupied(WHITE), out); 
    out << "\nBlack occupancy\n\n";
    printBitboard(cb.occupied(BLACK), out);

    out << "\nAll occupancy\n\n";
    printBitboard(cb.allOccupied(),out);

    out << "White King Square: " << cb.kingSquare(WHITE) << "   Black King Square: " << cb.kingSquare(BLACK);
    out << "\n\nSide to Move: " << cb.sideToMove();
    out << "\n\nEn Passant Square: " << cb.enPassantSquare();
    out << "\n\ncastlingRights: ";

    int castRight = cb.castlingRights();
    std::string castRightFEN = "";

    if (castRight & whiteKingSide) {castRightFEN += "K";}
    if (castRight & whiteQueenSide) {castRightFEN += "Q";}
    if (castRight & blackKingSide) {castRightFEN += "k";}
    if (castRight & blackQueenSide) {castRightFEN += "q";}

    out << castRightFEN;

    out << "\n\nHalf Move Counter: " << cb.halfmoveClock();
    out << "\n\nFull Move Counter: " << cb.fullmoveNumber();
}
