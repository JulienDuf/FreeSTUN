#include <FreeSTUN/FreeStun.h>
#include <FreeSTUN/FreeStunRequest.h>
#include <FreeSTUN/FreeStunSocket.h>
#include <SDL2/SDL_thread.h>
#include <signal.h>

/*int thread_function(void* data) {

    IPaddress IP;
    SDLNet_ResolveHost(&IP, NULL, 53401);
    TCPsocket server = SDLNet_TCP_Open(&IP);

    SDLNet_SocketSet set = SDLNet_AllocSocketSet(2);
    SDLNet_TCP_AddSocket(set, server);

    while (1) {

        int numActiveSockets = SDLNet_CheckSockets(set, 1);
        int serverSocketActivity = SDLNet_SocketReady(server);

        if (serverSocketActivity != 0) {
            continue;
        }
    }

    return 0;
}*/

int init(void) {

    void (*handler)(int);
    handler = signal(SIGPIPE, SIG_IGN);

    if (handler != SIG_DFL)
        signal(SIGPIPE, handler);

    return 0;
}

uint16_t sendAndGetInfo(uint32_t* ip, uint16_t* port) {

    FreeStunIPAddress iPAddress;
    int status = FreeStun_ResolveHost(&iPAddress, "159.203.63.242", 21225);

    FreeStunSocketSet *set = FreeStunSocket_AllocSocketSet(2);
    FreeStunSocket *socket = FreeStunSocket_Open(&iPAddress);
    uint16_t ret = FreeStunSocket_GetLocalAddress(socket)->port;
    FreeStunSocket_AddSocket(set, socket);

    int gotMessage;
    char* buffer = (char*)malloc(STUN_MAX_SIZE);

    while (1) {

        status = FreeStunSocket_CheckSockets(set, 10);

        if (status != 0) {

            gotMessage = FreeStunSocket_SocketReady(socket);

            if (gotMessage != 0) {

                int serverResponseByteCount = FreeStunSocket_Recv(socket, buffer, STUN_MAX_SIZE - 1);
                break;
            }
        }
    }

    FreeStunBuffer *buf = FreeStunBuffer_New(NULL, 512);
    FreeStunBuffer_WriteUint32(buf, *ip);
    FreeStunBuffer_WriteUint16(buf, *port);
    FreeStunBuffer_SealLength(buf);
    FreeStunSocket_Send(socket, FreeStunBuffer_GetBuffer(buf), FreeStunBuffer_GetLength(buf));

    FreeStunBuffer *recvBuffer;
    uint32_t myIP = *ip;

    while (1) {

        status = FreeStunSocket_CheckSockets(set, 10);

        if (status != 0) {

            gotMessage = FreeStunSocket_SocketReady(socket);

            if (gotMessage != 0) {

                int serverResponseByteCount = FreeStunSocket_Recv(socket, buffer, 512 - 1);

                if (serverResponseByteCount != 0) {
                    recvBuffer = FreeStunBuffer_New(buffer, 512);

                    FreeStunBuffer_ReadUint32(recvBuffer, ip);
                    FreeStunBuffer_ReadUint16(recvBuffer, port);

                    if (myIP != *ip)
                        break;
                }
            }
        }
    }

    // TODO: Deal wih the new IP

    FreeStunBuffer_Free(buf);
    FreeStunSocket_DelSocket(set, socket);
    FreeStunSocket_FreeSocketSet(set);
    FreeStunSocket_Close(socket);

    return ret;
 }

const char* getIPFromDecimal(uint32_t ip) {

    int a = (ip & 0xFF000000) >> 24;
    int b = (ip & 0x00FF0000) >> 16;
    int c = (ip & 0x0000FF00) >> 8;
    int d = (ip & 0x000000FF);

    char* ret = (char*) malloc(20);

    sprintf(ret, "%d.%d.%d.%d", a,b,c,d);

    return ret;
}

int main(int argc, char** argv) {

    srand(time(NULL));
    init();

    char host[100];

    int status;

    FreeStunIPAddress iPAddress;
    status = FreeStun_ResolveHost(&iPAddress, "104.236.69.2", STUN_PORT);

    FreeStunSocket *socket = FreeStunSocket_Open(&iPAddress);
    FreeStunSocketSet *set = FreeStunSocket_AllocSocketSet(2);
    FreeStunSocket_AddSocket(set, socket);

    status = FreeStunSocket_CheckSockets(set, 10);
    int gotMessage;
    char* buffer = (char*)malloc(STUN_MAX_SIZE);

    if (status != 0) {

        gotMessage = FreeStunSocket_SocketReady(socket);

        if (gotMessage != 0) {

            int serverResponseByteCount = FreeStunSocket_Recv(socket, buffer, STUN_MAX_SIZE - 1);

        }
    }

    FreeStunRequest* request = FreeStunRequest_New();
    FreeStunRequest_Encode(request, STUN_METHOD_BINDING, STUN_CLASS_REQUEST);

    status = FreeStunSocket_Send(socket, FreeStunRequest_GetData(request), FreeStunRequest_GetLength(request));

    while (1) {

        status = FreeStunSocket_CheckSockets(set, 10);

        if (status != 0) {

            gotMessage = FreeStunSocket_SocketReady(socket);

            if (gotMessage != 0) {

                int serverResponseByteCount = FreeStunSocket_Recv(socket, buffer, STUN_MAX_SIZE - 1);

                if (serverResponseByteCount != 0) {

                    FreeStunRequest_Decode(request, buffer);
                    break;
                }
            }
        }
    }

    uint32_t ip = request->attributes[2]->attribute.xorMappedAddress.address;
    uint16_t port = request->attributes[2]->attribute.xorMappedAddress.port;

    uint16_t localPort = sendAndGetInfo(&ip, &port);


    //SDL_Thread *thread = SDL_CreateThread(thread_function, "Thread", NULL);

    FreeStunIPAddress IP;
    FreeStun_ResolveHost(&IP, getIPFromDecimal(ip), port);
    getIPFromDecimal(IP.host);
    FreeStunSocket *server = FreeStunSocket_Open(&IP);

    FreeStunSocketSet* set2 = FreeStunSocket_AllocSocketSet(2);
    FreeStunSocket_AddSocket(set2, server);

    while (!server) {

        server = FreeStunSocket_Open(&IP);
    }

    return 0;
}

