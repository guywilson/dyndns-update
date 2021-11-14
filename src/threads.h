#include "posixthread.h"

#ifndef _INCL_THREADS
#define _INCL_THREADS

class IPDiscoveryThread : public PosixThread
{
public:
    IPDiscoveryThread() : PosixThread(true) {}

    void * run();
};

class ThreadManager
{
public:
    static ThreadManager &  getInstance() {
        static ThreadManager instance;
        return instance;
    }

private:
    ThreadManager() {}

    IPDiscoveryThread *     pIPDiscoveryThread = NULL;

public:
    void                    startThreads();
    void                    killThreads();
};

#endif
