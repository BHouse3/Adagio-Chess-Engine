
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
    char piece;
    Square sq;
    pieceCode pieceAtSquare;
    for (int rank = 7; rank >= 0; --rank)
    {
        for (int file = 0; file < 8; file++)
        {
            sq = rank*8 + file;
            pieceAtSquare = cb.pieceBySquare(sq);
            switch (pieceAtSquare)
            {
                case (W_PAWN): piece = 'P'; break;
                case (W_KNIGHT): piece = 'N'; break;
                case (W_BISHOP): piece = 'B'; break;
                case (W_ROOK): piece = 'R'; break;
                case (W_QUEEN): piece = 'Q'; break;
                case (W_KING): piece = 'K'; break;
                case (B_PAWN): piece = '6'; break;
                case (B_KNIGHT): piece = 'n'; break;
                case (B_BISHOP): piece = 'b'; break;
                case (B_ROOK): piece = 'r'; break;
                case (B_QUEEN): piece = 'q'; break;
                case (B_KING): piece = 'k'; break;
                default: piece = '.'; break;
            }
            out << std::setw(5) << piece << " ";
        }
        out << std::endl;
    }

    out << "\n\nWhite Pawns\n\n";
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

void decodeHexMove(std::string& hexMove, std::ostream& out)
{
    int moveToMake = std::stoi(hexMove, nullptr, 16);

    int destSquare = moveToMake & 0x0000003F;
    int sourceSquare = (moveToMake & 0x00000FC0) >> 6;
    int special = (moveToMake & 0x0000F000) >> 12; 

    out << "Destination Square: " << destSquare << "\nsourceSquare: " << sourceSquare << "\nSpecial Code: " << special << "\n\n";
}
