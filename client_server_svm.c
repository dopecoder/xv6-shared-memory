
#include "types.h"
#include "user.h"

int main(){
    char * key = "Test Key";
    printf(1, "Client Server Demo using shared memory\n");
    getsharedpage(key, 1);
    int pid1 = fork();
    if(pid1 == 0){
        int pid2 = fork();
        if(pid2 == 0){
            char *gspclinet = (char*)getsharedpage(key, 1);
            printf(1, "%s\n", "CLIENT : Sending a Request to Server");
            while(strcmp("Hello World from Server!", gspclinet) != 0){}
            printf(1, "CLIENT : Server has Responded with : %s\n", gspclinet);
            printf(1, "%s\n", "CLIENT : Sending back Request to server!");
            strcpy(gspclinet, "Hello World from Client!");
            freesharedpage(key);
        }else{

            char *gspserver = (char*)getsharedpage(key, 1);
            strcpy(gspserver, "Hello World from Server!");
            while(strcmp("Hello World from Client!", gspserver) != 0){}
            printf(1, "SERVER : Client has sent a Reqeust : %s\n", gspserver);
            freesharedpage(key);
        }
    }else{
        wait();
        freesharedpage(key);
    }
    exit();
}