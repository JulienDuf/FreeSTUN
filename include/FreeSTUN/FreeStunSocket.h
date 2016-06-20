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

typedef struct free_stun_socket_set FreeStunSocketSet;

typedef struct free_stun_IPAddress {

    uint32_t host;
    uint16_t port;

} FreeStunIPAddress;

FreeStunSocket* FreeStunSocket_OpenWithLocalPort(FreeStunIPAddress* IP, uint16_t port);
FreeStunSocket* FreeStunSocket_Open(FreeStunIPAddress* IP);
void FreeStunSocket_Close(FreeStunSocket* socket);

FreeStunSocket* FreeStunSocket_Accept(FreeStunSocket* server);

FreeStunIPAddress *FreeStunSocket_GetPeerAddress(FreeStunSocket* socket);
FreeStunIPAddress *FreeStunSocket_GetLocalAddress(FreeStunSocket* socket);

int FreeStunSocket_Send(FreeStunSocket* socket, const void *datap, int len);
int FreeStunSocket_Recv(FreeStunSocket* socket, void *data, int maxLen);

int FreeStun_ResolveHost(FreeStunIPAddress *address, const char *host, uint16_t port);

FreeStunSocketSet* FreeStunSocket_AllocSocketSet(int maxSockets);

int FreeStunSocket_AddSocket(FreeStunSocketSet* set, FreeStunSocket* socket);
int FreeStunSocket_DelSocket(FreeStunSocketSet* set, FreeStunSocket* socket);
int FreeStunSocket_CheckSockets(FreeStunSocketSet* set, uint32_t timeout);
int FreeStunSocket_SocketReady(FreeStunSocket *socket);

void FreeStunSocket_FreeSocketSet(FreeStunSocketSet* set);

#endif //PROJECT_FREESTUNSOCKET_H
