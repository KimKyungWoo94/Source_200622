/**
 * @addtogroup cohda_pktbuf_user Cohda's userspace packet buffer library
 * @{
 *
 * @file
 * Userspace packet buffer support
 *
 * An unsullied (non-GPL) implementation of an sk_buff equivalent
 * Non-GPL references
 *  - http://www.skbuff.net/
 *  - http://vger.kernel.org/~davem/
 *  - http://lwn.net/images/pdf/LDD3/ch17.pdf
 *  - http://www.freebsd.org/cgi/man.cgi?query=queue&sektion=3
 *
 */

//------------------------------------------------------------------------------
// Copyright (c) 2017 Cohda Wireless Pty Ltd
//------------------------------------------------------------------------------

#ifndef __INCLUDE__LINUX__COHDA__PKTBUF_APP_H__
#define __INCLUDE__LINUX__COHDA__PKTBUF_APP_H__

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
//#include "queue.h"
#include <sys/queue.h>  // changed by gyun@KETI
#include "atomic.h"

#ifndef likely
#define likely(x)       __builtin_expect((x),1)
#endif
#ifndef unlikely
#define unlikely(x)     __builtin_expect((x),0)
#endif

/// Unique id value for a PktBuf queue
#define PKTBUF_MAGIC_HEAD (0x4EAD)
/// Unique id value for a PktBuf
#define PKTBUF_MAGIC_BUF  (0xB0F4)
/// Size of PktBuf private storage
#define PKTBUF_PRIV_CNT   (16)

struct PktBuf; // fwd decl

// Optimizer should reduce this to NOP as pQ == &(pQ->Head)
#define QHEAD(pQ) (&(pQ->Head))

///
typedef struct PktBufQueue
{
  /// Actual list head
  TAILQ_HEAD(PktBufHead, PktBuf) Head;
  /// Magic value (PKTBUF_MAGIC_HEAD)
  uint16_t Magic;
  /// Queue length
  int16_t Len;
  /// Lock
  int Lock; // TODO

} tPktBufQueue;


///
typedef struct PktBuf
{
  /// Queue entry
  TAILQ_ENTRY(PktBuf) Entry;

  /// Private storage area
  union
  {
    char Priv[PKTBUF_PRIV_CNT];
    char cb[PKTBUF_PRIV_CNT];
  };
  /// Magic value (PKTBUF_MAGIC_BUF)
  uint16_t Magic;
  /// Quick access to data length
  union
  {
    int16_t Len;
    int16_t len; ///<
  };

  /// Parent (if cloned)
  struct PktBuf *pParent;
  /// Reference count of users (clones)
  atomic32_t UsrCnt;

  // Pointers
  union {
    unsigned char *pHead;
    unsigned char *head;
  };
  union {
    unsigned char *pData;
    unsigned char *data;
  };
  union {
    unsigned char *pTail;
    unsigned char *tail;
  };
  union {
    unsigned char *pEnd;
    unsigned char *end;
  };
  union {
    uint16_t protocol;
  };
  union {
    void *dev;
  };
} tPktBuf;

/// @brief TODO
static inline int PktBuf_Init(void **ppPktBuf)
{
  if (ppPktBuf == NULL)
    return -1;

  *ppPktBuf = (void *)(0xDEADBEEF);
  return 0;
}

/// @brief TODO
static inline int PktBuf_Exit(void *pPktBuf)
{
  if (pPktBuf != (void *)(0xDEADBEEF))
    return -1;

  return 0;
}

/**
 Packet queue stuff to emulate:
  skb_queue_head_init
  skb_queue_len
  skb_dequeue
  skb_peek_tail
  skb_queue_head
  skb_queue_tail
  skb_queue_walk_safe
  skb_unlink
  skb_queue_purge
*/

/// @brief initialize the queue head
static inline void PktBuf_QueueInit(struct PktBufQueue *pQ)
{
  // Sanity check
  if (unlikely(pQ == NULL))
    return;

  memset(pQ, 0, sizeof(*pQ));
  pQ->Magic = PKTBUF_MAGIC_HEAD;
  ; // TODO: Lock init
  TAILQ_INIT(QHEAD(pQ));
  pQ->Len = 0;
}

/// @brief Queue length
static inline uint32_t PktBuf_QueueLen(const struct PktBufQueue *pQ)
{
  if (unlikely(pQ == NULL))
    return 0;
  if (unlikely(pQ->Magic != PKTBUF_MAGIC_HEAD))
    return 0;

  return pQ->Len;
}

/// @brief Is the given list empty?
static inline bool PktBuf_QueueEmpty(const struct PktBufQueue *pQ)
{
  if (unlikely(pQ == NULL))
    return true;
  if (unlikely(pQ->Magic != PKTBUF_MAGIC_HEAD))
    return true;

  return TAILQ_EMPTY(QHEAD(pQ));
}

/// @brief Return, but do not remove, the first SKB on the list, else NULL.
static inline struct PktBuf *PktBuf_Peek(struct PktBufQueue *pQ)
{
  struct PktBuf *pPkb = NULL;

  if (unlikely(pQ == NULL))
    goto Error;
  if (unlikely(pQ->Magic != PKTBUF_MAGIC_HEAD))
    goto Error;

  pPkb = TAILQ_FIRST(QHEAD(pQ));

Error:
  return pPkb;
}

/// @brief Exactly like PktBuf_Peek(), except it returns the last on the list.
static inline struct PktBuf *PktBuf_PeekTail(struct PktBufQueue *pQ)
{
  struct PktBuf *pPkb = NULL;

  if (unlikely(pQ == NULL))
    goto Error;
  if (unlikely(pQ->Magic != PKTBUF_MAGIC_HEAD))
    goto Error;

  pPkb = TAILQ_LAST(QHEAD(pQ), PktBufHead);

Error:
  return pPkb;
}

/// @brief Remove and return the first SKB on 'list'
static inline struct PktBuf *PktBuf_Dequeue(struct PktBufQueue *pQ)
{
  struct PktBuf *pPkb = NULL;

  if (unlikely(pQ == NULL))
    goto Error;
  if (unlikely(pQ->Magic != PKTBUF_MAGIC_HEAD))
    goto Error;

  pPkb = TAILQ_FIRST(QHEAD(pQ));
  if (pPkb != NULL)
  {
    ; // TODO: Lock
    TAILQ_REMOVE(QHEAD(pQ), pPkb, Entry);
    ; // TODO: UnLock
    pQ->Len -= 1;
  }

Error:
  return pPkb;
}

/// @brief Remove and return the last SKB on 'list'
static inline struct PktBuf *PktBuf_DequeueTail(struct PktBufQueue *pQ)
{
  struct PktBuf *pPkb = NULL;

  if (unlikely(pQ == NULL))
    goto Error;
  if (unlikely(pQ->Magic != PKTBUF_MAGIC_HEAD))
    goto Error;

  pPkb = TAILQ_LAST(QHEAD(pQ), PktBufHead);
  if (pPkb != NULL)
  {
    ; // TODO: Lock
    TAILQ_REMOVE(QHEAD(pQ), pPkb, Entry)
    ; // TODO: UnLock
    pQ->Len -= 1;
  }

Error:
  return pPkb;
}

/// @brief Insert 'pNew' before 'pOld' on the list that 'pOld' is on
static inline int PktBuf_Insert(struct PktBuf *pOld,
                                struct PktBuf *pNew,
                                struct PktBufQueue *pQ)
{
  int Res = -1;

  if (unlikely((pOld == NULL) || (pNew == NULL) || (pQ == NULL)))
    goto Error;
  TAILQ_INSERT_BEFORE(pOld, pNew, Entry);
  Res = 0;

Error:
  return Res;
}

/// @brief Insert 'pNew' after 'pOld' on the list that 'pOld' is on
static inline int PktBuf_Append(struct PktBuf *pOld,
                                struct PktBuf *pNew,
                                struct PktBufQueue *pQ)
{
  int Res = -1;

  if (unlikely((pOld == NULL) || (pNew == NULL) || (pQ == NULL)))
    goto Error;
  TAILQ_INSERT_AFTER(QHEAD(pQ), pOld, pNew, Entry);
  Res = 0;

Error:
  return Res;
}

/// Unlink 'PktBuf' from the list that it is on
static inline int PktBuf_Unlink(struct PktBuf *pPkb)
{
  int Res = -1;
  struct PktBufQueue *pQ = NULL;

  if (unlikely(pPkb == NULL))
    goto Error;
  ; // TODO: Find pQ somehow
  if (unlikely(pQ == NULL))
    goto Error;
  TAILQ_REMOVE(QHEAD(pQ), pPkb, Entry);
  Res = 0;

Error:
  return Res;
}

/// @brief Insert 'pNew' at the head of the list
static inline int PktBuf_QueueHead(struct PktBufQueue *pQ, struct PktBuf *pNew)
{
  int Res = -1;

  if (unlikely(pQ == NULL))
    goto Error;
  if (unlikely(pQ->Magic != PKTBUF_MAGIC_HEAD))
    goto Error;

  TAILQ_INSERT_HEAD(QHEAD(pQ), pNew, Entry);
  pQ->Len += 1;
  Res = 0;
Error:
  return Res;
}

/// @brief Insert 'pNew' at the end of the list
static inline int PktBuf_QueueTail(struct PktBufQueue *pQ, struct PktBuf *pNew)
{
  int Res = -1;

  if (unlikely(pQ == NULL))
    goto Error;
  if (unlikely(pQ->Magic != PKTBUF_MAGIC_HEAD))
    goto Error;
  TAILQ_INSERT_TAIL(QHEAD(pQ), pNew, Entry);
  pQ->Len += 1;
  Res = 0;

Error:
  return Res;
}

/// @brief Flush the entire queue
static inline void PktBuf_QueuePurge(struct PktBufQueue *pQ)
{
  if (unlikely(pQ == NULL))
    goto Error;
  if (unlikely(pQ->Magic != PKTBUF_MAGIC_HEAD))
    goto Error;
  ; // TODO
  pQ->Len = 0;

Error:
  return;
}

/// @brief Walk the queue safely (can unlink in loop)
#define PktBuf_QueueWalkSafe(pQ, pPkb, pTmp) TAILQ_FOREACH_SAFE(pPkb, QHEAD(pQ), Entry, pTmp)

/**
 Socket buf stuff to emulate:
  alloc_skb       = PktBuf_Alloc()
  kfree_skb       = PktBuf_Free()
  skb_headroom    = PktBuf_Headroom()
  skb_tailroom    = PktBuf_Tailroom()
  skb_reserve     = PktBuf_Reserve()
  skb_pull        = PktBuf_Pull()
  skb_push        = PktBuf_Push()
  skb_put         = PktBuf_Put()
  skb_trim        = PktBuf_Trim()
  skb_clone       = PktBuf_Clone()
  skb_share_check = PktBuf_ShareCheck()
  skb_copy        = PktBuf_Copy()
  skb_copy_expand
  skb_store_bits
*/

/// @brief TODO
static inline struct PktBuf *PktBuf_Alloc(int Len)
{
  struct PktBuf *pPkb = NULL;
  unsigned char *pBuf = NULL;

  if (unlikely(Len < 0))
    goto Error;

  pBuf = malloc(sizeof(*pPkb) + Len);
  if (unlikely(pBuf == NULL))
    goto Error;
  memset(pBuf, 0, sizeof(*pPkb));

  pPkb = (struct PktBuf *)pBuf;
  pPkb->Magic = PKTBUF_MAGIC_BUF;
  pBuf += sizeof(*pPkb);
  pPkb->pHead = pBuf;
  pPkb->pData = pBuf;
  pPkb->pTail = pBuf;
  pPkb->pEnd  = pBuf + Len;
  pPkb->UsrCnt = 1;

Error:
  return pPkb;
}

/// @brief TODO
static inline void PktBuf_Free(struct PktBuf *pPkb)
{
  if (unlikely(pPkb == NULL))
    return;
  if (unlikely(pPkb->Magic != PKTBUF_MAGIC_BUF))
    return;

  if (likely(pPkb->pParent == NULL))
  {
    // Not a clone, free the underlying memory if no more users
    if (atomic_dec_and_test(&(pPkb->UsrCnt)))
    {
      pPkb->Magic = 0;
      free(pPkb);
    }
  }
  else
  {
    // We're a clone, release the parent
    PktBuf_Free(pPkb->pParent);
    pPkb->Magic = 0;
    free(pPkb);
  }
}

/// @brief Clone the pPkb (data buffer is shared with pParent)
static inline struct PktBuf *PktBuf_Clone(struct PktBuf *pParent)
{
  struct PktBuf *pPkb = NULL;

  if (unlikely(pParent == NULL))
    goto Error;
  if (unlikely(pParent->Magic != PKTBUF_MAGIC_BUF))
    goto Error;
  // Use the ultimate parent
  while (pParent->pParent != NULL)
    pParent = pParent->pParent;
  if (unlikely(pParent->Magic != PKTBUF_MAGIC_BUF))
    goto Error;

  pPkb = (struct PktBuf *)malloc(sizeof(*pPkb));
  if (unlikely(pPkb == NULL))
    goto Error;
  // Increment the parent's user count
  atomic_inc(&(pParent->UsrCnt));
  memcpy(pPkb, pParent, sizeof(*pPkb));
  pPkb->pParent = pParent;
  pPkb->UsrCnt = 1;

Error:
  return pPkb;
}

///
/**
 * @brief If shared: Clone
 * @ref http://www.ecsl.cs.sunysb.edu/elibrary/linux/network/iprecv.pdf
 * The skb_share_check() function determines if the sk_buff is shared. If so, the sk_buff is cloned,
 * the original is freed, and a pointer to the clone is returned. If it is not shared, a pointer to the
 * original is returned. If the sk_buff is shared, but the attempt to clone it fails, NULL is returned
 */
static inline struct PktBuf *PktBuf_ShareCheck(struct PktBuf *pOld)
{
  struct PktBuf *pNew = NULL;

  if (unlikely(pOld == NULL))
    goto Error;
  if (unlikely(pOld->Magic != PKTBUF_MAGIC_BUF))
    goto Error;

  if ((pOld->pParent != NULL) || (pOld->UsrCnt > 1))
  {
    pNew = PktBuf_Clone(pOld);
    if (pNew != NULL)
      PktBuf_Free(pOld);
  }
  else
    pNew = pOld;

Error:
  return pNew;
}

/// @brief Add data to the tail of the buffer
static inline unsigned char *PktBuf_Put(struct PktBuf *pPkb, int Len)
{
  ; // TODO: Sanity checks
  pPkb->Len   += Len;
  pPkb->pTail += Len;

  return pPkb->pData;
}

/// @brief Add data to the front of a buffer
static inline unsigned char *PktBuf_Push(struct PktBuf *pPkb, int Len)
{
  ; // TODO: Sanity checks
  pPkb->Len   += Len;
  pPkb->pData -= Len;

  return pPkb->pData;
}

/// @brief Remove data from the front of a buffer
static inline unsigned char *PktBuf_Pull(struct PktBuf *pPkb, int Len)
{
  ; // TODO: Sanity checks
  pPkb->Len   -= Len;
  pPkb->pData += Len;

  return pPkb->pData;
}

/// @brief Remove data from the tail of a buffer
static inline void PktBuf_Trim(struct PktBuf *pPkb, int Len)
{
  ; // TODO: Sanity checks
  pPkb->Len = Len;
  pPkb->pTail = pPkb->pData + Len;
}

/// @brief Bytes at buffer head
static inline int PktBuf_Headroom(struct PktBuf *pPkb)
{
  return pPkb->pData - pPkb->pHead;
}

/// @brief Bytes at buffer end
static inline int PktBuf_Tailroom(struct PktBuf *pPkb)
{
  return pPkb->pEnd - pPkb->pTail;
}

/// Reserve some space at the front of the buffer
static inline void PktBuf_Reserve(struct PktBuf *pPkb, int Len)
{
  ; // TODO: Sanity checks
  pPkb->pData += Len;
  pPkb->pTail += Len;
}

/// @brief Create a copy of the pPkb and the data buffer too
static inline struct PktBuf *PktBuf_Copy(struct PktBuf *pSrc)
{
  struct PktBuf *pPkb = NULL;
  struct PktBuf *pParent = pSrc;
  int Len;

  if (unlikely(pParent == NULL))
    goto Error;
  if (unlikely(pSrc->Magic != PKTBUF_MAGIC_BUF))
    goto Error;
  // Use the ultimate parent
  while (pSrc->pParent != NULL)
    pSrc = pSrc->pParent;
  if (unlikely(pSrc->Magic != PKTBUF_MAGIC_BUF))
    goto Error;
  // Full length is from pHead to pEnd
  Len = pParent->pEnd - pParent->pHead;
  pPkb = PktBuf_Alloc(Len);
  if (unlikely(pPkb == NULL))
    goto Error;
  // Mimic the headroom
  Len = PktBuf_Headroom(pParent);
  PktBuf_Reserve(pPkb, Len);
  // Copy the data
  Len = pParent->Len;
  PktBuf_Put(pPkb, Len);
  memcpy(pPkb->pData, pParent->pData, Len);

Error:
  return pPkb;
}

/// @brief Create a copy of the pPkb and expand the data buffer too
static inline struct PktBuf *PktBuf_CopyExpand(struct PktBuf *pSrc, int Pre, int Post)
{
  struct PktBuf *pPkb = NULL;
  struct PktBuf *pParent = pSrc;
  int Len;

  if (unlikely(pParent == NULL))
    goto Error;
  if (unlikely(pSrc->Magic != PKTBUF_MAGIC_BUF))
    goto Error;
  // Use the ultimate parent
  while (pSrc->pParent != NULL)
    pSrc = pSrc->pParent;
  if (unlikely(pSrc->Magic != PKTBUF_MAGIC_BUF))
    goto Error;
  // Full length is from pData to pTail (+ Pre + Post)
  Len = Pre + (pParent->pTail - pParent->pData) + Post;
  pPkb = PktBuf_Alloc(Len);
  if (unlikely(pPkb == NULL))
    goto Error;
  // Set the headroom
  PktBuf_Reserve(pPkb, Pre);
  // Copy the data
  PktBuf_Put(pPkb, pParent->Len);
  memcpy(pPkb->pData, pParent->pData, pParent->Len);

Error:
  return pPkb;
}

/// @brief Copy pPkb's data contents to pDst
static inline int PktBuf_CopyBits(const struct PktBuf *pPkb, int Offset, void *pDst, int Len)
{
  int Cnt = 0;

  if (unlikely(pPkb == NULL))
    goto Error;
  // Use the ultimate parent
  while (pPkb->pParent != NULL)
    pPkb = pPkb->pParent;
  // Sanity checks
  Cnt = Len;
  if (Cnt > (pPkb->Len - Offset))
    Cnt = (pPkb->Len - Offset);
  // Copy the data
  if (Cnt > 0)
    memcpy(pDst, (pPkb->pData + Offset), Cnt);

Error:
  return Cnt;
}

/// @brief Store pSrc's data contents to pPkb's buffer
static inline int PktBuf_StoreBits(struct PktBuf *pPkb, int Offset, void *pSrc, int Len)
{
  int Cnt = 0;
  if (unlikely((pPkb == NULL) || (pSrc == NULL) || (Len < 0)))
    goto Error;
  // Sanity checks
  Cnt = Len;
  if (Cnt > (pPkb->Len - Offset))
    Cnt = (pPkb->Len - Offset);
  // Copy the data
  if (Cnt > 0)
  {
    memcpy((pPkb->pData + Offset), pSrc, Cnt);
  }

Error:
  return Cnt;
}

/// @brief TODO
static inline unsigned char *PktBuf_GetMacHdr(struct PktBuf *pPkb)
{
  return pPkb->pData; // TODO
}
/// @brief TODO
static inline void PktBuf_SetMacHdr(struct PktBuf *pPkb, int Len)
{
  ; // TODO
}
/// @brief TODO
static inline void PktBuf_ResetMacHdr(struct PktBuf *pPkb)
{
  ; // TODO
}

/// @brief TODO
static inline unsigned char *PktBuf_GetNetworkHdr(struct PktBuf *pPkb)
{
  return pPkb->pData; // TODO
}
/// @brief TODO
static inline void PktBuf_SetNetworkHdr(struct PktBuf *pPkb, int Len)
{
  ; // TODO
}
/// @brief TODO
static inline void PktBuf_ResetNetworkHdr(struct PktBuf *pPkb)
{
  ; // TODO
}

/// @brief TODO
static inline unsigned char *PktBuf_GetTransportHdr(struct PktBuf *pPkb)
{
  return pPkb->pData; // TODO
}
/// @brief TODO
static inline void PktBuf_SetTransportHdr(struct PktBuf *pPkb, int Len)
{
  ; // TODO
}
/// @brief TODO
static inline void PktBuf_ResetTransportHdr(struct PktBuf *pPkb)
{
  ; // TODO
}

/// @brief TODO
static inline void PktBuf_Orphan(struct PktBuf *pPkb)
{
  ; // TODO
}


/**
 @todo
skb_recv_datagram
skb_copy_datagram_iovec
skb_free_datagram
*/

#endif // #ifndef __INCLUDE__LINUX__COHDA__PKTBUF_APP_H__
// Close the doxygen group
/**
 * @}
 */
