#ifndef RENDERER_H
#define RENDERER_H
// ****************************************************************************
//  renderer.h                                                    DB48X project
// ****************************************************************************
//
//   File Description:
//
//     Structure used to record information about rendering
//
//     This works in two modes:
//     - Write to a fixed-size buffer, e.g. while rendering stack
//     - Write to the scratchpad, e.g. to edit
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

#include "settings.h"
#include "types.h"
#include "utf8.h"


struct file;

struct renderer
// ----------------------------------------------------------------------------
//  Arguments to the RENDER command
// ----------------------------------------------------------------------------
{
    renderer(char *buf = nullptr, size_t len = ~0U,
             bool stk = false, bool ml = false)
        : target(buf), length(len), written(0), saving(), tabs(0),
          edit(!stk && buf == nullptr),
          eq(false), stk(stk), mlstk(ml),
          space(false), sign(false),
          cr(false), txt(false), nl(false) {}
    renderer(bool equation, bool edit = false, bool stk = false, bool ml = false)
        : target(nullptr), length(~0U), written(0), saving(), tabs(0),
          edit(edit),
          eq(equation), stk(stk), mlstk(ml), space(false), sign(false),
          cr(false), txt(false), nl(false) {}
    renderer(file *f)
        : target(), length(~0U), written(0), saving(f), tabs(0),
          edit(true),
          eq(false), stk(false), mlstk(false),
          space(false), sign(false),
          cr(false), txt(false), nl(false) {}
    ~renderer();

    bool   put(char c);
    bool   put(cstring s);
    bool   put(cstring s, size_t len);
    bool   put(unicode code);
    bool   put(utf8 s)                  { return put(cstring(s)); }
    bool   put(utf8 s, size_t len)      { return put(cstring(s), len); }
    bool   put(object::id fmt, utf8 s, size_t len = ~0UL);

    bool   editing() const              { return edit; }
    bool   equation() const             { return eq; }
    bool   stack() const                { return stk; }
    bool   multiline_stack() const      { return mlstk; }
    file * file_save() const            { return saving; }
    size_t size() const                 { return written + sign; }
    void   clear()                      { written = 0; }
    void   need_sign()                  { sign = true; }
    utf8   text() const;

    size_t printf(const char *format, ...);
    void   indent(int i)
    {
        tabs += i;
    }
    bool   indent()
    {
        indent(1);
        return put('\n');
    }
    bool   unindent()
    {
        indent(-1);
        return put('\n');
    }
    bool   hadCR()
    {
        return cr;
    }
    void   wantCR()
    {
        nl = true;
    }
    void   flush()
    {
        if (nl)
        {
            nl = false;
            put('\n');
        }
    }

protected:
    char        *target;        // Buffer where we render the object, or nullptr
    size_t      length;         // Available space
    size_t      written;        // Number of bytes written
    file *      saving;         // Save area for a program or object
    uint        tabs;           // Amount of indent
    bool        edit  : 1;      // For editor (e.g. render all digits)
    bool        eq    : 1;      // As equation
    bool        stk   : 1;      // Format for stack rendering
    bool        mlstk : 1;      // Format for multi-line stack rendering
    bool        space : 1;      // Had a space
    bool        sign  : 1;      // Need to insert '+' if next char is not '-'
    bool        cr    : 1;      // Just emitted a CR
    bool        txt   : 1;      // Inside text
    bool        nl    : 1;      // Pending CR
};

#endif // RENDERER_H
