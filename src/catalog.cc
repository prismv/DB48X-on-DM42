// ****************************************************************************
//  catalog.cc                                                    DB48X project
// ****************************************************************************
//
//   File Description:
//
//     Auto-completion for commands (Catalog)
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

#include "catalog.h"
#include "runtime.h"
#include "user_interface.h"

#include <string.h>
#include <stdlib.h>


MENU_BODY(Catalog)
// ----------------------------------------------------------------------------
//   Process the MENU command for Catalog
// ----------------------------------------------------------------------------
{
    uint  nitems = count_commands();
    items_init(mi, nitems, 1);
    ui.menu_auto_complete();
    list_commands(mi);
    if (mi.page >= mi.pages)
        mi.page = 0;
    return OK;
}


static uint16_t *sorted_ids = nullptr;


static int sort_ids(const void *left, const void *right)
// ----------------------------------------------------------------------------
//   Sort the IDs alphabetically based on their fancy name
// ----------------------------------------------------------------------------
{
    uint16_t l = *((uint16_t *) left);
    uint16_t r = *((uint16_t *) right);
    if (!object::spellings[l].name || !object::spellings[r].name)
        return !!object::spellings[l].name - !!object::spellings[r].name;
    return strcasecmp(object::spellings[l].name, object::spellings[r].name);
}


static void initialize_sorted_ids()
// ----------------------------------------------------------------------------
//   Sort IDs alphabetically
// ----------------------------------------------------------------------------
{
    size_t count = object::spelling_count;
    sorted_ids   = (uint16_t *) realloc(sorted_ids, count * sizeof(uint16_t));
    if (sorted_ids)
    {
        for (uint i = 0; i < count; i++)
            sorted_ids[i] = i;
        qsort(sorted_ids, count, sizeof(sorted_ids[0]), sort_ids);
    }
}


static bool matches(utf8 start, size_t size, utf8 name)
// ----------------------------------------------------------------------------
//   Check if what was typed matches the name
// ----------------------------------------------------------------------------
{
    size_t len   = strlen(cstring(name));
    bool   found = false;
    for (uint o = 0; !found && o + size <= len; o++)
    {
        found = true;
        for (uint i = 0; found && i < size; i++)
            found = tolower(start[i]) == tolower(name[i + o]);
    }
    return found;
}


uint Catalog::count_commands()
// ----------------------------------------------------------------------------
//    Count the commands to display in the catalog
// ----------------------------------------------------------------------------
{
    utf8   start  = 0;
    size_t size   = 0;
    bool   filter = ui.current_word(start, size);
    uint   count  = 0;

    for (size_t i = 0; i < spelling_count; i++)
    {
        object::id ty = object::spellings[i].type;
        if (!object::is_command(ty))
            continue;

        if (cstring name = spellings[i].name)
            if (!filter || matches(start, size, utf8(name)))
                count++;
    }

    return count;
}


void Catalog::list_commands(info &mi)
// ----------------------------------------------------------------------------
//   Fill the menu with all possible spellings of the command
// ----------------------------------------------------------------------------
{
    utf8   start  = nullptr;
    size_t size   = 0;
    bool   filter = ui.current_word(start, size);

    if (!sorted_ids)
        initialize_sorted_ids();

    for (size_t i = 0; i < spelling_count; i++)
    {
        uint16_t j = sorted_ids ? sorted_ids[i] : i;
        object::id ty = object::spellings[j].type;
        if (!object::is_command(ty))
            continue;

        if (cstring name = spellings[j].name)
            if (!filter || matches(start, size, utf8(name)))
                menu::items(mi, name, command::static_object(ty));
    }
}
