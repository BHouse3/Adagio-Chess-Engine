//#define ENABLE_TIMING


#include "core/types.h"
#include "core/board.h"
#include "util/debug.h"
#include "util/knightLookup.h"
#include "util/kingLookup.h"
#include "core/movegen.h"
#include "tests/perft.h"


int main(int argc, char* argv[])
{
    std::ofstream myFile("debug.txt");

    if (argc < 3) 
    {
        std::cerr << "Usage: ./Engine <depth> <fen> [<moves>]" << std::endl;
        return 1;
    }

    int depth = std::stoi(argv[1]);
    std::string fen = argv[2];
    std::string moves = argc > 3 ? argv[3] : "";


    cBoard cb;
    cb.setFromFEN(fen);

    if (moves != "")
    {
        //decode move
        //apply the special flags
        //encode the move
        //Apply it to the board state

        std::stringstream moveList(moves);
        std::string currMov = "";
        while (getline(moveList, currMov, ' '))
        {
            int sourceFile = currMov[0] - 'a';
            int sourceRank = currMov[1] - '1';
            uint64_t sourceSq = (sourceRank * 8) + sourceFile;

            int destFile = currMov[2] - 'a';
            int destRank = currMov[3] - '1';
            uint64_t destSq = (destRank * 8) + destFile;

            int special = 0;

            uint64_t sourceSquareMask = 1ULL << sourceSq;
            uint64_t destSquareMask = 1ULL << destSq;

            Side color = cb.sideToMove();

            if (color == WHITE)
            {
                if (destSquareMask & cb.occupied(BLACK)) {special = 4;}
                if (destSquareMask & (1ULL << cb.enPassantSquare())) {special = 5;}
                if ((cb.pieceBySquare(sourceSq) == W_PAWN) && (destSquareMask == (sourceSquareMask << 16))) {special = 1;}
                if ((cb.pieceBySquare(sourceSq) == W_KING) && (destSq == 6) && (sourceSq == 4)) {special = 2;}
                if ((cb.pieceBySquare(sourceSq) == W_KING) && (destSq == 2) && (sourceSq == 4)) {special = 3;}

                if (currMov.size() == 5)
                {
                    char promoPiece = currMov[4];
                    switch (promoPiece)
                    {
                        case ('N'): if (destSquareMask & cb.occupied(BLACK)) {special = 12;} else {special = 8;} break;
                        case ('B'): if (destSquareMask & cb.occupied(BLACK)) {special = 13;} else {special = 9;} break;
                        case ('R'): if (destSquareMask & cb.occupied(BLACK)) {special = 14;} else {special = 10;} break;
                        case ('Q'): if (destSquareMask & cb.occupied(BLACK)) {special = 15;} else {special = 11;} break;
                    }
                }
            }
            else
            {
                if (destSquareMask & cb.occupied(WHITE)) {special = 4;}
                if (destSquareMask & (1ULL << cb.enPassantSquare())) {special = 5;}
                if ((cb.pieceBySquare(sourceSq) == B_PAWN) && (destSquareMask == (sourceSquareMask >> 16))) {special = 1;}
                if ((cb.pieceBySquare(sourceSq) == B_KING) && (destSq == 62) && (sourceSq == 60)) {special = 2;}
                if ((cb.pieceBySquare(sourceSq) == B_KING) && (destSq == 58) && (sourceSq == 60)) {special = 3;}

                if (currMov.size() == 5)
                {
                    char promoPiece = currMov[4];
                    switch (promoPiece)
                    {
                        case ('n'): if (destSquareMask & cb.occupied(WHITE)) {special = 12;} else {special = 8;} break;
                        case ('b'): if (destSquareMask & cb.occupied(WHITE)) {special = 13;} else {special = 9;} break;
                        case ('r'): if (destSquareMask & cb.occupied(WHITE)) {special = 14;} else {special = 10;} break;
                        case ('q'): if (destSquareMask & cb.occupied(WHITE)) {special = 15;} else {special = 11;} break;
                    }
                }
            }

            Move decodedMove = encodeMove(sourceSq, destSq, special);
            cb.makeMove(decodedMove);
        }
    }

    perft(depth, cb, true);

    // std::ofstream myFile("debug.txt");

    // Move moveList[256];

    // cBoard b;
    // b.resetBoard();

    // moveGenerator mg;

    // int numMoves = mg.generateMoves(b, moveList);

    // for (int i = 0; i < numMoves; i++)
    // {
    //     std::cout << decodeMove(moveList[i]) << "\n";
    //     b.makeMove(moveList[i]);
    //     verifyBoardState(b, myFile);
    //     b.undoMove();
    // }
    // std::ofstream knightLookup("util/pawnLookup.h");

    // knightLookup << "constexpr Bitboard pawnPushes[2][64] = {{";

    // for (int j = 0; j < 2; j++)
    // {
    //     for (int i = 0; i < 64; i++)
    //     {
    //         uint64_t squareMask = 1ULL << i;

    //         uint64_t wNo = (squareMask << 8);
    //         uint64_t wNoWe = (squareMask << 7) & 0x7F7F7F7F7F7F7F7F;
    //         uint64_t wNoEa = (squareMask << 9) & 0xFEFEFEFEFEFEFEFE;
    //         uint64_t wWe = (squareMask >> 1) & 0x7F7F7F7F7F7F7F7F;
    //         uint64_t wEa = (squareMask << 1) & 0xFEFEFEFEFEFEFEFE;
    //         uint64_t wSo = (squareMask >> 8);
    //         uint64_t wSoEa = (squareMask >> 7) & 0xFEFEFEFEFEFEFEFE;
    //         uint64_t wSoWe = (squareMask >> 9) & 0x7F7F7F7F7F7F7F7F;

    //         uint64_t kingAttacks = wNo| wNoWe | wNoEa | wWe | wEa | wSo | wSoEa | wSoWe;
            
    //         knightLookup << kingAttacks << "ULL, ";
    //     }
    // }
    // knightLookup << "};";

    // knightLookup.close();

    // std::ifstream testPositions("tests/moveTestPositions.csv");
    
    // std::ofstream myFile("debug.txt");

    // for (int i = 0; i < 64; i++)
    // {
    //     myFile << i << "\n\n";
    //     printBitboard(kingAttacks[i],myFile);
    //     myFile << "\n";
    // }

    // cBoard board;

    // std::string line;
    // while (std::getline(testPositions,line))
    // {
    //     std::stringstream ss(line);
    //     std::string arr[2];
    //     std::getline(ss,arr[0],',');
    //     std::getline(ss,arr[1]);

    //     int moveToMake = std::stoi(arr[1],nullptr,16);
    //     //std::cout << moveToMake << std::endl;

    //     myFile << "************* Test Position ****************\n";
    //     myFile << arr[0] << "\n\n";
    //     board.setFromFEN(arr[0]);
    //     myFile << "After setFromFEN() call\n\n";
    //     decodeHexMove(arr[1], myFile);
    //     verifyBoardState(board, myFile);
    //     board.makeMove(moveToMake);
    //     myFile << "\n\nAfter makeMove call\n\n";
    //     verifyBoardState(board, myFile);
    //     board.undoMove();
    //     myFile << "\n\nAfter undoMove call\n\n";
    //     verifyBoardState(board, myFile);
    // }

    // //board.resetBoard();
    // // board.setFromFEN("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");

    // // verifyBoardState(board, myFile);

    // // std::cout << board.getFEN();

    // myFile.close();
    // testPositions.close();

    return 0;
}
