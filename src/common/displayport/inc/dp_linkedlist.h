/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_linkedlist.h                                                   *
*    A linked list that uses DislayPort::List as a backend, but which       *
*    allocates the list backbone dynamically.                               *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_LINKEDLIST_H
#define INCLUDED_DP_LINKEDLIST_H

#include "dp_list.h"

namespace DisplayPort
{
    template<typename T>
    class LinkedList : public Object
    {
        // The Element class forms the list backbone and contains pointers to
        // each item in the list.
        class Element : public ListElement
        {
        public:
            Element(T *item) : item(item) { }
            T *item;
        };

        List list;

        // No public copy constructor.
        LinkedList(LinkedList &other) { }

        // Find the Element containing an item.
        Element *containing(T *item)
        {
            for (ListElement *le = list.begin(); le != list.end(); le = le->next)
            {
                Element *e = static_cast<Element *>(le);
                if (e->item == item)
                    return e;
            }
            return NULL;
        }

    public:
        // The list starts out empty.
        LinkedList() { }

        // Insert an item at the front of the list.
        void insertFront(T *item)
        {
            // Construct an element and add it to the list.
            Element *e = new Element(item);
            DP_ASSERT(e);
            if (e)
            {
                list.insertFront(e);
            }
        }

        // Remove an item from the list.
        // O(n) to find the item to remove.
        // It is an error to try to remove an item that is not in the list.
        void remove(T *item)
        {
            Element *e = containing(item);
            DP_ASSERT(e && "Item was not a member of the list");
            delete e;
        }

        // Find the next item in the list after the specified item.  If item is
        // NULL, this returns the first item.
        T *next(T *prev)
        {
            if (list.isEmpty())
                return NULL;

            // If prev is NULL or not in the list, return the first item.
            Element *e = containing(prev);
            if (!e)
            {
                e = static_cast<Element *>(list.begin());
                return e->item;
            }
            else if (e->next != list.end())
            {
                e = static_cast<Element *>(e->next);
                return e->item;
            }
            else
            {
                // prev was the last element in the list.
                return NULL;
            }
        }

        // Query whether an item is a member of the list.
        // O(n)
        bool contains(T *item)
        {
            Element *e = containing(item);
            return e != NULL;
        }

        bool isEmpty()
        {
            return list.isEmpty();
        }

        T *pop()
        {
            DP_ASSERT(!list.isEmpty());
            Element *e = static_cast<Element *>(list.last());
            T *item = e->item;
            delete e;
            return item;
        }
    };
}

#endif // INCLUDED_DP_LINKEDLIST_H
