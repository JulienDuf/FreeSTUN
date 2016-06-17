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

#include <FreeSTUN/FreeStun.h>

int MappedAddress_Decode(MappedAddress* mappedAddress, FreeStunBuffer* buffer) {

    uint16_t type;
    FreeStunBuffer_ReadUint16(buffer, &type);

    if (type == 0x1) {
        FreeStunBuffer_ReadUint16(buffer, &mappedAddress->port);
        FreeStunBuffer_ReadUint32(buffer, &mappedAddress->address);
    }

    return 0;
}

int UserName_Decode(UserName *userName, size_t length, FreeStunBuffer* buffer) {

    userName->userName = (char*) calloc(length, sizeof(char));
    FreeStunBuffer_Read(buffer, userName->userName, length);

    return 0;
}

int MessageIntegrity_Decode(MessageIntegrity* message, size_t length, FreeStunBuffer* buffer) {

    if (length > 20)
        return -1;

    FreeStunBuffer_Read(buffer, message->integrity, 20);

    return 0;
}

int ErrorCode_Decode(ErrorCode* errorCode, size_t length, FreeStunBuffer* buffer) {

    uint32_t header = 0;
    FreeStunBuffer_ReadUint32(buffer, &header);

    uint8_t class = (uint8_t)((header & 0x00000700) >> 6);
    uint8_t number = (uint8_t)(header & 0x000000FF);

    errorCode->code = (uint16_t)((class * 100) + number);

    FreeStunBuffer_Read(buffer, errorCode->error, length);

    return 0;
}

int UnknownAttribute_Decode(UnknownAttribute* attribute, size_t length, FreeStunBuffer* buffer) {

    int i = 0;
    for (; i < length / 2; ++i) {

        if (attribute->counter >= 8)
            break;

        FreeStunBuffer_ReadUint16(buffer, &attribute->unknownTypes[i]);
        ++attribute->counter;
    }

    return 0;
}

int Realm_Decode(Realm *realm, size_t length, FreeStunBuffer* buffer) {

    if (length > 763)
        return -1;

    realm->realm = (char*) calloc(length, sizeof(char));
    FreeStunBuffer_Read(buffer, realm->realm, length);

    return 0;
}

int Nonce_Decode(Nonce *nonce, size_t length, FreeStunBuffer* buffer) {

    if (length > 763)
        return -1;

    nonce->nonce = (char*) calloc(length, sizeof(char));
    FreeStunBuffer_Read(buffer, nonce->nonce, length);

    return 0;
}

int XorMappedAddress_Decode(XorMappedAddress* mappedAddress, FreeStunBuffer* buffer) {

    uint16_t type;
    FreeStunBuffer_ReadUint16(buffer, &type);

    if (type == 0x1) {
        FreeStunBuffer_ReadUint16(buffer, &mappedAddress->port);
        mappedAddress->port ^= ((STUN_MAGIC_COOKIE & 0xFFFF0000) >> 16);

        FreeStunBuffer_ReadUint32(buffer, &mappedAddress->address);
        mappedAddress->address ^= STUN_MAGIC_COOKIE;
    }

    return 0;
}

int Software_Decode(Software *software, size_t length, FreeStunBuffer* buffer) {

    if (length > 763)
        return -1;

    software->software = (char*) calloc(length, sizeof(char));
    FreeStunBuffer_Read(buffer, software->software, length);

    return 0;
}

int AlternativeServer_Decode(AlternativeServer* server, FreeStunBuffer* buffer) {

    FreeStunBuffer_ReadUint16(buffer, &server->port);
    FreeStunBuffer_ReadUint32(buffer, &server->address);

    return 0;
}

int FingerPrint_Decode(FingerPrint* fingerPrint, FreeStunBuffer* buffer) {


    FreeStunBuffer_ReadUint32(buffer, &fingerPrint->fingerPrint);

    return 0;
}

int FreeStunAttribute_Decode(StunAttribute* attribute, FreeStunBuffer* buffer) {

    int status = 0;
    int padding = 0;

    FreeStunBuffer_ReadUint16(buffer, &attribute->type);
    FreeStunBuffer_ReadUint16(buffer, &attribute->length);

    padding = (attribute->length % 4) == 0 ? 0 : 4 - (attribute->length % 4);

    switch (attribute->type) {

        case STUN_ATTRIBUTE_MAPPED_ADDR:
            status = MappedAddress_Decode(&attribute->attribute.mappedAddress, buffer);
            break;

        case STUN_ATTRIBUTE_USERNAME:
            status = UserName_Decode(&attribute->attribute.userName, attribute->length, buffer);
            break;

        case STUN_ATTRIBUTE_MSG_INTEGRITY:
            status = MessageIntegrity_Decode(&attribute->attribute.messageIntegrity, attribute->length, buffer);
            break;

        case STUN_ATTRIBUTE_ERR_CODE:
            status = ErrorCode_Decode(&attribute->attribute.errorCode, attribute->length, buffer);
            break;

        case STUN_ATTRIBUTE_UNKNOWN_ATTR:
            status = UnknownAttribute_Decode(&attribute->attribute.unknownAttribute, attribute->length, buffer);
            break;

        case STUN_ATTRIBUTE_REALM:
            status = Realm_Decode(&attribute->attribute.realm, attribute->length, buffer);
            break;

        case STUN_ATTRIBUTE_NONCE:
            status = Nonce_Decode(&attribute->attribute.nonce, attribute->length, buffer);
            break;

        case STUN_ATTRIBUTE_XOR_MAPPED_ADDR:
            status = XorMappedAddress_Decode(&attribute->attribute.xorMappedAddress, buffer);
            break;

        case STUN_ATTRIBUTE_SOFTWARE:
            status = Software_Decode(&attribute->attribute.software, attribute->length, buffer);
            break;

        case STUN_ATTRIBUTE_ALT_SERVER:
            status = AlternativeServer_Decode(&attribute->attribute.alternativeServer, buffer);
            break;

        case STUN_ATTRIBUTE_FINGERPRINT:
            status = FingerPrint_Decode(&attribute->attribute.fingerPrint, buffer);
            break;

        default:
            FreeStunBuffer_Skip(buffer, attribute->length);
            break;
    }

    if (status >= 0)
        status = attribute->length + padding + STUN_ATTR_HEADER_SIZE;

    return status;
}

int FreeStunAttribute_Encode(StunAttribute* attribute, uint16_t type) {

    int status = 0;

    switch (type) {

        case STUN_ATTRIBUTE_MAPPED_ADDR:
            break;

        case STUN_ATTRIBUTE_USERNAME:
            break;

        case STUN_ATTRIBUTE_MSG_INTEGRITY:
            break;

        case STUN_ATTRIBUTE_ERR_CODE:
            break;

        case STUN_ATTRIBUTE_UNKNOWN_ATTR:
            break;

        case STUN_ATTRIBUTE_REALM:
            break;

        case STUN_ATTRIBUTE_NONCE:
            break;

        case STUN_ATTRIBUTE_XOR_MAPPED_ADDR:
            break;

        case STUN_ATTRIBUTE_SOFTWARE:
            break;

        case STUN_ATTRIBUTE_ALT_SERVER:
            break;

        case STUN_ATTRIBUTE_FINGERPRINT:
            break;

        default:
            break;
    }

    return status;
}

StunAttribute* FreeStunAttribute_New() {

    StunAttribute *attribute = (StunAttribute*) malloc(sizeof(StunAttribute));

    if (!attribute)
        return NULL;

    return attribute;
}

void FreeStunAttribute_Free(StunAttribute* attribute) {

    if (!attribute)
        return;

    free(attribute);
}