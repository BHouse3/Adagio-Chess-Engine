
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

std::string moveToString(Move encodedMove)
{
    Bitboard destSquare = (encodedMove & 0x0000003F);
    Bitboard sourceSquare = ((encodedMove & 0x00000FC0) >> 6);
    int special = (encodedMove & 0x0000F000) >> 12;

    std::string output = "";

    int sourceRank = sourceSquare/8;
    int sourceFile = sourceSquare % 8;
    switch (sourceFile)
    {
        case (0): output += "a"; break; 
        case (1): output += "b"; break; 
        case (2): output += "c"; break; 
        case (3): output += "d"; break; 
        case (4): output += "e"; break; 
        case (5): output += "f"; break; 
        case (6): output += "g"; break; 
        case (7): output += "h"; break; 
    }
    output += std::to_string(sourceRank+1);

    int destRank = destSquare/8;
    int destFile = destSquare % 8;
    switch (destFile)
    {
        case (0): output += "a"; break; 
        case (1): output += "b"; break; 
        case (2): output += "c"; break; 
        case (3): output += "d"; break; 
        case (4): output += "e"; break; 
        case (5): output += "f"; break; 
        case (6): output += "g"; break; 
        case (7): output += "h"; break; 
    }
    output += std::to_string(destRank+1);

    switch (special)
    {
        case (8): if (destRank == 7) {output += "N";} else {output += "n";} break;
        case (9): if (destRank == 7) {output += "B";} else {output += "b";} break;
        case (10): if (destRank == 7) {output += "R";} else {output += "r";} break;
        case (11): if (destRank == 7) {output += "Q";} else {output += "q";} break;
        case (12): if (destRank == 7) {output += "N";} else {output += "n";} break;
        case (13): if (destRank == 7) {output += "B";} else {output += "b";} break;
        case (14): if (destRank == 7) {output += "R";} else {output += "r";} break;
        case (15): if (destRank == 7) {output += "Q";} else {output += "q";} break;
    }

    return output;
}

std::string decodeMove(Move m)
{
    std::string out = moveToString(m);
    out += " Special Code: " + std::to_string((m & 0x0000F000) >> 12);
    return out;
}