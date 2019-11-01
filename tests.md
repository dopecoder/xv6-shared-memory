
# Tests for shared virtual memory system in Xv6

NOTE 1 : The testing code is available in svmtests.c under the src directory for reference.

NOTE 2 : The answer to ```Why a test is written?``` is answered under ```PURPOSE``` of output for each test. 

## ```1. void * getsharedpage(const char * key, int npages)```

### PARTITION 1 : Key
```
    VALID CASES
        -> 1. key is not null
        -> 2. key is a non empty string
    
    INVALID CASES
        -> 1. key is null
        -> 2. key is an empty string
```
1) Test for ```key = NULL```
    ```
    Testing Partition 1, test 1
    Purpose : This test, tests a type of invalid string
    Input => key : NULL, npages : 1
    Output => -2

    ```
1) Test for ```key = empty string```
    ```
    Testing Partition 1, test 2
    Purpose : This test, tests a type of invalid string
    Input => key : "", npages : 1
    Output => -2
    ```
1) Test for ```key = NON NULL valid string```
    ```
    Testing Partition 1, test 3
    Purpose : This test, tests for both a valid key and valid number of pages
    Input => key : test-1:3, npages : 5
    Output => 16384
    ```

### PARTITION 2 : Number Of Pages
```
    VALID CASES
        -> 1. npages is greater than 0
        -> 2. npages is less than the limit
    
    INVALID CASES
        -> 1. npages is negative
        -> 2. npages is zero
        -> 3. npages is greater than the limit
```

1) Test for ```npages = -ve```
    ```
    Testing Partition 2, test 1
    Purpose : This test, tests for an invalid npages arg, which is negative!
    Input => key : test-2:1, npages : -5
    Output => -3
    ```
1) Test for ```npages = 0```
    ```
    Testing Partition 2, test 2
    Purpose : This test, tests for an invalid npages arg, which is zero!
    Input => key : test-2:2, npages : 0
    Output => -3
    ```
1) Test for ```npages = 0 < val < limit``` (can test this with above 3rd test)
    ```
    Testing Partition 2, test 3
    Purpose : This test, tests for both an npages arg above the limit 1000, which is 1001!
    Input => key : test-2:3, npages : 1001
    Output => -3
    ```
1) Test for ```npages = < limit```
    ```
    Testing Partition 2, test 4
    This is already tested by Partition 1, test 3
    ```
<br/><br/><br/><br/>
## ```2. int freesharedpage(const char * key)```

### PARTITION 3 : Key
```
    VALID CASES
        -> 1. key is not null
        -> 2. key is a non empty string
        -> 3. key exists
        -> 4. key is allocated for the process

    INVALID CASES
        -> 1. key is null
        -> 2. key is an empty string
        -> 3. key does not exist
        -> 4. key is not allocated for the process
```

1) Test for ```key = NULL```
    ```
    Testing Partition 3, test 1
    Purpose : This test, tests a type of invalid string
    Input => key : NULL
    Output => -2
    ```
1) Test for ```key = empty string```
    ```
    Testing Partition 3, test 2
    Purpose : This test, tests a type of invalid string
    Input => key : ""
    Output => -2
    ```
1) Test for ```key = NON NULL valid string```
    ```
    Testing Partition 3, test 3
    Purpose : This test, tests for a valid key, tests if it is allocated for a key and also checks if it is     allocated for this process
    Input => key : test-3:3
    Output => 0
    ```
1) Test for ```freesharedpages``` with ```key not allocated```
    ```
    Testing Partition 3, test 4
    Purpose : This test, tests what happens if we try to free a key which was not allocated
    Input => key : test-3:4
    No shared memory associated with this key : test-3:4
    Output => -3
    ```
1) Test for ```freesharedpages``` with ```key not allocated for this process```
    ```
    Testing Partition 3, test 5
    Purpose : This test, tests what happens if we try to free a key which was not allocated
    Input => key : test-3:4
    Shared memory not associated with this process pid : 4
    Output => -4
    ```


### PARTITION 4 : Dependencies (Order of using the functions getsharedpage and freesharedpage in the right way)
```
    VALID
        -> Shared pages must be created by the parent first
        -> doing the same call getsharedpage should return the same 
            address twice
        -> freeharedpages should be called only once
    INVALID
        -> Shared pages are not created by the parent, and 
            the children will allocate and deallocate to end up
            always allocating and deallocating shared pages without
            actually using it.
        -> calling free more than once should throw an error
```

1) Test for using ```getsharedpage``` and ```freesharedpage``` with correct dependecies
    ```
    Testing Partition 4, test 1
    Purpose : This test, tests if getsharedpage and freeshared are used correctly with right    dependency
    Input => key : test-4:1
    Output => 
    Parent has written into shared page : Hello World from Parent!
    Child has written into shared page : Hello World from child!
    ```
1) Test for using ```getsharedpage``` and ```freesharedpage``` with incorrect dependecies
    ```
    Testing Partition 4, test 2
    Purpose : This test, tests if getsharedpage and freeshared are used incorrectly with wrong  dependency
    Input => key : test-4:2
    Output => 
    Child => Writing into shared page : Hello World from child!
    Parent => Child has written into shared page : 
    NOTE : The string is empty intentionally, as the child freed the shared page
             which it created first, this is the problem if the parent doesn't create the shared    page first
             (references becomes zero and the physical page gets allocated and deallocated everytime)
    ```
1) Test for calling ```gestsharedpage``` twice
    ```
    Testing Partition 4, test 3
    Purpose : This test, tests what happens when getsharedpage is called twice for the same process
    Input => key : test-4:3
    Output => 
    FIRST CALL : getsharedpage returned virtual address : B000
    SECOND CALL : getsharedpage returned virtual address : B000
    NOTE : Both the calls are returning the same address as they are not being allocated twice!
    ```
1) Test for calling ```freeharedpage``` twice
    ```
    Testing Partition 4, test 4
    Purpose : This test, tests what happens when freesharedpage is called twice for the same key and    process
    Input => key : test-4:4
    Output => 
    FIRST CALL : freesharedpage returned : 0
    No shared memory associated with this key : test-4:4
    SECOND CALL : freesharedpage returned : -3
    NOTE : Its returning -3 as freesharedpage has already freed the shared pages,
           It would return -4, if the references are not zero like if called twice from a child     process!
    ```
