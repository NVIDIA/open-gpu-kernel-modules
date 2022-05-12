/*
 * Intel i82586 Ethernet definitions
 *
 * This is an extension to the Linux operating system, and is covered by the
 * same Gnu Public License that covers that work.
 *
 * copyrights (c) 1994 by Michael Hipp (hippm@informatik.uni-tuebingen.de)
 *
 * I have done a look in the following sources:
 *   crynwr-packet-driver by Russ Nelson
 *   Garret A. Wollman's i82586-driver for BSD
 */

/*
 * Cloned from ni52.h, copyright as above.
 *
 * Modified for Sun3 OBIO i82586 by Sam Creasey (sammy@sammy.net)
 */


/* defines for the obio chip (not vme) */
#define IEOB_NORSET 0x80        /* don't reset the board */
#define IEOB_ONAIR  0x40        /* put us on the air */
#define IEOB_ATTEN  0x20        /* attention! */
#define IEOB_IENAB  0x10        /* interrupt enable */
#define IEOB_XXXXX  0x08        /* free bit */
#define IEOB_XCVRL2 0x04        /* level 2 transceiver? */
#define IEOB_BUSERR 0x02        /* bus error */
#define IEOB_INT    0x01        /* interrupt */

/* where the obio one lives */
#define IE_OBIO 0xc0000
#define IE_IRQ 3

/*
 * where to find the System Configuration Pointer (SCP)
 */
#define SCP_DEFAULT_ADDRESS 0xfffff4


/*
 * System Configuration Pointer Struct
 */

struct scp_struct
{
  unsigned short zero_dum0;	/* has to be zero */
  unsigned char  sysbus;	/* 0=16Bit,1=8Bit */
  unsigned char  zero_dum1;	/* has to be zero for 586 */
  unsigned short zero_dum2;
  unsigned short zero_dum3;
  char          *iscp;		/* pointer to the iscp-block */
};


/*
 * Intermediate System Configuration Pointer (ISCP)
 */
struct iscp_struct
{
  unsigned char  busy;          /* 586 clears after successful init */
  unsigned char  zero_dummy;    /* has to be zero */
  unsigned short scb_offset;    /* pointeroffset to the scb_base */
  char          *scb_base;      /* base-address of all 16-bit offsets */
};

/*
 * System Control Block (SCB)
 */
struct scb_struct
{
  unsigned char rus;
  unsigned char cus;
  unsigned char cmd_ruc;           /* command word: RU part */
  unsigned char cmd_cuc;           /* command word: CU part & ACK */
  unsigned short cbl_offset;    /* pointeroffset, command block list */
  unsigned short rfa_offset;    /* pointeroffset, receive frame area */
  unsigned short crc_errs;      /* CRC-Error counter */
  unsigned short aln_errs;      /* allignmenterror counter */
  unsigned short rsc_errs;      /* Resourceerror counter */
  unsigned short ovrn_errs;     /* OVerrunerror counter */
};

/*
 * possible command values for the command word
 */
#define RUC_MASK	0x0070	/* mask for RU commands */
#define RUC_NOP		0x0000	/* NOP-command */
#define RUC_START	0x0010	/* start RU */
#define RUC_RESUME	0x0020	/* resume RU after suspend */
#define RUC_SUSPEND	0x0030	/* suspend RU */
#define RUC_ABORT	0x0040	/* abort receiver operation immediately */

#define CUC_MASK        0x07  /* mask for CU command */
#define CUC_NOP         0x00  /* NOP-command */
#define CUC_START       0x01  /* start execution of 1. cmd on the CBL */
#define CUC_RESUME      0x02  /* resume after suspend */
#define CUC_SUSPEND     0x03  /* Suspend CU */
#define CUC_ABORT       0x04  /* abort command operation immediately */

#define ACK_MASK        0xf0  /* mask for ACK command */
#define ACK_CX          0x80  /* acknowledges STAT_CX */
#define ACK_FR          0x40  /* ack. STAT_FR */
#define ACK_CNA         0x20  /* ack. STAT_CNA */
#define ACK_RNR         0x10  /* ack. STAT_RNR */

/*
 * possible status values for the status word
 */
#define STAT_MASK       0xf0  /* mask for cause of interrupt */
#define STAT_CX         0x80  /* CU finished cmd with its I bit set */
#define STAT_FR         0x40  /* RU finished receiving a frame */
#define STAT_CNA        0x20  /* CU left active state */
#define STAT_RNR        0x10  /* RU left ready state */

#define CU_STATUS       0x7   /* CU status, 0=idle */
#define CU_SUSPEND      0x1   /* CU is suspended */
#define CU_ACTIVE       0x2   /* CU is active */

#define RU_STATUS	0x70	/* RU status, 0=idle */
#define RU_SUSPEND	0x10	/* RU suspended */
#define RU_NOSPACE	0x20	/* RU no resources */
#define RU_READY	0x40	/* RU is ready */

/*
 * Receive Frame Descriptor (RFD)
 */
struct rfd_struct
{
  unsigned char  stat_low;	/* status word */
  unsigned char  stat_high;	/* status word */
  unsigned char  rfd_sf;	/* 82596 mode only */
  unsigned char  last;		/* Bit15,Last Frame on List / Bit14,suspend */
  unsigned short next;		/* linkoffset to next RFD */
  unsigned short rbd_offset;	/* pointeroffset to RBD-buffer */
  unsigned char  dest[ETH_ALEN];	/* ethernet-address, destination */
  unsigned char  source[ETH_ALEN];	/* ethernet-address, source */
  unsigned short length;	/* 802.3 frame-length */
  unsigned short zero_dummy;	/* dummy */
};

#define RFD_LAST     0x80	/* last: last rfd in the list */
#define RFD_SUSP     0x40	/* last: suspend RU after  */
#define RFD_COMPL    0x80
#define RFD_OK       0x20
#define RFD_BUSY     0x40
#define RFD_ERR_LEN  0x10     /* Length error (if enabled length-checking */
#define RFD_ERR_CRC  0x08     /* CRC error */
#define RFD_ERR_ALGN 0x04     /* Alignment error */
#define RFD_ERR_RNR  0x02     /* status: receiver out of resources */
#define RFD_ERR_OVR  0x01     /* DMA Overrun! */

#define RFD_ERR_FTS  0x0080	/* Frame to short */
#define RFD_ERR_NEOP 0x0040	/* No EOP flag (for bitstuffing only) */
#define RFD_ERR_TRUN 0x0020	/* (82596 only/SF mode) indicates truncated frame */
#define RFD_MATCHADD 0x0002     /* status: Destinationaddress !matches IA (only 82596) */
#define RFD_COLLDET  0x0001	/* Detected collision during reception */

/*
 * Receive Buffer Descriptor (RBD)
 */
struct rbd_struct
{
  unsigned short status;	/* status word,number of used bytes in buff */
  unsigned short next;		/* pointeroffset to next RBD */
  char          *buffer;	/* receive buffer address pointer */
  unsigned short size;		/* size of this buffer */
  unsigned short zero_dummy;    /* dummy */
};

#define RBD_LAST	0x8000	/* last buffer */
#define RBD_USED	0x4000	/* this buffer has data */
#define RBD_MASK	0x3fff	/* size-mask for length */

/*
 * Statusvalues for Commands/RFD
 */
#define STAT_COMPL   0x8000	/* status: frame/command is complete */
#define STAT_BUSY    0x4000	/* status: frame/command is busy */
#define STAT_OK      0x2000	/* status: frame/command is ok */

/*
 * Action-Commands
 */
#define CMD_NOP		0x0000	/* NOP */
#define CMD_IASETUP	0x0001	/* initial address setup command */
#define CMD_CONFIGURE	0x0002	/* configure command */
#define CMD_MCSETUP	0x0003	/* MC setup command */
#define CMD_XMIT	0x0004	/* transmit command */
#define CMD_TDR		0x0005	/* time domain reflectometer (TDR) command */
#define CMD_DUMP	0x0006	/* dump command */
#define CMD_DIAGNOSE	0x0007	/* diagnose command */

/*
 * Action command bits
 */
#define CMD_LAST	0x8000	/* indicates last command in the CBL */
#define CMD_SUSPEND	0x4000	/* suspend CU after this CB */
#define CMD_INT		0x2000	/* generate interrupt after execution */

/*
 * NOP - command
 */
struct nop_cmd_struct
{
  unsigned short cmd_status;	/* status of this command */
  unsigned short cmd_cmd;       /* the command itself (+bits) */
  unsigned short cmd_link;      /* offsetpointer to next command */
};

/*
 * IA Setup command
 */
struct iasetup_cmd_struct
{
  unsigned short cmd_status;
  unsigned short cmd_cmd;
  unsigned short cmd_link;
  unsigned char  iaddr[6];
};

/*
 * Configure command
 */
struct configure_cmd_struct
{
  unsigned short cmd_status;
  unsigned short cmd_cmd;
  unsigned short cmd_link;
  unsigned char  byte_cnt;   /* size of the config-cmd */
  unsigned char  fifo;       /* fifo/recv monitor */
  unsigned char  sav_bf;     /* save bad frames (bit7=1)*/
  unsigned char  adr_len;    /* adr_len(0-2),al_loc(3),pream(4-5),loopbak(6-7)*/
  unsigned char  priority;   /* lin_prio(0-2),exp_prio(4-6),bof_metd(7) */
  unsigned char  ifs;        /* inter frame spacing */
  unsigned char  time_low;   /* slot time low */
  unsigned char  time_high;  /* slot time high(0-2) and max. retries(4-7) */
  unsigned char  promisc;    /* promisc-mode(0) , et al (1-7) */
  unsigned char  carr_coll;  /* carrier(0-3)/collision(4-7) stuff */
  unsigned char  fram_len;   /* minimal frame len */
  unsigned char  dummy;	     /* dummy */
};

/*
 * Multicast Setup command
 */
struct mcsetup_cmd_struct
{
  unsigned short cmd_status;
  unsigned short cmd_cmd;
  unsigned short cmd_link;
  unsigned short mc_cnt;		/* number of bytes in the MC-List */
  unsigned char  mc_list[0][6];  	/* pointer to 6 bytes entries */
};

/*
 * DUMP command
 */
struct dump_cmd_struct
{
  unsigned short cmd_status;
  unsigned short cmd_cmd;
  unsigned short cmd_link;
  unsigned short dump_offset;    /* pointeroffset to DUMP space */
};

/*
 * transmit command
 */
struct transmit_cmd_struct
{
  unsigned short cmd_status;
  unsigned short cmd_cmd;
  unsigned short cmd_link;
  unsigned short tbd_offset;	/* pointeroffset to TBD */
  unsigned char  dest[6];       /* destination address of the frame */
  unsigned short length;	/* user defined: 802.3 length / Ether type */
};

#define TCMD_ERRMASK     0x0fa0
#define TCMD_MAXCOLLMASK 0x000f
#define TCMD_MAXCOLL     0x0020
#define TCMD_HEARTBEAT   0x0040
#define TCMD_DEFERRED    0x0080
#define TCMD_UNDERRUN    0x0100
#define TCMD_LOSTCTS     0x0200
#define TCMD_NOCARRIER   0x0400
#define TCMD_LATECOLL    0x0800

struct tdr_cmd_struct
{
  unsigned short cmd_status;
  unsigned short cmd_cmd;
  unsigned short cmd_link;
  unsigned short status;
};

#define TDR_LNK_OK	0x8000	/* No link problem identified */
#define TDR_XCVR_PRB	0x4000	/* indicates a transceiver problem */
#define TDR_ET_OPN	0x2000	/* open, no correct termination */
#define TDR_ET_SRT	0x1000	/* TDR detected a short circuit */
#define TDR_TIMEMASK	0x07ff	/* mask for the time field */

/*
 * Transmit Buffer Descriptor (TBD)
 */
struct tbd_struct
{
  unsigned short size;		/* size + EOF-Flag(15) */
  unsigned short next;          /* pointeroffset to next TBD */
  char          *buffer;        /* pointer to buffer */
};

#define TBD_LAST 0x8000         /* EOF-Flag, indicates last buffer in list */




