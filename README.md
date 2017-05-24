# WIFSS

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

1. You can try the system by connecting your client to a local server with `localhost` as address, once the server is running and listening.  

2. No data are saved by server, it's just a "bridge" between clients.
