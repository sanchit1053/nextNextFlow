#ifndef _PARSER_H_
#define _PARSER_H_

#include "container.hpp"
#include "channel.hpp"
#include "logger.hpp"
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include <tuple>
#include <fstream>
#include <filesystem>

namespace Parser
{
    std::string IN = "in";
    std::string OUT = "out";
    std::pair<std::vector<RawChannel *>, RawChannel *> parse(const std::string &filename, std::unordered_map<std::string, RawChannel *> &channels, std::unordered_map<std::string, Mux *> &muxes, std::unordered_map<std::string, Container *> &processes, std::unordered_map<pid_t, Container *> &output_mapping);
};

std::pair<std::vector<RawChannel *>, RawChannel *> Parser::parse(const std::string &filename, std::unordered_map<std::string, RawChannel *> &channels, std::unordered_map<std::string, Mux *> &muxes, std::unordered_map<std::string, Container *> &processes, std::unordered_map<pid_t, Container *> &output_mapping)
{
    rapidxml::xml_document<> doc;
    rapidxml::file<> xmlFile(filename.c_str());
    doc.parse<0>(xmlFile.data());

    rapidxml::xml_node<> *workflow_node = doc.first_node("workflow");

    std::pair<std::vector<RawChannel *>, RawChannel *> io_pair;

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
                io_pair.first.push_back(channels[name]);
            }
            if (channel_node->first_node("global")->value() == Parser::OUT)
            {
                io_pair.second = channels[name];
            }
        }
        if (channel_node->first_node("init"))
        {
            channels[name]->push(channel_node->first_node("init")->value());
        }
    }

    for (rapidxml::xml_node<> *mux_node = workflow_node->first_node("mux");
         mux_node != nullptr;
         mux_node = mux_node->next_sibling("mux"))
    {
        std::string name = mux_node->first_attribute("name")->value();
        std::vector<RawChannel *> input_channels;
        std::vector<RawChannel *> output_channels;
        for (rapidxml::xml_node<> *input_node = mux_node->first_node("input");
             input_node != nullptr;
             input_node = input_node->next_sibling("input"))
        {
            input_channels.push_back(channels[input_node->value()]);
        }
        for (rapidxml::xml_node<> *output_node = mux_node->first_node("output");
             output_node != nullptr;
             output_node = output_node->next_sibling("output"))
        {
            output_channels.push_back(channels[output_node->value()]);
        }
        muxes[name] = new Mux(
            Mux::config(
                {input_channels,
                 output_channels}));
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
            assert(channels[input_node->value()] && "No channel found");
            input_channels.push_back(channels[input_node->value()]);
        }
        std::vector<std::string> output_names;
        std::vector<RawChannel *> output_channels;
        for (rapidxml::xml_node<> *output_node = process_node->first_node("output");
             output_node != nullptr;
             output_node = output_node->next_sibling("output"))
        {
            output_names.push_back(output_node->value());
            assert(channels[output_node->value()] && "No channel found");
            output_channels.push_back(channels[output_node->value()]);
        }
        std::string script;
        if (process_node->first_node("command"))
        {
            script = process_node->first_node("command")->value();
        }
        else if (process_node->first_node("script"))
        {
            std::ifstream fin;
            fin.open(process_node->first_node("script")->value());
            std::stringstream data;
            data << fin.rdbuf();
            script = data.str();
        }
        else
        {
            LOG(ERROR, "No command or script found");
        }
        LOG(DEBUG, script);
        processes[name] = new Container(
            Container::config(
                {"nginx:alpine",
                 name,
                 input_names,
                 output_names,
                 script,
                 input_channels,
                 output_channels,
                 &output_mapping}));
    }
    return io_pair;
}

#endif
