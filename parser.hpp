#ifndef _PARSER_H_
#define _PARSER_H_

#include "container.hpp"
#include "channel.hpp"
#include "logger.hpp"
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include <tuple>

namespace Parser
{
    std::string IN = "in";
    std::string OUT = "out";
    std::pair<RawChannel *, RawChannel *> parse(const std::string &filename, std::unordered_map<std::string, RawChannel *> &channels, std::unordered_map<std::string, Container *> &processes, std::unordered_map<pid_t, Container *> &output_mapping);
};

std::pair<RawChannel *, RawChannel *> Parser::parse(const std::string &filename, std::unordered_map<std::string, RawChannel *> &channels, std::unordered_map<std::string, Container *> &processes, std::unordered_map<pid_t, Container *> &output_mapping)
{
    rapidxml::xml_document<> doc;
    rapidxml::file<> xmlFile(filename.c_str());
    doc.parse<0>(xmlFile.data());

    rapidxml::xml_node<> *workflow_node = doc.first_node("workflow");

    std::pair<RawChannel *, RawChannel *> io_pair;

    for (rapidxml::xml_node<> *channel_node = workflow_node->first_node("channel");
         channel_node != nullptr;
         channel_node = channel_node->next_sibling("channel"))
    {
        std::string name = channel_node->first_attribute("name")->value();
        channels[name] = new Channel<std::string>(Channel<std::string>::config({
            [](std::string s)
            { return s; },
            [](std::string s)
            { return s; },
        }));
        if (channel_node->first_node("global"))
        {
            if (channel_node->first_node("global")->value() == Parser::IN)
            {
                io_pair.first = channels[name];
            }
            if (channel_node->first_node("global")->value() == Parser::OUT)
            {
                io_pair.second = channels[name];
            }
        }
    }

    for (rapidxml::xml_node<> *process_node = workflow_node->first_node("process");
         process_node != nullptr;
         process_node = process_node->next_sibling("process"))
    {
        std::string name = process_node->first_attribute("name")->value();
        std::vector<std::string> input_names;
        std::vector<RawChannel *> input_channels;
        for (rapidxml::xml_node<> *input_node = process_node->first_node("input");
             input_node != nullptr;
             input_node = input_node->next_sibling("input"))
        {
            input_names.push_back(input_node->value());
            input_channels.push_back(channels[input_node->value()]);
        }
        std::string output_name = process_node->first_node("output")->value();
        RawChannel *output_channel = channels[process_node->first_node("output")->value()];
        std::string script = process_node->first_node("command")->value();
        LOG(DEBUG, script);
        processes[name] = new Container(
            Container::config(
                {"nginx:alpine",
                 name,
                 input_names,
                 output_name,
                 script,
                 input_channels,
                 output_channel,
                 &output_mapping}));
    }
    return io_pair;
}

#endif