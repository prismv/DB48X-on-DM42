#ifndef LIST_H
#define LIST_H
// ****************************************************************************
//  list.h                                                        DB48X project
// ****************************************************************************
//
//   File Description:
//
//     RPL list objects
//
//     A list is a sequence of distinct objects
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
//
// Payload format:
//
//   A list is a sequence of bytes containing:
//   - The type ID
//   - The LEB128-encoded length of the payload
//   - Each object in the list in turn
//
//   To save space, there is no explicit marker for the end of list

#include "command.h"
#include "object.h"
#include "text.h"


GCP(list);


struct list : text
// ----------------------------------------------------------------------------
//   RPL list type
// ----------------------------------------------------------------------------
{
    list(id type, gcbytes bytes, size_t len): text(type, bytes, len)
    { }

    template <typename... Args>
    list(id type, const gcp<Args> & ...args): text(type, utf8(""), 0)
    {
        byte *p = (byte *) payload();
        size_t sz = required_args_memory(args...);
        p = leb128(p, sz);
        copy(p, args...);
    }

    static size_t required_memory(id i, gcbytes UNUSED bytes, size_t len)
    {
        return text::required_memory(i, bytes, len);
    }

    static list_p make(gcbytes bytes, size_t len)
    {
        return rt.make<list>(bytes, len);
    }

    static list_p make(id ty, gcbytes bytes, size_t len)
    {
        return rt.make<list>(ty, bytes, len);
    }

    template<typename ...Args>
    static list_p make(id ty, const gcp<Args> &...args)
    {
        return rt.make<list>(ty, args...);
    }

    template<typename ...Args>
    static list_p make(const gcp<Args> &...args)
    {
        return rt.make<list>(ID_list, args...);
    }

    template <typename Arg>
    static size_t required_args_memory(const gcp<Arg> &arg)
    {
        return arg->size();
    }

    template <typename Arg, typename ...Args>
    static size_t required_args_memory(const gcp<Arg> &arg,
                                       const gcp<Args> &...args)
    {
        return arg->size() + required_args_memory(args...);
    }

    template<typename ...Args>
    static size_t required_memory(id i, const gcp<Args> &...args)
    {
        size_t sz = required_args_memory(args...);
        return leb128size(i) + leb128size(sz) + sz;
    }

    template<typename Arg>
    static void copy(byte *p, const gcp<Arg> &arg)
    {
        size_t sz = arg->size();
        memmove(p, +arg, sz);
    }

    template<typename Arg, typename ...Args>
    static void copy(byte *p, const gcp<Arg> &arg, const gcp<Args> &...args)
    {
        size_t sz = arg->size();
        memmove(p, +arg, sz);
        p += sz;
        copy(p, args...);
    }

    object_p objects(size_t *size = nullptr) const
    {
        return object_p(value(size));
    }

    // Iterator, built in a way that is robust to garbage collection in loops
    struct iterator
    {

        typedef object_p value_type;
        typedef size_t difference_type;

        explicit iterator(list_p list, bool atend = false)
            : size(0),
              first(list->objects(&size)),
              index(atend ? size : 0) {}
        explicit iterator(list_p list, size_t skip)
            : size(0),
              first(list->objects(&size)),
              index(0)
        {
            while (skip && index < size)
            {
                operator++();
                skip--;
            }
        }
        iterator() : size(0), first(), index(0) {}

    public:
        iterator& operator++()
        {
            if (index < size)
            {
                object_p obj = +first + index;
                size_t objsize = obj->size();
                index += objsize;
            }

            return *this;
        }
        iterator operator++(int)
        {
            iterator prev = *this;
            ++(*this);
            return prev;
        }
        bool operator==(iterator other) const
        {
            return !first || !other.first ||
                   (index == other.index &&
                    +first == +other.first &&
                    size == other.size);
        }
        bool operator!=(iterator other) const
        {
            return !(*this == other);
        }
        value_type operator*() const
        {
            return index < size ? +first+ index : nullptr;
        }

    public:
        size_t   size;
        object_g first;
        size_t   index;
    };
    iterator begin() const      { return iterator(this); }
    iterator end() const        { return iterator(this, true); }


    size_t items() const
    // ------------------------------------------------------------------------
    //   Return number of items in the list
    // ------------------------------------------------------------------------
    {
        size_t result = 0;
        for (object_p obj UNUSED : *this)
            result++;
        return result;
    }


    bool expand_without_size() const;
    bool expand() const;
    // ------------------------------------------------------------------------
    //   Expand items to the stack, and return number of them
    // ------------------------------------------------------------------------


    object_p operator[](size_t index) const
    // ------------------------------------------------------------------------
    //   Return the n-th element in the list
    // ------------------------------------------------------------------------
    {
        return at(index);
    }


    object_p at(size_t index) const
    // ------------------------------------------------------------------------
    //   Return the n-th element in the list
    // ------------------------------------------------------------------------
    {
        return *iterator(this, index);
    }


    template<typename ...args>
    object_p at(size_t index, args... rest) const
    // ------------------------------------------------------------------------
    //   N-dimensional array access
    // ------------------------------------------------------------------------
    {
        object_p inner = at(index);
        if (!inner)
            return nullptr;
        id type = inner->type();
        if (type != ID_array && type != ID_list)
            return nullptr;
        list_p list = list_p(inner);
        return list->at(rest...);
    }

    object_p    head() const;
    list_p      tail() const;
    // ------------------------------------------------------------------------
    //   Return the head and tail of a list
    // ------------------------------------------------------------------------


    // Apply an algebraic function to all elements in list
    list_p map(object_p prg) const;
    list_p map(algebraic_fn fn) const;
    list_p map(arithmetic_fn fn, algebraic_r y) const;
    list_p map(algebraic_r x, arithmetic_fn fn) const;
    static list_p map(algebraic_fn fn, list_r x)
    {
        return x->map(fn);
    }
    static list_p map(arithmetic_fn fn, list_r x, algebraic_r y)
    {
        return x->map(fn, y);
    }
    static list_p map(arithmetic_fn fn, algebraic_r x, list_r y)
    {
        return y->map(x, fn);
    }

    // Append data to a list
    list_p append(list_p a) const;
    list_p append(object_p o) const;

    // Reduce and filter operations
    object_p reduce(object_p prg) const;
    list_p   filter(object_p prg) const;
    object_p map_as_object(object_p prg) const          { return map(prg);    }
    object_p filter_as_object(object_p prg) const       { return filter(prg); }

public:
    // Shared code for parsing and rendering, taking delimiters as input
    static result list_parse(id type, parser &p, unicode open, unicode close);
    intptr_t      list_render(renderer &r, unicode open, unicode close) const;

public:
    OBJECT_DECL(list);
    PARSE_DECL(list);
    RENDER_DECL(list);
    HELP_DECL(list);
};
typedef const list *list_p;

COMMAND_DECLARE(ToList);
COMMAND_DECLARE(FromList);
COMMAND_DECLARE(Size);
COMMAND_DECLARE(Get);
COMMAND_DECLARE(Put);
COMMAND_DECLARE(GetI);
COMMAND_DECLARE(PutI);
COMMAND_DECLARE(Sort);
COMMAND_DECLARE(QuickSort);
COMMAND_DECLARE(ReverseSort);
COMMAND_DECLARE(ReverseQuickSort);
COMMAND_DECLARE(ReverseList);
COMMAND_DECLARE(Head);
COMMAND_DECLARE(Tail);
COMMAND_DECLARE(Map);
COMMAND_DECLARE(Reduce);
COMMAND_DECLARE(Filter);
COMMAND_DECLARE(Sum);
COMMAND_DECLARE(Product);



inline list_g operator+(list_r x, list_r y)
// ----------------------------------------------------------------------------
//   Concatenate two lists, leveraging text concatenation
// ----------------------------------------------------------------------------
{
    text_r xt = (text_r) x;
    text_r yt = (text_r) y;
    return list_p(+(xt + yt));
}


inline list_g operator*(list_r x, uint y)
// ----------------------------------------------------------------------------
//    Repeat a list, leveraging text repetition
// ----------------------------------------------------------------------------
{
    text_r xt = (text_r) x;
    return list_p(+(xt * y));
}

#endif // LIST_H
