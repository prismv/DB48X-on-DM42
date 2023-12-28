// ****************************************************************************
//  functions.cc                                                  DB48X project
// ****************************************************************************
//
//   File Description:
//
//     Standard mathematical functions
//
//
//
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

#include "functions.h"

#include "arithmetic.h"
#include "array.h"
#include "bignum.h"
#include "decimal128.h"
#include "expression.h"
#include "fraction.h"
#include "integer.h"
#include "list.h"
#include "logical.h"
#include "tag.h"
#include "unit.h"


bool function::should_be_symbolic(id type)
// ----------------------------------------------------------------------------
//   Check if we should treat the type symbolically
// ----------------------------------------------------------------------------
{
    return is_symbolic(type);
}


algebraic_p function::symbolic(id op, algebraic_r x)
// ----------------------------------------------------------------------------
//    Check if we should process this function symbolically
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;
    return expression::make(op, x);
}


object::result function::evaluate(id op,
#ifndef CONFIG_NO_DECIMAL128
                                  bid128_fn op128,
#endif // CONFIG_NO_DECIMAL128
                                  complex_fn zop)
// ----------------------------------------------------------------------------
//   Shared code for evaluation of all common math functions
// ----------------------------------------------------------------------------
{
    algebraic_g x = algebraic_p(rt.top());
    if (!x)
        return ERROR;

#ifndef CONFIG_NO_DECIMAL128
    x = evaluate(x, op, op128, zop);
#else // CONFIG_NO_DECIMAL128
    x = evaluate(x, op, zop);
#endif // CONFIG_NO_DECIMAL128
    if (x && rt.top(x))
        return OK;
    return ERROR;
}


bool function::exact_trig(id op, algebraic_g &x)
// ----------------------------------------------------------------------------
//   Optimize cases where we can do exact trigonometry (avoid rounding)
// ----------------------------------------------------------------------------
//   This matters to get exact results for rectangular -> polar
{
    // When in radians mode, we cannot avoid rounding except for 0
    id amode = Settings.AngleMode();
    if (amode == ID_Rad && !x->is_zero(false))
        return false;

    algebraic_g degrees = x;
    switch(amode)
    {
    case object::ID_Grad:
        degrees = degrees * integer::make(90) / integer::make(100);
        break;
    case object::ID_PiRadians:
        degrees = degrees * integer::make(180);
        break;
    default:
        break;
    }

    ularge angle = 42;      // Not a special case...
    if (integer_p posint = degrees->as<integer>())
        angle = posint->value<ularge>();
    else if (const neg_integer *negint = degrees->as<neg_integer>())
        angle = 360 - negint->value<ularge>() % 360;
    else if (bignum_p posint = degrees->as<bignum>())
        angle = posint->value<ularge>();
    else if (const neg_bignum *negint = degrees->as<neg_bignum>())
        angle = 360 - negint->value<ularge>() % 360;
    angle %= 360;

    switch(op)
    {
    case ID_cos:
        angle = (angle + 90) % 360;
        // fallthrough
    case ID_sin:
        switch(angle)
        {
        case 0:
        case 180:       x = integer::make(0);  return true;
        case 270:       x = integer::make(-1); return true;
        case 90:        x = integer::make(1);  return true;
        case 30:
        case 150:       x = +fraction::make(integer::make(1),
                                            integer::make(2));
                        return true;
        case 210:
        case 330:       x = +fraction::make(integer::make(-1),
                                            integer::make(2));
                        return true;
        }
        return false;
    case ID_tan:
        switch(angle)
        {
        case 0:
        case 180:       x = integer::make(0);  return true;
        case 45:
        case 225:       x = integer::make(1);  return true;
        case 135:
        case 315:       x = integer::make(-1); return true;
        }
    default:
        break;
    }

    return false;
}


algebraic_p function::evaluate(algebraic_r xr,
                               id          op,
#ifndef CONFIG_NO_DECIMAL128
                               bid128_fn   op128,
#endif // CONFIG_NO_DECIMAL128
                               complex_fn  zop)
// ----------------------------------------------------------------------------
//   Shared code for evaluation of all common math functions
// ----------------------------------------------------------------------------
{
    if (!xr)
        return nullptr;

    algebraic_g x = xr;

    // Check if we are computing exact trigonometric values
    if (op >= ID_sin && op <= ID_tan)
        if (exact_trig(op, x))
            return x;

    // Convert arguments to numeric if necessary
    if (Settings.NumericalResults())
        (void) to_decimal(x, true);   // May fail silently, and that's OK

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(op, x);

    if (is_complex(xt))
        return algebraic_p(zop(complex_g(complex_p(+x))));

    // Check if need to promote integer values to decimal
    if (is_integer(xt))
    {
        // Do not accept sin(#123h)
        if (!is_real(xt))
        {
            rt.type_error();
            return nullptr;
        }
    }

    // Call the right function
    // We need to only call the bid128 functions here, because the 32 and 64
    // variants are not in the DM42's QSPI, and take too much space here
#ifndef CONFIG_NO_DECIMAL128
    if (real_promotion(x, ID_decimal128))
    {
        bid128 xv = decimal128_p(algebraic_p(x))->value();
        bid128 res;
        if (op == ID_sin || op == ID_cos || op == ID_tan)
            decimal128::adjust_from_angle(xv);
        op128(&res.value, &xv.value);
        int finite = false;
        bid128_isFinite(&finite, &res.value);
        if (!finite)
        {
            rt.domain_error();
            return nullptr;
        }
        if (op == ID_asin || op == ID_acos || op == ID_atan)
            decimal128::adjust_to_angle(res);
        x = rt.make<decimal128>(ID_decimal128, res);
        return x;
    }
#endif // CONFIG_NO_DECIMAL128

    // All other cases: report an error
    rt.type_error();
    return nullptr;
}


object::result function::evaluate(algebraic_fn op, bool mat)
// ----------------------------------------------------------------------------
//   Perform the operation from the stack, using a C++ operation
// ----------------------------------------------------------------------------
{
    if (!rt.args(1))
        return ERROR;
    if (object_p top = rt.top())
    {
        id topty = top->type();
        while(topty == ID_tag)
        {
            top = tag_p(top)->tagged_object();
            topty = top->type();
        }
        if (topty == ID_list || (topty == ID_array && !mat))
        {
            top = list_p(top)->map(op);
        }
        else
        {
            algebraic_g x = algebraic_p(top);
            x = op(x);
            top = +x;
        }
        if (top && rt.top(top))
            return OK;
    }
    return ERROR;
}



FUNCTION_BODY(abs)
// ----------------------------------------------------------------------------
//   Implementation of 'abs'
// ----------------------------------------------------------------------------
//   Special case where we don't need to promote argument to decimal128
{
    if (!x)
        return nullptr;

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(ID_abs, x);

    if (xt == ID_neg_integer  ||
        xt == ID_neg_bignum   ||
        xt == ID_neg_fraction ||
        xt == ID_neg_big_fraction)
    {
        // We can keep the object, just changing the type
        id absty = id(xt - 1);
        algebraic_p clone = algebraic_p(rt.clone(x));
        byte *tp = (byte *) clone;
        *tp = absty;
        return clone;
    }
    else if (is_integer(xt) || is_bignum(xt) || is_fraction(xt))
    {
        // No-op
        return x;
    }
    else if (is_complex(xt))
    {
        return complex_p(algebraic_p(x))->mod();
    }
    else if (xt == ID_array)
    {
        return array_p(algebraic_p(x))->norm();
    }

    // Fall-back to floating-point abs
#ifndef CONFIG_NO_DECIMAL128
    return function::evaluate(x, ID_abs, bid128_abs, nullptr);
#else
    return function::evaluate(x, ID_abs, nullptr);
#endif // CONFIG_NO_DECIMAL128
}


FUNCTION_BODY(arg)
// ----------------------------------------------------------------------------
//   Implementation of the complex argument (0 for non-complex values)
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(ID_arg, x);
    auto angle_mode = Settings.AngleMode();
    if (is_complex(xt))
        return complex_p(algebraic_p(x))->arg(angle_mode);
    algebraic_g zero = integer::make(0);
    bool negative = x->is_negative(false);
    return complex::convert_angle(zero, angle_mode, angle_mode, negative);
}


FUNCTION_BODY(re)
// ----------------------------------------------------------------------------
//   Extract the real part of a number
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(ID_re, x);
    if (is_complex(xt))
        return complex_p(algebraic_p(x))->re();
    if (!is_real(xt))
        rt.type_error();
    return x;
}


FUNCTION_BODY(im)
// ----------------------------------------------------------------------------
//   Extract the imaginary part of a number (0 for real values)
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(ID_im, x);
    if (is_complex(xt))
        return complex_p(algebraic_p(x))->im();
    if (!is_real(xt))
        rt.type_error();
    return integer::make(0);
}


FUNCTION_BODY(conj)
// ----------------------------------------------------------------------------
//   Compute the conjugate of input
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(ID_conj, x);
    if (is_complex(xt))
        return complex_p(algebraic_p(x))->conjugate();
    if (!is_real(xt))
        rt.type_error();
    return x;
}


FUNCTION_BODY(sign)
// ----------------------------------------------------------------------------
//   Implementation of 'sign'
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(ID_sign, x);

    if (x->is_negative(false))
    {
        return integer::make(-1);
    }
    else if (x->is_zero(false))
    {
        return integer::make(0);
    }
    else if (is_integer(xt) || is_bignum(xt) || is_fraction(xt) || is_real(xt))
    {
        return integer::make(1);
    }
    else if (is_complex(xt))
    {
        return polar::make(integer::make(1),
                           complex_p(algebraic_p(x))->pifrac(),
                           object::ID_PiRadians);
    }

    rt.type_error();
    return nullptr;
}


FUNCTION_BODY(IntPart)
// ----------------------------------------------------------------------------
//   Implementation of 'IP'
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(ID_IntPart, x);

    if (is_real(xt))
    {
        algebraic_g one = integer::make(1);
        algebraic_g r = rem::evaluate(x, one);
        return x - r;
    }
    rt.type_error();
    return nullptr;
}


FUNCTION_BODY(FracPart)
// ----------------------------------------------------------------------------
//   Implementation of 'FP'
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(ID_FracPart, x);

    if (is_real(xt))
    {
        algebraic_g one = integer::make(1);
        return rem::evaluate(x, one);
    }
    rt.type_error();
    return nullptr;
}


FUNCTION_BODY(ceil)
// ----------------------------------------------------------------------------
//   The `ceil` command returns the integer, or the integer immediately above
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(ID_ceil, x);

    if (is_real(xt))
    {
        algebraic_g one = integer::make(1);
        algebraic_g r = mod::evaluate(one - x, one);
        return x + r;
    }
    rt.type_error();
    return nullptr;
}


FUNCTION_BODY(floor)
// ----------------------------------------------------------------------------
//   The `floor` command returns the integer imediately below
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;

    id xt = x->type();
    if (should_be_symbolic(xt))
        return symbolic(ID_floor, x);

    if (is_real(xt))
    {
        algebraic_g one = integer::make(1);
        algebraic_g r = mod::evaluate(x, one);
        return x - r;
    }
    rt.type_error();
    return nullptr;
}


FUNCTION_BODY(inv)
// ----------------------------------------------------------------------------
//   Invert is implemented as 1/x
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;
    if (x->is_symbolic())
        return symbolic(ID_inv, x);
    else if (x->type() == ID_array)
        return array_p(+x)->invert();

    algebraic_g one = rt.make<integer>(ID_integer, 1);
    return one / x;
}


INSERT_BODY(inv)
// ----------------------------------------------------------------------------
//   x⁻¹ is a postfix
// ----------------------------------------------------------------------------
{
    return ui.edit(o->fancy(), ui.POSTFIX);

}


FUNCTION_BODY(neg)
// ----------------------------------------------------------------------------
//   Negate is implemented as 0-x
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;
    if (unit_p uobj = x->as<unit>())
    {
        algebraic_g v = uobj->value();
        algebraic_g u = uobj->uexpr();
        v = neg::run(v);
        return unit::simple(v, u);
    }
    if (x->is_symbolic())
        return symbolic(ID_neg, x);
    algebraic_g zero = rt.make<integer>(ID_integer, 0);
    return zero - x;
}


FUNCTION_BODY(sq)
// ----------------------------------------------------------------------------
//   Square is implemented using a multiplication
// ----------------------------------------------------------------------------
{
    if (!+x)
        return nullptr;
    if (x->is_symbolic())
        if (!Settings.AutoSimplify() || x->type() != ID_ImaginaryUnit)
            return expression::make(ID_sq, x);
    return x * x;
}


INSERT_BODY(sq)
// ----------------------------------------------------------------------------
//   x² is a postfix
// ----------------------------------------------------------------------------
{
    return ui.edit(o->fancy(), ui.POSTFIX);

}


FUNCTION_BODY(cubed)
// ----------------------------------------------------------------------------
//   Cubed is implemented as two multiplications
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;
    if (x->is_symbolic())
        if (!Settings.AutoSimplify() || x->type() != ID_ImaginaryUnit)
            return expression::make(ID_cubed, x);
    return x * x * x;
}


COMMAND_BODY(xroot)
// ----------------------------------------------------------------------------
//   Compute the x-th root
// ----------------------------------------------------------------------------
{
    if (rt.args(2))
    {
        if (object_p x = rt.pop())
        {
            if (object_p y = rt.top())
            {
                algebraic_g xa = x->as_algebraic();
                algebraic_g ya = y->as_algebraic();
                if (!xa.Safe() || !ya.Safe())
                {
                    rt.type_error();
                }
                else
                {
                    xa = pow(ya, integer::make(1) / xa);
                    if (+xa && rt.top(xa))
                        return OK;
                }
            }
        }
    }
    return ERROR;
}


INSERT_BODY(cubed)
// ----------------------------------------------------------------------------
//   x³ is a postfix
// ----------------------------------------------------------------------------
{
    return ui.edit(o->fancy(), ui.POSTFIX);

}


FUNCTION_BODY(fact)
// ----------------------------------------------------------------------------
//   Perform factorial for integer values, fallback to gamma otherwise
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;

    if (x->is_symbolic())
        return expression::make(ID_fact, x);

    if (integer_p ival = x->as<integer>())
    {
        ularge maxl = ival->value<ularge>();
        uint max = uint(maxl);
        if (max != maxl)
        {
            rt.domain_error();
            return nullptr;
        }
        algebraic_g result = integer::make(1);
        for (uint i = 2; i <= max; i++)
            result = result * integer::make(i);
        return result;
    }

    if (x->is_real() || x->is_complex())
        return tgamma::run(x + algebraic_g(integer::make(1)));

    rt.type_error();
    return nullptr;
}


INSERT_BODY(fact)
// ----------------------------------------------------------------------------
//   A factorial is inserted in postfix form in
// ----------------------------------------------------------------------------
{
    // We need to pass "x!' because ui.edit() strips the x
    return ui.edit(utf8("x!"), 2, ui.POSTFIX);
}


FUNCTION_BODY(Expand)
// ----------------------------------------------------------------------------
//   Expand equations
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;
    if (expression_p eq = x->as<expression>())
        return algebraic_p(eq->expand());
    if (x->is_algebraic())
        return x;
    rt.type_error();
    return nullptr;
}


FUNCTION_BODY(Collect)
// ----------------------------------------------------------------------------
//   Collect equations
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;
    if (expression_p eq = x->as<expression>())
        return algebraic_p(eq->collect());
    if (x->is_algebraic())
        return x;
    rt.type_error();
    return nullptr;
}


FUNCTION_BODY(Simplify)
// ----------------------------------------------------------------------------
//   Simplify equations
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;
    if (expression_p eq = x->as<expression>())
        return algebraic_p(eq->simplify());
    if (x->is_algebraic())
        return x;
    rt.type_error();
    return nullptr;
}


FUNCTION_BODY(ToDecimal)
// ----------------------------------------------------------------------------
//   Convert numbers to a decimal value
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;
    algebraic_g xg = x;
    if (algebraic::to_decimal(xg, false))
        return +xg;
    return nullptr;
}


FUNCTION_BODY(ToFraction)
// ----------------------------------------------------------------------------
//   Convert numbers to fractions
// ----------------------------------------------------------------------------
{
    if (!x)
        return nullptr;
    algebraic_g xg = x;
    if (arithmetic::decimal_to_fraction(xg))
        return xg;
    if (!rt.error())
        rt.type_error();
    return nullptr;
}
