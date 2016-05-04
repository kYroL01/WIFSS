# WIFSS
_WIFSS Is not a File Sharing System_  

## How to use it  

Compile the whole project with: ```make ```  
... or just one side with: ```make SERVER ``` or ```make CLIENT ```  

Run with:  
``` ./clientBin ```  and ```./serverBin ``` 

## Commands  

### List of commands currently available for Client:  
```? ```  
```help ```  
```send <message> ```  
```whisper <idClient> <message> ```  
```list ```  
```rename <file> <newFileName> ```  
```remove <file> ```  
```ispresent <idClient> <file> ```  
```asklist <idClient> ```  
```quit ```  
```exit ```  
```logout ```  
```clear ```  
```close ```  
```download <idClient> <file> ```  
```checkfolder ```  

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
