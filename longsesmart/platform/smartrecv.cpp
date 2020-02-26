#include "smartrecv.h"
#include <cerrno>

char SmartRecv::recvbuf[kMQLen];

SmartRecv::SmartRecv() :
  start_(false), parser(SmartParse::Inst()) {
  topic_.push_back(ktypesmart);
  topic_.push_back(ktypefeature);
  topic_.push_back(ktypecrop);
  topic_.push_back(ktypebackground);
  topic_.push_back(ktypeh264);
}

bool SmartRecv::Init() {
  mq_.reset(new sunrise::HobotMQ("/var/run/ipc_sunrise_data"));
  start_ = false;
  if (!mq_) {
    errmsg_ = "mq create failed";
    return false;
  }
  if (0 != mq_->Init(sunrise::MQ_SUB, topic_)) {
    errmsg_ = mq_->GetErrorInfo();
    return false;
  }
  start_ = true;
  int ret = pthread_create(&td_, NULL, RecvSmartThread, this);
  if (0 != ret) {
    start_ = false;
    errmsg_ = "recv data thread create failed";
    return false;
  }
  start_ = true;
  // bool result = trigger_.Start();
  // if (!result)
  //   errmsg_ = "callback thread create failed";
  return start_;
}

void* SmartRecv::RecvSmartThread(void *param) {
  SmartRecv* impl = reinterpret_cast<SmartRecv*>(param);
  while (impl->start_) {
    int ret = impl->mq_->RecvMsg(reinterpret_cast<uint8_t*>(recvbuf), kMQLen);
    if (-1 == ret) {
    } else if (ret > kMQLen) {
    } else {
      if (impl->parser.Parse(recvbuf, ret)) {
        
      }
    }
  }
}

void SmartRecv::Exit() {
  if (start_) {
    start_ = false;
    pthread_join(td_, NULL);
    // trigger_.Stop();
  }
}

const char* SmartRecv::GetErrorInfo() {
  return errmsg_.c_str();
}