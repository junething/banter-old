#ifndef BUILTINS_H 
#define BUILTINS_H
#include "includes.h"
#include "astnodes.h"
#include "banter_type.h"
BanterType *intType;
BanterType *boolType;
BanterType *voidType;
BanterType *methodType;
BanterType *letType;
BanterType *ProgramType;
BanterType *stringType;
BanterType *typeType;
BanterType *cType;
BanterType *typeExtendType;
void initBuiltins(void);
#endif
