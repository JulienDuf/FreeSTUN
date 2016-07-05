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

#include <FreeSTUN/FreeStunSocket.h>

struct free_stun_socket {

    SOCKET socket;
    int ready;
    int flag;

    FreeStunIPAddress remoteAddress;
    FreeStunIPAddress localAddress;
};


int FreeStun_ResolveHost(FreeStunIPAddress *address, const char *host, uint16_t port) {

        int status = 0;

        if (host == NULL)
                address->host = INADDR_ANY;

        else {
                address->host = inet_addr(host);
                if (address->host == INADDR_NONE) {
                        struct hostent *hp;

                        hp = gethostbyname(host);
                        if (hp)
                                memcpy(&address->host, hp->h_addr, hp->h_length);

                        else
                                status = -1;

                }
        }
        address->port = htons(port);

        return status;
}

FreeStunSocket* FreeStunSocket_OpenWithLocalPort(FreeStunIPAddress* IP, uint16_t port) {

        FreeStunSocket *sock;
        struct sockaddr_in sock_addr;
        struct sockaddr_in local;

        sock = (FreeStunSocket*) malloc(sizeof(sock));
        if (!sock)
                goto error_return;

        sock->socket = socket(AF_INET, SOCK_STREAM, 0);
        if (sock->socket == INVALID_SOCKET)
                goto error_return;

        if ((IP->host != INADDR_NONE) && (IP->host != INADDR_ANY)) {

                int yes = 1;
                setsockopt(sock->socket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));

                yes = 1;
                setsockopt(sock->socket, SOL_SOCKET, SO_REUSEPORT, (char*)&yes, sizeof(yes));

                if (port) {

                        memset(&local, 0, sizeof(local));
                        local.sin_family = AF_INET;
                        local.sin_addr.s_addr = INADDR_ANY;
                        local.sin_port = port;

                        if (bind(sock->socket, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR)
                                goto error_return;
                }

                memset(&sock_addr, 0, sizeof(sock_addr));
                sock_addr.sin_family = AF_INET;
                sock_addr.sin_addr.s_addr = IP->host;
                sock_addr.sin_port = IP->port;

                if (connect(sock->socket, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR)
                        goto error_return;

                sock->flag = 0;
        }

        else {

                memset(&sock_addr, 0, sizeof(sock_addr));
                sock_addr.sin_family = AF_INET;
                sock_addr.sin_addr.s_addr = INADDR_ANY;
                sock_addr.sin_port = IP->port;


                int yes = 1;
                setsockopt(sock->socket, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes));

                if (bind(sock->socket, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR )
                        goto error_return;

                if (listen(sock->socket, 5) == SOCKET_ERROR)
                        goto error_return;

                fcntl(sock->socket, F_SETFL, O_NONBLOCK);

                sock->flag = 1;

        }
        sock->ready = 0;

        int yes = 1;
        setsockopt(sock->socket, IPPROTO_TCP, TCP_NODELAY, (char*)&yes, sizeof(yes));

        sock->remoteAddress.host = sock_addr.sin_addr.s_addr;
        sock->remoteAddress.port = sock_addr.sin_port;

        struct sockaddr_in sin;
        int addrlen = sizeof(sin);
        if (!getsockname(sock->socket, (struct sockaddr*)&sin, &addrlen)) {

                sock->localAddress.host = sin.sin_addr.s_addr;
                sock->localAddress.port = sin.sin_port;
        }

        return sock;
        int u;
        error_return:

        u = errno;

        FreeStunSocket_Close(sock);
        return NULL;
}

FreeStunSocket* FreeStunSocket_Open(FreeStunIPAddress* IP) {

        return FreeStunSocket_OpenWithLocalPort(IP, NULL);
}

void FreeStunSocket_Close(FreeStunSocket* socket) {

        if (socket) {

                if (socket->socket != INVALID_SOCKET)
                        close(socket->socket);

                free(socket);
        }
}

FreeStunSocket* FreeStunSocket_Accept(FreeStunSocket* server) {

        FreeStunSocket* sock;
        struct sockaddr_in sock_addr;
        socklen_t sock_alen;

        if (!server->flag)
                return NULL;

        server->ready = 0;

        sock = (FreeStunSocket*) malloc(sizeof(FreeStunSocket));
        if (sock == NULL)
                goto error_return;

        sock_alen = sizeof(sock_addr);
        sock->socket = accept(server->socket, (struct sockaddr *)&sock_addr, &sock_alen);
        if (sock->socket == INVALID_SOCKET)
                goto error_return;


        int flags = fcntl(sock->socket, F_GETFL, 0);
        fcntl(sock->socket, F_SETFL, flags & ~O_NONBLOCK);

        sock->remoteAddress.host = sock_addr.sin_addr.s_addr;
        sock->remoteAddress.port = sock_addr.sin_port;

        sock->flag = 0;
        sock->ready = 0;

        return sock;

        error_return:
        FreeStunSocket_Close(sock);
        return NULL;
}

FreeStunIPAddress *FreeStunSocket_GetPeerAddress(FreeStunSocket* socket) {

        if (socket->flag)
                return NULL;

        return(&socket->remoteAddress);
}

FreeStunIPAddress *FreeStunSocket_GetLocalAddress(FreeStunSocket* socket) {

        if (socket->flag)
                return NULL;

        return(&socket->localAddress);
}

int FreeStunSocket_Send(FreeStunSocket* socket, const void *datap, int len) {

        const u_int8_t *data = (const u_int8_t *)datap;
        int sent, left;

        if (socket->flag)
                return -1;

        left = len;
        sent = 0;
        FreeStun_SetLastError(0);

        do {
                len = send(socket->socket, (const char *) data, left, 0);
                if (len > 0) {
                        sent += len;
                        left -= len;
                        data += len;
                }

        } while ((left > 0) && ((len > 0) || (FreeStun_GetLastError() == EINTR)));

        return sent;
}

int FreeStunSocket_Recv(FreeStunSocket* socket, void *data, int maxLen) {

        int len;

        if (socket->flag)
                return -1;

        FreeStun_SetLastError(0);
        do {

                len = recv(socket->socket, (char *) data, maxLen, 0);
        } while (FreeStun_GetLastError() == EINTR);

        socket->ready = 0;

        return len;
}

/* Socket set */

struct free_stun_socket_set {

    int numsockets;
    int maxsockets;
    FreeStunSocket **sockets;
};

FreeStunSocketSet* FreeStunSocket_AllocSocketSet(int maxSockets) {

        FreeStunSocketSet *set;
        int i;

        set = (FreeStunSocketSet*) malloc(sizeof(FreeStunSocketSet));

        if (set) {

                set->numsockets = 0;
                set->maxsockets = maxSockets;
                set->sockets = (FreeStunSocket**) calloc(maxSockets, 8);

                if (set->sockets) {
                        for (i = 0; i < maxSockets; ++i)
                                set->sockets[i] = NULL;
                }

                else {

                        free(set);
                        set = NULL;
                }
        }

        return set;
}

int FreeStunSocket_AddSocket(FreeStunSocketSet* set, FreeStunSocket* socket) {

        if (socket) {
                if (set->numsockets == set->maxsockets)
                        return -1;

                set->sockets[set->numsockets++] = socket;
        }
        return set->numsockets;
}

int FreeStunSocket_DelSocket(FreeStunSocketSet* set, FreeStunSocket* socket) {

        int i;

        if (socket) {

                for (i = 0; i < set->numsockets; ++i) {

                        if (set->sockets[i] == socket)
                                break;
                }

                if (i == set->numsockets)
                        return -1;

                --set->numsockets;
                for (; i < set->numsockets; ++i )
                        set->sockets[i] = set->sockets[i + 1];

        }
        return set->numsockets;
}

int FreeStunSocket_CheckSockets(FreeStunSocketSet* set, uint32_t timeout) {

        int i;
        SOCKET maxfd;
        int retval;
        struct timeval tv;
        fd_set mask;

        maxfd = 0;
        for ( i = set->numsockets-1; i >= 0; --i) {
                if (set->sockets[i]->socket > maxfd)
                        maxfd = set->sockets[i]->socket;
        }

        do {
                FreeStun_SetLastError(0);

                FD_ZERO(&mask);
                for (i = set->numsockets-1; i >= 0; --i)
                        FD_SET(set->sockets[i]->socket, &mask);


                /* Set up the timeout */
                tv.tv_sec = timeout / 1000;
                tv.tv_usec = (timeout % 1000) * 1000;

                /* Look! */
                retval = select(maxfd+1, &mask, NULL, NULL, &tv);
        } while (FreeStun_GetLastError() == EINTR);

        if (retval > 0) {

                for (i = set->numsockets-1; i >= 0; --i) {
                        if (FD_ISSET(set->sockets[i]->socket, &mask))
                                set->sockets[i]->ready = 1;
                }
        }
        return retval;
}

void FreeStunSocket_FreeSocketSet(FreeStunSocketSet* set) {

        if (set) {

                free(set->sockets);
                free(set);
        }
}

int FreeStunSocket_SocketReady(FreeStunSocket *socket) {

        return (socket != NULL) && (socket->ready);
}