#include "system.h"

std::string System::get_api_key() {
  return this->api_key;
}

void System::set_api_key(std::string api_key) {
  this->api_key = api_key;
}

std::string System::get_bcfy_api_key() {
  return this->bcfy_api_key;
}

void System::set_bcfy_api_key(std::string bcfy_api_key) {
  this->bcfy_api_key = bcfy_api_key;
}

int System::get_bcfy_system_id() {
  return this->bcfy_system_id;
}

void System::set_bcfy_system_id(int bcfy_system_id) {
  this->bcfy_system_id = bcfy_system_id;
}

std::string System::get_short_name() {
  return this->short_name;
}

void System::set_short_name(std::string short_name) {
  this->short_name = short_name;
}

std::string System::get_upload_script() {
  return this->upload_script;
}

void System::set_upload_script(std::string script) {
  this->upload_script = script;
}

bool System::get_compress_wav() {
  return this->compress_wav;
}

void System::set_compress_wav(bool compress) {
  this->compress_wav = compress;
}

double System::get_min_duration() {
  return this->min_call_duration;
}

void System::set_min_duration(double duration) {
  this->min_call_duration = duration;
}

double System::get_max_duration() {
  return this->max_call_duration;
}

void System::set_max_duration(double duration) {
  this->max_call_duration = duration;
}

System::System(int sys_num) {
  this->sys_num = sys_num;
  sys_id = 0;
  wacn = 0;
  nac = 0;
  current_control_channel = 0;
  xor_mask_len = 0;
  xor_mask = NULL;
  // Setup the talkgroups from the CSV file
  talkgroups = new Talkgroups();
  // Setup the unit tags from the CSV file
  unit_tags = new UnitTags();
  talkgroup_patches = {};
  talkgroup_subscribers = {};
  d_delaycreateoutput = false;
  d_hideEncrypted = false;
  d_hideUnknown = false;
  d_mdc_enabled = false;
  d_fsync_enabled = false;
  d_star_enabled = false;
  d_tps_enabled = false;
  retune_attempts = 0;
  message_count = 0;
}

void System::set_xor_mask(unsigned long sys_id, unsigned long wacn, unsigned long nac) {
  if (sys_id && wacn && nac) {
    this->sys_id = sys_id;
    this->wacn = wacn;
    this->nac = nac;
    BOOST_LOG_TRIVIAL(info) << "Setting XOR Mask: System ID " << std::dec << sys_id << " WACN: " << wacn << " NAC: " << nac << std::dec;
    if (sys_id && wacn && nac) {
      lfsr = new p25p2_lfsr(nac, sys_id, wacn);
      xor_mask = lfsr->getXorChars(xor_mask_len);
      
      BOOST_LOG_TRIVIAL(info) << "XOR Mask len: " << xor_mask_len;
      for (unsigned i=0; i<xor_mask_len; i++) {
        std::cout << (short)xor_mask[i] << ", ";
      }
    }
  }
}
bool System::update_status(TrunkMessage message) {
  if (!sys_id || !wacn || !nac) {
    sys_id = message.sys_id;
    wacn = message.wacn;
    nac = message.nac;
    BOOST_LOG_TRIVIAL(info) << "[" << short_name << "]\tDecoding System ID "
                            << std::hex << std::uppercase << message.sys_id << " WACN: "
                            << std::hex << std::uppercase << message.wacn << " NAC: " << std::hex << std::uppercase << message.nac;
    if (sys_id && wacn && nac) {
      lfsr = new p25p2_lfsr(nac, sys_id, wacn);
      xor_mask = lfsr->getXorChars(xor_mask_len);
      /*
     BOOST_LOG_TRIVIAL(info) << "XOR Mask len: " << xor_mask_len;
     for (unsigned i=0; i<xor_mask_len; i++) {
       std::cout << (short)xor_mask[i] << ", ";
     }*/
    }
    return true;
  }
  return false;
}

const char *System::get_xor_mask() {
  return xor_mask;
}

int System::get_sys_num() {
  return this->sys_num;
}

unsigned long System::get_sys_id() {
  return this->sys_id;
}

unsigned long System::get_nac() {
  return this->nac;
}

unsigned long System::get_wacn() {
  return this->wacn;
}

bool System::get_call_log() {
  return this->call_log;
}

void System::set_call_log(bool call_log) {
  this->call_log = call_log;
}

void System::set_squelch_db(double s) {
  squelch_db = s;
}

double System::get_squelch_db() {
  return squelch_db;
}

void System::set_filter_width(double filter_width) {
  this->filter_width = filter_width;
}

double System::get_filter_width() {
  return filter_width;
}

void System::set_max_dev(int max_dev) {
  this->max_dev = max_dev;
}

int System::get_max_dev() {
  return max_dev;
}

void System::set_analog_levels(double r) {
  analog_levels = r;
}

double System::get_analog_levels() {
  return analog_levels;
}

void System::set_digital_levels(double r) {
  digital_levels = r;
}

double System::get_digital_levels() {
  return digital_levels;
}

void System::set_qpsk_mod(bool m) {
  qpsk_mod = m;
}

bool System::get_qpsk_mod() {
  return qpsk_mod;
}

void System::set_mdc_enabled(bool b) { d_mdc_enabled = b; };
void System::set_fsync_enabled(bool b) { d_fsync_enabled = b; };
void System::set_star_enabled(bool b) { d_star_enabled = b; };
void System::set_tps_enabled(bool b) { d_tps_enabled = b; }

bool System::get_mdc_enabled() { return d_mdc_enabled; };
bool System::get_fsync_enabled() { return d_fsync_enabled; };
bool System::get_star_enabled() { return d_star_enabled; };
bool System::get_tps_enabled() { return d_tps_enabled; };

bool System::get_audio_archive() {
  return this->audio_archive;
}

void System::set_audio_archive(bool audio_archive) {
  this->audio_archive = audio_archive;
}

bool System::get_transmission_archive() {
  return this->transmission_archive;
}

void System::set_transmission_archive(bool transmission_archive) {
  this->transmission_archive = transmission_archive;
}


bool System::get_record_unknown() {
  return this->record_unknown;
}

void System::set_record_unknown(bool unknown) {
  this->record_unknown = unknown;
}

std::string System::get_system_type() {
  return this->system_type;
}

void System::set_system_type(std::string sys_type) {
  this->system_type = sys_type;
}

std::string System::get_talkgroups_file() {
  return this->talkgroups_file;
}

std::string System::get_unit_tags_file() {
  return this->unit_tags_file;
}

void System::set_talkgroups_file(std::string talkgroups_file) {
  BOOST_LOG_TRIVIAL(info) << "Loading Talkgroups...";
  this->talkgroups_file = talkgroups_file;
  this->talkgroups->load_talkgroups(talkgroups_file);
}

void System::set_unit_tags_file(std::string unit_tags_file) {
  BOOST_LOG_TRIVIAL(info) << "Loading Unit Tags...";
  this->unit_tags_file = unit_tags_file;
  this->unit_tags->load_unit_tags(unit_tags_file);
}

Source *System::get_source() {
  return this->source;
}

void System::set_source(Source *s) {
  this->source = s;
}

Talkgroup *System::find_talkgroup(long tg_number) {
  return talkgroups->find_talkgroup(tg_number);
}

UnitTag *System::find_unit_tag(long unitID) {
  return unit_tags->find_unit_tag(unitID);
}

std::vector<double> System::get_channels() {
  return channels;
}

int System::channel_count() {
  return channels.size();
}

void System::add_conventional_recorder(analog_recorder_sptr rec) {
  conventional_recorders.push_back(rec);
}
std::vector<analog_recorder_sptr> System::get_conventional_recorders() {
  return conventional_recorders;
}

void System::add_conventionalP25_recorder(p25_recorder_sptr rec) {
  conventionalP25_recorders.push_back(rec);
}

void System::add_conventionalDMR_recorder(dmr_recorder_sptr rec) {
  conventionalDMR_recorders.push_back(rec);
}

std::vector<p25_recorder_sptr> System::get_conventionalP25_recorders() {
  return conventionalP25_recorders;
}

std::vector<dmr_recorder_sptr> System::get_conventionalDMR_recorders() {
  return conventionalDMR_recorders;
}

void System::add_channel(double channel) {
  if (channels.size() == 0) {
    channels.push_back(channel);
  } else {
    if (std::find(channels.begin(), channels.end(), channel) == channels.end()) {
      channels.push_back(channel);
    }
  }
}

int System::control_channel_count() {
  return control_channels.size();
}

std::vector<double> System::get_control_channels() {
  return control_channels;
}

void System::add_control_channel(double control_channel) {
  if (control_channels.size() == 0) {
    control_channels.push_back(control_channel);
  } else {
    if (std::find(control_channels.begin(), control_channels.end(),
                  control_channel) == control_channels.end()) {
      control_channels.push_back(control_channel);
    }
  }
}

double System::get_current_control_channel() {
  return this->control_channels[current_control_channel];
}

double System::get_next_control_channel() {
  current_control_channel++;
  if (current_control_channel >= control_channels.size()) {
    current_control_channel = 0;
  }
  return this->control_channels[current_control_channel];
}

void System::set_conversation_mode(bool mode) {
  this->conversation_mode = mode;
}

 bool System::get_conversation_mode() {
  return this->conversation_mode;
}

void System::set_bandplan(std::string bandplan) {
  this->bandplan = bandplan;
}

std::string System::get_bandplan() {
  return this->bandplan;
}

void System::set_bandfreq(int freq) {
  this->bandfreq = freq;
}

int System::get_bandfreq() {
  return this->bandfreq;
}

void System::set_bandplan_high(double high) {
  this->bandplan_high = high;
}

double System::get_bandplan_high() {
  return this->bandplan_high / 1000000;
}

void System::set_bandplan_base(double base) {
  this->bandplan_base = base;
}

double System::get_bandplan_base() {
  return this->bandplan_base / 1000000;
}

void System::set_bandplan_spacing(double space) {
  this->bandplan_spacing = space / 1000000;
}

double System::get_bandplan_spacing() {
  return this->bandplan_spacing;
}

void System::set_bandplan_offset(int offset) {
  this->bandplan_offset = offset;
}

int System::get_bandplan_offset() {
  return this->bandplan_offset;
}

void System::set_talkgroup_display_format(TalkgroupDisplayFormat format) {
  talkgroup_display_format = format;
}

System::TalkgroupDisplayFormat System::get_talkgroup_display_format() {
  return talkgroup_display_format;
}

bool System::get_delaycreateoutput() {
  return d_delaycreateoutput;
}

void System::set_delaycreateoutput(bool delaycreateoutput) {
  d_delaycreateoutput = delaycreateoutput;
}

bool System::get_hideEncrypted() {
  return d_hideEncrypted;
}
void System::set_hideEncrypted(bool hideEncrypted) {
  d_hideEncrypted = hideEncrypted;
}

bool System::get_hideUnknown() {
  return d_hideUnknown;
}

void System::set_hideUnknown(bool hideUnknown) {
  d_hideUnknown = hideUnknown;
}

boost::property_tree::ptree System::get_stats() {
  boost::property_tree::ptree system_node;
  system_node.put("id", this->get_sys_num());
  system_node.put("name", this->get_short_name());
  system_node.put("type", this->get_system_type());
  system_node.put("sysid", this->get_sys_id());
  system_node.put("wacn", this->get_wacn());
  system_node.put("nac", this->get_nac());

  return system_node;
}

boost::property_tree::ptree System::get_stats_current(float timeDiff) {
  boost::property_tree::ptree system_node;
  system_node.put("id", this->get_sys_num());
  system_node.put("decoderate", this->message_count / timeDiff);

  return system_node;
}

std::vector<unsigned long> System::get_talkgroup_patch(unsigned long talkgroup){
  //Given a single TGID, return a vector of TGIDs that are part of the same patch
  std::vector<unsigned long> patched_tgids;
  BOOST_FOREACH (auto& patch, talkgroup_patches) {
    if (patch.second.find(talkgroup) != patch.second.end()) {
      //talkgroup passed in is part of this patch, so add all talkgroups from this patch to our output vector
      BOOST_FOREACH(auto& patch_element, patch.second){
        patched_tgids.push_back(patch_element.first);
      }
    }
  }
  return patched_tgids;
}

void System::update_active_talkgroup_patches(PatchData patch_data){
  std::time_t update_time = std::time(nullptr);
  bool new_flag = true;

  BOOST_FOREACH (auto& patch, talkgroup_patches) {
    if (patch.first == patch_data.sg){
      new_flag = false;
      if (0 != patch_data.sg){
        patch.second[patch_data.sg] = update_time;
      }
      if (0 != patch_data.ga1){
        patch.second[patch_data.ga1] = update_time;
      }
      if (0 != patch_data.ga2){
        patch.second[patch_data.ga2] = update_time;
      }
      if (0 != patch_data.ga3){
        patch.second[patch_data.ga3] = update_time;
      }
    }
  }
  if (new_flag == true){
    //TGIDs from the Message were not found in an existing patch, so add them to a new one
    //BOOST_LOG_TRIVIAL(debug) << "Adding a new patch";
    std::map<unsigned long,std::time_t> new_patch;
    if (0 != patch_data.sg){
      new_patch[patch_data.sg] = update_time;
    }
    if (0 != patch_data.ga1){
      new_patch[patch_data.ga1] = update_time;
    }
    if (0 != patch_data.ga2){
      new_patch[patch_data.ga2] = update_time;
    }
    if (0 != patch_data.ga3){
      new_patch[patch_data.ga3] = update_time;
    }
    talkgroup_patches[patch_data.sg] = new_patch;
  }
}

void System::delete_talkgroup_patch(PatchData patch_data){
  BOOST_FOREACH (auto& patch, talkgroup_patches) {
    if (patch.first == patch_data.sg){
      patch.second.erase(patch_data.ga1);
      patch.second.erase(patch_data.ga2);
      patch.second.erase(patch_data.ga3);
    }
  }
}

void System::clear_stale_talkgroup_patches(){
  std::vector<unsigned long> stale_patches;

  BOOST_FOREACH (auto& patch, talkgroup_patches) {
    //patch.first (map key) is supergroup TGID, patch.second (map value) is the map of all TGIDs in this patch and associated timestamps
    std::vector<unsigned long> stale_talkgroups;
    BOOST_FOREACH(auto& patch_element, patch.second){
      //patch_element.first (map key) is TGID, patch.second (map value) is the timestamp
      if (std::time(nullptr) - patch_element.second >= 10){  //10 second hard coded timeout for now
        stale_talkgroups.push_back(patch_element.first);  //add this tgid to the list that we'll delete from this patch since it's expired
      }
    }
    BOOST_FOREACH(auto& stale_talkgroup, stale_talkgroups){
      BOOST_LOG_TRIVIAL(debug) << "Going to remove stale TGID " << stale_talkgroup << "from patch with sg id " << patch.first;
      patch.second.erase(stale_talkgroup);
    }
    if (patch.second.size() == 0){
      stale_patches.push_back(patch.first);  //This patch is not empty, so add it to the list of patches we'll delete
    }
  }
  BOOST_FOREACH(auto& stale_patch, stale_patches){
    BOOST_LOG_TRIVIAL(debug) << "Going to remove entire patch with sg id " << stale_patch;
    talkgroup_patches.erase(stale_patch);
  }
  
  //Print out all active patches to the console
  BOOST_LOG_TRIVIAL(debug) << "Found " << talkgroup_patches.size() << " active talkgroup patches:";
  BOOST_FOREACH (auto& patch, talkgroup_patches) {
    std::string printstring;
    BOOST_FOREACH(auto& patch_element, patch.second){
      printstring+=" ";
      printstring+= std::to_string(patch_element.first);
    }
    BOOST_LOG_TRIVIAL(debug) << "Active Patch of TGIDs" << printstring;
  }
}

void System::update_active_talkgroup_subscribers(TrunkMessage message){
  std::time_t update_time = std::time(nullptr);
  bool new_tgid = true;

  if(message.source == -1) {
    return;
  }

  BOOST_FOREACH (auto& talkgroup, talkgroup_subscribers) {
    //talkgroup.first (map key) is TGID, patch.second is a vector of SubscriberData
    if (talkgroup.first == message.talkgroup){
      new_tgid = false;
      BOOST_LOG_TRIVIAL(error) << "Updating TG: " << talkgroup.first;

      bool new_subscriber = true;
      BOOST_FOREACH (auto& subscriber, talkgroup.second) {
        if(subscriber.suid == message.source)
        {
          new_subscriber = false;
          subscriber.last_activity = update_time;
          BOOST_LOG_TRIVIAL(error) << "Updating TG: " << talkgroup.first << ". Setting " << subscriber.suid << " last activity time."; 
        }
      }

      if(new_subscriber == true){
        SubscriberData new_subscriber;
        new_subscriber.suid = message.source;
        new_subscriber.affiliation_time = update_time;
        new_subscriber.last_activity = update_time;
        talkgroup.second.push_back(new_subscriber);
        BOOST_LOG_TRIVIAL(error) << "Updating TG: " << talkgroup.first << ". Adding " << new_subscriber.suid;
      }

    }
  }

  if (new_tgid == true){
    //TGID from the Message does not currently have subscribers tracked
    BOOST_LOG_TRIVIAL(error) << "Adding TG: " << message.talkgroup;

    SubscriberData new_subscriber;
    new_subscriber.suid = message.source;
    new_subscriber.affiliation_time = update_time;
    new_subscriber.last_activity = update_time;

    talkgroup_subscribers[message.talkgroup].push_back(new_subscriber);
  }

}

void delete_active_talkgroup_subscriber(TrunkMessage message){

  if(message.source == -1) {
    return;
  }

  BOOST_FOREACH (auto& talkgroup, talkgroup_subscribers) {
    //talkgroup.first (map key) is TGID, patch.second is a vector of SubscriberData
    if (talkgroup.first == message.talkgroup){
      BOOST_LOG_TRIVIAL(error) << "Cleaning TG: " << talkgroup.first;

      for (std::vector<int>::iterator it = talkgroup.second.begin(); it != talkgroup.second.end(); ) {
        if(subscriber.suid == message.source)
        {
          talkgroup.second.erase(it);
          BOOST_LOG_TRIVIAL(error) << "Cleaning TG: " << talkgroup.first << ". Removing " << subscriber.suid << "."; 
        }
      }

}


