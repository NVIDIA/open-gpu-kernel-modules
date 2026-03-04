/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort********************************\
*                                                                           *
* Module: dp_hashmap.h                                                      *
*    DP hash map implementation                                             *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_HASHMAP_H
#define INCLUDED_DP_HASHMAP_H

#include "dp_list.h"
#include "dp_object.h"

#define DP_HASHMAP_CAPACITY          2000U  // Hashmap size and maximum size it can dynamically grow to
#define DP_HASHMAP_PRUNED_SIZE       500U   // Desired size of hashmap after pruning
#define DP_HASHMAP_PRUNED_THRESHOLD  500U   // Age difference from current age for pruning

namespace DisplayPort
{
    struct HashMapElement : virtual public Object, public ListElement
    {
        friend class HashMap;

        HashMapElement() = default;
        virtual ~HashMapElement() = default;

        /**
         * Calculate the hash value of the element.
         * @return The hash value of the element.
         */
        virtual unsigned int hash() const = 0;

        /**
         * Determine if two HashMapElements are equal. Used for collision resolution.
         * @param other The element to compare to.
         * @return True if the elements are equal, false otherwise.
         */
        virtual bool isEqual(const HashMapElement *other) const = 0;

    private:
        unsigned int m_age = 0;
    };

    //
    // Hash map implementation with LRU eviction policy, maintaining a cache capacity of
    // DP_HASHMAP_CAPACITY.
    //
    // The HashMap is implemented as an array of linked lists, where each linked list contains
    // elements with the same hash value.
    //
    // The HashMap is pruned when the number of elements in the HashMap exceeds DP_HASHMAP_CAPACITY
    // until the number of elements in the HashMap is DP_HASHMAP_PRUNED_SIZE. Any element whose age
    // is greater than current age minus DP_HASHMAP_PRUNED_THRESHOLD is pruned.
    //
    class HashMap : virtual public Object
    {
    public:
        HashMap() = default;
        virtual ~HashMap();

        /**
         * Get an element from the cache.
         * @param query The element to query.
         * @return The element from the cache, or NULL if the element is not found.
         */
        HashMapElement *get(const HashMapElement *query);

        /**
         * Add an element to the cache.
         * @param element The element to add to the cache. The HashMap will take ownership of the
         *                element and handle memory management.
         */
        void add(HashMapElement *element);

    private:
        void pruneCache();

        List m_hashMap[DP_HASHMAP_CAPACITY];
        unsigned int m_currentAge = 0;
        unsigned int m_added = 0;
    };
}

#endif // INCLUDED_DP_HASHMAP_H
