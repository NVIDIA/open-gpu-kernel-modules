/*
 * Copyright © 2010 Intel Corporation
 * Copyright © 2010 Francisco Jerez <currojerez@riseup.net>
 * Copyright © 2012 NVIDIA Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

/*
 * This file was copied from the X.Org X server source at commit
 * 5884e7dedecdd82ddbb037360cf9c85143e094b5 and modified to match NVIDIA's X
 * driver code style.
 */

#ifndef _NV_LIST_H_
#define _NV_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "nvmisc.h"

    #define HAVE_TYPEOF 1

/**
 * @file Classic doubly-link circular list implementation.
 * For real usage examples of the linked list, see the file test/list.c
 *
 * Example:
 * We need to keep a list of struct foo in the parent struct bar, i.e. what
 * we want is something like this.
 *
 *     struct bar {
 *          ...
 *          struct foo *list_of_foos; -----> struct foo {}, struct foo {}, struct foo{}
 *          ...
 *     }
 *
 * We need one list head in bar and a list element in all list_of_foos (both are of
 * data type 'NVListRec').
 *
 *     struct bar {
 *          ...
 *          NVListRec list_of_foos;
 *          ...
 *     }
 *
 *     struct foo {
 *          ...
 *          NVListRec entry;
 *          ...
 *     }
 *
 * Now we initialize the list head:
 *
 *     struct bar bar;
 *     ...
 *     nvListInit(&bar.list_of_foos);
 *
 * Then we create the first element and add it to this list:
 *
 *     struct foo *foo = malloc(...);
 *     ....
 *     nvListAdd(&foo->entry, &bar.list_of_foos);
 *
 * Repeat the above for each element you want to add to the list. Deleting
 * works with the element itself.
 *      nvListDel(&foo->entry);
 *      free(foo);
 *
 * Note: calling nvListDel(&bar.list_of_foos) will set bar.list_of_foos to an empty
 * list again.
 *
 * Looping through the list requires a 'struct foo' as iterator and the
 * name of the field the subnodes use.
 *
 * struct foo *iterator;
 * nvListForEachEntry(iterator, &bar.list_of_foos, entry) {
 *      if (iterator->something == ...)
 *             ...
 * }
 *
 * Note: You must not call nvListDel() on the iterator if you continue the
 * loop. You need to run the safe for-each loop instead:
 *
 * struct foo *iterator, *next;
 * nvListForEachEntry_safe(iterator, next, &bar.list_of_foos, entry) {
 *      if (...)
 *              nvListDel(&iterator->entry);
 * }
 *
 */

/**
 * The linkage struct for list nodes. This struct must be part of your
 * to-be-linked struct. NVListRec is required for both the head of the
 * list and for each list node.
 *
 * Position and name of the NVListRec field is irrelevant.
 * There are no requirements that elements of a list are of the same type.
 * There are no requirements for a list head, any NVListRec can be a list
 * head.
 */
typedef struct NVList {
    struct NVList *next, *prev;
} NVListRec, *NVListPtr;

/**
 * Initialize the list as an empty list.
 *
 * Example:
 * nvListInit(&bar->list_of_foos);
 *
 * @param The list to initialized.
 */
static NV_INLINE void
nvListInit(NVListPtr list)
{
    list->next = list->prev = list;
}

/**
 * Initialize the list as an empty list.
 *
 * This is functionally the same as nvListInit, but can be used for
 * initialization of global variables.
 *
 * Example:
 * static NVListRec list_of_foos = NV_LIST_INIT(&list_of_foos);
 *
 * @param The list to initialized.
 */
#define NV_LIST_INIT(head) { .prev = (head), .next = (head) }

static NV_INLINE void
__nvListAdd(NVListPtr entry, NVListPtr prev, NVListPtr next)
{
    next->prev = entry;
    entry->next = next;
    entry->prev = prev;
    prev->next = entry;
}

/**
 * Insert a new element after the given list head. The new element does not
 * need to be initialised as empty list.
 * The list changes from:
 *      head -> some element -> ...
 * to
 *      head -> new element -> older element -> ...
 *
 * Example:
 * struct foo *newfoo = malloc(...);
 * nvListAdd(&newfoo->entry, &bar->list_of_foos);
 *
 * @param entry The new element to prepend to the list.
 * @param head The existing list.
 */
static NV_INLINE void
nvListAdd(NVListPtr entry, NVListPtr head)
{
    __nvListAdd(entry, head, head->next);
}

/**
 * Append a new element to the end of the list given with this list head.
 *
 * The list changes from:
 *      head -> some element -> ... -> lastelement
 * to
 *      head -> some element -> ... -> lastelement -> new element
 *
 * Example:
 * struct foo *newfoo = malloc(...);
 * nvListAppend(&newfoo->entry, &bar->list_of_foos);
 *
 * @param entry The new element to prepend to the list.
 * @param head The existing list.
 */
static NV_INLINE void
nvListAppend(NVListPtr entry, NVListPtr head)
{
    __nvListAdd(entry, head->prev, head);
}

static NV_INLINE void
__nvListDel(NVListPtr prev, NVListPtr next)
{
    next->prev = prev;
    prev->next = next;
}

/**
 * Remove the element from the list it is in. Using this function will reset
 * the pointers to/from this element so it is removed from the list. It does
 * NOT free the element itself or manipulate it otherwise.
 *
 * Using nvListDel on a pure list head (like in the example at the top of
 * this file) will NOT remove the first element from
 * the list but rather reset the list as empty list.
 *
 * Example:
 * nvListDel(&foo->entry);
 *
 * @param entry The element to remove.
 */
static NV_INLINE void
nvListDel(NVListPtr entry)
{
    __nvListDel(entry->prev, entry->next);
    nvListInit(entry);
}

/**
 * Check if the list is empty.
 *
 * Example:
 * nvListIsEmpty(&bar->list_of_foos);
 *
 * @return True if the list contains one or more elements or False otherwise.
 */
static NV_INLINE NvBool
nvListIsEmpty(const NVListRec *head)
{
    return head->next == head;
}

static NV_INLINE int
nvListCount(const NVListRec *head)
{
    NVListPtr next;
    int count = 0;

    for (next = head->next; next != head; next = next->next) {
        count++;
    }

    return count;
}

/**
 * Check if entry is present in the list.
 *
 * Example:
 * nvListPresent(&foo->entry, &bar->list_of_foos);
 *
 * @return 1 if the list contains the specified entry; otherwise, return 0.
 */
static NV_INLINE NvBool
nvListPresent(const NVListRec *entry, const NVListRec *head)
{
    const NVListRec *next;

    for (next = head->next; next != head; next = next->next) {
        if (next == entry) {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/**
 * Returns a pointer to the container of this list element.
 *
 * Example:
 * struct foo* f;
 * f = nv_container_of(&foo->entry, struct foo, entry);
 * assert(f == foo);
 *
 * @param ptr Pointer to the NVListRec.
 * @param type Data type of the list element.
 * @param member Member name of the NVListRec field in the list element.
 * @return A pointer to the data struct containing the list head.
 */
#ifndef nv_container_of
#define nv_container_of(ptr, type, member) \
    (type *)((char *)(ptr) - NV_OFFSETOF(type, member))
#endif

/**
 * Alias of nv_container_of
 */
#define nvListEntry(ptr, type, member) \
    nv_container_of(ptr, type, member)

/**
 * Retrieve the first list entry for the given list pointer.
 *
 * Example:
 * struct foo *first;
 * first = nvListFirstEntry(&bar->list_of_foos, struct foo, list_of_foos);
 *
 * @param ptr The list head
 * @param type Data type of the list element to retrieve
 * @param member Member name of the NVListRec field in the list element.
 * @return A pointer to the first list element.
 */
#define nvListFirstEntry(ptr, type, member) \
    nvListEntry((ptr)->next, type, member)

/**
 * Retrieve the last list entry for the given listpointer.
 *
 * Example:
 * struct foo *first;
 * first = nvListLastEntry(&bar->list_of_foos, struct foo, list_of_foos);
 *
 * @param ptr The list head
 * @param type Data type of the list element to retrieve
 * @param member Member name of the NVListRec field in the list element.
 * @return A pointer to the last list element.
 */
#define nvListLastEntry(ptr, type, member) \
    nvListEntry((ptr)->prev, type, member)

#ifdef HAVE_TYPEOF
#define __nv_container_of(ptr, sample, member)  \
    nv_container_of(ptr, __typeof__(*sample), member)
#else
/* This implementation of __nv_container_of has undefined behavior according
 * to the C standard, but it works in many cases.  If your compiler doesn't
 * support __typeof__() and fails with this implementation, please try a newer
 * compiler.
 */
#define __nv_container_of(ptr, sample, member)                         \
    (void *)((char *)(ptr)                                             \
            - ((char *)&(sample)->member - (char *)(sample)))
#endif

/**
 * Loop through the list given by head and set pos to struct in the list.
 *
 * Example:
 * struct foo *iterator;
 * nvListForEachEntry(iterator, &bar->list_of_foos, entry) {
 *      [modify iterator]
 * }
 *
 * This macro is not safe for node deletion. Use nvListForEachEntry_safe
 * instead.
 *
 * @param pos Iterator variable of the type of the list elements.
 * @param head List head
 * @param member Member name of the NVListRec in the list elements.
 *
 */
#ifdef HAVE_TYPEOF
#define __NV_LIST_SET(x, y) x = y
#else
static NV_INLINE void __nvListSet(void **x, void *y)
{
    *x = y;
}

#define __NV_LIST_SET(x, y) __nvListSet((void **) &x, (void *) (y))
#endif

#define nvListForEachEntry(pos, head, member)                           \
    for (__NV_LIST_SET(pos, __nv_container_of((head)->next, pos, member));            \
         &pos->member != (head);                                        \
         __NV_LIST_SET(pos, __nv_container_of(pos->member.next, pos, member)))

/**
 * Loop through the list, keeping a backup pointer to the element. This
 * macro allows for the deletion of a list element while looping through the
 * list.
 *
 * See nvListForEachEntry for more details.
 */
#define nvListForEachEntry_safe(pos, tmp, head, member)                        \
    for (__NV_LIST_SET(pos, __nv_container_of((head)->next, pos, member)),       \
         __NV_LIST_SET(tmp, __nv_container_of(pos->member.next, pos, member));   \
         &pos->member != (head);                                               \
         __NV_LIST_SET(pos, tmp),                                                \
         __NV_LIST_SET(tmp, __nv_container_of(pos->member.next, tmp, member)))

/* NULL-Terminated List Interface
 *
 * The interface below does _not_ use the NVListRec as described above.
 * It is mainly for legacy structures that cannot easily be switched to
 * NVListRec.
 *
 * This interface is for structs like
 *      struct foo {
 *          [...]
 *          struct foo *next;
 *           [...]
 *      };
 *
 * The position and field name of "next" are arbitrary.
 */

/**
 * Init the element as null-terminated list.
 *
 * Example:
 * struct foo *list = malloc();
 * nvNTListInit(list, next);
 *
 * @param list The list element that will be the start of the list
 * @param member Member name of the field pointing to next struct
 */
#define nvNTListInit(_list, _member) \
        (_list)->_member = NULL

/**
 * Returns the next element in the list or NULL on termination.
 *
 * Example:
 * struct foo *element = list;
 * while ((element = nvNTListNext(element, next)) { }
 *
 * This macro is not safe for node deletion. Use nvListForEachEntry_safe
 * instead.
 *
 * @param list The list or current element.
 * @param member Member name of the field pointing to next struct.
 */
#define nvNTListNext(_list, _member) \
        (_list)->_member

/**
 * Iterate through each element in the list.
 *
 * Example:
 * struct foo *iterator;
 * nvNTListForEachEntry(iterator, list, next) {
 *      [modify iterator]
 * }
 *
 * @param entry Assigned to the current list element
 * @param list The list to iterate through.
 * @param member Member name of the field pointing to next struct.
 */
#define nvNTListForEachEntry(_entry, _list, _member)            \
        for (_entry = _list; _entry; _entry = (_entry)->_member)

/**
 * Iterate through each element in the list, keeping a backup pointer to the
 * element. This macro allows for the deletion of a list element while
 * looping through the list.
 *
 * See nvNTListForEachEntry for more details.
 *
 * @param entry Assigned to the current list element
 * @param tmp The pointer to the next element
 * @param list The list to iterate through.
 * @param member Member name of the field pointing to next struct.
 */
#define nvNTListForEachEntrySafe(_entry, _tmp, _list, _member)          \
        for (_entry = _list, _tmp = (_entry) ? (_entry)->_member : NULL;\
                _entry;                                                 \
                _entry = _tmp, _tmp = (_tmp) ? (_tmp)->_member: NULL)

/**
 * Append the element to the end of the list. This macro may be used to
 * merge two lists.
 *
 * Example:
 * struct foo *elem = malloc(...);
 * nvNTListInit(elem, next)
 * nvNTListAppend(elem, list, struct foo, next);
 *
 * Resulting list order:
 * list_item_0 -> list_item_1 -> ... -> elem_item_0 -> elem_item_1 ...
 *
 * @param entry An entry (or list) to append to the list
 * @param list The list to append to. This list must be a valid list, not
 * NULL.
 * @param type The list type
 * @param member Member name of the field pointing to next struct
 */
#define nvNTListAppend(_entry, _list, _type, _member)                   \
    do {                                                                \
        _type *__iterator = _list;                                      \
        while (__iterator->_member) { __iterator = __iterator->_member;}\
        __iterator->_member = _entry;                                   \
    } while (0)

/**
 * Insert the element at the next position in the list. This macro may be
 * used to insert a list into a list.
 *
 * struct foo *elem = malloc(...);
 * nvNTListInit(elem, next)
 * nvNTListInsert(elem, list, struct foo, next);
 *
 * Resulting list order:
 * list_item_0 -> elem_item_0 -> elem_item_1 ... -> list_item_1 -> ...
 *
 * @param entry An entry (or list) to append to the list
 * @param list The list to insert to. This list must be a valid list, not
 * NULL.
 * @param type The list type
 * @param member Member name of the field pointing to next struct
 */
#define nvNTListInsert(_entry, _list, _type, _member)                   \
    do {                                                                \
        nvNTListAppend((_list)->_member, _entry, _type, _member);       \
        (_list)->_member = _entry;                                      \
    } while (0)

/**
 * Delete the entry from the list by iterating through the list and
 * removing any reference from the list to the entry.
 *
 * Example:
 * struct foo *elem = <assign to right element>
 * nvNTListDel(elem, list, struct foo, next);
 *
 * @param entry The entry to delete from the list. entry is always
 * re-initialized as a null-terminated list.
 * @param list The list containing the entry, set to the new list without
 * the removed entry.
 * @param type The list type
 * @param member Member name of the field pointing to the next entry
 */
#define nvNTListDel(_entry, _list, _type, _member)              \
        do {                                                    \
                _type *__e = _entry;                            \
                if (__e == NULL || _list == NULL) break;        \
                if ((_list) == __e) {                           \
                    _list = __e->_member;                       \
                } else {                                        \
                    _type *__prev = _list;                      \
                    while (__prev->_member && __prev->_member != __e)   \
                        __prev = nvNTListNext(__prev, _member); \
                    if (__prev->_member)                        \
                        __prev->_member = __e->_member;         \
                }                                               \
                nvNTListInit(__e, _member);                     \
        } while(0)

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* _NV_LIST_H_ */
