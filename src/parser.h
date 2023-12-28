#ifndef PARSER_H
#define PARSER_H
// ****************************************************************************
//  parser.h                                                      DB48X project
// ****************************************************************************
//
//   File Description:
//
//     RPL parser information
//
//     The parser turns text into RPL objects.
//     It needs to operate on objects that can move due to garbage collection
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

#include "object.h"
#include "runtime.h"

struct parser
// ----------------------------------------------------------------------------
//  Arguments to the PARSE command
// ----------------------------------------------------------------------------
{
    typedef object::id id;

    parser(utf8 source, size_t length, int precedence = 0)
        : candidate(),
          source(source), length(length),
          end(), out(nullptr),
          precedence(precedence),
          child(false) {}

    parser(const parser &from, utf8 source, int precedence)
        : candidate(),
          source(source),
          length(from.length - (+source - +from.source)),
          end(), out(nullptr),
          precedence(precedence),
          child(true) {}

public:
    id          candidate;      // Candidate ID for lookup
    gcutf8      source;         // Text to parse
    size_t      length;         // Length to parse
    size_t      end;            // End position after parsing
    object_g    out;            // Output object if any
    int         precedence;     // Precedence level in equations
    bool        child;
};

#endif // PARSER_H
