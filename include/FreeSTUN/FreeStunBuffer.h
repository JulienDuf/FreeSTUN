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

#ifndef PROJECT_FREESTUNBUFFER_H
#define PROJECT_FREESTUNBUFFER_H

#include <FreeSTUN/FreeStunConfig.h>

typedef struct free_stun_buffer FreeStunBuffer;

FreeStunBuffer* FreeStunBuffer_New(unsigned char* buffer, size_t size);
void FreeStunBuffer_Free(FreeStunBuffer* buffer);

void FreeStunBuffer_Clear(FreeStunBuffer* buffer);
void FreeStunBuffer_SealLength(FreeStunBuffer* buffer);
void FreeStunBuffer_SetPosition(FreeStunBuffer* buffer, int position);

void FreeStunBuffer_WriteUint8(FreeStunBuffer* buffer, uint8_t data);
void FreeStunBuffer_WriteUint16(FreeStunBuffer* buffer, uint16_t data);
void FreeStunBuffer_WriteUint32(FreeStunBuffer* buffer, uint32_t data);

void FreeStunBuffer_ReadUint8(FreeStunBuffer* buffer, uint8_t* data);
void FreeStunBuffer_ReadUint16(FreeStunBuffer* buffer, uint16_t* data);
void FreeStunBuffer_ReadUint32(FreeStunBuffer* buffer, uint32_t* data);

void FreeStunBuffer_Read(FreeStunBuffer* buffer, char* dest, size_t size);
void FreeStunBuffer_Skip(FreeStunBuffer* buffer, size_t size);

unsigned char* FreeStunBuffer_GetBuffer(FreeStunBuffer* buffer);
size_t FreeStunBuffer_GetLength(FreeStunBuffer* buffer);

#endif //PROJECT_FREESTUNBUFFER_H
