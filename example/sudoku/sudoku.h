
/******************************************************
*   Copyright (C)2019 All rights reserved.
*
*   Author        : owb
*   Email         : 2478644416@qq.com
*   File Name     : sudoku.h
*   Last Modified : 2019-11-21 20:07
*   Describe      :
*
*******************************************************/

#ifndef  _EXAMPLE_SUDOKU_SUDOKU_H
#define  _EXAMPLE_SUDOKU_SUDOKU_H

#include "ideal/base/StringPiece.h"
#include <string>

std::string solveSudoku(const ideal::StringPiece& puzzle);

const int kCells = 81;
extern const char kNoSolution[];

#endif // _EXAMPLE_SUDOKU_SUDOKU_H


