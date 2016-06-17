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

#include <FreeSTUN/FreeStunRequest.h>

int FreeStunRequest_GenTransactionID(StunHeader *header) {

    int i = 0;
    for (; i < STUN_TID_SIZE; ++i)
    {
        header->tid[i] = (uint8_t)rand();
    }

    return 1;
}

FreeStunRequest* FreeStunRequest_New() {

    FreeStunRequest* request = (FreeStunRequest*) malloc(sizeof(FreeStunRequest));

    if (!request)
        return NULL;

    request->buffer = FreeStunBuffer_New(NULL, STUN_MAX_SIZE);
    request->attributes = (StunAttribute**) calloc(5, sizeof(StunAttribute));
    request->attributesCount = 5;

    return request;
}

void FreeStunRequest_Free(FreeStunRequest* request) {

    if (!request)
        return;

    if (request->buffer)
        FreeStunBuffer_Free(request->buffer);

    free(request);
}

void FreeStunRequest_Encode(FreeStunRequest *request, uint16_t method, uint8_t class) {

    if (!request)
        request = FreeStunRequest_New();

    /* Header Encoding */
    request->header.type = STUN_TYPE(method, class);
    request->header.len = 0;
    FreeStunRequest_GenTransactionID(&request->header);

    FreeStunBuffer_WriteUint16(request->buffer, request->header.type);
    FreeStunBuffer_WriteUint16(request->buffer, request->header.len);
    FreeStunBuffer_WriteUint32(request->buffer, STUN_MAGIC_COOKIE);

    int i = 0;
    for (; i < STUN_TID_SIZE; ++i)
    {
        FreeStunBuffer_WriteUint8(request->buffer, request->header.tid[i]);
    }

}

void FreeStunRequest_Decode(FreeStunRequest* request, char* data) {

    FreeStunBuffer_Free(request->buffer);
    request->buffer = FreeStunBuffer_New(data, strlen(data));

    FreeStunBuffer_ReadUint16(request->buffer, &request->header.type);
    FreeStunBuffer_ReadUint16(request->buffer, &request->header.len);
    FreeStunBuffer_ReadUint32(request->buffer, &request->header.cookie);

    uint8_t class = STUN_CLASS(request->header.type);
    uint16_t method = STUN_METHOD(request->header.type);

    int i = 0;
    for (; i < STUN_TID_SIZE; ++i)
    {
        FreeStunBuffer_ReadUint8(request->buffer, &request->header.tid[i]);
    }

    int len = request->header.len;
    int status;
    int count = 0;

    // TODO: Finish decoding STUN Message

    StunAttribute* attribute;

    while (len > 0) {

        attribute = FreeStunAttribute_New();

        status = FreeStunAttribute_Decode(attribute, request->buffer);

        if (status > 0)
            len -= status;

        if (count < request->attributesCount)
            request->attributes[count++] = attribute;
    }
}

void* FreeStunRequest_GetData(FreeStunRequest* request) {

    return FreeStunBuffer_GetBuffer(request->buffer);
}

size_t FreeStunRequest_GetLength(FreeStunRequest* request) {

    FreeStunBuffer_SealLength(request->buffer);
    return FreeStunBuffer_GetLength(request->buffer);
}
