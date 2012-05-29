#include "../LCMRPC.hpp"
class responder {
public:
  lcm::LCM lcm;
  responder() :
      lcm("udpm://239.255.76.69:9999?ttl=0")
  {
  }
  void handle(const lcm::ReceiveBuffer* rbuf, const std::string& chan, const lcm_utils::rpc_command_t* msg)
  {
    lcm_utils::rpc_ack_t ret;
    ret.nonce = 3;
    ret.seqno = 5;
    ret.data_sz = 0;

    lcm_utils::rpc_command_t ret_wrapper;
    ret_wrapper.seqno = msg->seqno;
    ret_wrapper.nonce = msg->nonce;
    ret_wrapper.data_sz = ret.getEncodedSize();
    ret_wrapper.data.resize(ret_wrapper.data_sz);
    ret.encode(&ret_wrapper.data[0],0,ret_wrapper.data_sz);
    lcm.publish(chan + "_RETURN", &ret_wrapper);
  }
};

int main(int argc, char ** argv)
{

  if (argc > 1) {
    responder resp;

    resp.lcm.subscribe("BLAH", &responder::handle, &resp);
    while (1) {
      resp.lcm.handle();
    }
  }
  else {
    lcm_utils::LCMRPC_client<lcm_utils::rpc_ack_t, lcm_utils::rpc_ack_t> * lcmrpc = new lcm_utils::LCMRPC_client<
        lcm_utils::rpc_ack_t,
        lcm_utils::rpc_ack_t>();

    lcm_utils::rpc_ack_t cmd;
    cmd.data_sz = 0;
    cmd.seqno = 3;
    cmd.nonce = 5;
    lcm_utils::rpc_ack_t ret;
    lcmrpc->launch_cmd(&cmd, "BLAH", &ret);

    fprintf(stderr, "ret: %d, %d\n", ret.nonce, ret.seqno);
    return 0;
  }
}

