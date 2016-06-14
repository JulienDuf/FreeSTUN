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

#include <FreeSTUN/FreeStunBuffer.h>

struct free_stun_buffer {

    char* buffer;
    char* pointer;

    size_t capacity;
    size_t len;
};

FreeStunBuffer* FreeStunBuffer_New(char* buf, size_t size) {

    FreeStunBuffer* buffer = (FreeStunBuffer*) malloc(sizeof(FreeStunBuffer));

    if (!buffer)
        return NULL;

    if (!buf)
        buffer->buffer = (char*) malloc(size);

    else
        buffer->buffer = buf;

    buffer->pointer = buffer->buffer;

    buffer->capacity = size;
    buffer->len = size;

    return buffer;
}

void FreeStunBuffer_Free(FreeStunBuffer* buffer) {

    if (!buffer)
        return;

    if (buffer->buffer)
        free(buffer->buffer);

    free(buffer);

}

void FreeStunBuffer_Clear(FreeStunBuffer* buffer) {

    memset(buffer->buffer, 0, buffer->capacity);
    buffer->pointer = buffer->buffer;
}

void FreeStunBuffer_SealLength(FreeStunBuffer* buffer) {

    buffer->len = (buffer->pointer - buffer->buffer);
}

void FreeStunBuffer_SetPosition(FreeStunBuffer* buffer, int position) {

    buffer->pointer = (buffer->buffer + position);
}

void FreeStunBuffer_WriteUint8(FreeStunBuffer* buffer, uint8_t data) {

    *buffer->pointer++ = (uint8_t)(data);
}

void FreeStunBuffer_WriteUint16(FreeStunBuffer* buffer, uint16_t data) {

    *buffer->pointer++ = ((data) >> 8) & 0xFF;
	*buffer->pointer++ = (data) & 0xFF;
}

void FreeStunBuffer_WriteUint32(FreeStunBuffer* buffer, uint32_t data) {

    FreeStunBuffer_WriteUint16(buffer, ((data) >> 16 & 0xFFFF));
    FreeStunBuffer_WriteUint16(buffer, ((data) & 0xFFFF));
}

void FreeStunBuffer_ReadUint8(FreeStunBuffer* buffer, uint8_t* data) {

    *data = (uint8_t)(*buffer->pointer);
    buffer->pointer++;
}

void FreeStunBuffer_ReadUint16(FreeStunBuffer* buffer, uint16_t* data) {

    *data = (((uint16_t)(*buffer->pointer)) << 8) + (uint16_t)(*(buffer->pointer + 1));
	buffer->pointer += 2;
}

void FreeStunBuffer_ReadUint32(FreeStunBuffer* buffer, uint32_t* data) {

    *data = (((uint32_t)(*(buffer->pointer))) << 24) + (((uint32_t)(*(buffer->pointer + 1))) << 16) +
            (((uint32_t)(*(buffer->pointer + 2))) << 8) + (((uint32_t)(*(buffer->pointer + 3))));

    buffer->pointer += 4;
}

void FreeStunBuffer_Read(FreeStunBuffer* buffer, char* dest, size_t size) {

    memcpy(dest, buffer->pointer, size);
    buffer->pointer += size;
}

void FreeStunBuffer_Skip(FreeStunBuffer* buffer, size_t size) {

    buffer->pointer += size;
}

char* FreeStunBuffer_GetBuffer(FreeStunBuffer* buffer) {

    return buffer->buffer;
}

size_t FreeStunBuffer_GetLength(FreeStunBuffer* buffer) {

    return buffer->len;
}