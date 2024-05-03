# NextNextFlow
By Vedang Asgaonkar (200050154) and Sanchit Jindal (200020120)

## Usage
- To compile the executable run `make`
- To run a workflow `./orchestrator <xml-file>`


## xml file format

There are 3 possible components 
- Channels
- Muxs
- Processes

They should be defined in the above order correct executions

#### Channel format
```xml
<channel name="{channel_name}">
    <global> [in|out] </global> <!-- if the channel acts as input or output to whole workflow -->
    <init> Starting text </int> <!-- Initial value in the channel -->
</channel>
```

#### Mux format
```xml
<mux name="{mux_name}">
    <input> {channel_name_1} </input>
    <input> {channel_name_2} </input> <!-- any number of input channels -->
    <output> {channel_name_2} </output> <!-- any number of output channels -->
</mux>
```

#### Process Format
```xml
<process name="{process_name}">
    <input> {channel_name_1} </input>
    <input> {channel_name_2} </input> <!-- any number of input channels -->
    <output> {channel_name_2} </output> <!-- any number of output channels -->

    <script> {path to script file} </script> <!-- We can give either a file or directly the commands -->
    <command> {Bash commands to run} </command>
```


