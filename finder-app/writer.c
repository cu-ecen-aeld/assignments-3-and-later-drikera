

#include <stdio.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

int main ( int argc, char *argv[] ) {

   FILE *file2write;

   int rc;

   openlog ( NULL, 0, LOG_USER );


   if ( argc != 3 ) {
      printf ( "Incorrect number of arguments specified.\n" );
      printf ( "Usage: ./writer ARG1 ARG2\n" );
      printf ( "  ARG1: full path to a file (including filename) on the filesystem\n" );
      printf ( "  ARG2: text string which will be written within this file\n" );

      syslog ( LOG_ERR, "Invalid number of arguments: %d", argc );

      rc = 1;

   } else { // right number of args provided

      syslog ( LOG_DEBUG, "Writing %s to %s", argv[2], argv[1] );

      file2write = fopen ( argv[1], "w" );

      if ( file2write == NULL ) {
         syslog ( LOG_ERR, "Error opening file: %s", strerror( errno ) );
         rc = 1;
      } else {
         fprintf ( file2write, argv[2] );
         fclose ( file2write );
         rc = 0;
      }
      
   }

   closelog();

   return rc;
}
