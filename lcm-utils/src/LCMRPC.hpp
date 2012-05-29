#ifndef LCMRPC_HPP_
#define LCMRPC_HPP_
#include <lcm/lcm-cpp.hpp>
#include <lcmtypes/lcm_utils.hpp>

namespace lcm_utils {

template<typename commandLcmType, typename returnLcmType>
class LCMRPC_client {
public:
  lcm::LCM lcm;
  bool received_ret;
  returnLcmType * ret;
  int seqno;
  int nonce;

  lcm::Subscription * ret_sub;
  lcm::Subscription * ack_sub;

  LCMRPC_client<commandLcmType, returnLcmType>(const std::string & lcm_url = "udpm://239.255.76.69:9999?ttl=0") :
      lcm(lcm_url), received_ret(false), seqno(0), nonce(rand())
  {

  }

  void handleReturn(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const lcm_utils::rpc_command_t* msg)
  {
    ret->decode(&msg->data[0], 0, msg->data_sz);
    received_ret = true;
  }
  void handleAck(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const lcm_utils::rpc_ack_t* msg)
  {

  }

  void launch_cmd(const commandLcmType * command, const std::string &chan, returnLcmType * _ret)
  {
    ret = _ret;
    ret_sub = lcm.subscribe(chan + "_RETURN", &LCMRPC_client<commandLcmType, returnLcmType>::handleReturn, this);
    ack_sub = lcm.subscribe(chan + "_ACK", &LCMRPC_client<commandLcmType, returnLcmType>::handleAck, this);

    rpc_command_t cmd_msg;
    cmd_msg.seqno = seqno++;
    cmd_msg.nonce = rand();
    cmd_msg.data_sz = command->getEncodedSize();
    cmd_msg.data.resize(cmd_msg.data_sz);
    command->encode(&cmd_msg.data[0], 0, cmd_msg.data_sz);
    lcm.publish(chan, &cmd_msg);
    while (!received_ret) {
      lcm.handle();
    }

    lcm.unsubscribe(ret_sub);
    lcm.unsubscribe(ack_sub);

  }
};

template<typename commandLcmType, typename returnLcmType, typename userType>
class LCMRPC_server {
public:
  lcm::LCM lcm;
  bool received_ret;
  returnLcmType * ret;
  int seqno;
  int nonce;

  lcm::Subscription * ret_sub;
  lcm::Subscription * ack_sub;

  LCMRPC_server<commandLcmType, returnLcmType,userType>(const std::string & lcm_url = "udpm://239.255.76.69:9999?ttl=0") :
      lcm(lcm_url), received_ret(false), seqno(0), nonce(rand())
  {

  }

  void handleCommand(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const lcm_utils::rpc_command_t* msg)
  {
    ret->decode(&msg->data[0], 0, msg->data_sz);
    received_ret = true;
  }
  void handleAck(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const lcm_utils::rpc_ack_t* msg)
  {

  }

  void register_cmd(const commandLcmType * command, const std::string &chan, returnLcmType * _ret)
  {
    ret = _ret;
    ret_sub = lcm.subscribe(chan + "_RETURN", &LCMRPC_client<commandLcmType, returnLcmType>::handleReturn, this);
    ack_sub = lcm.subscribe(chan + "_ACK", &LCMRPC_client<commandLcmType, returnLcmType>::handleAck, this);

    rpc_command_t cmd_msg;
    cmd_msg.seqno = seqno++;
    cmd_msg.nonce = rand();
    cmd_msg.data_sz = command->getEncodedSize();
    cmd_msg.data.resize(cmd_msg.data_sz);
    command->encode(&cmd_msg.data[0], 0, cmd_msg.data_sz);
    lcm.publish(chan, &cmd_msg);
    while (!received_ret) {
      lcm.handle();
    }

    lcm.unsubscribe(ret_sub);
    lcm.unsubscribe(ack_sub);

  }
};
} /* namespace lcm_utils */
#endif /* LCMRPC_HPP_ */
