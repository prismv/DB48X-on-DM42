// ****************************************************************************
//  finance.cc                                                    DB48X project
// ****************************************************************************
//
//   File Description:
//
//     Implementation of finance operations
//
//
//
//
//
//
//
//
// ****************************************************************************
//   (C) 2025 Christophe de Dinechin <christophe@dinechin.org>
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

#include "finance.h"

#include "equations.h"
#include "expression.h"
#include "solve.h"


COMMAND_BODY(TVMAmort)
// ----------------------------------------------------------------------------
//   Amortize payments
// ----------------------------------------------------------------------------
{
    return OK;
}


COMMAND_BODY(TVMRoot)
// ----------------------------------------------------------------------------
//    Solve for a given variable
// ----------------------------------------------------------------------------
{
    return OK;
}


COMMAND_BODY(TVMEquation)
// ----------------------------------------------------------------------------
//    Return the equation used for solving
// ----------------------------------------------------------------------------
{
    if (equation_p eq = equation::lookup("CompoundInterest"))
        if (rt.push(eq))
            return OK;
    return ERROR;
}


MENU_BODY(FinanceSolverMenu)
// ----------------------------------------------------------------------------
//   Process the MENU command for FinanceSolverMenu
// ----------------------------------------------------------------------------
{
    return true;
}
