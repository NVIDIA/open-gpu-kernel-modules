============================
struct request documentation
============================

Jens Axboe <jens.axboe@oracle.com> 27/05/02


.. FIXME:
   No idea about what does mean - seems just some noise, so comment it

   1.0
   Index

   2.0 Struct request members classification

       2.1 struct request members explanation

   3.0


   2.0



Short explanation of request members
====================================

Classification flags:

	=	====================
	D	driver member
	B	block layer member
	I	I/O scheduler member
	=	====================

Unless an entry contains a D classification, a device driver must not access
this member. Some members may contain D classifications, but should only be
access through certain macros or functions (eg ->flags).

<linux/blkdev.h>

=============================== ======= =======================================
Member				Flag	Comment
=============================== ======= =======================================
struct list_head queuelist	BI	Organization on various internal
					queues

``void *elevator_private``	I	I/O scheduler private data

unsigned char cmd[16]		D	Driver can use this for setting up
					a cdb before execution, see
					blk_queue_prep_rq

unsigned long flags		DBI	Contains info about data direction,
					request type, etc.

int rq_status			D	Request status bits

kdev_t rq_dev			DBI	Target device

int errors			DB	Error counts

sector_t sector			DBI	Target location

unsigned long hard_nr_sectors	B	Used to keep sector sane

unsigned long nr_sectors	DBI	Total number of sectors in request

unsigned long hard_nr_sectors	B	Used to keep nr_sectors sane

unsigned short nr_phys_segments	DB	Number of physical scatter gather
					segments in a request

unsigned short nr_hw_segments	DB	Number of hardware scatter gather
					segments in a request

unsigned int current_nr_sectors	DB	Number of sectors in first segment
					of request

unsigned int hard_cur_sectors	B	Used to keep current_nr_sectors sane

int tag				DB	TCQ tag, if assigned

``void *special``		D	Free to be used by driver

``char *buffer``		D	Map of first segment, also see
					section on bouncing SECTION

``struct completion *waiting``	D	Can be used by driver to get signalled
					on request completion

``struct bio *bio``		DBI	First bio in request

``struct bio *biotail``		DBI	Last bio in request

``struct request_queue *q``	DB	Request queue this request belongs to

``struct request_list *rl``	B	Request list this request came from
=============================== ======= =======================================
