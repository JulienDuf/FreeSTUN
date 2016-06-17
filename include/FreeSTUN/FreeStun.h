/**
 * FreeSTUN: A free STUN client Implementation
 * Copyright 2016 Julien Dufresne <dufresne_julien@hotmail.ca>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FREE_STUN_H
#define FREE_STUN_H

#include <FreeSTUN/FreeStunConfig.h>
#include <FreeSTUN/FreeStunBuffer.h>

typedef unsigned char BYTE;

#define STUN_TYPE(method, class)\
((method) & 0x0f80) << 2 |        \
((method) & 0x0070) << 1 |        \
((method) & 0x000f) << 0 |        \
((class) & 0x00002) << 7 |        \
((class) & 0x00001) << 4 |        \
((class) & 0x00000) << 12

#define STUN_CLASS(type) \
((type >> 7 | type >> 4) & 0x3)

#define STUN_METHOD(type) \
((type & 0x3E00) >> 2 | (type & 0x00E0) >> 1 | (type & 0x000f))

enum {
    STUN_MAGIC_COOKIE = 0x2112A442,
    STUN_MAGIC_COOKIE_2 = 0x2115A442
};

/** STUN Protocol values */
enum {
    STUN_PORT        = 3478,   /* STUN Port number */
    STUNS_PORT       = 5349,   /* STUNS Port number */
    STUN_MAX_SIZE    = 548,    /* STUN message max length */
    STUN_HEADER_SIZE = 20,     /* Number of bytes in header */
    STUN_ATTR_HEADER_SIZE = 4, /* Size of attribute header */
    STUN_TID_SIZE    = 12,     /* Number of bytes in transaction ID */
    STUN_DEFAULT_RTO = 500,    /* Default Retrans Timeout in [ms] */
    STUN_DEFAULT_RC  = 7,      /* Default number of retransmits */
    STUN_DEFAULT_RM  = 16,     /* Wait time after last request is sent */
    STUN_DEFAULT_TI  = 39500   /* Reliable timeout */
};

/** STUN Methods */
enum stun_method {
    STUN_METHOD_BINDING    = 0x001,
    STUN_METHOD_ALLOCATE   = 0x003,
    STUN_METHOD_REFRESH    = 0x004,
    STUN_METHOD_SEND       = 0x006,
    STUN_METHOD_DATA       = 0x007,
    STUN_METHOD_CREATEPERM = 0x008,
    STUN_METHOD_CHANBIND   = 0x009,
};

/** STUN Message class */
enum stun_msg_class {
    STUN_CLASS_REQUEST      = 0x0,  /* STUN Request */
    STUN_CLASS_INDICATION   = 0x1,  /* STUN Indication */
    STUN_CLASS_SUCCESS_RESP = 0x2,  /* STUN Success Response */
    STUN_CLASS_ERROR_RESP   = 0x3   /* STUN Error Response */
};

/** STUN Attributes */
enum stun_attributes {
    /* Comprehension-required range (0x0000-0x7FFF) */
    STUN_ATTRIBUTE_MAPPED_ADDR        = 0x0001,
    STUN_ATTRIBUTE_CHANGE_REQ         = 0x0003,
    STUN_ATTRIBUTE_USERNAME           = 0x0006,
    STUN_ATTRIBUTE_MSG_INTEGRITY      = 0x0008,
    STUN_ATTRIBUTE_ERR_CODE           = 0x0009,
    STUN_ATTRIBUTE_UNKNOWN_ATTR       = 0x000a,
    STUN_ATTRIBUTE_CHANNEL_NUMBER     = 0x000c,
    STUN_ATTRIBUTE_LIFETIME           = 0x000d,
    STUN_ATTRIBUTE_XOR_PEER_ADDR      = 0x0012,
    STUN_ATTRIBUTE_DATA               = 0x0013,
    STUN_ATTRIBUTE_REALM              = 0x0014,
    STUN_ATTRIBUTE_NONCE              = 0x0015,
    STUN_ATTRIBUTE_XOR_RELAY_ADDR     = 0x0016,
    STUN_ATTRIBUTE_REQ_ADDR_FAMILY    = 0x0017,
    STUN_ATTRIBUTE_EVEN_PORT          = 0x0018,
    STUN_ATTRIBUTE_REQ_TRANSPORT      = 0x0019,
    STUN_ATTRIBUTE_DONT_FRAGMENT      = 0x001a,
    STUN_ATTRIBUTE_XOR_MAPPED_ADDR    = 0x0020,
    STUN_ATTRIBUTE_RSV_TOKEN          = 0x0022,
    STUN_ATTRIBUTE_PRIORITY           = 0x0024,
    STUN_ATTRIBUTE_USE_CAND           = 0x0025,
    STUN_ATTRIBUTE_PADDING            = 0x0026,
    STUN_ATTRIBUTE_RESP_PORT          = 0x0027,

    /* Comprehension-optional range (0x8000-0xFFFF) */
    STUN_ATTRIBUTE_SOFTWARE           = 0x8022,
    STUN_ATTRIBUTE_ALT_SERVER         = 0x8023,
    STUN_ATTRIBUTE_FINGERPRINT        = 0x8028,
    STUN_ATTRIBUTE_CONTROLLED         = 0x8029,
    STUN_ATTRIBUTE_CONTROLLING        = 0x802A,
    STUN_ATTRIBUTE_RESP_ORIGIN        = 0x802B,
    STUN_ATTRIBUTE_OTHER_ADDR         = 0x802C,
} ;

/** STUN Header */
typedef struct stun_header {

    uint16_t type;               /** Message type   */
    uint16_t len;                /** Payload length */
    uint32_t cookie;
    uint8_t tid[STUN_TID_SIZE];  /** Transaction ID */

} StunHeader;

typedef struct mapped_address {

    uint16_t port;
    uint32_t address;

} MappedAddress;

typedef struct user_name {

    char* userName;
} UserName;

typedef struct message_integrity {

    char integrity[20];

} MessageIntegrity;

typedef struct error_code {

    uint16_t code;
    char* error;

} ErrorCode;

typedef struct unknown_attribute {

    uint16_t unknownTypes[8];
    uint32_t counter;

} UnknownAttribute;

typedef struct realm {

    char* realm;
} Realm;

typedef struct nonce {

    char* nonce;
} Nonce;

typedef struct xor_mapped_address {

    uint16_t port;
    uint32_t address;

} XorMappedAddress;

typedef struct software {

    char* software;
} Software;

typedef struct alternative_server {

    uint16_t port;
    uint32_t address;

} AlternativeServer;

typedef struct finger_print {

    uint32_t fingerPrint;
} FingerPrint;

typedef struct stun_attribute {

    uint16_t type;
    uint16_t length;

    union {

        MappedAddress mappedAddress;
        UserName userName;
        MessageIntegrity messageIntegrity;
        ErrorCode errorCode;
        UnknownAttribute unknownAttribute;
        Realm realm;
        Nonce nonce;
        XorMappedAddress xorMappedAddress;
        Software software;
        AlternativeServer alternativeServer;
        FingerPrint fingerPrint;
    } attribute;

} StunAttribute;

extern StunAttribute* FreeStunAttribute_New();

extern void FreeStunAttribute_Free(StunAttribute* attribute);

extern int FreeStunAttribute_Decode(StunAttribute* attribute, FreeStunBuffer* buffer);

extern int FreeStunAttribute_Encode(StunAttribute* attribute, uint16_t type);



#endif