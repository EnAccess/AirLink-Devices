/*==============================================================================
 Copyright (c) 2016-2018, The Linux Foundation.
 Copyright (c) 2018-2021, Laurence Lundblade.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are
 met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above
 copyright notice, this list of conditions and the following
 disclaimer in the documentation and/or other materials provided
 with the distribution.
 * Neither the name of The Linux Foundation nor the names of its
 contributors, nor the name "Laurence Lundblade" may be used to
 endorse or promote products derived from this software without
 specific prior written permission.

 THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =============================================================================*/


#ifndef qcbor_private_h
#define qcbor_private_h


#include <stdint.h>
#include "UsefulBuf.h"


#ifdef __cplusplus
extern "C" {
#if 0
} // Keep editor indention formatting happy
#endif
#endif


/*
 The maxium nesting of arrays and maps when encoding or decoding.
 (Further down in the file there is a definition that refers to this
 that is public. This is done this way so there can be a nice
 separation of public and private parts in this file.
*/
#define QCBOR_MAX_ARRAY_NESTING1 15 // Do not increase this over 255


/* The largest offset to the start of an array or map. It is slightly
 less than UINT32_MAX so the error condition can be tested on 32-bit machines.
 UINT32_MAX comes from uStart in QCBORTrackNesting being a uin32_t.

 This will cause trouble on a machine where size_t is less than 32-bits.
 */
#define QCBOR_MAX_ARRAY_OFFSET  (UINT32_MAX - 100)


/* The number of tags that are 16-bit or larger that can be handled
 in a decode.
 */
#define QCBOR_NUM_MAPPED_TAGS 4

/* The number of tags (of any size) recorded for an individual item. */
#define QCBOR_MAX_TAGS_PER_ITEM1 4



/*
 PRIVATE DATA STRUCTURE

 Holds the data for tracking array and map nesting during encoding. Pairs up
 with the Nesting_xxx functions to make an "object" to handle nesting encoding.

 uStart is a uint32_t instead of a size_t to keep the size of this
 struct down so it can be on the stack without any concern.  It would be about
 double if size_t was used instead.

 Size approximation (varies with CPU/compiler):
    64-bit machine: (15 + 1) * (4 + 2 + 1 + 1 pad) + 8 = 136 bytes
    32-bit machine: (15 + 1) * (4 + 2 + 1 + 1 pad) + 4 = 132 bytes
*/
typedef struct __QCBORTrackNesting {
   // PRIVATE DATA STRUCTURE
   struct {
      // See function QCBOREncode_OpenMapOrArray() for details on how this works
      uint32_t  uStart;   // uStart is the byte position where the array starts
      uint16_t  uCount;   // Number of items in the arrary or map; counts items
                          // in a map, not pairs of items
      uint8_t   uMajorType; // Indicates if item is a map or an array
   } pArrays[QCBOR_MAX_ARRAY_NESTING1+1], // stored state for the nesting levels
   *pCurrentNesting; // the current nesting level
} QCBORTrackNesting;


/*
 PRIVATE DATA STRUCTURE

 Context / data object for encoding some CBOR. Used by all encode functions to
 form a public "object" that does the job of encdoing.

 Size approximation (varies with CPU/compiler):
   64-bit machine: 27 + 1 (+ 4 padding) + 136 = 32 + 136 = 168 bytes
   32-bit machine: 15 + 1 + 132 = 148 bytes
*/
struct _QCBOREncodeContext {
   // PRIVATE DATA STRUCTURE
   UsefulOutBuf      OutBuf;  // Pointer to output buffer, its length and
                              // position in it
   uint8_t           uError;  // Error state, always from QCBORError enum
   QCBORTrackNesting nesting; // Keep track of array and map nesting
};


/*
 PRIVATE DATA STRUCTURE

 Holds the data for array and map nesting for decoding work. This
 structure and the DecodeNesting_Xxx() functions in qcbor_decode.c
 form an "object" that does the work for arrays and maps. All access
 to this structure is through DecodeNesting_Xxx() functions.

 64-bit machine size
   128 = 16 * 8 for the two unions
   64  = 16 * 4 for the uLevelType, 1 byte padded to 4 bytes for alignment
   16  = 16 bytes for two pointers
   208 TOTAL

 32-bit machine size is 200 bytes
 */
typedef struct __QCBORDecodeNesting  {
   // PRIVATE DATA STRUCTURE
   struct nesting_decode_level {
      /*
       This keeps tracking info for each nesting level. There are two
       main types of levels:
         1) Byte count tracking. This is for the top level input CBOR
         which might be a single item or a CBOR sequence and byte
         string wrapped encoded CBOR.
         2) Item tracking. This is for maps and arrays.

       uLevelType has value QCBOR_TYPE_BYTE_STRING for 1) and
       QCBOR_TYPE_MAP or QCBOR_TYPE_ARRAY or QCBOR_TYPE_MAP_AS_ARRAY
       for 2).

       Item tracking is either for definite or indefinite-length
       maps/arrays. For definite lengths, the total count and items
       unconsumed are tracked. For indefinite-length, uTotalCount is
       QCBOR_COUNT_INDICATES_INDEFINITE_LENGTH (UINT16_MAX) and there
       is no per-item count of members. For indefinite-length maps and
       arrays, uCountCursor is UINT16_MAX if not consumed and zero if
       it is consumed in the pre-order traversal. Additionally, if
       entered in bounded mode, uCountCursor is
       QCBOR_COUNT_INDICATES_ZERO_LENGTH to indicate it is empty.

       This also records whether a level is bounded or not.  All
       byte-count tracked levels (the top-level sequence and
       bstr-wrapped CBOR) are bounded. Maps and arrays may or may not
       be bounded. They are bounded if they were Entered() and not if
       they were traversed with GetNext(). They are marked as bounded
       by uStartOffset not being UINT32_MAX.
       */
      /*
       If uLevelType can put in a separately indexed array, the union/
       struct will be 8 bytes rather than 9 and a lot of wasted
       padding for alignment will be saved.
       */
      uint8_t  uLevelType;
      union {
         struct {
#define QCBOR_COUNT_INDICATES_INDEFINITE_LENGTH UINT16_MAX
#define QCBOR_COUNT_INDICATES_ZERO_LENGTH UINT16_MAX-1
            uint16_t uCountTotal;
            uint16_t uCountCursor;
#define QCBOR_NON_BOUNDED_OFFSET UINT32_MAX
            uint32_t uStartOffset;
         } ma; /* for maps and arrays */
         struct {
            /* The end of the input before the bstr was entered so that
             * it can be restored when the bstr is exited. */
            uint32_t uSavedEndOffset;
            /* The beginning of the bstr so that it can be rewound. */
            uint32_t uBstrStartOffset;
         } bs; /* for top-level sequence and bstr-wrapped CBOR */
      } u;
   } pLevels[QCBOR_MAX_ARRAY_NESTING1+1],
    *pCurrent,
    *pCurrentBounded;
   /*
    pCurrent is for item-by-item pre-order traversal.

    pCurrentBounded points to the current bounding level or is NULL if
    there isn't one.

    pCurrent must always be below pCurrentBounded as the pre-order
    traversal is always bounded by the bounding level.

    When a bounded level is entered, the pre-order traversal is set to
    the first item in the bounded level. When a bounded level is
    exited, the pre-order traversl is set to the next item after the
    map, array or bstr. This may be more than one level up, or even
    the end of the input CBOR.
    */
} QCBORDecodeNesting;


typedef struct  {
   // PRIVATE DATA STRUCTURE
   void *pAllocateCxt;
   UsefulBuf (* pfAllocator)(void *pAllocateCxt, void *pOldMem, size_t uNewSize);
} QCBORInternalAllocator;


/*
 PRIVATE DATA STRUCTURE

 The decode context. This data structure plus the public QCBORDecode_xxx
 functions form an "object" that does CBOR decoding.

 Size approximation (varies with CPU/compiler):
   64-bit machine: 32 + 1 + 1 + 6 bytes padding + 72 + 16 + 8 + 8 = 144 bytes
   32-bit machine: 16 + 1 + 1 + 2 bytes padding + 68 +  8 + 8 + 4 = 108 bytes
 */
struct _QCBORDecodeContext {
   // PRIVATE DATA STRUCTURE
   UsefulInputBuf InBuf;


   QCBORDecodeNesting nesting;


   // If a string allocator is configured for indefinite-length
   // strings, it is configured here.
   QCBORInternalAllocator StringAllocator;

   // These are special for the internal MemPool allocator.
   // They are not used otherwise. We tried packing these
   // in the MemPool itself, but there are issues
   // with memory alignment.
   uint32_t uMemPoolSize;
   uint32_t uMemPoolFreeOffset;

   // A cached offset to the end of the current map
   // 0 if no value is cached.
#define QCBOR_MAP_OFFSET_CACHE_INVALID UINT32_MAX
   uint32_t uMapEndOffsetCache;

   uint8_t  uDecodeMode;
   uint8_t  bStringAllocateAll;
   uint8_t  uLastError;  // QCBORError stuffed into a uint8_t

   /* See MapTagNumber() for description of how tags are mapped. */
   uint64_t auMappedTags[QCBOR_NUM_MAPPED_TAGS];

   uint16_t uLastTags[QCBOR_MAX_TAGS_PER_ITEM1];

};

// Used internally in the impementation here
// Must not conflict with any of the official CBOR types
#define CBOR_MAJOR_NONE_TYPE_RAW  9
#define CBOR_MAJOR_NONE_TAG_LABEL_REORDER 10
#define CBOR_MAJOR_NONE_TYPE_BSTR_LEN_ONLY 11

// Add this to types to indicate they are to be encoded as indefinite lengths
#define QCBOR_INDEFINITE_LEN_TYPE_MODIFIER 0x80
#define CBOR_MAJOR_NONE_TYPE_ARRAY_INDEFINITE_LEN \
            CBOR_MAJOR_TYPE_ARRAY + QCBOR_INDEFINITE_LEN_TYPE_MODIFIER
#define CBOR_MAJOR_NONE_TYPE_MAP_INDEFINITE_LEN \
            CBOR_MAJOR_TYPE_MAP + QCBOR_INDEFINITE_LEN_TYPE_MODIFIER
#define CBOR_MAJOR_NONE_TYPE_SIMPLE_BREAK \
            CBOR_MAJOR_TYPE_SIMPLE + QCBOR_INDEFINITE_LEN_TYPE_MODIFIER


/* Value of QCBORItem.val.string.len when the string length is
 * indefinite. Used temporarily in the implementation and never
 * returned in the public interface.
 */
#define QCBOR_STRING_LENGTH_INDEFINITE SIZE_MAX


/* The number of elements in a C array of a particular type */
#define C_ARRAY_COUNT(array, type) (sizeof(array)/sizeof(type))


#ifdef __cplusplus
}
#endif

#endif /* qcbor_private_h */
