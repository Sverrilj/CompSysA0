#include <stdio.h>  // fprintf, stdout, stderr.
#include <stdlib.h> // exit, EXIT_FAILURE, EXIT_SUCCESS.
#include <string.h> // strerror.
#include <errno.h>  // errno.



int is_ascii(FILE *file){

  unsigned char byte;

  while (fread(&byte, 1, 1, file) ==1){

    if ((byte>>7)!=0){

      return 0;

      }
    }
  
  return 1;

}

int main(int argc , char *argv[]) {
  if (argc < 2){
      printf("Usage: %s <filename>\n", argv[0]);
      return EXIT_FAILURE;
  }


  FILE *file = fopen(argv[1], "rb");

  if (!file) { 

      perror("Error opening file");
      return EXIT_FAILURE;
  }

  fseek(file,0,SEEK_END);
  long file_size = ftell(file);
  if (file_size == 0){
      printf("the file is empty.\n");
      fclose(file);
      return EXIT_SUCCESS;
  }
  rewind(file);

  if (is_ascii(file)) {
    printf("the file contains only ascii characters.\n");
    
  }
  else {
    printf("the file contains non-ascii characters.\n");
  }

  fclose(file);

  return EXIT_SUCCESS;

}
