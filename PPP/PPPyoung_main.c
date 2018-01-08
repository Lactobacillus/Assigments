#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <linux/ip.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <fcntl.h>
#include "PPPyoung_headers.h"

#define UP 0
#define DOWN 1
#define OPEN 2
#define CLOSE 3
#define TO_P 4
#define TO_M 5
#define RCR_P 6
#define RCR_M 7
#define RCA 8
#define RCN 9
#define RTR 10
#define RTA 11
#define RUC 12
#define RXJ_P 13
#define RXJ_M 14
#define RXR 15

#define INITIAL 0
#define STARTING 1
#define CLOSED 2
#define STOPPED 3
#define CLOSING 4
#define STOPPING 5
#define REQ_SENT 6
#define ACK_RCVD 7
#define ACK_SENT 8
#define OPENED 9

#define tlu 1
#define tld 2
#define tls 3
#define tlf 4
#define irc 5
#define zrc 6
#define scr 7
#define sca 8
#define scn 9
#define str 10
#define sta 11
#define scj 12
#define ser 13

#define LCP 0
#define NCP 1
#define NOACCOMP 0
#define NOPCOMP 1

#define BAUDRATE B38400
#define QUEUESIZE 4095
#define SERIALDEVICE "/dev/ttyS0"

int LOWER_state;
int LOWER_event;
int LCP_state;
int LCP_event;
int NCP_state;
int NCP_event;
int UPPER_state;
int UPPER_event;
int negotiation[2];
unsigned char buffer[255];
unsigned char queue[4096];
unsigned char frame[2048];

int main(int argc, char * argv[]){

    int idx;
    int rcvd;
    int sent;
    int loop;
    int fileDes;
    int saved_len;
    int idx_frame;
    int len_frame;
    int front, rear;
    struct termios oldtio, newtio;
    unsigned char saved_frame[2048];

    char * event_name[16] = {

        "UP", "DOWN", "OPEN", "CLOSE", "TO+", "TO-", "RCR+", "RCR-", "RCA", "RCN", "RTR", "RTA", "RUC", "RXJ+", "RXJ-", "RXR"
    };
    char * state_name[11] = {

        "INITIAL", "STARTING", "CLOSED", "STOPPED", "CLOSING", "STOPPING", "REQ_SENT", "ACK_RCVD", "ACK_SENT", "OPENED"
    };

    int (* funcPtr[10]) (int, int *, int *, unsigned char *, int *) = {

        initial, starting, closed, stopped, closing, stopping, req_sent, ack_rcvd, ack_sent, opened
    };

        bzero(&buffer, sizeof(buffer));
        fileDes = open(SERIALDEVICE, O_RDWR | O_NOCTTY, O_NONBLOCK);

        if (fileDes < 0){

            perror(SERIALDEVICE);
            exit(-1);
        }

        tcgetattr(fileDes, &oldtio);
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR | ICRNL;
        tcflush(fileDes, TCIFLUSH);
        tcsetattr(fileDes, TCSANOW, &newtio);
        tcflush(fileDes, TCIFLUSH);
        tcflush(fileDes, TCIFLUSH);
        tcflush(fileDes, TCIFLUSH);

        // Set queue
        front = 0;
        rear = 0;

        // Set state machine
        LCP_state = INITIAL;
        NCP_state = INITIAL;
        LCP_event = OPEN;
        NCP_event = OPEN;

        // Set negotiation option
        for (idx = 0; idx < argc; idx++){

            if (strcmp("nopcomp", argv[idx]) == 0){

                negotiation[NOPCOMP] = 1;
            }

            if (strcmp("noaccomp", argv[idx]) == 0){

                negotiation[NOACCOMP] = 1;
            }
        }

        // temp variable for change_event
        loop = 0;

        while (1){

            int done = 0;

            idx_frame = 0;
            len_frame = 0;

            // make one frame
            while (1){

                fflush(stdin);
                fflush(stdout);

                rcvd = read(fileDes, buffer, 255);

                for (idx = 0; idx < rcvd; idx++){

                    queue[front] = buffer[idx];
                    front = nextIdxQueue(front);
                }

                while (isQueueEmpty(front, rear) != 1){

                    frame[idx_frame] = queue[rear];
                    idx_frame += 1;
                    rear = nextIdxQueue(rear);

                    if (frame[idx_frame - 1] == 0x7E){

                        done += 1;

                        if (done == 2){

                            rear = previousIdxQueue(rear);
                            break;
                        }
                    }
                }

                if (done == 2){

                    break;
                }
            }

            len_frame = idx_frame;

            // use frame
            if (len_frame > 2){

                memset(saved_frame, 0x00, 2048);

                printf("rcvd :\t");
                for (idx_frame = 0; idx_frame < len_frame; idx_frame++){

                    printf("%02X\t", frame[idx_frame]);
                }
                printf("\n\n");

                len_frame = del_escape(frame, len_frame);
                saved_len = len_frame;

                printf("unescaped :\t");
                for (idx_frame = 0; idx_frame < len_frame; idx_frame++){

                    saved_frame[idx_frame] = frame[idx_frame];
                    printf("%02X\t", frame[idx_frame]);
                }
                printf("\n\n");

            }
            else{

                len_frame = saved_len;

                printf("saved :\t");
                for (idx_frame = 0; idx_frame < len_frame; idx_frame++){

                    frame[idx_frame] = saved_frame[idx_frame];
                    printf("%02X\t", saved_frame[idx_frame]);
                }
                printf("\n\n");
            }

            // Change events
            if (loop > 1){

                change_event(&LCP_event, &NCP_event, saved_frame);
            }
            printf("LCP_event : %s (%d)\nNCP_event : %s (%d)\n", event_name[LCP_event], LCP_event, event_name[NCP_event], NCP_event);
            loop += 1;

            if (LCP_state != -1 && NCP_state != -1){

                LCP_state = funcPtr[LCP_state](LCP, &LCP_event, &NCP_event, frame, negotiation);
                NCP_state = funcPtr[NCP_state](NCP, &NCP_event, &UPPER_event, frame, negotiation);
                printf("LCP_state : %s (%d)\nNCP_state : %s (%d)\n\n", state_name[LCP_state], LCP_state, state_name[NCP_state], NCP_state);
            }
            else{

                printf("Something is wrong!\n");
                return 0;
            }

            // get length of frame to send
            len_frame = 1;
            while (frame[len_frame] != 0x7E){

                len_frame += 1;
            }
            len_frame += 1;

            if (len_frame > 2048){

                len_frame = 0;
            }

            if (len_frame > 0){

                sent = write(fileDes, frame, len_frame*2);

                printf("sent :\t");
                for (idx_frame = 0; idx_frame < len_frame; idx_frame++){

                    printf("%02X\t", frame[idx_frame]);
                }
            }

            printf("\n--------------------------------------------------------------------------------\n\n");
        }

        tcsetattr(fileDes, TCSANOW, &oldtio);

    return 0;
}
