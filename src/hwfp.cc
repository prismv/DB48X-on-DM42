// ****************************************************************************
//  hwfp.cc                                                       DB48X project
// ****************************************************************************
//
//   File Description:
//
//     Support code for hardware floating-point support
//
//
//
//
//
//
//
//
// ****************************************************************************
//   (C) 2024 Christophe de Dinechin <christophe@dinechin.org>
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

#include "hwfp.h"

#include "arithmetic.h"
#include "parser.h"
#include "settings.h"

#include <cmath>

size_t hwfp_base::render(renderer &r, double x, char suffix)
// ----------------------------------------------------------------------------
//   Render the value, ignoring formatting for now
// ----------------------------------------------------------------------------
{
    if (std::isfinite(x))
    {
        decimal_g dec = decimal::from(x);
        if (dec && dec->render(r))
        {
            r.put(suffix);
            return r.size();
        }
    }
    else if (std::isinf(x))
    {
        r.put(x < 0 ? "-∞" : "∞");
    }
    else
    {
        r.put("NaN");
    }
    return r.size();
}


template <typename hw>
algebraic_p hwfp<hw>::to_fraction(uint count, uint prec) const
// ----------------------------------------------------------------------------
//   Convert hwfp number to fraction
// ----------------------------------------------------------------------------
{
    hw   num = value();
    bool neg = num < 0;
    if (neg)
        num = -num;

    hw whole_part   = std::floor(num);
    hw decimal_part = num - whole_part;
    if (decimal_part == 0.0)
        return to_integer();

    hw   v1num  = whole_part;
    hw   v1den  = 1.0;
    hw   v2num  = 1.0;
    hw   v2den  = 0.0;

    uint maxdec = Settings.Precision() - 3;
    if (prec > maxdec)
        prec = maxdec;
    hw eps = std::exp(-prec * M_LN10);

    while (count--)
    {
        // Check if the decimal part is small enough
        if (decimal_part == 0.0)
            break;

        if (decimal_part < eps)
            break;

        hw next = 1.0 / decimal_part;
        whole_part = std::floor(next);

        hw s = v1num;
        v1num = whole_part * v1num + v2num;
        v2num = s;

        s = v1den;
        v1den = whole_part * v1den + v2den;
        v2den = s;

        decimal_part = next - whole_part;
    }

    ularge      numerator   = ularge(v1num);
    ularge      denominator = ularge(v1den);
    algebraic_g result;
    if (denominator == 1)
        result = +integer::make(numerator);
    else
        result = +fraction::make(integer::make(numerator),
                                 integer::make(denominator));
    if (neg)
        result = -result;
    return +result;

}


template algebraic_p hwfp<float>::to_fraction(uint count, uint prec) const;
template algebraic_p hwfp<double>::to_fraction(uint count, uint prec) const;
