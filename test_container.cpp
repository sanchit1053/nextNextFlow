#include "container.hpp"
#include "logger.hpp"
#include "channel.hpp"
#include <fmt/core.h>

int main()
{
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
         "#!/bin/sh\ncat foo\ncat foo > bar",
         std::vector<RawChannel *>({&input}),
         &output
      }
      )
   );
   c.run();
}