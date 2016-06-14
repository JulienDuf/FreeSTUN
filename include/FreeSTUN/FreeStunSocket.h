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

#ifndef PROJECT_FREESTUNSOCKET_H
#define PROJECT_FREESTUNSOCKET_H

#include <FreeSTUN/FreeStunConfig.h>
#include <stdint.h>

typedef struct free_stun_socket FreeStunSocket;

typedef struct free_stun_IPAdress {

    uint32_t host;
    uint16_t port;

} FreeStunIPAdress;

FreeStunSocket* FreeStunSocket_New();
void FreeStunSocket_Free(FreeStunSocket* socket);

#endif //PROJECT_FREESTUNSOCKET_H
