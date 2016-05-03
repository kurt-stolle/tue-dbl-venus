#include "ControlThread.h"
#include "ThreadController.h"
#include "Arduino.h"

ThreadController controlThreadPool;

ControlThread::ControlThread(void (*f)()) {
  this->runtime = f;
  this->setInterval(100);
  controlThreadPool.add(this);
}

void ControlThread::run() {
  this->runtime();
}


