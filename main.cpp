//#define ENABLE_TIMING


#include "core/types.h"
#include "core/board.h"
#include "util/debug.h"

int main()
{
    std::ifstream testPositions("tests/moveTestPositions.csv");
    
    std::ofstream myFile("debug.txt");

    cBoard board;

    std::string line;
    while (std::getline(testPositions,line))
    {
        std::stringstream ss(line);
        std::string arr[2];
        std::getline(ss,arr[0],',');
        std::getline(ss,arr[1]);

        int moveToMake = std::stoi(arr[1],nullptr,16);
        //std::cout << moveToMake << std::endl;

        myFile << "************* Test Position ****************\n";
        myFile << arr[0] << "\n\n";
        board.setFromFEN(arr[0]);
        myFile << "After setFromFEN() call\n\n";
        decodeHexMove(arr[1], myFile);
        verifyBoardState(board, myFile);
        board.makeMove(moveToMake);
        myFile << "\n\nAfter makeMove call\n\n";
        verifyBoardState(board, myFile);
        board.undoMove();
        myFile << "\n\nAfter undoMove call\n\n";
        verifyBoardState(board, myFile);
    }

    //board.resetBoard();
    // board.setFromFEN("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");

    // verifyBoardState(board, myFile);

    // std::cout << board.getFEN();

    myFile.close();
    testPositions.close();

    return 0;
}
