#ifndef TALKGROUP_H
#define TALKGROUP_H

#include <iostream>
#include <stdio.h>
#include <string>
//#include <sstream>

class Talkgroup {
public:
  long number;
  std::string mode;
  std::string alpha_tag;
  std::string description;
  std::string tag;
  std::string group;
  int priority;
  int sys_num;


  // For Conventional
  double freq;
  double tone;

  Talkgroup(int sys_num, long num, std::string mode, std::string alpha_tag, std::string description, std::string tag, std::string group, int priority, unsigned long preferredNAC);
  Talkgroup(int sys_num, long num, double freq, double tone, std::string alpha_tag, std::string description, std::string tag, std::string group);

  bool is_active();
  int get_priority();
  unsigned long get_preferredNAC();
  int get_preferred_sys_rfss();
  int get_preferred_sys_site_id();
  void set_priority(int new_priority);
  void set_active(bool a);
  std::string menu_string();

private:
  unsigned long preferredNAC;
  int sys_rfss;
  int sys_site_id;

  bool active;
};

#endif
