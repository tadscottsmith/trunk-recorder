#include <curl/curl.h>
#include <time.h>
#include <vector>

// #include "../../trunk-recorder/global_structs.h"
// #include "../../trunk-recorder/recorders/recorder.h"
#include "../../trunk-recorder/source.h"
// #include "../../trunk-recorder/systems/system.h"
#include "../../trunk-recorder/plugin_manager/plugin_api.h"
#include "../trunk-recorder/gr_blocks/decoder_wrapper.h"
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

typedef struct stat_plugin_t stat_plugin_t;

struct stat_plugin_t {
  std::vector<Source *> sources;
  std::vector<System *> systems;
  std::vector<Call *> calls;
  Config *config;
};

struct splunk_system {
  std::string api_key;
  std::string short_name;
  std::string system_id;
  std::string talkgroupsFile;
  Talkgroups *talkgroups;
  bool compress_wav;
};

struct splunk_data {
  std::vector<splunk_system> systems;
  std::string server;
};

class splunk : public Plugin_Api {

  splunk_data data;

  int retry_attempt;

  std::vector<Source *> sources;
  std::vector<System *> systems;
  std::vector<Call *> calls;
  Config *config;

  bool m_config_sent = false;

public:

  splunk_system *get_system(std::string short_name) {
    for (std::vector<splunk_system>::iterator it = data.systems.begin(); it != data.systems.end(); ++it) {
      if (it->short_name == short_name) {
        return &(*it);
      }
    }
    return NULL;
  }


  int system_rates(std::vector<System *> systems, float timeDiff) {
    this->systems = systems;

    boost::property_tree::ptree nodes;

    for (std::vector<System *>::iterator it = systems.begin(); it != systems.end(); it++) {
      System *system = *it;
      nodes.push_back(std::make_pair("", system->get_stats_current(timeDiff)));
    }
    return send_object(nodes, "rates", "rates");
  }

  void send_config(std::vector<Source *> sources, std::vector<System *> systems) {

    if (config_sent())
      return;

    boost::property_tree::ptree root;
    boost::property_tree::ptree systems_node;
    boost::property_tree::ptree sources_node;

    for (std::vector<Source *>::iterator it = sources.begin(); it != sources.end(); it++) {
      Source *source = *it;
      std::vector<Gain_Stage_t> gain_stages;
      boost::property_tree::ptree source_node;
      source_node.put("source_num", source->get_num());
      source_node.put("antenna", source->get_antenna());

      source_node.put("silence_frames", source->get_silence_frames());

      source_node.put("min_hz", source->get_min_hz());
      source_node.put("max_hz", source->get_max_hz());
      source_node.put("center", source->get_center());
      source_node.put("rate", source->get_rate());
      source_node.put("driver", source->get_driver());
      source_node.put("device", source->get_device());
      source_node.put("error", source->get_error());
      source_node.put("gain", source->get_gain());
      gain_stages = source->get_gain_stages();
      for (std::vector<Gain_Stage_t>::iterator gain_it = gain_stages.begin(); gain_it != gain_stages.end(); gain_it++) {
        source_node.put(gain_it->stage_name + "_gain", gain_it->value);
      }
      source_node.put("antenna", source->get_antenna());
      source_node.put("analog_recorders", source->analog_recorder_count());
      source_node.put("digital_recorders", source->digital_recorder_count());
      source_node.put("debug_recorders", source->debug_recorder_count());
      source_node.put("sigmf_recorders", source->sigmf_recorder_count());
      sources_node.push_back(std::make_pair("", source_node));
    }

    for (std::vector<System *>::iterator it = systems.begin(); it != systems.end(); ++it) {
      System *sys = (System *)*it;

      boost::property_tree::ptree sys_node;
      boost::property_tree::ptree channels_node;
      sys_node.put("audioArchive", sys->get_audio_archive());
      sys_node.put("systemType", sys->get_system_type());
      sys_node.put("shortName", sys->get_short_name());
      sys_node.put("sysNum", sys->get_sys_num());
      sys_node.put("uploadScript", sys->get_upload_script());
      sys_node.put("recordUnkown", sys->get_record_unknown());
      sys_node.put("callLog", sys->get_call_log());
      sys_node.put("talkgroupsFile", sys->get_talkgroups_file());
      sys_node.put("analog_levels", sys->get_analog_levels());
      sys_node.put("digital_levels", sys->get_digital_levels());
      sys_node.put("qpsk", sys->get_qpsk_mod());
      sys_node.put("squelch_db", sys->get_squelch_db());
      std::vector<double> channels;

      if ((sys->get_system_type() == "conventional") || (sys->get_system_type() == "conventionalP25")) {
        channels = sys->get_channels();
      } else {
        channels = sys->get_control_channels();
      }

      // std::cout << "starts: " << std::endl;

      for (std::vector<double>::iterator chan_it = channels.begin(); chan_it != channels.end(); chan_it++) {
        double channel = *chan_it;
        boost::property_tree::ptree channel_node;
        // std::cout << "Hello: " << channel << std::endl;
        channel_node.put("", channel);

        // Add this node to the list.
        channels_node.push_back(std::make_pair("", channel_node));
      }
      sys_node.add_child("channels", channels_node);

      if (sys->get_system_type() == "smartnet") {
        sys_node.put("bandplan", sys->get_bandplan());
        sys_node.put("bandfreq", sys->get_bandfreq());
        sys_node.put("bandplan_base", sys->get_bandplan_base());
        sys_node.put("bandplan_high", sys->get_bandplan_high());
        sys_node.put("bandplan_spacing", sys->get_bandplan_spacing());
        sys_node.put("bandplan_offset", sys->get_bandplan_offset());
      }
      systems_node.push_back(std::make_pair("", sys_node));
    }
    root.add_child("sources", sources_node);
    root.add_child("systems", systems_node);
    root.put("captureDir", this->config->capture_dir);
    root.put("uploadServer", this->config->upload_server);

    // root.put("defaultMode", default_mode);
    root.put("callTimeout", this->config->call_timeout);
    root.put("logFile", this->config->log_file);
    root.put("instanceId", this->config->instance_id);
    root.put("instanceKey", this->config->instance_key);
    root.put("logFile", this->config->log_file);
    root.put("type", "config");

    if (this->config->broadcast_signals == true) {
      root.put("broadcast_signals", this->config->broadcast_signals);
    }

    std::stringstream stats_str;
    boost::property_tree::write_json(stats_str, root);

    // std::cout << stats_str;
    send_stat(stats_str.str());
    m_config_sent = true;
  }

  int send_systems(std::vector<System *> systems) {
    boost::property_tree::ptree node;

    for (std::vector<System *>::iterator it = systems.begin(); it != systems.end(); it++) {
      System *system = *it;
      node.push_back(std::make_pair("", system->get_stats()));
    }
    return send_object(node, "systems", "systems");
  }

  int send_system(System *system) {

    return send_object(system->get_stats(), "system", "system");
  }

  int calls_active(std::vector<Call *> calls) {

    boost::property_tree::ptree node;

    for (std::vector<Call *>::iterator it = calls.begin(); it != calls.end(); it++) {
      Call *call = *it;
      // if (call->get_state() == RECORDING) {
      node.push_back(std::make_pair("", call->get_stats()));
      //}
    }

    return send_object(node, "calls", "calls_active");
  }

  int send_recorders(std::vector<Recorder *> recorders) {

    boost::property_tree::ptree node;

    for (std::vector<Recorder *>::iterator it = recorders.begin(); it != recorders.end(); it++) {
      Recorder *recorder = *it;
      node.push_back(std::make_pair("", recorder->get_stats()));
    }

    return send_object(node, "recorders", "recorders");
  }

  int call_start(Call *call) {

    return send_object(call->get_stats(), "call", "call_start");
  }

  int call_end(Call_Data_t call_info) {

    return 0;
    // send_object(call->get_stats(), "call", "call_end");
  }

  int send_recorder(Recorder *recorder) {

    return send_object(recorder->get_stats(), "recorder", "recorder");
  }

  int send_object(boost::property_tree::ptree data, std::string name, std::string type) {

    boost::property_tree::ptree root;

    root.add_child(name, data);
    root.put("type", type);
    root.put("instanceId", this->config->instance_id);
    root.put("instanceKey", this->config->instance_key);
    std::stringstream stats_str;
    boost::property_tree::write_json(stats_str, root);

    // std::cout << stats_str;
    return send_stat(stats_str.str());
  }

  int poll_one() {
    return 0;
  }

  bool config_sent() {

    return m_config_sent;
  }

  int send_stat(std::string val) {


    std::string api_key;

    splunk_system *sys = get_system(call_info.short_name);

    if (sys) {
      api_key = sys->api_key;
      system_id = sys->system_id;
    }

    fprintf(stderr, "%s\n", val.c_str());

  CURL *curl = curl_easy_init();
  if (!curl) {
    // Handle error
    return -1;
  }

        std::string url = data.server + "/api/call-upload";

  /* what URL that receives this POST */
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, val.c_str());

  // Set the HTTP headers
  struct curl_slist *headers = nullptr;
  headers = curl_slist_append(headers, "Authorization: Splunk " + api_key.c_str());
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

  // Perform the request
  CURLcode res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    return -1;
  }

  // Clean up
  curl_slist_free_all(headers);
  curl_easy_cleanup(curl);

    // PUT THE CURL MAGIC HERE
    return 0;
  }

  int signal(long unitId, const char *signaling_type, gr::blocks::SignalType sig_type, Call *call, System *system, Recorder *recorder) {
    if (this->config->broadcast_signals == false)
      return 1;

    boost::property_tree::ptree signal;
    signal.put("unit_id", unitId);
    // signal.put("signal_system_type", signaling_type);
    // signal.put("signal_type", sig_type);

    if (call != NULL) {
      signal.add_child("call", call->get_stats());
    }

    if (recorder != NULL) {
      signal.add_child("recorder", recorder->get_stats());
    }

    if (system != NULL) {
      signal.add_child("system", system->get_stats());
    }

    return send_object(signal, "signal", "signaling");
  }

  int init(Config *config, std::vector<Source *> sources, std::vector<System *> systems) {

    this->sources = sources;
    this->systems = systems;
    this->config = config;

    return 0;
  }

  int start() {
    return 0;
  }

  int setup_recorder(Recorder *recorder) {
    this->send_recorder(recorder);
    return 0;
  }

  int setup_system(System *system) {
    this->send_system(system);
    return 0;
  }

  int setup_systems(std::vector<System *> systems) {
    this->systems = systems;

    this->send_systems(systems);
    return 0;
  }

  int setup_config(std::vector<Source *> sources, std::vector<System *> systems) {

    this->sources = sources;
    this->systems = systems;

    send_config(sources, systems);
    return 0;
  }

  // Factory method
  static boost::shared_ptr<splunk> create() {
    return boost::shared_ptr<splunk>(
        new splunk());
  }

  int parse_config(boost::property_tree::ptree &cfg) {

    // Tests to see if the rdioscannerServer value exists in the config file
    boost::optional<std::string> upload_server_exists = cfg.get_optional<std::string>("server");
    if (!upload_server_exists) {
      return 1;
    }

    this->data.server = cfg.get<std::string>("server", "");
    BOOST_LOG_TRIVIAL(info) << "Splunk Server: " << this->data.server;

    // from: http://www.zedwood.com/article/cpp-boost-url-regex
    boost::regex ex("(http|https)://([^/ :]+):?([^/ ]*)(/?[^ #?]*)\\x3f?([^ #]*)#?([^ ]*)");
    boost::cmatch what;

    if (!regex_match(this->data.server.c_str(), what, ex)) {
      BOOST_LOG_TRIVIAL(error) << "Unable to parse Splunk Server URL\n";
      return 1;
    }

    // Gets the API key for each system, if defined
    BOOST_FOREACH (boost::property_tree::ptree::value_type &node, cfg.get_child("systems")) {
      boost::optional<boost::property_tree::ptree &> splunk_exists = node.second.get_child_optional("apiKey");
      if (splunk_exists) {
        splunk_system sys;

        sys.api_key = node.second.get<std::string>("apiKey", "");
        sys.system_id = node.second.get<std::string>("systemId", "");
        sys.short_name = node.second.get<std::string>("shortName", "");
        BOOST_LOG_TRIVIAL(info) << "Sending Splunk stats for: " << sys.short_name;
        this->data.systems.push_back(sys);
      }
    }

    if (this->data.systems.size() == 0) {
      BOOST_LOG_TRIVIAL(error) << "Splunk Server set, but no Systems are configured\n";
      return 1;
    }

    return 0;
  }

  int stop() {
    return 0;
  }

  int setup_sources(std::vector<Source *> sources) { return 0; }
};

BOOST_DLL_ALIAS(
    splunk::create, // <-- this function is exported with...
    create_plugin   // <-- ...this alias name
)
