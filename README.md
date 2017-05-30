# WIFSS
_WIFSS Is a F*cking Simple Service_

## Wahat do I have to install and how can I use it ?

1.  `#  aptitude install gcc make openssl libssl-dev`

2. * `$  openssl genrsa -des3 -out server.temp.key 4096`
   * `$  openssl rsa -in server.temp.key -out server.key && rm server.temp.key`
   * `$  openssl req -new -key server.key -out server.csr`
   * `$  openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt`
   * `$  cp server.crt client.crt`

3. `$  make`

4. `$  ./WServer`

5. `$  ./WClient`

## Commands available...

### ... for the clients

`help`  
`who`  
`send <message>`  
`whisper <idClient> <message>`  
`exit`  
`clear`

### ... for the server

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

3. Do not hesitate to open a new **issue** if you encounter any problem or any case not yet handled by _WIFSS_.

## What we had to say

### Why ?

> Because at the very beginning, the project was about creating a software solution which could propose a "file transfer service", between some clients, relayed by an unique server.  
But _WIFSS_ original architecture was not designed to deal with "protocols" like that, so we've fallen back on a simple centralized chat service (like the v0.1 [never tagged] was in 2014 :confused:).

### And the real _why_ behind all this ?

> Yeah sorry. This project has been created in order to learn how we can use the API socket in C ("The real one", should we say ?), and... despite everything, we think this is the case now :relieved:
