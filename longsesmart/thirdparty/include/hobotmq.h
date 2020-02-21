
//
// Created by ronghua.li on 19-04-08.
//
#ifndef INCLUDE_HOBOTMQ_HOBOTMQ_H
#define INCLUDE_HOBOTMQ_HOBOTMQ_H
#include <stdint.h>
#include <string>
#include <vector>
namespace sunrise {
// supported mq type till now
enum MQ_TYPE {
  MQ_REQ,
  MQ_REP,
  MQ_PUB,
  MQ_SUB,
  MQ_PULL,
  MQ_PUSH,
};

// supported mq protocol till now
enum MQ_PROTOCOL {
  PROTO_IPC,
};

class HobotMQ {
 public:
  /*
   *@path: both sides of communication share the same path.
   *@example: /var/xxx_abc
   */
  explicit HobotMQ(const std::string& path);
  virtual ~HobotMQ();
  /*
  *init mq
  *@type: MQ_TYPE, MQ_REQ<->MQ_REP,MQ_PUB<->MQ_SUB,MQ_PULL<->MQ_PUSH
  *@topic: if type is MQ_SUB, use topic as filter, other type not care of topics
  *@proto: just use PROTO_IPC as default now
  *@ret：0 success, -1 failed.
    if ret -1, use  GetErrorInfo to get the error msg.
  */
  virtual int Init(MQ_TYPE type, std::vector<std::string> topics,
                   MQ_PROTOCOL proto = PROTO_IPC);
  /*
   *recv msg
   *@buf: user buf
   *@len: user buf len
   *@ret：return the actual size user recved.
    if ret -1, use  GetErrorInfo to get the error msg.
  */
  virtual int RecvMsg(uint8_t* buf, int len);
  /*
   *send msg
   *@buf: user buf
   *@len: actual len want to send
   *@ret：return the actual len user send.
    if ret -1, use  GetErrorInfo to get the error msg.
  */
  virtual int SendMsg(const uint8_t* buf, int len);
  /*
   *get the error msg while error happen.
   */
  virtual const char* GetErrorInfo();

 private:
  void* context_;
  void* zmqsock_;
  std::string path_;
};

}  // namespace sunrise

#endif //INCLUDE_HOBOTMQ_HOBOTMQ_H