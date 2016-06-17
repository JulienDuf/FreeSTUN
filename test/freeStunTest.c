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

    FreeStunRequest* request = FreeStunRequest_New();
    FreeStunRequest_Encode(request, STUN_METHOD_BINDING, STUN_CLASS_REQUEST);

    status = FreeStunSocket_Send(socket, FreeStunRequest_GetData(request), FreeStunRequest_GetLength(request));

    int gotMessage;
    char* buffer = (char*)malloc(STUN_MAX_SIZE);
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

    //SDL_Thread *thread = SDL_CreateThread(thread_function, "Thread", NULL);

    FreeStunIPAddress IP;
    FreeStun_ResolveHost(&IP, "173.177.172.66", 53401);
    FreeStunSocket *server = FreeStunSocket_Open(&IP);

    FreeStunSocketSet* set2 = FreeStunSocket_AllocSocketSet(2);
    FreeStunSocket_AddSocket(set2, server);

    while (!server) {

        server = FreeStunSocket_Open(&IP);
    }

    return 0;
}

