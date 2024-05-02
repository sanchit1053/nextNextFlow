#ifndef _HANDLER_H_
#define _HANDLER_H_

#include "container.hpp"
#include <functional>
#include <unordered_map>
#include <unistd.h>
#include <sys/wait.h>

namespace Handler
{
   std::function<void(int)> _handler;
   void signal_handler(int signal) {_handler(signal);}
   void setup_handler(std::unordered_map<pid_t, Container *> &output_mapping);
}

void Handler::setup_handler(std::unordered_map<pid_t, Container *> &output_mapping)
{
    Handler::_handler = [&output_mapping](int sig){
      pid_t pid = wait(NULL);
      signal(SIGCHLD, Handler::signal_handler);
      if (output_mapping.find(pid) != output_mapping.end())
      {
         output_mapping[pid]->output_ready();
      }
   };
   signal(SIGCHLD, Handler::signal_handler);
}

#endif
