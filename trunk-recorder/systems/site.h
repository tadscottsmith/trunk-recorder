#ifndef SITE_H
#define SITE_H

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

class site {

private:
  unsigned long sys_id;
  int sys_rfss;
  int sys_site_id;
  bool conventional;
  bool failed;
  bool valid;
  bool active;

  double primary_control_channel;
  std::vector<double> secondary_control_channels;

public:

  unsigned long get_sys_id();
  int get_sys_rfss();
  int get_sys_site_id();
  bool get_conventional();
  bool get_failed();
  bool get_valid();
  bool get_active();
  
  void set_sys_id();
  void set_sys_rfss();
  void set_sys_site_id();
  void set_conventional();
  void set_failed();
  void set_valid();
  void set_active();


};
#endif
