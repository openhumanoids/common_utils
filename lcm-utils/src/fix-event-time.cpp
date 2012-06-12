#include <stdio.h>
#include <lcm/lcm-cpp.hpp>
#include <lcmtypes/bot_core.hpp>
#include <ConciseArgs>
#include <list>
#include <string>

using namespace std;

class lcmLogEventData: public lcm::LogEvent {
public:
  lcmLogEventData(const lcm::LogEvent & event) :
      lcm::LogEvent(event)
  {
    this->data = malloc(this->datalen);
    memcpy(this->data, event.data, this->datalen);
  }

  lcmLogEventData(const lcmLogEventData & other)
  {
    this->channel = other.channel;
    this->datalen = other.datalen;
    this->eventnum = other.eventnum;
    this->timestamp = other.timestamp;

    this->data = malloc(this->datalen);
    memcpy(this->data, other.data, this->datalen);
  }

  ~lcmLogEventData()
  {
    free(this->data);
  }

  lcm::LogEvent getOr()
  {
    lcm::LogEvent orig;
    orig.channel = this->channel;
    orig.data = this->data;
    orig.datalen = this->datalen;
    orig.eventnum = this->eventnum;
    orig.timestamp = this->timestamp;
    return orig;
  }
};

int main(int argc, char** argv)
{

  string poseChannel = "STATE_ESTIMATOR_POSE";
  string logfileName;
  string outFileName;
  ConciseArgs opt(argc, argv, "logFname outFname");
  opt.add(poseChannel, "p", "pose_channel", "pose channel to clock off");
  opt.parse(logfileName, outFileName);

  printf("setting event time to pose utime on channel %s\n", poseChannel.c_str());
  printf("from log %s\n", logfileName.c_str());
  printf("and writing it to %s\n", outFileName.c_str());

  // Open the log file.
  lcm::LogFile log(logfileName, "r");
  if (!log.good()) {
    perror("LogFile");
    std::cerr << "couldn't open log file: " << logfileName << std::endl;
    return 1;
  }

  lcm::LogFile out_log(outFileName, "w");

  list<lcmLogEventData> eventList = list<lcmLogEventData>();

  while (1) {
    // Read a log event.
    const lcm::LogEvent *event = log.readNextEvent();
    if (!event)
      break;
    eventList.push_back(lcmLogEventData(*event));
    if (event->channel == poseChannel) {
      bot_core::pose_t pmsg;
      pmsg.decode(event->data, 0, event->datalen);
      int64_t pose_utime = pmsg.utime;
      list<lcmLogEventData>::iterator event_it;
      for (event_it = eventList.begin(); event_it != eventList.end(); event_it++) {
        event_it->timestamp = pose_utime;
        lcm::LogEvent orig = event_it->getOr();
        out_log.writeEvent(&orig);
      }
      eventList.clear();
    }
  }

  fprintf(stderr, "all done!\n");

  return 0;
}
