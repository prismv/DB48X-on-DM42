#ifndef LOGICAL_H
#define LOGICAL_H
// ****************************************************************************
//  logical.h                                                     DB48X project
// ****************************************************************************
//
//   File Description:
//
//     Define logical operations
//
//     Logical operations can operate bitwise on based integers, or
//     as truth values on integers, real numbers and True/False
//
//
//
//
//
// ****************************************************************************
//   (C) 2022 Christophe de Dinechin <christophe@dinechin.org>
//   This software is licensed under the terms outlined in LICENSE.txt
// ****************************************************************************
//   This file is part of DB48X.
//
//   DB48X is free software: you can redistribute it and/or modify
//   it under the terms outlined in the LICENSE.txt file
//
//   DB48X is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// ****************************************************************************

#include "arithmetic.h"
#include "bignum.h"
#include "functions.h"


struct logical : arithmetic
// ----------------------------------------------------------------------------
//   Shared by all logical operations
// ----------------------------------------------------------------------------
{
    logical(id i): arithmetic(i) {}

    static int       as_truth(object_p obj);
    typedef ularge   (*binary_fn)(ularge x, ularge y);
    typedef bignum_p (*big_binary_fn)(bignum_r x, bignum_r y);
    static result    evaluate(binary_fn opn, big_binary_fn opb, bool num);
    typedef ularge   (*unary_fn)(ularge x);
    typedef bignum_p (*big_unary_fn)(bignum_r x);
    static result    evaluate(unary_fn opn, big_unary_fn opb, bool num);
    enum { numerical = false };

    template <typename Cmp> static result evaluate()
    // ------------------------------------------------------------------------
    //   The actual evaluation for all binary operators
    // ------------------------------------------------------------------------
    {
        return evaluate(Cmp::native, Cmp::bignum, Cmp::numerical);
    }

    static ularge       rol(ularge X, ularge Y = 1);
    static bignum_p     rol(bignum_r X, bignum_r Y);
    static bignum_p     rol(bignum_r X, uint Y = 1);
    static ularge       ror(ularge X, ularge Y = 1);
    static bignum_p     ror(bignum_r X, bignum_r Y);
    static bignum_p     ror(bignum_r X, uint Y = 1);
    static ularge       asr(ularge X, ularge Y = 1);
    static bignum_p     asr(bignum_r X, bignum_r Y);
    static bignum_p     asr(bignum_r X, uint Y = 1);
};


#define BINARY_LOGICAL(derived, prec, code)                             \
        BINARY_LOGICAL_EXT(derived, false, prec, code)
#define BINARY_LOGICAL_NUM(derived, prec, code)                         \
        BINARY_LOGICAL_EXT(derived, true, prec, code)
#define BINARY_LOGICAL_EXT(derived, num, prec, code)                    \
/* ----------------------------------------------------------------- */ \
/*  Macro to define an arithmetic command                            */ \
/* ----------------------------------------------------------------- */ \
struct derived : logical                                                \
{                                                                       \
    derived(id i = ID_##derived) : logical(i) {}                        \
                                                                        \
    OBJECT_DECL(derived);                                               \
    ARITY_DECL(2);                                                      \
    PREC_DECL(prec);                                                    \
    EVAL_DECL(derived)                                                  \
    {                                                                   \
        rt.command(o);                                                  \
        return evaluate<derived>();                                     \
    }                                                                   \
                                                                        \
    enum { numerical = num };                                           \
    static ularge    native(ularge Y, ularge X)        { return code; } \
    static bignum_p  bignum(bignum_r Y, bignum_r X)    { return code; } \
}


#define UNARY_LOGICAL(derived, code)                                    \
        UNARY_LOGICAL_EXT(derived, false, code)
#define UNARY_LOGICAL_NUM(derived, code)                                \
        UNARY_LOGICAL_EXT(derived, true, code)
#define UNARY_LOGICAL_EXT(derived, num, code)                           \
/* ----------------------------------------------------------------- */ \
/*  Macro to define an arithmetic command                            */ \
/* ----------------------------------------------------------------- */ \
struct derived : logical                                                \
{                                                                       \
    derived(id i = ID_##derived) : logical(i) {}                        \
                                                                        \
    OBJECT_DECL(derived);                                               \
    ARITY_DECL(1);                                                      \
    PREC_DECL(NONE);                                                    \
    EVAL_DECL(derived)                                                  \
    {                                                                   \
        rt.command(o);                                                  \
        return evaluate<derived>();                                     \
    }                                                                   \
    enum { numerical = num };                                           \
    static ularge    native(ularge X)           { return code; }        \
    static bignum_p  bignum(bignum_r X)         { return code; }        \
}


BINARY_LOGICAL(And,      LOGICAL,       Y &  X);
BINARY_LOGICAL(Or,       LOGICAL,       Y |  X);
BINARY_LOGICAL(Xor,      LOGICAL,       Y ^  X);
BINARY_LOGICAL(NAnd,     LOGICAL,     ~(Y &  X));
BINARY_LOGICAL(NOr,      LOGICAL,     ~(Y |  X));
BINARY_LOGICAL(Implies,  RELATIONAL,   ~Y |  X);
BINARY_LOGICAL(Equiv,    RELATIONAL,  ~(Y ^  X));
BINARY_LOGICAL(Excludes, RELATIONAL,    Y & ~X); // If Y then X=0
UNARY_LOGICAL(Not,                      ~X);

UNARY_LOGICAL_NUM(RL,                   rol(X));
UNARY_LOGICAL_NUM(RR,                   ror(X));
UNARY_LOGICAL_NUM(RLB,                  rol(X, 8));
UNARY_LOGICAL_NUM(RRB,                  ror(X, 8));
UNARY_LOGICAL_NUM(SL,                   X << 1U);
UNARY_LOGICAL_NUM(SR,                   X >> 1U);
UNARY_LOGICAL_NUM(ASR,                  asr(X));
UNARY_LOGICAL_NUM(SLB,                  X << 8U);
UNARY_LOGICAL_NUM(SRB,                  X >> 8U);
UNARY_LOGICAL_NUM(ASRB,                 asr(X, 8U));

BINARY_LOGICAL_NUM(SLC,     LOGICAL,    Y << X);
BINARY_LOGICAL_NUM(SRC,     LOGICAL,    Y >> X);
BINARY_LOGICAL_NUM(ASRC,    LOGICAL,    asr(Y, X));
BINARY_LOGICAL_NUM(RLC,     LOGICAL,    rol(Y, X));
BINARY_LOGICAL_NUM(RRC,     LOGICAL,    ror(Y, X));

#endif // LOGICAL_H
