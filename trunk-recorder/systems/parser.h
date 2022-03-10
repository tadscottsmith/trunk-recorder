#ifndef PARSE_H
#define PARSE_H
#include <iostream>
#include <vector>
#include <time.h>

enum MessageType {
  GRANT = 0,
  STATUS = 1,
  UPDATE = 2,
  CONTROL_CHANNEL = 3,
  REGISTRATION = 4,
  DEREGISTRATION = 5,
  AFFILIATION = 6,
  SYSID = 7,
  ACKNOWLEDGE = 8,
  LOCATION = 9,
  PATCH_ADD = 10,
  PATCH_DELETE = 11,
  UNKNOWN = 99
};

struct PatchData {
  unsigned long sg;
  unsigned long ga1;
  unsigned long ga2;
  unsigned long ga3;
};

struct SubscriberData {
  unsigned long suid;
  std::time_t affiliation_time;
  std::time_t last_activity;
};

struct TrunkMessage {
  MessageType message_type;
  std::string meta;
  double freq;
  long talkgroup;
  bool encrypted;
  bool emergency;
  bool duplex;
  bool mode;
  long priority;
  int tdma_slot;
  bool phase2_tdma;
  long source;
  int sys_num;
  unsigned long sys_id;
  unsigned long nac;
  unsigned long wacn;
  PatchData patch_data;
};

class TrunkParser {
  std::vector<TrunkMessage> parse_message(std::string s);
};
#endif
