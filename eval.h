#pragma once
#include "memory.h"

Cell* eval(Cell* expr, Cell* env);
Cell* apply(Cell* fn, Cell* args, Cell* env);
