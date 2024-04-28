#include "container.hpp"
#include "logger.hpp"
#include "channel.hpp"
#include "mux.hpp"
#include "handler.hpp"
#include "parser.hpp"
#include <fmt/core.h>
#include <string>
#include <unordered_map>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        LOG(ERROR, "Correct usage: orchestrator <xml-file-name>");
        exit(1);
    }

    system("mkdir -p scratch"); // ew wtf
    system("chmod 777 scratch/");
    std::unordered_map<pid_t, Container *> output_mapping;
    Handler::setup_handler(output_mapping);

    // queues need to have data types. Not everything can be dealt with as a stream of values, need to allow for aggregate types as well
    // maybe allow an argument in the queue, that sets the size of its memory?
    // might also need a demux for the queues: Can we attach the output of a queue to the input of multiple others?
    std::unordered_map<std::string, RawChannel *> channels;
    std::unordered_map<std::string, Mux *> muxes;
    std::unordered_map<std::string, Container *> processes;
    auto [input_channel, output_channel] = Parser::parse(argv[1], channels, muxes, processes, output_mapping);

    // if (input_channel)
    // {
    //     input_channel->push("Some data\n");
    // }

    while (true)
    {
        // can we have global input here?
        for (auto &it : processes)
        {
            if (it.second->poll())
            {
                it.second->run();
            }
            if (it.second->poll_output())
            {
                it.second->output();
            }
        }
        for (auto &it : muxes)
        {
            if (it.second->poll())
            {
                it.second->output();
            }
        }
        if (output_channel && output_channel->poll())
        {
            LOG(INFO, output_channel->pop());
            // break;
        }
    }

    system("rm -rf scratch/");
    for (auto &it : processes)
    {
        delete it.second;
    }
    for (auto &it : channels)
    {
        delete it.second;
    }
}
