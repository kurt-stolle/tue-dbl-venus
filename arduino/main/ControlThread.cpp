#include "ControlThread.h"
#include "ThreadController.h"
#include "Arduino.h"

ThreadController controlThreadPool;

ControlThread::ControlThread(void (*f)()) {
  this->runtime = f;
  this->setInterval(1); // So the thread will run every time the timer ticks
  controlThreadPool.add(this);
}

void ControlThread::run() {
  this->runtime();
  runned();
}
