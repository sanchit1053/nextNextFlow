#include "container.hpp"
#include "logger.hpp"
#include "channel.hpp"
#include <fmt/core.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <unordered_map>

namespace Handler
{
   std::function<void(int)> _handler;
   void signal_handler(int signal) {_handler(signal);}
}

int main()
{
   system("mkdir -p scratch");
   system("chmod 777 scratch/");
   std::unordered_map<pid_t, Container *> output_mapping;
   Handler::_handler = [&output_mapping](int sig){
      pid_t pid = wait(NULL);
      signal(SIGCHLD, Handler::signal_handler);
      if (output_mapping.find(pid) != output_mapping.end())
      {
         output_mapping[pid]->output();
      }
   };
   signal(SIGCHLD, Handler::signal_handler);
   Channel<std::string> input(Channel<std::string>::config({
      [](std::string s){return s;},
      [](std::string s){return s;},
   }));
   Channel<std::string> output({
      [](std::string s){return s;},
      [](std::string s){return s;},
   });
   input.push("Some data\n");
   Container c(
      Container::config(
      {
         "nginx:alpine", 
         "Mycontainer", 
         std::vector<std::string>({"foo"}), 
         "bar", 
         "#!/bin/sh\nsleep 1\ncat foo\ncat foo > bar",
         std::vector<RawChannel *>({&input}),
         &output,
         &output_mapping
      }
      )
   );
   c.run();
   while(!output.poll());
   LOG(INFO, output.pop());
}