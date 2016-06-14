#include <FreeSTUN/FreeStun.h>
#include <FreeSTUN/FreeStunRequest.h>
#include <SDL2/SDL_net.h>
#include <SDL_net.h>


int main(int argc, char** argv) {

    srand(time(NULL));
    SDLNet_Init();

    char host[100];

    int status;

    IPaddress iPAddress;
    status = SDLNet_ResolveHost(&iPAddress, "104.236.69.2", STUN_PORT);

    TCPsocket socket = SDLNet_TCP_Open(&iPAddress);
    SDLNet_SocketSet set = SDLNet_AllocSocketSet(2);
    SDLNet_TCP_AddSocket(set, socket);

    FreeStunRequest* request = FreeStunRequest_New();
    FreeStunRequest_Encode(request, STUN_METHOD_BINDING, STUN_CLASS_REQUEST);

    status = SDLNet_TCP_Send(socket, FreeStunRequest_GetData(request), FreeStunRequest_GetLength(request));

    int gotMessage;
    char* buffer = (char*)malloc(STUN_MAX_SIZE);
    while (1) {

        status = SDLNet_CheckSockets(set, 10);

        if (status != 0) {

            gotMessage = SDLNet_SocketReady(socket);

            if (gotMessage != 0) {

                int serverResponseByteCount = SDLNet_TCP_Recv(socket, buffer, STUN_MAX_SIZE - 1);

                if (serverResponseByteCount != 0) {

                    FreeStunRequest_Decode(request, buffer);
                    break;
                }
            }
        }
    }

    uint32_t address = request->attributes[0]->attribute.mappedAddress.address;

    uint32_t swapped = ((address >> 24) & 0xff) | ((address << 8) & 0xff0000) | ((address >> 8) & 0xff00) | ((address << 24) & 0xff000000);

    void* ptr = (void*)&swapped;

    inet_ntop(AF_INET, ptr, host, 100);
    printf("%s\n", host);

    address = request->attributes[2]->attribute.mappedAddress.address;

   swapped = ((address >> 24) & 0xff) | ((address << 8) & 0xff0000) | ((address >> 8) & 0xff00) | ((address << 24) & 0xff000000);

    ptr = (void*)&swapped;

    inet_ntop(AF_INET, ptr, host, 100);
    printf("%s\n", host);

    SDLNet_Quit();
    return 0;
}

