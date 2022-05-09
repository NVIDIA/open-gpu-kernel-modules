/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/******************************* List **************************************\
*                                                                           *
* Module: dp_list.cpp                                                       *
*    Simple doubly linked list                                              *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_list.h"

using namespace DisplayPort;
ListElement::ListElement()
    : next(0), 
      prev(0)
{
}

ListElement::~ListElement()
{
    if (this->next)
    {
        this->prev->next = this->next;
        this->next->prev = this->prev;
        this->next = 0;
    }
}

List::List()
{
    this->next = this;
    this->prev = this;
}

void List::clear()
{
    while(!isEmpty())
        delete begin();
}

List::~List()
{
    clear();
    this->next = this;
    this->prev = this;
}

bool List::isEmpty() 
{
    return this->next == this;
}

void List::insertFront(ListElement * item)
{
    DP_ASSERT(item->next == 0 && "Attempt to insert when it's already in a list");
    item->prev = this;
    item->next = this->next;
    item->prev->next = item;
    item->next->prev = item;
}

void List::insertBack(ListElement * item)
{
    DP_ASSERT(item->next == 0 && "Attempt to insert when it's already in a list");
    item->prev = this->prev;
    item->next = this;
    item->prev->next = item;
    item->next->prev = item;
}

void List::insertBefore(ListElement * insertBeforeThis, ListElement * item)
{
    DP_ASSERT(item->next == 0 && "Attempt to insert when it's already in a list");
    item->next = insertBeforeThis;
    item->prev = insertBeforeThis->prev;
    insertBeforeThis->prev->next = item;
    insertBeforeThis->prev = item;
}

ListElement* List::front() 
{
    DP_ASSERT(!isEmpty());
    return this->next;
}

ListElement* List::last() 
{
    DP_ASSERT(!isEmpty());
    return this->prev;
}

ListElement * List::remove(ListElement * item)
{
    // Skip if its not already in a list
    if (!item->next)
        return item;

    item->prev->next = item->next;
    item->next->prev = item->prev;
    item->next = 0;
    item->prev = 0;

    return item;
}

bool List::contains(ListElement * item)
{
    for (ListElement * i = begin(); i!=end(); i = i->next)
    {
        if (i == item)
            return true;
    }
    return false;
}

ListElement * List::replace(ListElement * replacement, ListElement * replacee)
{
    if (!(replacement && replacee))
    {
        DP_ASSERT(0 && "replacement or replaces is NULL pointer");
        return 0;
    }

    DP_ASSERT(replacement->next && replacement->prev);

    // we are assuming replacee does exist in the list.
    replacement->next = replacee->next;
    replacement->prev = replacee->prev;

    if (replacement->next)
        replacement->next->prev = replacement;
    
    if (replacement->prev)
        replacement->prev->next = replacement;
    
    return replacee;
}
