// FILE - testgsp.c

#include "types.h"
#include "user.h"

#define NULL ((void *)0)

// struct news{
//     int test;
// };

// int main(int argc, char *argv[])
// {
//   char *test = "hello";
//   void *addr = (void *)getsharedpage("Hello", 10);
//   struct news *news1 = (struct news *)addr;

//   news1->test = 1;
//   printf(1, "Address : %p\n", addr);
//   printf(1, "String address : %p\n", test);
//   printf(1, "news1 test : %d\n", news1->test);
//   exit();
// }

// typedef struct{
//     // char * name;
//     int age;
// }student_info;

// int main(int argc, char *argv[])
// {
//     void *parent_addr = (void *)getsharedpage("Hello", 1);
//     int * test1 = (int *)parent_addr;
//     *test1 = 1000;
//     printf(1, " 1 : %d\n", *test1);

//     int pid = fork();
//     if(pid < 0){
//         printf(1, "Failed to fork, exiting...\n");
//         exit();
//     }
//     if(pid == 0){
//         // void *child_addr =
//         // (void *)
//         // getsharedpage("Hello", 1);
//         // char * test = (char *)child_addr;
//         // printf(1, " 3 : %c\n", *test);
//         // student_info * st_info_child = (student_info *)child_addr;
//         // // st_info_child->name = "Nithin Rao";
//         // st_info_child->age = 23;
//         // printf(1, "Student Age : %d\n", st_info_child->age);
//         printf(1, "Assigned\n");
//     }else{
//         wait();
//         // student_info * st_info = (student_info *)parent_addr;
//         // printf(1, "Student Name : %s\n", st_info->name);
//         // printf(1, "Student Age : %d\n", st_info->age);

//         printf(1, " 2 : %d\n", *test1);

//     }
//   exit();
// }

// int main(int argc, char *argv[])
// {
//     void *parent_addr = (void *)getsharedpage("Hello", 1);
//     int * test1 = (int *)parent_addr;
//     *test1 = 1000;
//     printf(1, " 1 : %d\n", *test1);
//     printf(1, " 1.1 : %x\n", test1);

//     int pid = fork();
//     if(pid < 0){
//         printf(1, "Failed to fork, exiting...\n");
//         exit();
//     }
//     if(pid == 0){
//         printf(1, " 2 : %d\n", *test1);
//         printf(1, " 2.1 : %x\n", test1);
//         void *child_addr = (void *)getsharedpage("Hello", 1);
//         int * test = (int *)child_addr;
//         printf(1, " 3 : %d\n", *test);
//         printf(1, " 3.1 : %x\n", test);
//         *test = 2000;
//         printf(1, " 4 : %d\n", *test);

//         // void *child_addr2 = (void *)getsharedpage("Hello", 1);
//         // int * test2 = (int *)child_addr2;
//         // printf(1, " 2 : %d\n", *test2);
//         // printf(1, "Assigned\n");
//         if(freesharedpage("Hello")){
//             printf(1, "freesharedpage call succeeded!\n");
//         }
//     }else{
//         wait();
//         void *parent_addr = (void *)getsharedpage("Hello", 1);
//         int * test1 = (int *)parent_addr;
//         printf(1, " 5 : %d\n", *test1);
//         printf(1, " 6 : %x\n", test1);
//     }
//   exit();
// }

int main(int argc, char *argv[]) {
  // void *parent_addr = (void *)getsharedpage("Hello", 1);
  // int * test1 = (int *)parent_addr;
  // *test1 = 1000;
  // printf(1, " 1 : %d\n", *test1);
  // printf(1, " 1.1 : %x\n", test1);

  int p = getsharedpage("HelloWorld", 2);
  int *test= (int *)p;
  *test = 2;
  int t = 100;
  int *x = &t;
  printf(1, "Parent : %x, %d, %x, %d\n", test, *test, x, *x);

  int pid1 = fork();
  if (pid1 < 0) {
    printf(1, "Failed to fork(1), exiting...\n");
    exit();
  }
  if (pid1 == 0) {
    int pid2 = fork();
    if (pid2 < 0) {
      printf(1, "Failed to fork(2), exiting...\n");
      exit();
    }
    if (pid2 == 0) {
      p = getsharedpage("HelloWorld", 2);
      test = (int *)p;
      *test = 30;
      *x = 130;
      printf(1, "Grand Child : %x, %d, %x, %d\n", test, *test, x, *x);
      if (freesharedpage("HelloWorld")==0) {
        printf(1, "freesharedpage(1) call succeeded!\n");
      }
    } else {
      wait();
      p = getsharedpage("HelloWorld", 2);
      test = (int *)p;
      // *test = 40;
      *x = 140;
      printf(1, "Child : %x, %d, %x, %d\n", test, *test, x, *x);
      if (freesharedpage("HelloWorld")==0) {
        printf(1, "freesharedpage(2) call succeeded!\n");
      }
    }

  } else {
    wait();
    p = getsharedpage("HelloWorld", 2);
    test = (int *)p;
    printf(1, "Parent : %x, %d, %x, %d\n", test, *test, x, *x);
    if (freesharedpage("HelloWorld")==0) {
      printf(1, "freesharedpage(3) call succeeded!\n");
    }
  }
  exit();
}