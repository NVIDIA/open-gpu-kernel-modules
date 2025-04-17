/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
 * This file is part of the NVOC runtime.
 */

#pragma once

#include "nvoc/prelude.h"

#include "g_object_nvoc.h"

#ifndef _NVOC_OBJECT_H_
#define _NVOC_OBJECT_H_

#include "nvtypes.h"
#include "nvstatus.h"



class Object;
struct NVOC_CLASS_INFO;

/*!
 * Tracks the head of an object's child list, and the next object in its
 * parent's child list.
 */
struct NVOC_CHILD_TREE
{
    Object *pChild;
    Object *pSibling;
};

//! The base class of all instantiable NVOC objects.
NVOC_PREFIX(obj) class Object
{
public:

    //! Runtime ownership tree parent node.
    Object *pParent;

    //! Runtime ownership tree child and sibling links.
    struct NVOC_CHILD_TREE childTree;

    //! IP Version value.  Temporary until NVOC-style HALs are rolled out.
    NvU32 ipVersion;

    //! flags used to create the object.
    NvU32 createFlags;

    /*!
     * @brief Add pChild as a child of this object.
     *
     * This method is wrapped by objCreate and typically doesn't need to be
     * called directly.
     *
     * Asserts if pChild is already a child of any object.
     */
    void objAddChild(Object *pObj, Object *pChild);

    /*!
     * @brief Remove pChild as a child of this object.
     *
     * This method is wrapped by objDelete and typically doesn't need to be
     * called directly.
     *
     * Asserts if pChild is not a child of this object.
     */
    void objRemoveChild(Object *pObj, Object *pChild);

    /*!
     * @brief Gets the head of this object's child list from the child tree.
     *
     * This is a constant-time operation.
     */
    Object *objGetChild(Object *pObj);

    /*!
     * @brief Gets the next child of this object's parent from the child tree.
     *
     * This is a constant-time operation.
     */
    Object *objGetSibling(Object *pObj);

    /*!
     * @brief Gets the direct parent of this object.
     *
     * This is a constant-time operation.
     */
    Object *objGetDirectParent(Object *pObj);
};

//
// IP versioning definitions are temporary until NVOC halspec support is
// finished.
//
// IP_VERSION format as defined by the hardware engines.
// A _MAJOR value of 0 means the object has no version number.
//

#define NV_ODB_IP_VER_DEV                                       7:0 /* R-IVF */
#define NV_ODB_IP_VER_ECO                                      15:8 /* R-IVF */
#define NV_ODB_IP_VER_MINOR                                   23:16 /* R-IVF */
#define NV_ODB_IP_VER_MAJOR                                   31:24 /* R-IVF */

#define IPVersion(pObj)                            staticCast((pObj), Object)->ipVersion
//  v0 .. v1  inclusive
#define IsIPVersionInRange(pObj, v0, v1)           ((IPVersion(pObj) >= (v0)) && (IPVersion(pObj) <= (v1)))

#endif
