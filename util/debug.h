#pragma once
#include "../core/board.h"
#include "../core/types.h"

#include <fstream>
#include <iostream>

void printBitboard(const Bitboard& board, std::ostream& out = std::cout);

void verifyBoardState(const cBoard& cb, std::ostream& out = std::cout);
