#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int parse (char *line,char **argv)
//transform line of arrays to arguments
{
 int argc = 0; //arguments counter
 while (*line !='\0')
 {

   // ignoring spaces and tabs
   while (*line==' ' || *line=='\t')
   //separtors ==> terminators
   {
       *line ='\0';
       line++;
   }

   //save the pointer to the argument if the line is not enter or null
   if (*line != '\0' && *line != '\n')
   {
    *argv=line;
    argc++;
    argv++;
   }

   //scan input until reach tab, space or null
   while (*line !=' ' && *line != '\t' && *line != '\0' )
   {
     //handling double quote
      if (*line == '"') {
        line ++;
        //shift input untill end of quote or null or enter
        while (*line != '"' && *line != '\0' && *line != '\n')
        {
          *(line - 1) = *line;
          line ++;
        }
        //end of quote
        if (*line == '"'){
          *(line - 1) = '\0';
        }else {// end quote not found
          perror("missing \"\n" );
          return -1; //return as flag to check in main
        }
      }
      //enter is reached
      if (*line == '\n')
      {
        *line = '\0' ;
      }
      line ++ ;
   }
  }
 *argv = '\0' ;
 return argc;


}

void execute (char **argv, int is_foreground)
 {
//fork a child to do execvp

   pid_t pid =  fork();
   if (pid<0)
   //fork failed
   {
     perror("error , no fork\n");
   }
   else if (pid == 0)
   //in child process
   {
      execvp ( argv[0],  argv);
      //if returned from execvp then there is an error
      perror("fail to execute\n");
      exit (0);
   }
   else
   //in parent process
   {//in case of foreground, the parent waits for the child
      if (is_foreground == 1)
        waitpid (pid, NULL, 0);
   }
}

int main ()
{
  char line[514];
  char *argv[128];
  while (1)
  //main
  {

    printf("Shell>>");
    fgets(line,514,stdin); //scan the input

    //check for long command
    if (strlen (line) > 512)
    {
        printf("long command\n" );
          //skip unwanted characters longer than 512 till reach enter
        while (getchar () != '\n'){}
        continue;
    }

    int argc = parse(line,argv);
    // empty command or error command of quote
    if (argc == 0 || argc == -1)
    {
       continue;
    }
    //terminate if exit
    if (argc == 1 && strcmp (argv[0], "exit") == 0)
    {
      exit (0);
    }
//handling the & command only (special handling)
    if (argc == 1 && strcmp (argv[0], "&") == 0)
    {
      printf("Error commmand\n");
      continue;
    }

    int is_foreground = 1; //flag for foreground or background
    //compare last word if it is &
    if ((strcmp (argv[argc - 1], "&") == 0))
    {
      is_foreground = 0;
      argv[argc - 1] = '\0';
    }
    //compare last character of last word if it is &
    else if (argv[argc - 1][strlen (argv[argc - 1]) - 1] == '&')
    {
      is_foreground = 0;
      argv[argc - 1][strlen (argv[argc - 1]) - 1] = '\0';
    }
    //change directory command
    if(strcmp(argv[0],"cd") == 0)
       chdir(argv[1]);
    else
    {
       execute(argv, is_foreground);
    }
  }
  return 0;
}
