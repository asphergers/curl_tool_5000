# curl_tool_5000
this tool is currently in development. As of right now all the bot can do is send post requests given a curl command. 
#

# Usage
## Compiling
gcc -lcurl main.c --output main
##
## Running
 ./main [curl command], to run to send out the curl request repeatedly on 1 thread <br>
 ./main [curl command] -threads [thread amount], does the same as the pervious example but on multiple threads with a max of 30 threads.
##
#

# "Roadmap" <br >
## Arguments System <br>
Different types of http requests X <br>
User set rate limits X <br>
Threading system O <br>
Verbose mode X <br>
##

## Terminal Design <br>
Header and information conformation screen X <br>
concurrent thread counting X <br>
##
#

# Know Bugs
 
#
