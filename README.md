# WIFSS
_WIFSS Is a F*cking Simple Service_

## How to use it

Compile the whole project with: `make`  
... or just one side with: `make WClient` or `make WServer`

Run with:

` ./WClient`  and `./WServer`

## Commands

### List of commands currently available for clients

`help`  
`who`  
`send <message>`  
`whisper <idClient> <message>`  
`exit`  
`clear`

### List of commands currently available for the server

`help`  
`who`  
`send <message>`  
`whisper <idClient> <message>`  
`disconnect <idClient> ['-1' = all]`  
`exit`  
`clear`

### Additional notes

1. The client now relies on `getAddrInfo()` to retrieve some additional information about the address you wanna connect to. So you may use `localhost` as host address to test this project in local, once the server is running and listening. 

2. No data are saved by server, it's just a "bridge" between clients.

### Why ?

Because at the very beginning, the project was about creating a software solution which could propose a "file transfer service", between some clients, relayed by an unique server.  
But _WIFSS_ original architecture was not designed to deal with "protocols" like that, so we've fallen back on a simple centralized chat service (like the v0.1 [never tagged] was in 2014 :confused:).

> And the real _why_ behind all this ?

Yeah sorry. This project has been created in order to learn how we can use the API socket in C (the real one we should say ?), and... despite everything, we think this is the case now :relieved:
