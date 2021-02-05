

//The questions for this quiz are also embedded in bold in the code below:

 

//exercise.cpp - Test file for Quiz 3

//

// 26-Jan-19  M. Watler         Created.

//

#include <fcntl.h>

#include <stdlib.h>

#include <unistd.h>

 

int main(void)

{

    int fd1, fd2, fd3;

    const char file[]="Quiz3.txt";

 

    fd1 = open(file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    fd2 = dup(fd1);

    fd3 = open(file, O_RDWR);

//Question 1: Do fd1 and fd2 share the same file offset value?
//yes
 

 

//Question 2: Do fd1 and fd3 share the same file offset value?
//yes

 

    write(fd1, "Hello,", 6);

    write(fd2, "world", 5);

//Question 3: What will you see in Quiz3.txt at this point?

 

 

    lseek(fd2, 0, SEEK_SET);

   write(fd1, "HELLO,", 6);

//Question 4: What will you see in Quiz3.txt at this point?

 

 

   write(fd3, "Gidday", 6);

//Question 5: What will you see in Quiz3.txt at this point?

 

 

    close(fd1);

    close(fd2);

    close(fd3);

    return 0;

}

