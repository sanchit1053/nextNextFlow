#include "container.hpp"
#include "logger.hpp"
#include <fmt/core.h>

int main()
{
   Container c("nginx:alpine", "blah", {"foo"}, "bar", "#!/bin/sh\ncat foo\ncat foo > bar");
   Channel output;
   std::vector<Channel> ivec(1);
   c.run(ivec, output);
}