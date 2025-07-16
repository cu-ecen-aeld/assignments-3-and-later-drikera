#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "systemcalls.h"

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
   //pid_t pid;
   int wstatus;
   
   wstatus = system ( cmd );

   if ( wstatus == -1 ) {
      // child process could not be created
      return false;
   }
   if ( cmd == NULL ) {
      // null command
      return false; 
   }

   //pid = wait ( &wstatus );

   //if ( pid == -1 ) {
   //   printf("\n\n\nFailed here\n\n\n");
   //   return false;
   //}


   //printf("\n\n\nThis is the     wstatus: %d\n\n\n", wstatus );
   //printf("\n\n\nExited:                  %s\n\n\n", (WIFEXITED ( wstatus ) ? "true" : "false") ); 
   //printf("\n\n\nThis is the exit status: %d\n\n\n", WEXITSTATUS ( wstatus ) );
   //printf("\n\n\nTerm by a signal         %s\n\n\n", (WIFSIGNALED ( wstatus ) ? "true" : "false") ); 
   //printf("\n\n\nThis is the signal:      %d\n\n\n", WTERMSIG ( wstatus ) );

   //return WIFEXITED ( wstatus );

   if ( WIFEXITED ( wstatus ) != 0 ) {
      if ( WEXITSTATUS( wstatus ) == 0 ) {
         return true;
      }
      else {
         return false;
      }
      //else if ( WEXITSTATUS( wstatus ) != 0 ) {
      //   return false;
      //}
   } else {
      return false;
   }

}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

/*
 * TODO:
 *   Execute a system command by calling fork, execv(),
 *   and wait instead of system (see LSP page 161).
 *   Use the command[0] as the full path to the command to execute
 *   (first argument to execv), and use the remaining arguments
 *   as second argument to the execv() command.
 *
*/

   pid_t child_pid;
   int wstatus;
   int rc;

   rc = 0;
   if ( command[0][0] != '/' ) {
      printf("not absolute path\n\n");
      return false;
   }

   child_pid = fork ();


   if ( child_pid == 0 ) {
      // child
      rc = execv ( command[0], command) ;
      if ( rc == -1 ) {
         return false;
      }
   } else {
      // parent
      printf("\nCommand[0]: %s\n", command[0]);
      waitpid (child_pid, &wstatus, 0 );
      if ( WIFEXITED ( wstatus ) != 0 ) {
         if ( WEXITSTATUS ( wstatus ) == 0 ) {
            printf("test complete\n");
            return true;
         } else {
            printf("exited with status non0\n");
            printf("test failed 1\n");
            return false;
         }
      } else {
         printf("test failed 2\n");
         return false;
      }
   } 

   va_end(args);

   printf("test failed 3\n");
   return false;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];


/*
 * TODO
 *   Call execv, but first using https://stackoverflow.com/a/13784315/1446624 as a refernce,
 *   redirect standard out to a file specified by outputfile.
 *   The rest of the behaviour is same as do_exec()
 *
*/
   int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644 );

   if ( fd < 0 ) {
      return false;
   }

   pid_t childpid;
   int rc;
   int wstatus;

   childpid = fork();

   if ( childpid == -1 ) {
      // fork error
      printf("Fail 1\n");
      return false;
   }
   else if ( childpid == 0 ) {
      // child
      if ( dup2( fd, 1 ) < 0 ) {
         // error with dup2
         printf("Fail 2\n");
         return false;
      }
      close ( fd );
      //rc = execvp("/bin/echo", &command[1] );
      //printf("\n\n\ncommand[0]: %s\n\n\n", command[0] );
      rc = execvp ( command[0], command );
      if ( rc == -1 ) {
         return false;  
      }
   }
   else {
      // parent
      close(fd);
      wait ( &wstatus );
      if ( WIFEXITED ( wstatus ) != 0 ) {
         printf("Exited\n");
         if ( WEXITSTATUS ( wstatus ) == 0 ) {
            printf("Exited with 0\n");
            return true;
         } else {
            printf("Exited with non0\n");
            return false;
         }
      } else {
         return false;
      }
   }


   va_end(args);

   return false;
}
