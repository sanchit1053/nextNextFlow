#include "logger.hpp"

int main(int argc, char *argv[])
{
    // arguments specify the input filename
    if (argc != 2)
    {
        LOG(ERROR, "Correct usage: orchestrator <file-name>");
        exit(1);
    }

    // parse xml file to get scripts of all functions and channels to be created

    // create channel queues
    // queues need to have data types. Not everything can be dealt with as a stream of values, need to allow for aggregate types as well
    // maybe allow an argument in the queue, that sets the size of its memory?
    // might also need a demux for the queues: Can we attach the output of a queue to the input of multiple others?

    while (true)
    {
        // listen for any running containers that terminate. That means they are done outputting, put their output into the right channel
        // listen for global input, and put it into the correct queue
        
        // see if any function can be called, i.e. all its input queues have data
    }
}