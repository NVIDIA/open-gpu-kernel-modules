.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media_ioc_enum_links:

**************************
ioctl MEDIA_IOC_ENUM_LINKS
**************************

Name
====

MEDIA_IOC_ENUM_LINKS - Enumerate all pads and links for a given entity

Synopsis
========

.. c:macro:: MEDIA_IOC_ENUM_LINKS

``int ioctl(int fd, MEDIA_IOC_ENUM_LINKS, struct media_links_enum *argp)``

Arguments
=========

``fd``
    File descriptor returned by :c:func:`open()`.

``argp``
    Pointer to struct :c:type:`media_links_enum`.

Description
===========

To enumerate pads and/or links for a given entity, applications set the
entity field of a struct :c:type:`media_links_enum`
structure and initialize the struct
:c:type:`media_pad_desc` and struct
:c:type:`media_link_desc` structure arrays pointed by
the ``pads`` and ``links`` fields. They then call the
MEDIA_IOC_ENUM_LINKS ioctl with a pointer to this structure.

If the ``pads`` field is not NULL, the driver fills the ``pads`` array
with information about the entity's pads. The array must have enough
room to store all the entity's pads. The number of pads can be retrieved
with :ref:`MEDIA_IOC_ENUM_ENTITIES`.

If the ``links`` field is not NULL, the driver fills the ``links`` array
with information about the entity's outbound links. The array must have
enough room to store all the entity's outbound links. The number of
outbound links can be retrieved with :ref:`MEDIA_IOC_ENUM_ENTITIES`.

Only forward links that originate at one of the entity's source pads are
returned during the enumeration process.

.. c:type:: media_links_enum

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct media_links_enum
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    *  -  __u32
       -  ``entity``
       -  Entity id, set by the application.

    *  -  struct :c:type:`media_pad_desc`
       -  \*\ ``pads``
       -  Pointer to a pads array allocated by the application. Ignored if
	  NULL.

    *  -  struct :c:type:`media_link_desc`
       -  \*\ ``links``
       -  Pointer to a links array allocated by the application. Ignored if
	  NULL.

    *  -  __u32
       -  ``reserved[4]``
       -  Reserved for future extensions. Drivers and applications must set
          the array to zero.

.. c:type:: media_pad_desc

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct media_pad_desc
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    *  -  __u32
       -  ``entity``
       -  ID of the entity this pad belongs to.

    *  -  __u16
       -  ``index``
       -  Pad index, starts at 0.

    *  -  __u32
       -  ``flags``
       -  Pad flags, see :ref:`media-pad-flag` for more details.

    *  -  __u32
       -  ``reserved[2]``
       -  Reserved for future extensions. Drivers and applications must set
          the array to zero.


.. c:type:: media_link_desc

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct media_link_desc
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    *  -  struct :c:type:`media_pad_desc`
       -  ``source``
       -  Pad at the origin of this link.

    *  -  struct :c:type:`media_pad_desc`
       -  ``sink``
       -  Pad at the target of this link.

    *  -  __u32
       -  ``flags``
       -  Link flags, see :ref:`media-link-flag` for more details.

    *  -  __u32
       -  ``reserved[2]``
       -  Reserved for future extensions. Drivers and applications must set
          the array to zero.

Return Value
============

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.

EINVAL
    The struct :c:type:`media_links_enum` ``id``
    references a non-existing entity.
