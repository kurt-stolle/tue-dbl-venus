/*
 * Thread for wheel control
 */

#ifndef ControlThread_h
#define ControlThread_h

#include "Thread.h"
#include "ThreadController.h"

extern ThreadController controlThreadPool;

class ControlThread: public Thread {
  public:
    ControlThread(void (*f)());
    void run();
  private:
    void (*runtime)();
};






#endif
