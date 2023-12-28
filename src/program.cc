// ****************************************************************************
//  program.cc                                                    DB48X project
// ****************************************************************************
//
//   File Description:
//
//     Implementation of RPL programs and blocks
//
//     Programs are lists with a special way to execute
//
//
//
//
//
// ****************************************************************************
//   (C) 2023 Christophe de Dinechin <christophe@dinechin.org>
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

#include "program.h"

#include "parser.h"
#include "settings.h"
#include "variables.h"

#ifdef SIMULATOR
#include "tests.h"
#endif // SIMULATOR

RECORDER(program, 16, "Program evaluation");



// ============================================================================
//
//    Program
//
// ============================================================================

EVAL_BODY(program)
// ----------------------------------------------------------------------------
//   Normal evaluation from a program places it on stack
// ----------------------------------------------------------------------------
{
    if (running)
        return rt.push(o) ? OK : ERROR;
    return o->run_program();
}


PARSE_BODY(program)
// ----------------------------------------------------------------------------
//    Try to parse this as a program
// ----------------------------------------------------------------------------
{
    return list_parse(ID_program, p, L'«', L'»');
}


RENDER_BODY(program)
// ----------------------------------------------------------------------------
//   Render the program into the given program buffer
// ----------------------------------------------------------------------------
{
    return o->list_render(r, L'«', L'»');
}


program_p program::parse(utf8 source, size_t size)
// ----------------------------------------------------------------------------
//   Parse a program without delimiters (e.g. command line)
// ----------------------------------------------------------------------------
{
    record(program, ">Parsing command line [%s]", source);
    parser p(source, size);
    result r = list_parse(ID_program, p, 0, 0);
    record(program, "<Command line [%s], end at %u, result %p",
           utf8(p.source), p.end, object_p(p.out));
    if (r != OK)
        return nullptr;
    object_p  obj  = p.out;
    if (!obj)
        return nullptr;
    program_p prog = obj->as<program>();
    return prog;
}


#ifdef DM42
#  pragma GCC push_options
#  pragma GCC optimize("-O3")
#endif // DM42

object::result program::run(bool synchronous) const
// ----------------------------------------------------------------------------
//   Execute a program
// ----------------------------------------------------------------------------
//   The 'save_last_args' indicates if we save `LastArgs` at this level
{
    size_t   depth     = rt.call_depth();
    bool     outer     = depth == 0 && !running;
    object_p first     = objects();
    object_p end       = skip();

    record(program, "Run %p (%p-%p) %+s",
           this, first, end, outer ? "outer" : "inner");

    if (!rt.run_push(first, end))
        return ERROR;
    if (outer || synchronous)
        return run_loop(depth);

    return OK;
}


object::result program::run(object_p obj, bool sync)
// ----------------------------------------------------------------------------
//    Run a program as top-level
// ----------------------------------------------------------------------------
{
    if (program_p prog = obj->as_program())
        return prog->run(sync);
    if (directory_p dir = obj->as<directory>())
        return dir->enter();
    return obj->evaluate();
}

#ifdef DM42
#  pragma GCC pop_options
#endif // DM42


object::result program::run_loop(size_t depth)
// ----------------------------------------------------------------------------
//   Continue executing a program
// ----------------------------------------------------------------------------
//   The 'save_last_args' indicates if we save `LastArgs` at this level
{
    result   result    = OK;
    bool     outer     = depth == 0 && !running;
    bool     last_args =
        outer ? Settings.SaveLastArguments() : Settings.ProgramLastArguments();

    save<bool> save_running(running, true);
    while (object_p obj = rt.run_next(depth))
    {
        if (interrupted())
        {
            obj->defer();
            if (!halted)
            {
                result = ERROR;
                rt.interrupted_error().command(obj);
            }
            break;
        }
        if (result == OK)
        {
            if (last_args)
                rt.need_save();
            result = obj->evaluate();
        }

        if (stepping)
        {
            ui.draw_busy(L'›');
            halted = --stepping == 0;
        }
    }

    return result;
}


bool program::interrupted()
// ----------------------------------------------------------------------------
//   Return true if the current program must be interrupted
// ----------------------------------------------------------------------------
{
    reset_auto_off();
    while (!key_empty())
    {
        int tail = key_tail();
        if (tail == KEY_EXIT)
        {
            halted = true;
            stepping = 0;
            return true;
        }
#if SIMULATOR
        int key = key_pop();
        extern int last_key;

        record(program, "Runner popped key %d, last=%d", key, last_key);
        if (key == tests::KEYSYNC)
            keysync_done = keysync_sent;
        else if (key > 0)
            last_key = key;
        else if (last_key > 0)
            last_key = -last_key;
#else
        key_pop();
#endif
    }
    return halted;
}




// ============================================================================
//
//    Block
//
// ============================================================================

PARSE_BODY(block)
// ----------------------------------------------------------------------------
//  Blocks are parsed in structures like loops, not directly
// ----------------------------------------------------------------------------
{
    return SKIP;
}


RENDER_BODY(block)
// ----------------------------------------------------------------------------
//   Render the program into the given program buffer
// ----------------------------------------------------------------------------
{
    return o->list_render(r, 0, 0);
}


EVAL_BODY(block)
// ----------------------------------------------------------------------------
//   Normal evaluation from a program places it on stack
// ----------------------------------------------------------------------------
{
    return o->run_program();
}



// ============================================================================
//
//   Debugging
//
// ============================================================================

bool program::running = false;
bool program::halted = false;
uint program::stepping = 0;


COMMAND_BODY(Halt)
// ----------------------------------------------------------------------------
//   Set the 'halted' flag to interrupt the program
// ----------------------------------------------------------------------------
{
    program::halted = true;
    return OK;
}


COMMAND_BODY(Debug)
// ----------------------------------------------------------------------------
//   Take a program and evaluate it in halted mode
// ----------------------------------------------------------------------------
{
    if (rt.args(1))
    {
        if (object_p obj = rt.top())
        {
            if (program_p prog = obj->as_program())
            {
                rt.pop();
                program::halted = true;
                prog->run_program();
                return OK;
            }
            else
            {
                rt.type_error();
            }
        }
    }
    return ERROR;
}


COMMAND_BODY(SingleStep)
// ----------------------------------------------------------------------------
//   Single step an instruction
// ----------------------------------------------------------------------------
{
    program::stepping = 1;
    program::halted = false;
    return program::run_loop(0);
}


COMMAND_BODY(StepOver)
// ----------------------------------------------------------------------------
//   Step over the next instruction
// ----------------------------------------------------------------------------
{
    if (object_p next = rt.run_next(0))
    {
        size_t depth = rt.call_depth();
        save<bool> no_halt(program::halted, false);
        if (!next->defer())
            return ERROR;
        return program::run_loop(depth);
    }
    return OK;
}


COMMAND_BODY(StepOut)
// ----------------------------------------------------------------------------
//   Step over the next instruction
// ----------------------------------------------------------------------------
{
    size_t depth = rt.call_depth();
    if (depth > 2)
    {
        save<bool> no_halt(program::halted, false);
        return program::run_loop(depth - 2);
    }
    return OK;
}


COMMAND_BODY(MultipleSteps)
// ----------------------------------------------------------------------------
//   Step multiple instructions
// ----------------------------------------------------------------------------
{
    if (rt.args(1))
    {
        if (object_p obj = rt.top())
        {
            if (uint steps = obj->as_uint32())
            {
                rt.pop();
                program::stepping = steps;
                program::halted = false;
                return program::run_loop(0);
            }
        }
    }
    return ERROR;
}


COMMAND_BODY(Continue)
// ----------------------------------------------------------------------------
//   Resume execution of the current program
// ----------------------------------------------------------------------------
{
    program::halted = false;
    return program::run_loop(0);
}


COMMAND_BODY(Kill)
// ----------------------------------------------------------------------------
//   Kill program execution
// ----------------------------------------------------------------------------
{
    // Flush the current program
    while (rt.run_next(0));
    program::halted = false;
    program::stepping = 0;
    return OK;
}
