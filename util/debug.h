#pragma once
#include "../core/board.h"
#include "../core/types.h"

#include <fstream>
#include <iostream>
#include <iomanip>

void printBitboard(const Bitboard& board, std::ostream& out = std::cout);

void verifyBoardState(const cBoard& cb, std::ostream& out = std::cout);

void decodeHexMove(std::string& hexMove, std::ostream& out = std::cout);

std::string moveToString(Move encodedMove);

std::string decodeMove(Move m);