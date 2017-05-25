# WIFSS
_WIFSS Is a F*cking Simple Service_

## How to use it

Compile the whole project with: `make`  
... or just one side with: `make WClient` or `make WServer`

Run with:

` ./WClient`  and `./WServer`

## Commands

### List of commands currently available for Client

`help`  
`who`  
`send <message>`  
`whisper <idClient> <message>`  
`exit`  
`logout`  
`clear`  

### List of commands currently available for Server

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
