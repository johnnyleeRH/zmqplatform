#ifndef SRC_SMARTTRANSIMPL_HPP
#define SRC_SMARTTRANSIMPL_HPP

#include "eventtrigger.h"
#include "smartdatatrans.h"
#include "hobotmq.h"
#include "msgparse.h"
#include "smartpack.h"
#include <cstdint>
#include <memory>
#include <pthread.h>
#include <string>

namespace horizon {

const uint32_t kMQLen = 409600;

class SmartTransImpl : public SmartTrans {
public:
  virtual bool Init();
  virtual void RegisteFaceDetectHandle(FaceDetectFunc detect);
  virtual void RegisteFaceCmpHandle(FaceCmpFunc cmp);
  virtual void Exit();
  virtual const char* GetErrorInfo();
  SmartTransImpl();
private:
  static void* RecvSmartThread(void *);
  std::shared_ptr<sunrise::HobotMQ> mq_;
  bool start_;
  pthread_t td_;
  std::vector<std::string> topic_;
  static char recvbuf[kMQLen];
  MsgParse& parser;
  SmartPack& pack;
  EventTrigger trigger_;
  std::string errmsg_;
};

}

#endif  // SRC_SMARTTRANSIMPL_HPP