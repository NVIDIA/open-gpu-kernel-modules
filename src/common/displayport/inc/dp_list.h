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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_list.h                                                         *
*    Simple doubly linked list queue                                        *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_LIST_H
#define INCLUDED_DP_LIST_H

#include "dp_object.h"

namespace DisplayPort
{
    //
    //    List is an intrusive container, it may
    //    only contain elements that derive from ListElement
    //   
    //    NOTE!  Deleting an element automatically unlinks it 
    //         from the enclosing container.  
    //
    struct ListElement : virtual public Object
    {
        ListElement * next, * prev;

        ListElement();
        virtual ~ListElement();
    };


    class List : public ListElement
    {
    public:
        bool    isEmpty();
        void    insertFront(ListElement * item);
        void    insertBack(ListElement * item);
        void    insertBefore(ListElement * insertBeforeThis, ListElement * item);
        void    clear();
        ListElement* front();
        ListElement* last();

        ListElement* begin() { return this->next; }
        ListElement* end()   { return this; }

        static ListElement *   remove(ListElement * item);    // Removes but does not delete
        bool contains(ListElement * item);
        ListElement *   replace(ListElement * replacement, ListElement * replacee);
        List();
        ~List();

        unsigned size()
        {
            unsigned count = 0;
            for (ListElement * i = begin(); i!=end(); i = i->next)
                count++;
            return count;
        }
    };
}

#endif //INCLUDED_DP_LIST_H
