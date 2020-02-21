#include "smarttransimpl.h"
#include "msgparse.h"
#include <cerrno>

namespace horizon {

char SmartTransImpl::recvbuf[kMQLen];

SmartTransImpl::SmartTransImpl() :
  start_(false), parser(MsgParse::Inst()), pack(SmartPack::Inst()) {
  topic_.push_back(ktypesmart);
  topic_.push_back(ktypefeature);
  topic_.push_back(ktypecrop);
  topic_.push_back(ktypebackground);
}

bool SmartTransImpl::Init() {
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
  bool result = trigger_.Start();
  if (!result)
    errmsg_ = "callback thread create failed";
  return result;
}

void SmartTransImpl::RegisteFaceDetectHandle(FaceDetectFunc detect) {
  trigger_.RegisteDetectHandle(detect);
}

void SmartTransImpl::RegisteFaceCmpHandle(FaceCmpFunc cmp) {
  trigger_.RegisteCmpHandle(cmp);
}

void* SmartTransImpl::RecvSmartThread(void *param) {
  SmartTransImpl* impl = reinterpret_cast<SmartTransImpl*>(param);
  while (impl->start_) {
    int ret = impl->mq_->RecvMsg(reinterpret_cast<uint8_t*>(recvbuf), kMQLen);
    if (-1 == ret) {
    } else if (ret > kMQLen) {
    } else {
      if (impl->parser.Parse(recvbuf, ret)) {
        if (impl->pack.PackLongseSmart()) {

        }
      }
    }
  }
}

void SmartTransImpl::Exit() {
  if (start_) {
    start_ = false;
    pthread_join(td_, NULL);
    trigger_.Stop();
  }
}

const char* SmartTransImpl::GetErrorInfo() {
  return errmsg_.c_str();
}

}