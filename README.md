# WIFSS
_WIFSS Is not a File Sharing System_  

## How to use it  

Compile with:  
```gcc client/src/*.c -lpthread -Iclient/inc/ -o client.out && gcc server/src/*.c -lpthread -Iserver/inc/ -o server.out```  
Run with:  
``` ./client.out```  and ```./server.out``` 

## Commands  

### List of commands currently available for Client:  
```? ```  
```help ```  
```send <message> ```  
```whisper <idClient> <message> ```  
```quit ```  
```exit ```  
```logout ```  
```clear ```  
```close ```  
```download <idClient> <file> ```  

### List of commands currently available for Server:  
```? ```  
```help ```  
```send <message> ```  
```whisper <idClient> <message> ```  
```disconnect <idClient> ['-1' = all] ```  
```quit ```  
```exit ```  
```halt ```  
```stop ```  
```close ```  
```clear ```  

### Additionnal content  

You can try the system by connecting your client to your local server with ```localhost ``` or ```local ``` as address.