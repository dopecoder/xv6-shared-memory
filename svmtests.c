#include "types.h"
#include "user.h"

#define NULL ((void*)0)


void testgspnullkey(){
    char * test1key = NULL;
    int test1 = getsharedpage(test1key, 1);
    printf(1, "\nTesting Partition 1, test 1\n");
    printf(1, "Purpose : This test, tests a type of invalid string\n");
    printf(1, "Input => key : NULL, npages : 1\n");
    printf(1, "Output => %d\n", test1);
}

void testgspemptykey(){
    char * test2key = "";
    int test2 = getsharedpage(test2key, 1);
    printf(1, "\nTesting Partition 1, test 2\n");
    printf(1, "Purpose : This test, tests a type of invalid string\n");
    printf(1, "Input => key : \"\", npages : 1\n");
    printf(1, "Output => %d\n", test2);
}

void testgspvalidkey(){
    char * test3key = "test-1:3";
    int test3 = getsharedpage(test3key, 5);
    printf(1, "\nTesting Partition 1, test 3\n");
    printf(1, "Purpose : This test, tests for both a valid key and valid number of pages\n");
    printf(1, "Input => key : %s, npages : 5\n", test3key);
    printf(1, "Output => %d\n", test3);
}

void testgspnegnpages(){
    char * test1key = "test-2:1";
    int test1 = getsharedpage(test1key, -5);
    printf(1, "\nTesting Partition 2, test 1\n");
    printf(1, "Purpose : This test, tests for an invalid npages arg, which is negative!\n");
    printf(1, "Input => key : %s, npages : -5\n", test1key);
    printf(1, "Output => %d\n", test1);
}

void testgspzeronpages(){
    char * test2key = "test-2:2";
    int test2 = getsharedpage(test2key, 0);
    printf(1, "\nTesting Partition 2, test 2\n");
    printf(1, "Purpose : This test, tests for an invalid npages arg, which is zero!\n");
    printf(1, "Input => key : %s, npages : 0\n", test2key);
    printf(1, "Output => %d\n", test2);
}

void testgspvalidnpages(){
    char * test3key = "test-2:3";
    int test3 = getsharedpage(test3key, 1001);
    printf(1, "\nTesting Partition 2, test 3\n");
    printf(1, "Purpose : This test, tests for both an npages arg above the limit 1000, which is 1001!\n");
    printf(1, "Input => key : %s, npages : 1001\n", test3key);
    printf(1, "Output => %d\n", test3);
}

void testgspinvalidnpages(){
    printf(1, "\nTesting Partition 2, test 4\n");
    printf(1, "This is already tested by Partition 1, test 3\n");
}

void testfspnullkey(){
    char * test1key = NULL;
    int test1 = freesharedpage(test1key);
    printf(1, "\nTesting Partition 3, test 1\n");
    printf(1, "Purpose : This test, tests a type of invalid string\n");
    printf(1, "Input => key : NULL\n");
    printf(1, "Output => %d\n", test1);
}

void testfspemptykey(){
    char * test2key = "";
    int test2 = freesharedpage(test2key);
    printf(1, "\nTesting Partition 3, test 2\n");
    printf(1, "Purpose : This test, tests a type of invalid string\n");
    printf(1, "Input => key : \"\"\n");
    printf(1, "Output => %d\n", test2);
}

void testfspvalidkey(){
    char * test3key = "test-3:3";
    getsharedpage(test3key, 2);
    int test3fsp = freesharedpage(test3key);
    printf(1, "\nTesting Partition 3, test 3\n");
    printf(1, "Purpose : This test, tests for a valid key, tests if it is allocated for a key and also checks if it is allocated for this process\n");
    printf(1, "Input => key : %s\n", test3key);
    printf(1, "Output => %d\n", test3fsp);
}

void testfspnoalloc(){
    char * test4key = "test-3:4";
    printf(1, "\nTesting Partition 3, test 4\n");
    printf(1, "Purpose : This test, tests what happens if we try to free a key which was not allocated\n");
    printf(1, "Input => key : %s\n", test4key);
    int test4fsp = freesharedpage(test4key);
    printf(1, "Output => %d\n", test4fsp);
}

void testfspnoproc(){
    char * test5key = "test-3:4";
    getsharedpage(test5key, 2);
    printf(1, "\nTesting Partition 3, test 5\n");
    printf(1, "Purpose : This test, tests what happens if we try to free a key which was not allocated\n");
    printf(1, "Input => key : %s\n", test5key);
    int pid = fork();
    if(pid == 0){
        int test5fsp = freesharedpage(test5key);
        printf(1, "Output => %d\n", test5fsp);
        exit();
    }else if(pid > 0){
        wait();
    }else{
        printf(1, "\nTest Partition 3, test 5 failed to run as fork failed!\n");
    }
}

void testgspfsprightdeps(){

    char * test1key = "test-4:1";

    printf(1, "\nTesting Partition 4, test 1\n");
    printf(1, "Purpose : This test, tests if getsharedpage and freeshared are used correctly with right dependency\n");
    printf(1, "Input => key : %s\n", test1key);
    printf(1, "Output => \n");

    char *gspparent = (char*)getsharedpage(test1key, 1);
    strcpy(gspparent, "Hello World from Parent!");
    int pid1 = fork();
    if(pid1 == 0){
        char *gspchild = (char*)getsharedpage(test1key, 1);
        printf(1, "Parent has written into shared page : %s\n", gspchild);
        strcpy(gspchild, "Hello World from child!");
        freesharedpage(test1key);
        exit();
    }else{
        wait();
        printf(1, "Child has written into shared page : %s\n", gspparent);
        freesharedpage(test1key);
    }
    printf(1, "\n");
}

void testgspfspwrongdeps(){
    char * test2key = "test-4:2";
    printf(1, "\nTesting Partition 4, test 2\n");
    printf(1, "Purpose : This test, tests if getsharedpage and freeshared are used incorrectly with wrong dependency\n");
    printf(1, "Input => key : %s\n", test2key);
    printf(1, "Output => \n");

    int pid1 = fork();
    if(pid1 == 0){
        char *gspchild = (char*)getsharedpage(test2key, 1);
        strcpy(gspchild, "Hello World from child!");
        printf(1, "Child => Writing into shared page : %s\n", gspchild);
        freesharedpage(test2key);
        exit();
    }else{
        wait();
        char *gspparent = (char*)getsharedpage(test2key, 1);
        printf(1, "Parent => Child has written into shared page : %s\n", gspparent);
        printf(1, "NOTE : The string is empty intentionally, as the child freed the shared page\n \
        which it created first, this is the problem if the parent doesn't create the shared page first\n \
        (references becomes zero and the physical page gets allocated and deallocated everytime)\n");
        freesharedpage(test2key);
    }
    printf(1, "\n");
}

void testgspcalltwice(){
    char * test3key = "test-4:3";
    printf(1, "\nTesting Partition 4, test 3\n");
    printf(1, "Purpose : This test, tests what happens when getsharedpage is called twice for the same process\n");
    printf(1, "Input => key : %s\n", test3key);
    printf(1, "Output => \n");

    int gspparent = getsharedpage(test3key, 1);
    printf(1, "FIRST CALL : getsharedpage returned virtual address : %x\n", gspparent);
    gspparent = getsharedpage(test3key, 1);
    printf(1, "SECOND CALL : getsharedpage returned virtual address : %x\n", gspparent);
    printf(1, "NOTE : Both the calls are returning the same address as they are not being allocated twice!\n");
    freesharedpage(test3key);
    printf(1, "\n");
}

void testfspcalltwice(){
    char * test4key = "test-4:4";
    printf(1, "\nTesting Partition 4, test 4\n");
    printf(1, "Purpose : This test, tests what happens when freesharedpage is called twice for the same key and process\n");
    printf(1, "Input => key : %s\n", test4key);
    printf(1, "Output => \n");

    getsharedpage(test4key, 1); 
    int fspparent = freesharedpage(test4key);
    printf(1, "FIRST CALL : freesharedpage returned : %d\n", fspparent);
    fspparent = freesharedpage(test4key);
    printf(1, "SECOND CALL : freesharedpage returned : %d\n", fspparent);
    printf(1, "NOTE : Its returning -3 as freesharedpage has already freed the shared pages,\n \
      It would return -4, if the references are not zero like if called twice from a child process!\n");
    printf(1, "\n");
}

int main(int argc, char *argv[]) {

    // Partition 1
    testgspnullkey();
    testgspemptykey();
    testgspvalidkey();

    // Partition 2
    testgspnegnpages();
    testgspzeronpages();
    testgspvalidnpages();
    testgspinvalidnpages();

    // Partition 3
    testfspnullkey();
    testfspemptykey();
    testfspvalidkey();
    testfspnoalloc();
    testfspnoproc();

    // Partition 4
    testgspfsprightdeps();
    testgspfspwrongdeps();
    testgspcalltwice();
    testfspcalltwice();

    exit();
}