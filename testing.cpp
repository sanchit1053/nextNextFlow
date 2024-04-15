#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include <bits/stdc++.h>
#include <string>

using namespace rapidxml;
using namespace std;

class Process {
  string m_processName;
  string m_inputs;
  string m_outputs;
  string m_commands;

public:
  Process(string processName, string inputs, string outputs, string commands)
      : m_processName(processName), m_inputs(inputs), m_outputs(outputs),
        m_commands(commands) {}

  void debug() {
    cerr << "Process: " << m_processName << "\n\t" << m_commands << "\n\t"
         << m_inputs << "\n\t" << m_outputs << endl;
  }
};

class Channel {
  string m_channelName;

public:
  Channel(string channelName) : m_channelName(channelName) {}

  void debug() { cerr << "Channel: " << m_channelName << endl; }
};

class Workflow {
  vector<Process> m_processes;
  vector<Channel> m_channels;

public:
  Workflow(vector<Process> processes, vector<Channel> channels)
      : m_processes(processes), m_channels(channels) {}

  void debug() {
    for (Process p : m_processes) {
      p.debug();
    }

    for (Channel c : m_channels) {
      c.debug();
    }
  }
};

int main(int argc, char *argv[]) {
  xml_document<> doc;

  file<> xmlFile(argv[1]);
  doc.parse<0>(xmlFile.data());

  vector<Process> processes;
  vector<Channel> channels;

  xml_node<> *processes_node = doc.first_node("process");
  for (xml_node<> *child = processes_node->first_node(); child;
       child = child->next_sibling()) {
    string commands = child->first_node("command")->value();
    string inputs = child->first_node("input")->value();
    string outputs = child->first_node("output")->value();
    processes.push_back(Process(child->name(), inputs, outputs, commands));
  }

  xml_node<> *channel_node = doc.first_node("channel");
  for (xml_node<> *child = channel_node->first_node(); child;
       child = child->next_sibling()) {
    channels.push_back(Channel(child->name()));
  }

  Workflow workflow(processes, channels);

  workflow.debug();

  return 0;
}
