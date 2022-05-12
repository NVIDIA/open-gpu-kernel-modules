.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media_ioc_enum_entities:

*****************************
ioctl MEDIA_IOC_ENUM_ENTITIES
*****************************

Name
====

MEDIA_IOC_ENUM_ENTITIES - Enumerate entities and their properties

Synopsis
========

.. c:macro:: MEDIA_IOC_ENUM_ENTITIES

``int ioctl(int fd, MEDIA_IOC_ENUM_ENTITIES, struct media_entity_desc *argp)``

Arguments
=========

``fd``
    File descriptor returned by :c:func:`open()`.

``argp``
    Pointer to struct :c:type:`media_entity_desc`.

Description
===========

To query the attributes of an entity, applications set the id field of a
struct :c:type:`media_entity_desc` structure and
call the MEDIA_IOC_ENUM_ENTITIES ioctl with a pointer to this
structure. The driver fills the rest of the structure or returns an
EINVAL error code when the id is invalid.

.. _media-ent-id-flag-next:

Entities can be enumerated by or'ing the id with the
``MEDIA_ENT_ID_FLAG_NEXT`` flag. The driver will return information
about the entity with the smallest id strictly larger than the requested
one ('next entity'), or the ``EINVAL`` error code if there is none.

Entity IDs can be non-contiguous. Applications must *not* try to
enumerate entities by calling MEDIA_IOC_ENUM_ENTITIES with increasing
id's until they get an error.

.. c:type:: media_entity_desc

.. tabularcolumns:: |p{1.5cm}|p{1.7cm}|p{1.6cm}|p{1.5cm}|p{10.6cm}|

.. flat-table:: struct media_entity_desc
    :header-rows:  0
    :stub-columns: 0
    :widths: 2 2 1 8

    *  -  __u32
       -  ``id``
       -
       -  Entity ID, set by the application. When the ID is or'ed with
	  ``MEDIA_ENT_ID_FLAG_NEXT``, the driver clears the flag and returns
	  the first entity with a larger ID. Do not expect that the ID will
	  always be the same for each instance of the device. In other words,
	  do not hardcode entity IDs in an application.

    *  -  char
       -  ``name``\ [32]
       -
       -  Entity name as an UTF-8 NULL-terminated string. This name must be unique
          within the media topology.

    *  -  __u32
       -  ``type``
       -
       -  Entity type, see :ref:`media-entity-functions` for details.

    *  -  __u32
       -  ``revision``
       -
       -  Entity revision. Always zero (obsolete)

    *  -  __u32
       -  ``flags``
       -
       -  Entity flags, see :ref:`media-entity-flag` for details.

    *  -  __u32
       -  ``group_id``
       -
       -  Entity group ID. Always zero (obsolete)

    *  -  __u16
       -  ``pads``
       -
       -  Number of pads

    *  -  __u16
       -  ``links``
       -
       -  Total number of outbound links. Inbound links are not counted in
	  this field.

    *  -  __u32
       -  ``reserved[4]``
       -
       -  Reserved for future extensions. Drivers and applications must set
          the array to zero.

    *  -  union {
       -  (anonymous)

    *  -  struct
       -  ``dev``
       -
       -  Valid for (sub-)devices that create a single device node.

    *  -
       -  __u32
       -  ``major``
       -  Device node major number.

    *  -
       -  __u32
       -  ``minor``
       -  Device node minor number.

    *  -  __u8
       -  ``raw``\ [184]
       -
       -
    *  - }
       -

Return Value
============

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.

EINVAL
    The struct :c:type:`media_entity_desc` ``id``
    references a non-existing entity.
