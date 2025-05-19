
#include "core/types.h"
#include "core/board.h"
#include "util/debug.h"

int main()
{
    std::ofstream myFile("debug.txt");

    cBoard board;

    //board.resetBoard();
    board.setFromFEN("r1bqk2r/ppp2p2/2n1p2p/8/QbBPn3/2N2pB1/PP3PPP/2R2RK1 b kq - 1 12");

    verifyBoardState(board, myFile);

    std::cout << board.getFEN();

    myFile.close();

    return 0;
}
