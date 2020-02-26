#ifndef SRC_SMARTRECV_HPP
#define SRC_SMARTRECV_HPP

#include "hobotmq.h"
#include "smartparse.h"
#include <cstdint>
#include <memory>
#include <pthread.h>
#include <string>

const uint32_t kMQLen = 409600;

class SmartRecv {
public:
  virtual bool Init();
  virtual void Exit();
  virtual const char* GetErrorInfo();
  SmartRecv();
private:
  static void* RecvSmartThread(void *);
  std::shared_ptr<sunrise::HobotMQ> mq_;
  bool start_;
  pthread_t td_;
  std::vector<std::string> topic_;
  static char recvbuf[kMQLen];
  SmartParse& parser;
  std::string errmsg_;
};

#endif  // SRC_SMARTRECV_HPP