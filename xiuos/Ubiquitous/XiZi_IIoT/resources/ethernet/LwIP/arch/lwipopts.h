/**
  ******************************************************************************
  * @file    lwipopts.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   lwIP Options Configuration.
  *          This file is based on Utilities\lwip_v1.4.1\src\include\lwip\opt.h
  *          and contains the lwIP configuration for the STM32F4x7 demonstration.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include <xsconfig.h>

/* ---------- Debug options ---------- */
#ifndef LWIP_DEBUG
#define LWIP_DEBUG  1
#endif

#ifdef LWIP_DEBUG
#ifdef LWIP_SYS_DEBUG
#define SYS_DEBUG                   LWIP_DBG_ON
#else
#define SYS_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_ETHARP_DEBUG
#define ETHARP_DEBUG                LWIP_DBG_ON
#else
#define ETHARP_DEBUG                LWIP_DBG_OFF
#endif

#ifdef LWIP_PPP_DEBUG
#define PPP_DEBUG                   LWIP_DBG_ON
#else
#define PPP_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_MEM_DEBUG
#define MEM_DEBUG                   LWIP_DBG_ON
#else
#define MEM_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_MEMP_DEBUG
#define MEMP_DEBUG                  LWIP_DBG_ON
#else
#define MEMP_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_PBUF_DEBUG
#define PBUF_DEBUG                  LWIP_DBG_ON
#else
#define PBUF_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_API_LIB_DEBUG
#define API_LIB_DEBUG               LWIP_DBG_ON
#else
#define API_LIB_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_API_MSG_DEBUG
#define API_MSG_DEBUG               LWIP_DBG_ON
#else
#define API_MSG_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_TCPIP_DEBUG
#define TCPIP_DEBUG                 LWIP_DBG_ON
#else
#define TCPIP_DEBUG                 LWIP_DBG_OFF
#endif

#ifdef LWIP_NETIF_DEBUG
#define NETIF_DEBUG                 LWIP_DBG_ON
#else
#define NETIF_DEBUG                 LWIP_DBG_OFF
#endif

#ifdef LWIP_SOCKETS_DEBUG
#define SOCKETS_DEBUG               LWIP_DBG_ON
#else
#define SOCKETS_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_DNS_DEBUG
#define DNS_DEBUG                   LWIP_DBG_ON
#else
#define DNS_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_AUTOIP_DEBUG
#define AUTOIP_DEBUG                LWIP_DBG_ON
#else
#define AUTOIP_DEBUG                LWIP_DBG_OFF
#endif

#ifdef LWIP_DHCP_DEBUG
#define DHCP_DEBUG                  LWIP_DBG_ON
#else
#define DHCP_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_IP_DEBUG
#define IP_DEBUG                    LWIP_DBG_ON
#else
#define IP_DEBUG                    LWIP_DBG_OFF
#endif

#ifdef LWIP_IP_REASS_DEBUG
#define IP_REASS_DEBUG              LWIP_DBG_ON
#else
#define IP_REASS_DEBUG              LWIP_DBG_OFF
#endif

#ifdef LWIP_ICMP_DEBUG
#define ICMP_DEBUG                  LWIP_DBG_ON
#else
#define ICMP_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_IGMP_DEBUG
#define IGMP_DEBUG                  LWIP_DBG_ON
#else
#define IGMP_DEBUG                  LWIP_DBG_OFF
#endif

#ifdef LWIP_UDP_DEBUG
#define UDP_DEBUG                   LWIP_DBG_ON
#else
#define UDP_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_TCP_DEBUG
#define TCP_DEBUG                   LWIP_DBG_ON
#else
#define TCP_DEBUG                   LWIP_DBG_OFF
#endif

#ifdef LWIP_TCP_INPUT_DEBUG
#define TCP_INPUT_DEBUG             LWIP_DBG_ON
#else
#define TCP_INPUT_DEBUG             LWIP_DBG_OFF
#endif

#ifdef LWIP_TCP_OUTPUT_DEBUG
#define TCP_OUTPUT_DEBUG            LWIP_DBG_ON
#else
#define TCP_OUTPUT_DEBUG            LWIP_DBG_OFF
#endif

#ifdef LWIP_TCP_RTO_DEBUG
#define TCP_RTO_DEBUG               LWIP_DBG_ON
#else
#define TCP_RTO_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_TCP_CWND_DEBUG
#define TCP_CWND_DEBUG              LWIP_DBG_ON
#else
#define TCP_CWND_DEBUG              LWIP_DBG_OFF
#endif

#ifdef LWIP_TCP_WND_DEBUG
#define TCP_WND_DEBUG               LWIP_DBG_ON
#else
#define TCP_WND_DEBUG               LWIP_DBG_OFF
#endif

#ifdef LWIP_TCP_FR_DEBUG
#define TCP_FR_DEBUG                LWIP_DBG_ON
#else
#define TCP_FR_DEBUG                LWIP_DBG_OFF
#endif

#ifdef LWIP_TCP_QLEN_DEBUG
#define TCP_QLEN_DEBUG              LWIP_DBG_ON
#else
#define TCP_QLEN_DEBUG              LWIP_DBG_OFF
#endif

#ifdef LWIP_TCP_RST_DEBUG
#define TCP_RST_DEBUG               LWIP_DBG_ON
#else
#define TCP_RST_DEBUG               LWIP_DBG_OFF
#endif

#endif /* LWIP_DEBUG */


#define LWIP_TIMEVAL_PRIVATE    0
#define LWIP_NO_UNISTD_H        0
#define LWIP_NO_STDDEF_H        0
#define LWIP_NO_STDINT_H        0
#define LWIP_NO_INTTYPES_H      0
#define LWIP_NO_LIMITS_H        0
#define LWIP_NO_CTYPE_H         0
#define LWIP_SOCKET_SELECT      1
#define LWIP_SOCKET_POLL        1

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    1

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                  0

/**
 * NO_SYS_NO_TIMERS==1: Drop support for sys_timeout when NO_SYS==1
 * Mainly for compatibility to old versions.
 */
#define NO_SYS_NO_TIMERS        0

/* ---------- Memory options ---------- */
/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           4

/* MEM_SIZE: the size of the heap memory. If the application will send
a lot of data that needs to be copied, this should be set high. */
#define MEM_SIZE                (64*1024)

/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
#define MEMP_NUM_PBUF           32
/* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
   per active UDP "connection". */
#define MEMP_NUM_UDP_PCB        4
/* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP
   connections. */
#define MEMP_NUM_TCP_PCB        64
/* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP
   connections. */
#define MEMP_NUM_TCP_PCB_LISTEN 2
/* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP
   segments. */
#define MEMP_NUM_TCP_SEG        256
/* MEMP_NUM_SYS_TIMEOUT: the number of simulateously active
   timeouts. */
// #define MEMP_NUM_SYS_TIMEOUT    6
#define MEMP_NUM_SYS_TIMEOUT       (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_SUPPORT + (LWIP_IPV6 ? (1 + (2*LWIP_IPV6)) : 0))

/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#define PBUF_POOL_SIZE          255
/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
// #define PBUF_POOL_BUFSIZE       1024
#define PBUF_POOL_BUFSIZE       LWIP_MEM_ALIGN_SIZE(TCP_MSS+40+PBUF_LINK_ENCAPSULATION_HLEN+PBUF_LINK_HLEN)

/* ---------- ARP options ---------- */
#define LWIP_ARP                    1
#define ARP_TABLE_SIZE              10
#define ARP_QUEUEING                1

/* ---------- TCP options ---------- */
#define LWIP_TCP                1
#define TCP_TTL                 255

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#define TCP_QUEUE_OOSEQ         1

/* TCP Maximum segment size. */
#define TCP_MSS                 (1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF             (11*TCP_MSS)

/*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
  as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */

#define TCP_SND_QUEUELEN        (8* TCP_SND_BUF/TCP_MSS)

/* TCP receive window. */
#define TCP_WND                 8192
// #define TCP_WND                 (12 * TCP_MSS)


/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX                  12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX               4

/**
 * LWIP_TCP_KEEPALIVE==1: Enable TCP_KEEPIDLE, TCP_KEEPINTVL and TCP_KEEPCNT
 * options processing. Note that TCP_KEEPIDLE and TCP_KEEPINTVL have to be set
 * in seconds. (does not require sockets.c, and will affect tcp.c)
 */
#ifndef LWIP_TCP_KEEPALIVE
#define LWIP_TCP_KEEPALIVE              1
#endif

/**
 * LWIP_NETIF_HOSTNAME==1: Support netif hostname
 */
#ifndef LWIP_NETIF_HOSTNAME
#define LWIP_NETIF_HOSTNAME             1
#endif

/**
 * LWIP_NETIF_API==1: Support netif api (in netifapi.c)
 */
#ifndef LWIP_NETIF_API
#define LWIP_NETIF_API                  1
#endif

/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#ifndef LWIP_SO_SNDTIMEO
#define LWIP_SO_SNDTIMEO                1
#endif

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#ifndef LWIP_SO_RCVTIMEO
#define LWIP_SO_RCVTIMEO                1
#endif

/**
 * LWIP_SO_RCVBUF==1: Enable SO_RCVBUF processing.
 */
#ifndef LWIP_SO_RCVBUF
#define LWIP_SO_RCVBUF                  1
#endif

/**
 * If LWIP_SO_RCVBUF is used, this is the default value for recv_bufsize.
 */
#ifndef RECV_BUFSIZE_DEFAULT
#define RECV_BUFSIZE_DEFAULT            8192
#endif

/* ---------- ICMP options ---------- */
#define LWIP_ICMP               1


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
   turning this on does currently not work. */
#define LWIP_DHCP               1


/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define UDP_TTL                 255


/* ---------- Statistics options ---------- */
#define LWIP_PROVIDE_ERRNO      1

/* ---------- link callback options ---------- */
/* LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#define LWIP_NETIF_LINK_CALLBACK        0
/*
   --------------------------------------
   ---------- Checksum options ----------
   --------------------------------------
*/

/*
The STM32F4x7 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the  the checksum.
*/
#define CHECKSUM_BY_HARDWARE

#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
  /* CHECKSUM_CHECK_ICMP==0: Check checksums by hardware for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
  /* CHECKSUM_CHECK_ICMP==1: Check checksums by software for incoming ICMP packets.*/
  #define CHECKSUM_GEN_ICMP               1
#endif


/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/
/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    1

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                     1

/**
 * LWIP_SO_RCVBUF==1: Enable SO_RCVBUF processing.
 */
#define LWIP_SO_RCVBUF                  1

   /**
    * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
    * SO_SNDTIMEO processing.
    */
#ifndef LWIP_SO_SNDTIMEO
#define LWIP_SO_SNDTIMEO                1
#endif

    /**
     * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
     * SO_RCVTIMEO processing.
     */
#ifndef LWIP_SO_RCVTIMEO
#define LWIP_SO_RCVTIMEO                1
#endif

     /**
      * LWIP_SO_LINGER==1: Enable SO_LINGER processing.
      */
// #define LWIP_SO_LINGER                  1

/* ---------- IP options ---------- */
/* Define IP_FORWARD to 1 if you wish to have the ability to forward
   IP packets across network interfaces. If you are going to run lwIP
   on a device with only one network interface, define this to 0. */
#define IP_FORWARD                  0

/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#ifdef LWIP_REASSEMBLY_FRAG
#define IP_REASSEMBLY               1
#define IP_FRAG                     1
#define IP_REASS_MAX_PBUFS          10
#define MEMP_NUM_REASSDATA          10
#else
#define IP_REASSEMBLY               0
#define IP_FRAG                     0
#endif

/* ---------- ICMP options ---------- */
#define ICMP_TTL                    255

/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#define LWIP_DHCP                   1

/* 1 if you want to do an ARP check on the offered address
   (recommended). */
#define DHCP_DOES_ARP_CHECK         (LWIP_DHCP)

/* ---------- AUTOIP options ------- */
#define LWIP_AUTOIP                 0
#define LWIP_DHCP_AUTOIP_COOP       (LWIP_DHCP && LWIP_AUTOIP)


#define LWIP_UDPLITE                0
#define UDP_TTL                     255


/* ---------- Statistics options ---------- */
#define LWIP_STATS                  1
#define LWIP_STATS_DISPLAY          1

/*
   ---------------------------------
   ---------- OS options ----------
   ---------------------------------
*/

#define DEFAULT_RAW_RECVMBOX_SIZE       8
#define DEFAULT_UDP_RECVMBOX_SIZE       8
#define DEFAULT_TCP_RECVMBOX_SIZE       8
#define DEFAULT_ACCEPTMBOX_SIZE         10

#define DEFAULT_THREAD_PRIO             20
#define DEFAULT_THREAD_STACKSIZE        2048

#define TCPIP_THREAD_NAME               "tcp"
#define TCPIP_THREAD_STACKSIZE          2048
#define TCPIP_MBOX_SIZE                 16
#define TCPIP_THREAD_PRIO               20

/*
   ----------------------------------------
   ---------- Lwip Debug options ----------
   ----------------------------------------
*/
#define LWIP_IPV4                       1
#define LWIP_RAW                        1
#define LWIP_DNS                        1

#if LWIP_DNS
#define LWIP_RAND rand
#endif

#ifndef nfds_t
typedef unsigned int nfds_t;
#endif


#define MEMP_LIB_MALLOC                 1
#define MEMP_MEM_MALLOC                 1

#define lw_print KPrintf
#define lw_error KPrintf
#define lw_notice KPrintf

#endif /* __LWIPOPTS_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

