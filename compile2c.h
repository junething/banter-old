#ifndef COMPILE2C_H
#define COMPILE2C_H
#include "includes.h"
#include "list.h"
#include "dict.h"
#include "hashmap.h"
#include "irnodes.h"
int compile2c(IRNode *rootNode, CompileOptions *options);
int compile_program_to_c(BanterType** types, CompileOptions *options);
#endif
