#include <FreeSTUN/FreeStun.h>
#include <FreeSTUN/FreeStunRequest.h>
#include <FreeSTUN/FreeStunSocket.h>

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

    char host[100];

    int status;
    uint16_t localPort;

    FreeStunIPAddress iPAddress;
    status = FreeStun_ResolveHost(&iPAddress, "stun.stunprotocol.org", STUN_PORT);

    FreeStunSocket *socket = FreeStunSocket_Open(&iPAddress);
    FreeStunSocketSet *set = FreeStunSocket_AllocSocketSet(2);
    FreeStunSocket_AddSocket(set, socket);

    int gotMessage;
    char* buffer = (char*)malloc(STUN_MAX_SIZE);

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

    uint32_t ip = request->attributes[0]->attribute.xorMappedAddress.address;
    uint16_t port = request->attributes[0]->attribute.xorMappedAddress.port;

    printf("Your IP is %s:%d", getIPFromDecimal(ip), port);

    return 0;
}