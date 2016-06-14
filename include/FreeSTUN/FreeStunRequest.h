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

#ifndef PROJECT_FREESTUNREQUEST_H
#define PROJECT_FREESTUNREQUEST_H

#include <FreeSTUN/FreeStunBuffer.h>
#include <FreeSTUN/FreeStun.h>

typedef struct free_stun_request {

    FreeStunBuffer* buffer;
    StunHeader header;

    StunAttribute **attributes;
    int attributesCount;

} FreeStunRequest;

FreeStunRequest* FreeStunRequest_New();
void FreeStunRequest_Free(FreeStunRequest* request);

void FreeStunRequest_Encode(FreeStunRequest *request, uint16_t method, uint8_t class);
void FreeStunRequest_Decode(FreeStunRequest* request, char* data);

void* FreeStunRequest_GetData(FreeStunRequest* request);
size_t FreeStunRequest_GetLength(FreeStunRequest* request);

#endif //PROJECT_FREESTUNREQUEST_H
