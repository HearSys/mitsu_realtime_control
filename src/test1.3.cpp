#include <winsock.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>//Bibliothek fuer getch
#include <string.h>
#include <math.h>
#include "strdef.h"
#define NO_FLAGS_SET 0
#define MAXBUFLEN 512
#include<iostream>

using namespace std;//link with a library use syntax bsp. #pragma comment(lib, "ws2_32.lib","ws2_32.a")

INT main(VOID)
{
WSADATA Data;
SOCKADDR_IN destSockAddr;
SOCKET destSocket;
unsigned long destAddr;
int status;
int numsnt;
int numrcv;
char sendText[MAXBUFLEN];
char recvText[MAXBUFLEN];
char dst_ip_address[MAXBUFLEN];
unsigned short port;
char msg[MAXBUFLEN];
char buf[MAXBUFLEN];
char type, type_mon[4];
unsigned short IOSendType;// Send input/output signal data designation
unsigned short IORecvType;// Reply input/output signal data designation
unsigned short IOBitTop=0;
unsigned short IOBitMask=0xffff;
unsigned short IOBitData=0;
int debug;
//char debug[MAXBUFLEN];

cout << " Input connection destination IP address (192.168.0.1) ->";
cin.getline(dst_ip_address, MAXBUFLEN);

if(dst_ip_address[0]==0) strcpy(dst_ip_address, "192.168.0.1");

cout << " Input connection destination port No. (10000) -> (RC:10000,pc com1:10010 ip .0.2,usw)";
cin.getline(msg, MAXBUFLEN);

if(msg[0]!=0) port=atoi(msg);
    else port=10000;

cout << " Use input/output signal?([Y] / [N])-> ";
cin.getline(msg, MAXBUFLEN);

if(msg[0]!=0 && (msg[0]=='Y' || msg[0]=='y')){
    cout << "What is target? Input signal/output signal([I]nput / [O]utput)-> ";
    cin.getline(msg, MAXBUFLEN);

    switch(msg[0]) {
        case 'O':
            IOSendType = MXT_IO_OUT;// Set target to output signal
        case 'o':
            IORecvType = MXT_IO_OUT;
            break;
        case 'I':
            IOSendType = MXT_IO_NULL;// Set target to input signal
        case 'i':
            IORecvType = MXT_IO_IN;
        default:
            break;
        }
    cout << " Input head bit No. (0 to 32767)-> ";
    //Bit zahl noetig fuer wechseln des gesamte'message' zu integerzahl
    cin.getline(msg, MAXBUFLEN);

    if(msg[0]!=0)IOBitTop = atoi(msg);
        else IOBitTop = 0;

    if(IOSendType==MXT_IO_OUT) { // Only for output signal
        cout << "Input bit mask pattern for output as hexadecimal (0000 to FFFF)-> ";
        //message beschreibung in Hexa dezimal system
        //bitMask braucht man fuer transmission
        cin.getline(msg, MAXBUFLEN);

        if(msg[0]!=0) sscanf(msg,"%4x",&IOBitMask);
            else IOBitMask = 0;

        cout << "Input bit data for output as hexadecimal (0000 to FFFF)-> ";
        cin.getline(msg, MAXBUFLEN);
        //message beschreibung fuer datasend von output in hexadecimal sys
        if(msg[0]!=0) sscanf(msg,"%4x",&IOBitData);
            else IOBitData = 0;
        }
}

cout <<" --- Input the data type of command. --- \n";
cout <<"[0: None / 1: XYZ / 2:JOINT / 3: PULSE]\n";
cout <<" -- please input the number -- [0] - [3]->";
cin.getline(msg, MAXBUFLEN);
type = atoi(msg);

for(int k=0; k<4; k++) {
    sprintf (msg," --- input the datatype of monitor ( %d-th ) --- \n", k);
    cout << msg;
    cout << "[0: None]\n";
    cout << "[1: XYZ / 2:JOINT / 3: PULSE] Command value \n";
    cout << "[4: XYZ/ 5: JOINT/ 6: PULSE] Command value after the filter process \n";
    cout << "[7: XYZ/ 5:JOINT/ 6:PULSE] Feedback value. \n";
    cout << "[10: Electric current value / 11: Electric current feedback] ... Electric current value. \n";
    cout << "Input the numeral [0] to [11] -> ";
    cin.getline(msg, MAXBUFLEN);
    type_mon[k] = atoi(msg);
    }

sprintf(msg, "IP=%s / PORT=%d / SendType=%d / Monitor Type0/1/2/3=%d/%d/%d/%d"
    , dst_ip_address, port , type
    , type_mon[0], type_mon[1], type_mon[2], type_mon[3]);
cout << msg << endl;
cout << "[Enter]= End / [d]= Monitor data display";
cout << "[z/x]= Increment/decrement first command data transmitted by the delta amount. ";
cout << " Is it all right? [Enter] / [Ctrl+C] ";
cin.getline(msg, MAXBUFLEN);
// Windows Socket DLL initialization
status=WSAStartup(MAKEWORD(1, 1), &Data);//suchen windowsocket version 1.1 und return dien besten version was es kann

if (status != 0)
    cerr << "ERROR: WSAStartup unsuccessful" << endl;
// IP address, port, etc., setting
memset(&destSockAddr, 0, sizeof(destSockAddr));
destAddr=inet_addr(dst_ip_address);
    //inet_addr converts a string containing an IPv4 dotted-decimal address into a proper address for the IN_ADDR structure.
memcpy(&destSockAddr.sin_addr, &destAddr, sizeof(destAddr));
//koppiert adress destaddr auf system in address von destsockaddress
destSockAddr.sin_port=htons(port);
    //The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).
    //The htons function does not require that the Winsock DLL has previously been loaded with a successful
    //call to the WSAStartup function.
destSockAddr.sin_family=AF_INET;
// Socket creation
destSocket=socket(AF_INET, SOCK_DGRAM, 0);
    //AF_INET Address family internet fuer IPvN4, fuer IPvN6 ist AF_INET6
    //Artsocket SOCK_DGRAM: UDP Methode, für TCP Methode benutzt SOCK_STREAM,
    //0:protocol art auf socket (SOCK_DGRAM :protocol IPPROTO_UDP,SOCK_STREAM:IPPROTO_IP,SOCKRAW:IPPROTO_RAW oder IPPROTO_ICMP)

if (destSocket == INVALID_SOCKET) {//wenn "Socket accept" erfolgreich return socket damit kommunizeieren mit client,wenn nicht invalid socket
    cerr << "ERROR: socket unsuccessful" << endl;
    status=WSACleanup();
    if (status == SOCKET_ERROR)//wenn connect socket erfolgreich 0, nicht socket error
        cerr << "ERROR: WSACleanup unsuccessful" << endl;
    return(1);
    }

MXTCMD MXTsend;
MXTCMD MXTrecv;
JOINT jnt_now;
POSE pos_now;
PULSE pls_now;
unsigned long counter = 0;
int loop = 1;
unsigned int disp = 0;
int disp_data = 0;
int ch;
float delta=(float)0.0,delta1=(float)0.0, delta2=(float)0.0;

long ratio=1;
int retry;

fd_set SockSet;// Socket group used with select
timeval sTimeOut;// For timeout setting with seconds or microsecoonds
memset(&MXTsend, 0, sizeof(MXTsend));
memset(&jnt_now, 0, sizeof(JOINT));
memset(&pos_now, 0, sizeof(POSE));
memset(&pls_now, 0, sizeof(PULSE));

while(loop) {
    memset(&MXTsend, 0, sizeof(MXTsend));
    memset(&MXTrecv, 0, sizeof(MXTrecv));
    // Transmission data creation
    if(loop==1) {// Only first time
        MXTsend.Command = MXT_CMD_NULL;
        MXTsend.SendType = MXT_TYP_NULL;
        MXTsend.RecvType = type;
        MXTsend.SendIOType = MXT_IO_NULL;
        MXTsend.RecvIOType = IOSendType;
        MXTsend.CCount = counter = 0;
        }
    else {// Second and following times
        MXTsend.Command = MXT_CMD_MOVE;
        MXTsend.SendType = type;
        MXTsend.RecvType = type_mon[0];
        MXTsend.RecvType1= type_mon[1];
        MXTsend.RecvType2= type_mon[2];
        MXTsend.RecvType3= type_mon[3];

        switch(type) {
            case MXT_TYP_JOINT:
                memcpy(&MXTsend.dat.jnt, &jnt_now, sizeof(JOINT));
                MXTsend.dat.jnt.j1 += (float)(delta*ratio*3.141592/180.0);
                break;

            case MXT_TYP_POSE:
                memcpy(&MXTsend.dat.pos, &pos_now, sizeof(POSE));
                MXTsend.dat.pos.w.x += (delta*ratio);
                MXTsend.dat.pos.w.y += (delta1*ratio);
                MXTsend.dat.pos.w.z += (delta2*ratio);
                break;

            case MXT_TYP_PULSE:
                memcpy(&MXTsend.dat.pls, &pls_now, sizeof(PULSE));
                MXTsend.dat.pls.p1 += (long)((delta*ratio)*10);
                break;

            default:
                break;
            }

        MXTsend.SendIOType = IOSendType;
        MXTsend.RecvIOType = IORecvType;
        MXTsend.BitTop = IOBitTop;
        MXTsend.BitMask =IOBitMask;
        MXTsend.IoData = IOBitData;
        MXTsend.CCount = counter;
        }
    // Keyboard input
    // [Enter]=End / [d]= Display the monitor data, or none / [0/1/2/3]= Changeof monitor data display
    // [z/x]=Increment/decrement first command data transmitted by the delta amount
    // [c]=Zentrum 0 for jointspace or 0 for taskspace (EE im Ursprung) hi

//    ch = cin.get();
//
//
//    cout << "key pressed: " << ch << endl;
//    cout.flush();


    while(loop!=0){

    if(_kbhit())
    {
    ch=getch();
    cout << '\r'; // Goto beginning of line (carriage return)
    switch(ch) {
        case 27:
        //case 0x0d://ascii return taste
            MXTsend.Command = MXT_CMD_END;
            loop = 0;
            break;

        case 77: //pfleile nach rechts
            delta += (float)0.1;
            cout << "->";
            cout.flush();
            Beep(330,100);//Sleep(300);
            //cout << "x" << endl;
            break;

        case 75:   //pfeile nach links
            delta -= (float)0.1;
            cout << "<-";
            cout.flush();
            Beep(196,100);//
            Sleep(700);
            break;

        case 72:// pfeile nach oben
            delta1 +=(float)0.1;
            cout << "pfeile nach oben y+";
            cout.flush();
            Beep(246,100);//Sleep(300);
            break;
        case 80://pfeile nach unten
            delta1 -=(float)0.1;
            cout<<"pfeile nach unten y-";
            cout.flush();
            Beep(246,100);//Sleep(300);
            break;

        case 'A':
        case 'a':
            delta2 += (float)0.1;
            cout << "z+";
            cout.flush();
            Beep(164,300);//Sleep(300);
            break;

        case 'Z':
        case 'z':
            delta2 -= (float)0.1;
            cout << "z-";
            cout.flush();
            Beep(392,100);//Sleep(300);
            break;

        case 'C':
        case 'c':
            delta = (float)0.0;
            break;

        case 'd':
            disp =~disp;//wenn d gedrueckt wird disp=-1 da tilde bedeutet gegenwert von ursprung wert von disp
            cout<<disp;
            break;

        case '0': case '1': case '2': case '3'://change display data
            disp_data = ch - '0';
            break;
        default:
                    cout << "  ";
                    cout.flush();
    }
    }

    }
    cout << "out of while peek loop" << endl;



    cout << "sizeof(MXTsend) = " << sizeof(MXTsend) << endl;
    cout << "sizeof(sendText) = " << sizeof(sendText) << endl;



    memset(sendText, 0, MAXBUFLEN);
    memcpy(sendText, &MXTsend, sizeof(MXTsend));

    if(disp){//wenn true
        sprintf(buf, "Send (%d):",counter);
        cout << buf << endl;
        }

    numsnt=sendto(destSocket, sendText, sizeof(MXTCMD), NO_FLAGS_SET
        , (LPSOCKADDR) &destSockAddr, sizeof(destSockAddr));
        //no flags set for udp, tcp schon
        //LPSOCKADDR A pointer to a socket address represented as a SOCKADDR structure.
        //ssize_t sendto(int s, const void *buf, size_t len,
        //       int flags, const struct sockaddr *to,
        //       socklen_t tolen);
        //s [in]
        //A descriptor identifying a (possibly connected) socket.
        //buf [in]
        //A pointer to a buffer containing the data to be transmitted.
        //len [in]
        //The length, in bytes, of the data pointed to by the buf parameter.
        //flags [in]
        //A set of flags that specify the way in which the call is made.
        //to [in]
        //An optional pointer to a sockaddr structure that contains the address of the target socket.
        //tolen [in]
        //The size, in bytes, of the address pointed to by the to parameter.

    cout << "sent " << numsnt << " bytes:" << sendText << endl;
    if (numsnt != sizeof(MXTCMD)) {
        cerr << "ERROR: sendto unsuccessful" << endl;
        status=closesocket(destSocket);

        if (status == SOCKET_ERROR)
            cerr << "ERROR: closesocket unsuccessful" << endl;

        status=WSACleanup();

        if (status == SOCKET_ERROR)
            cerr << "ERROR: WSACleanup unsuccessful" << endl;

        return(1);
    }

    memset(recvText, 0, MAXBUFLEN);
    retry = 1;// No. of reception retries

    while(retry) {
        FD_ZERO(&SockSet);// SockSet initialization
        FD_SET(destSocket, &SockSet);// Socket registration
        sTimeOut.tv_sec = 2;// Transmission timeout setting (sec) choose the time valuation seconds true;microseconds no with 0
        sTimeOut.tv_usec = 0;// (u sec)
        status = select(0, &SockSet, (fd_set *)NULL, (fd_set *)NULL, &sTimeOut);
            //int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
            //If the exceptfds argument is not NULL, it points to an object of type fd_set that on input specifies the file
            //descriptors to be checked for error conditions pending, and on output indicates which file descriptors have
            //error conditions pending
            //PARAMETERS
            //nfds
            //Specifies how many descriptors should be examined. The descriptors checked are 0 through nfds-1.

            //readfds
            //Points to a bit mask that specifies the file descriptors to check for reading.

            //writefds
            //Points to a bit mask that specifies the file descriptors to check for writing.

            //exceptfds
            //Points to a bit mask that specifies the file descriptors to check for exception conditions.

            //timeout
            //When non-NULL, contains the address of a struct timeval that specifies how long to wait for the required condition before returning to the caller. When NULL, forces the call to block until a descriptor becomes ready or until a signal occurs
            //FD_SET(int fd, fd_set *fdset); fd=file desriptor
            //FD_CLR(int fd, fd_set *fdset);
            //FD_ISSET(int fd, fd_set *fdset);
            //FD_ZERO(fd_set *fdset)

        if(status == SOCKET_ERROR) {
            return(1);
            }
        // If it receives by the time-out
        cout << "status=" << status << endl;

        if((status > 0) && (FD_ISSET(destSocket,&SockSet) != 0)) {
            numrcv=recvfrom(destSocket, recvText, MAXBUFLEN, NO_FLAGS_SET, NULL, NULL);//art der sending :0 normal
                //s [in]
                //A descriptor identifying a bound socket.
                //buf [out]
                //A buffer for the incoming data.
                //len [in]
                //The length, in bytes, of the buffer pointed to by the buf parameter.
                //flags [in]
                //A set of options that modify the behavior of the function call beyond the options specified for the associated socket. See the Remarks below for more details.
                //from [out]
                //An optional pointer to a buffer in a sockaddr structure that will hold the source address upon return.
                //fromlen [in, out, optional]
                //An optional pointer to the size, in bytes, of the buffer pointed to by the from parameter.
            if (numrcv == SOCKET_ERROR) {
                cerr<< "ERROR: recvfrom unsuccessful" << endl;
                status=closesocket(destSocket);

                if (status == SOCKET_ERROR)
                    cerr << "ERROR: closesocket unsuccessful" << endl;

                status=WSACleanup();

                if (status == SOCKET_ERROR)
                    cerr << "ERROR: WSACleanup unsuccessful" << endl;

                return(1);
                }

            memcpy(&MXTrecv, recvText, sizeof(MXTrecv));
            char str[10];

            if(MXTrecv.SendIOType==MXT_IO_IN)
                sprintf(str,"IN%04x", MXTrecv.IoData);
            else if(MXTrecv.SendIOType==MXT_IO_OUT)
                sprintf(str,"OT%04x", MXTrecv.IoData);
            else sprintf(str,"------");

            int DispType;
            void *DispData;

            switch(disp_data) {
                case 0:
                    DispType = MXTrecv.RecvType;
                    DispData = &MXTrecv.dat;
                    break;

                case 1:
                    DispType = MXTrecv.RecvType1;
                    DispData = &MXTrecv.dat1;
                    break;

                case 2:
                    DispType = MXTrecv.RecvType2;
                    DispData = &MXTrecv.dat2;
                    break;

                case 3:
                    DispType = MXTrecv.RecvType3;
                    DispData = &MXTrecv.dat3;
                    break;

                default:
                    break;
                }

            switch(DispType) {
                case MXT_TYP_JOINT:
                case MXT_TYP_FJOINT:
                case MXT_TYP_FB_JOINT:
                    if(loop==1) {
                        memcpy(&jnt_now, DispData, sizeof(JOINT));
                        loop = 2;
                        }
                   if(disp) {
                        JOINT *j=(JOINT*)DispData;
                        sprintf(buf, "Receive (%ld): TCount=%d Type(JOINT)=%d\n ,%7.2f,%7.2f,%7.2f,%7.2f,%7.2f,%7.2f,%7.2f,%7.2f (%s)"
                        ,MXTrecv.CCount,MXTrecv.TCount,DispType
                        ,j->j1, j->j2, j->j3 ,j->j4, j->j5, j->j6, j->j7, j->j8, str);
                        cout << buf << endl;
                        cout << j->j1 <<endl;
                        }
                    break;

                case MXT_TYP_POSE:
                case MXT_TYP_FPOSE:
                case MXT_TYP_FB_POSE:
                    if(loop==1) {
                        memcpy(&pos_now, &MXTrecv.dat.pos, sizeof(POSE));
                        loop = 2;
                        }
                    if(disp) {
                        POSE *p=(POSE*)DispData;
                        sprintf(buf, "Receive (%ld): TCount=%d Type(POSE)=%d\n %7.2f,%7.2f,%7.2f,%7.2f,%7.2f,%7.2f, %04x,%04x (%s)"
                            ,MXTrecv.CCount,MXTrecv.TCount,DispType
                            ,p->w.x, p->w.y, p->w.z, p->w.a, p->w.b, p->w.c
                            , p->sflg1, p->sflg2, str);
                        cout << buf << endl;

                        }
                    break;

                case MXT_TYP_PULSE:
                case MXT_TYP_FPULSE:
                case MXT_TYP_FB_PULSE:
                case MXT_TYP_CMDCUR:
                case MXT_TYP_FBKCUR:
                    if(loop==1) {
                        memcpy(&pls_now, &MXTrecv.dat.pls, sizeof(PULSE));
                        loop = 2;
                        }
                    if(disp) {
                        PULSE *l=(PULSE*)DispData;
                        sprintf(buf, "Receive (%ld): TCount=%d Type(PULSE/OTHER)=%d\n %ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld (%s)"
                            ,MXTrecv.CCount,MXTrecv.TCount,DispType,l->p1,l->p2,l->p3,l->p4,l->p5,l->p6,l->p7,l->p8,str);
                          cout << buf << endl;
                        }
                    break;

                case MXT_TYP_NULL:
                    if(loop==1) {
                        loop = 2;
                    }
                    if(disp) {
                        sprintf(buf, "Receive (%ld): TCount=%d Type(NULL)=%d\n (%s)",MXTrecv.CCount,MXTrecv.TCount, DispType, str);
                       cout << buf << endl;
                    }
                    break;

                default:
                    cout << "Bad data type.\n" << endl;
                    break;
            }
            counter++;// Count up only when communication is successful
            retry=0;// Leave reception loop
        }
        else { // Reception timeout
            cout << "... Receive Timeout! <Push [Enter] to stop the program>" << endl;

            retry--;// No. of retries subtraction
            if(retry==0)  loop=0; // End program ifNo. of retries is 0
            }
    } /* while(retry) */
} /* while(loop) */
// End
cout << "/// End /// ";
sprintf(buf, "counter = %lf", counter);
cout << buf << endl;
//Close socket
status=closesocket(destSocket);

if (status == SOCKET_ERROR)
    cerr << "ERROR: closesocket unsuccessful" << endl;

status=WSACleanup();

if (status == SOCKET_ERROR)
    cerr << "ERROR: WSACleanup unsuccessful" << endl;
return 0;
}
