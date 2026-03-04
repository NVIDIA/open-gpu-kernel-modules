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
* Module: dp_hashmap.cpp                                                    *
*    DP hashmap implementation                                              *
*                                                                           *
\***************************************************************************/

#include "dp_hashmap.h"

using namespace DisplayPort;

HashMap::~HashMap()
{
    for (unsigned int i = 0; i < DP_HASHMAP_CAPACITY; i++)
        m_hashMap[i].clear();
}

HashMapElement* HashMap::get(const HashMapElement *query)
{
    if (query == NULL)
        return NULL;

    unsigned int index = query->hash() % DP_HASHMAP_CAPACITY;
    if (m_hashMap[index].isEmpty())
        return NULL;

    // Traverse in reverse order to find the most recent cache hit
    for (ListElement *i = m_hashMap[index].last(); i != m_hashMap[index].end(); i = i->prev)
    {
        auto *pCachedElement = (HashMapElement *)i;
        if (query->isEqual(pCachedElement))
            return pCachedElement;
    }

    return NULL;
}

void HashMap::add(HashMapElement *element)
{
    if (element == NULL)
        return;

    unsigned int index = element->hash() % DP_HASHMAP_CAPACITY;

    element->m_age = m_currentAge++;
    m_hashMap[index].insertBack(element);
    m_added++;

    if (m_added >= DP_HASHMAP_CAPACITY)
        pruneCache();
}

void HashMap::pruneCache()
{
    // Prune the cache by removing the oldest cache elements until we reach the prune size
    for (unsigned int i = 0; i < DP_HASHMAP_CAPACITY; i++)
    {
        if (m_hashMap[i].isEmpty())
            continue;

        // From the front, remove the oldest cache elements until we reach the prune size
        auto *pCurr = (HashMapElement *)m_hashMap[i].front();
        while (pCurr != m_hashMap[i].end() && m_added > DP_HASHMAP_PRUNED_SIZE)
        {
            if ((m_currentAge - pCurr->m_age) > DP_HASHMAP_PRUNED_THRESHOLD)
            {
                auto *pNext = (HashMapElement *)pCurr->next;

                m_hashMap[i].remove(pCurr);
                delete pCurr;
                pCurr = pNext;
                m_added--;
            }
            else
            {
                // Current element is too "young" to be pruned, go to next list
                break;
            }
        }
    }
}
