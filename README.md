# curl_tool_5000
this tool is currently in development. As of right now all the bot can do is send post requests given a curl command. 
#

# Usage
## Compiling
gcc -lcurl main.c --output main
##
## Running
 ./main [curl command], send a curl request repeatedly on 1 thread <br>
 ./main [curl command] -threads [thread amount], does the same as the pervious example but on multiple threads with a max of 30 threads. <br>
 ./main [curl command] -delay [seconds], casues each thread to wait between each curl request. <br>
 ### Type System
  ./main [curl command] -type [type int], sends a request that corresponds to the type int. <br>
  -type 20: sends a normal POST request with post data. <br>
  -type 21: sends a PATCH request with post data. <br>
  -type 22: sends a PUT request with post data. <br>
  -type 23: sends a POST request with no post data.
 ###
##
#

# "Roadmap" <br >
## Arguments System <br>
Different types of http requests O <br>
User set rate limits O <br>
Threading system O <br>
Verbose mode X <br>
##

## Terminal Design <br>
Header and information conformation screen X <br>
concurrent thread counting O <br>
##
#

# Know Bugs
 
#
