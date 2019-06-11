#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <mach/mach.h>
#include <pthread/pthread.h>

extern pthread_mutex_t DEATH_SERVER_DETACHED_MUTEX;
extern pthread_cond_t DEATH_SERVER_DETACHED_COND;

extern int AUTO_RESUME;
extern int NUM_EXCEPTIONS;



/*
extern pthread_mutex_t EXCEPTION_MUTEX;
extern pthread_cond_t REPROMPT_COND;
extern pthread_cond_t RESTART_COND;
extern pthread_cond_t MAIN_THREAD_READY_COND;


extern int HANDLING_EXCEPTIONS;
extern int KICK_MAIN_THREAD_OUT_OF_READLINE;

extern int SAVED_RL_INSTREAM;
extern int WAIT_FOR_MAIN_THREAD;
extern int IN_READLINE;

extern pthread_t MAIN_THREAD_TID;

extern pthread_mutex_t STUFF_CHAR_MUTEX;
int GOT_NEWLINE_STUFFED;
*/
typedef struct {
    mach_msg_header_t Head;
    /* start of the kernel processed data */
    mach_msg_body_t msgh_body;
    mach_msg_port_descriptor_t thread;
    mach_msg_port_descriptor_t task;
    /* end of the kernel processed data */
    NDR_record_t NDR;
    exception_type_t exception;
    mach_msg_type_number_t codeCnt;
    int code[2];
    mach_msg_trailer_t trailer;
} Request;

typedef struct {
    mach_msg_header_t Head;
    NDR_record_t NDR;
    kern_return_t RetCode;
} Reply;

void handle_exception(Request *, int *, int *, char **);
void reply_to_exception(Request *, kern_return_t);

#endif
