# WIFSS
_WIFSS Is not a File Sharing System_  

## How to use it  

Compile with:  
```gcc client/src/*.c -lpthread -Iclient/inc/ -o client.out && gcc server/src/*.c -lpthread -Iserver/inc/ -o server.out```  
Run with:  
``` ./client.out```  and ```./server.out``` 

## Commands  

List of functions available:  
```? ```  
```help ```  
```send <message> ```  
```sendp <idClient> <message> ```  
```quit ```  
```exit ```  
```logout ```  
```clear ```  
```tunnel <idClient> ```  
```download <file> <idClient> ```  

### Additionnal content  

You can try the system by connecting your client to your local server with ```localhost ``` or ```local ``` as address.