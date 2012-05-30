#ifndef LcmMatlabRpc_HPP_
#define LcmMatlabRpc_HPP_
#include <lcm/lcm-cpp.hpp>
#include <lcmtypes/lcm_utils.hpp>
#include <Eigen/Dense>
#include <bot_core/bot_core.h>
#include <eigen_utils/eigen_lcm.hpp>

namespace lcm_utils {

class LcmMatlabRpc {
public:
  lcm::LCM lcm;
  bool received_ack;

  lcm_utils::matlab_rpc_return_t * ret_msg;

  int nonce;

  std::string name;
  lcm::Subscription * ret_sub;
  lcm::Subscription * ack_sub;

  LcmMatlabRpc(const std::string &_name = "MATLAB_LCM_RPC") :
      lcm(), ret_msg(NULL), received_ack(false), name(_name)
  {
    ret_sub = lcm.subscribe(name + "_RETURN", &LcmMatlabRpc::handleReturn, this);
    ack_sub = lcm.subscribe(name + "_CMD_ACK", &LcmMatlabRpc::handleAck, this);

  }
  ~LcmMatlabRpc()
  {
    lcm.unsubscribe(ret_sub);
    lcm.unsubscribe(ack_sub);
  }

  void handleReturn(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const lcm_utils::matlab_rpc_return_t* msg)
  {
    if (msg->nonce == nonce) {
      printf("received return!\n");
      ret_msg = new lcm_utils::matlab_rpc_return_t(*msg);

    }

    //send back and ack
    lcm_utils::matlab_rpc_ack_t ack_msg;
    ack_msg.nonce = msg->nonce;
    lcm.publish(name + "_RET_ACK", &ack_msg);
  }
  void handleAck(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const lcm_utils::matlab_rpc_ack_t* msg)
  {
    if (msg->nonce == nonce) {
      printf("received ack!\n");
      received_ack = true;
    }
  }

  int run(const std::string command, const std::vector<Eigen::MatrixXd> & args, int numReturnVals,
      std::vector<Eigen::MatrixXd> &ret, int64_t timeout = -1)
  {
    std::string chan = name + "_CMD";
    lcm_utils::matlab_rpc_command_t cmd;
    nonce = rand();
    cmd.nonce = nonce;
    cmd.command = command;
    cmd.numArgs = args.size();
    cmd.args.resize(cmd.numArgs);
    for (int i = 0; i < cmd.numArgs; i++) {
      cmd.args[i] = eigen_utils::toLcmMsg(args[i]);
    }
    cmd.numReturnVals = numReturnVals;

    int64_t startTime = bot_timestamp_now();
    while (ret_msg == NULL) {
      if (!received_ack) {
        printf("sending command\n");
        lcm.publish(chan, &cmd);
        bot_lcm_handle_or_timeout(lcm.getUnderlyingLCM(), 1e3);
        continue;
      }
      else {
        bot_lcm_handle_or_timeout(lcm.getUnderlyingLCM(), 1e6);
      }
      if (timeout > 0 && bot_timestamp_now() - startTime > timeout)
        break;
    }

    if (ret_msg == NULL || ret_msg->return_status == 0) {
      fprintf(stderr, "matlab RPC failed with error:\n %s\n", ret_msg->error_msg.c_str());
      return 0;
    }

    ret.resize(ret_msg->numReturnVals);
    for (int i = 0; i < ret_msg->numReturnVals; i++) {
      ret[i] = eigen_utils::fromLcmMsg<Eigen::MatrixXd>(&ret_msg->returnVals[i]);
    }
    delete ret_msg;
    ret_msg = NULL;

    return 1;
  }
};

} /* namespace lcm_utils */
#endif /* LCMRPC_HPP_ */
