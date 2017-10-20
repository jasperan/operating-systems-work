/*-
* msh.c
*
* Minishell C source
* Show how to use "obtain_order" input interface function
*
* THIS FILE IS TO BE MODIFIED
*/

#include <stddef.h>			/* NULL */
#include <stdio.h>			/* setbuf, printf */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>	/*Header file for system call fstat*/
#include <sys/stat.h>
#include <dirent.h> 	/*Header file for system call opendir, closedir,readdir...*/


#define BUFFER_SIZE 1024

extern int obtain_order();		/* See parser.y for description */


void mycalc(int num1, char * op, int num2);
void mybak(char* file, char* path);

int main(void)
{
    char ***argvv;
    int command_counter;
    int num_commands;
    int args_counter;
    char *filev[3];
    int bg;
    int ret;
    int fd1, fd2, fd3;

    setbuf(stdout, NULL);			/* Unbuffered */
    setbuf(stdin, NULL);

    while (1)
    {
        fprintf(stderr, "%s", "msh> ");	/* Prompt */
        ret = obtain_order(&argvv, filev, &bg);
        if (ret == 0) break;		/* EOF */
        if (ret == -1) continue;	/* Syntax error */
        num_commands = ret - 1;		/* Line */
        if (num_commands == 0) continue;	/* Empty line */


        int status;
        int code;


        for (command_counter = 0; command_counter < num_commands; command_counter++)
        {
            while (argvv[command_counter][args_counter] != NULL)
            {
                args_counter++; // Get number of arguments + the command (means, we will always have +1)
            } // end while

            // After the counting has been performed, we now proceed to decode the type of operation the user wants. (one, two or three operations.)
            /*
            We had tried a lot of time to try to implement the minishell with 'n' commands, but finally we couldn't figure out what to do in order to communicate n processes. The issue that we had mainly is that we couldn't distinguish if we were at the start of the process tree or the end.
            */
        }

        if (num_commands == 1)
        {
            if (strcmp(argvv[0][0], "mycalc") == 0)
            {
                if (args_counter == 4)
                {
                    mycalc(atoi(argvv[0][1]), argvv[0][2], atoi(argvv[0][3]));
                    continue;
                }
                else
                {
                    perror("[ERROR] The structure of the command is <operand 1><add/mod><operand 2>\n");
                    continue;
                }
            }
            if (strcmp(argvv[0][0], "mybak") == 0)
            {
                if (args_counter == 3)
                {
                    mybak(argvv[0][1], argvv[0][2]);
                    continue;
                }
                else
                {
                    perror("[ERROR] The structure of the command is mybak <original file><output directory>\n");
                    continue;
                }
            }
            pid_t pchild = fork();
            if (pchild == -1) perror("Error creating the child\n");
            // Child case:
            else if (pchild == 0)
            {
                if (filev[0] != NULL) // In case the user has specified standard input redirection.
                {
                    /*
                    The strategy followed here, and in the whole file (also for filev[1] and filev[2]) is that the open takes the smallest available file descriptor possible. So, if we close the standard input (0)
                    the file will automatically take its place here.
                    We could also do something like:
                    close(0);
                    dup(fd1);
                    close(fd1);
                    after opening the file (it does exactly the same), but this is a little more tedious, and we believe our method is more elegant.
                    */
                    close(0); // Close standard input
                    fd1 = open(filev[0], O_RDONLY, 0777); // Takes smallest file descriptor available, in this case, 0.
                    if (fd1 == -1)
                    {
                        perror("Error opening the redirection file for the standard input.");
                            break; // The decision of using break insteas of 'return' is explained in the report in more detail.
                    }
                }
                if (filev[1] != NULL) // In case the user has specified standard output redirection.
                {
                    close(1);
                    fd2 = open(filev[1], O_CREAT | O_TRUNC | O_WRONLY, 0777);
                    if (fd2 == -1)
                    {
                        perror("Error opening or creating the redirection file for the standard output.");
                            break;
                    }

                }
                if (filev[2] != NULL) // In case the user has specified standard error redirection.
                {
                    close(2);
                    fd3 = open(filev[2], O_CREAT | O_TRUNC | O_WRONLY, 0777);
                    if (fd3 == -1)
                    {
                        perror("Error opening or creating the redirection file for the standard error.");
                            break;
                    }
                }
                //printf("Correctly created child.\n");

                //printf("Number of parameters: %d\n", args_counter);

                // We execute the instruction, and check for errors in exec.
                code = execvp(argvv[0][0], argvv[0]);
                if (code == -1)
                {
                    perror("Error in exec");
                    break;
                }
            } // end else if

            if (bg == 0) // In case the program is running in background (background == 1), then we don't call wait (the parent won't wait for the child) since it will run in background.
            {
                while (wait(&status) != pchild)
                {
                    if (status == -1)
                    {
                        perror("The child didn't exit successfully.\n");
                        break;
                    } // end if inside while
                } // end while
            } // END if bg
        }












        else if(num_commands == 2)
        {

            if(filev[2] != NULL) // We redirect the standard error if the user has said so. Standard input and output are left out of the picture since they will be redirected through pipes from one to the other.
            {
                close(2);
                fd3=open(filev[2], O_CREAT | O_WRONLY | O_TRUNC, 0777);
                if(fd3 == -1)
                {
                    perror("Error opening or creating the redirection file");
                    break;
                }

            }
            int pipeline[2]; // We create the communication pipeline between process one (command one) and process two (command two)
            pipe(pipeline); // We declare it as a pipe.
            int pchild1 = fork(); // We create the process, and check for errors in the fork.
            if (pchild1 == -1)
            {
                perror("Error creating the first child");
                break;
            }
            if (pchild1 == 0) // In case of the child (the first process) then we check if it has standard input redirection. If it does, we redirect it.
            {
                if(filev[0] != NULL) // We close the standard input, and we establish the file as the new standard input. This can also be done with dup2 or dup, but as we have said before, this method is more elegant.
                {
                    close(0);
                    fd1 = open(filev[0], O_RDONLY, 0777);
                    if (fd1 == -1)
                    {
                        perror("Error opening the redirection file for the standard input");
                            break;
                    }


                }
                /*
                For the pipeline: we establish the output of the first process to the start of the pipe. Later, we will 'attach/assign' the input of the second process to the end of the pipe. (in the second process.)
                */
                close(pipeline[0]);
                close(1);
                dup(pipeline[1]);
                close(pipeline[1]);

                //printf("Correctly created child.\n");
                //printf("Number of parameters: %d\n", args_counter);


                // We execute the first command, which can be found in the first argument of argvv, in the first position of its corresponding array.
                code = execvp(argvv[0][0],argvv[0]);
                if (code == -1) // We check for an error in the exec and print it, if it happens.
                {
                    perror("Error in exec");
                    break;
                }
            }
            if(bg == 0) // In case that the user has asked for foreground execution, then the parent shall wait for the child. If during this process we find an error, we print that the child didn't exit successfully, and break.
            {
                while(wait(&status) != pchild1)
                {
                    if (status == -1)
                    {
                        perror("The first child didn't exit successfully");
                        break;
                    } // end if inside while
                } // end while
            }

            // Now we create a second child, which will be the one receiving the output from the first process on its input.
            pid_t pchild2 = fork();
            if (pchild2 == -1)
            {
                perror("Error creating second the child");
                break;
            }
            if (pchild2 == 0)
            {
                if(filev[1] != NULL) // We redirect the output in case the user has specified a different output.
                {
                    close(1);
                    fd2=open(filev[1], O_CREAT | O_WRONLY | O_TRUNC, 0777);
                    if(fd2  == -1){
                        perror("Error creating or creation the redirection file for the standard output.");
                            break;
                    }
                }
                close(0); // Here, what we do is assign the input of the second process to the start of the pipe, in order to receive the input from the other process through this pipe.
                dup(pipeline[0]);
                close(pipeline[0]);
                close(pipeline[1]);
                //printf("Correctly created child.\n");
                //printf("Number of parameters: %d\n", args_counter);

                // Here, we execute the second instruction, which can be found in the second argvv position, with its corresponding set of arguments.
                code = execvp(argvv[1][0], argvv[1]);
                if (code == -1)
                {
                    perror("Error in exec");
                    break;
                }
            }
            /*
            We had so many problems trying to figure out why it didn't work. The issue were these two following lines of code. Without this, the output is not correct, since it is otherwise attached still to the pipe. We closed
            the pipeline (both ends of it) and then the issue was solved immediately.
            This can also be included as a switch structure, just like it's done in the case (num_commands == 3). However, it's not deemed necessary in this case since we want always to close the pipeline in the default case, so
            this part must always be executed.
            */
            close(pipeline[0]);
            close(pipeline[1]);

            if(bg == 0) // If foreground, wait for child.
            {
                while(wait(&status) != pchild2)
                {
                    if (status == -1)
                    {
                        perror("The child didn't exit successfully");
                        break;
                    }
                }

            }



            /*
            The explanation for the third case is very similar to the second so many comments are omitted. Only new stuff is commented.
            */

        }









        else if (num_commands == 3)
        {
            if(filev[2] != NULL) // We redirect the standard error if the user has said so. Standard input and output are left out of the picture since they will be redirected through pipes from one to the other.
            {
                close(2);
                fd3=open(filev[2], O_CREAT | O_WRONLY | O_TRUNC, 0777);
                if(fd3 == -1)
                {
                    perror("Error opening or creation the redirection file");
                    break;
                }
            }
            int pipeline1[2], pipeline2[2]; // We create two pipes, the first one to communicate P1 with P2, and the second one for P2 and P3.
            pipe(pipeline1); // We declare them as pipes.
            pipe(pipeline2);
            pid_t pchild1 = fork(); // We create the process, and check for errors in the fork.
            switch(pchild1)
            {
                case -1:
                perror("Error creating the first child");
                break;
                case 0:
                if(filev[0] != NULL) // We close the standard input, and we establish the file as the new standard input. This can also be done with dup2 or dup, but as we have said before, this method is more elegant.
                {
                    close(0);
                    fd1 = open(filev[0], O_RDONLY, 0777);
                    if (fd1 == -1)
                    {
                        perror("Error creating the redirection file for the standard input");
                            break;
                    }

                }
                /*
                For the first pipeline: we establish the output of the first process to the start of the pipe.
                */
                close(1);
                dup(pipeline1[1]);
                close(pipeline1[1]);
                close(pipeline1[0]);

                code = execvp(argvv[0][0],argvv[0]);
                if (code == -1)
                {
                    perror("Error in exec");
                    break;
                }

                default:
                if(bg == 0)
                {
                    while(wait(&status) != pchild1)
                    {
                        if (status == -1)
                        {
                            perror("The first child didn't exit successfully");
                            break;
                        } // end if inside while
                    } // end hwile
                }

            }

            // Now, for the second child:
            pid_t pchild2 = fork();
            switch(pchild2)
            {
                case -1:
                perror("Error creating the second child");
                break;
                case 0:
                // Here, no redirection is possible since it's the intermediate process.
                close(0); // Here, what we do is assign the input of the second process to the start of the pipe, in order to receive the input from the other process through this pipe.
                dup(pipeline1[0]);
                close(pipeline1[0]);
                close(pipeline1[1]);
                // And finally, we assign the output of the second process to the end of the pipe, in order for the third process to receive the input from this end of the pipe.
                close(1);
                dup(pipeline2[1]);
                close(pipeline2[1]);
                close(pipeline2[0]);
                // We execute the second instruction again and check for errors:
                code = execvp(argvv[1][0], argvv[1]);
                if (code == -1)
                {
                    perror("Error in exec");
                    break;
                }


                default:
                // We close to avoid errors as it has been explained before.
                close(pipeline1[0]);
                close(pipeline1[1]);
                if(bg == 0) // If foreground, wait for child again:
                {
                    while(wait(&status) != pchild2)
                    {
                        if (status == -1)
                        {
                            perror("The child didn't exit successfully");
                            break;
                        } // end status check
                    } // end while wait

                }   // end if bg
            }

            pid_t pchild3 = fork(); // Finally, we create the third process, and check for errors in the fork.
            switch(pchild3)
            {
                case -1:
                perror("Error creating the first child");
                break;
                case 0:
                if(filev[1] != NULL) // We close the standard output, and we establish the file as the new standard output.
                {
                    close(1);
                    fd2 = open(filev[1], O_CREAT | O_WRONLY | O_TRUNC, 0777);
                    if (fd2 == -1)
                    {
                        perror("Error creating the redirection file for the standard output");
                            break;
                    }

                }
                // Finally, we assign the standard input for the third command to the second pipeline's end (the end of the second process).
                close(0);
                dup(pipeline2[0]);
                close(pipeline2[0]);
                close(pipeline2[1]);



                code = execvp(argvv[2][0],argvv[2]); // Execute the last command and check for errors.
                if (code == -1)
                {
                    perror("Error in exec");
                    break;
                }

                default:
                /* Again, we close the pipeline to prevent errors. */
                close(pipeline2[0]);
                close(pipeline2[1]);
                if(bg == 0)
                {
                    while(wait(&status) != pchild3)
                    {
                        if (status == -1)
                        {
                            perror("The third child didn't exit successfully");
                            break;
                        } // end if inside while
                    } // end hwile
                } // end if bg == 0

            } // END SWITCH

        } // end big else if (case 3)
        else // In case the user inputs more than 4 commands. This part would have to be omitted for a generic amount of commands.
        {
            perror("Invalid number of commands");
        }


    } // end while(1)


    return 0;

} //end main

void mycalc(int num1, char * op, int num2)
{
    if (setenv("Acc","0", 0) < 0) // If it does not initially exist, sets to 0. Otherwise, it does not overwrite
    {
        perror("Error setting the environment variable");
        return;
    }


    if (strcmp(op, "add") == 0)
    {
        int result = num1 + num2;
        int newval = atoi(getenv("Acc"))+result;

        char buffer[12]; // Enough size (and more) to support an integer's max value.
        // Can't be done with strcpy since we need to pass as argument the value of the variable:
        sprintf(buffer, "%d", newval);

        if (setenv("Acc", buffer, 1) <0) // Put the new value.
        {
            perror("Error modifying the value of the environment variable in the addition operation");
            return;
        }
        printf("[OK] %d + %d = %d; Acc %s\n", num1, num2, result, buffer);
    }
    else if (strcmp(op, "mod") == 0)
    {
        int quotient = num1 / num2;
        int remainder = num1 % num2;
        printf("[OK] %d %% %d = %d * %d + %d\n", num1, num2, num2, quotient, remainder);
    }
    else
    {
        printf("[ERROR] The structure of the command is <operand 1><add/mod><operand 2>\n");
        return;
    }


} // end mycalc.

void mybak(char* file, char* path)
{
    int fd = open(file, O_RDONLY, 0777);
    char readbuffer[1024];
    if (fd == -1)
    {
        perror("[ERROR] Error opening original file");
        return;
    }


    DIR * directory;

        directory = opendir(path);
        if (directory == NULL)
        {
            perror("Error opening directory");
            return;
        }

    struct dirent * dp = readdir(directory);
    while (dp != NULL)
    {
        if (strcmp(dp->d_name, file) == 0) // In case the file already exists in that directory.
        {
            char buffer[2048]; // 2048 considered the longest path name.
            sprintf(buffer, "%s/%s", path, dp->d_name);
            int fdaux = open(buffer, O_WRONLY|O_TRUNC, 0777);
            int val = 0;
            while ((val = read(fd, readbuffer, 1024)) != 0)
            {
                write(fdaux, readbuffer, val);
            }
            return;
        }
        dp = readdir(directory);
    }
    // If it didn't find the file, it creates it and performs the same operation:
                char buffer[2048]; // 2048 considered the longest path name. We use this buffer as intermediate between the reading of the original file and the writing into the new file.
            sprintf(buffer, "%s/%s", path, file); // We write the path of the file into the buffer-
    int fd2 = open(buffer, O_WRONLY|O_CREAT|O_TRUNC, 0777); // We create the new file
    if (fd2 == -1) // Checking for errors in the creation of the file
    {
        perror("[ERROR] Error opening or creating the copied file");
        return;
    }
    int var = 0;
    while ((var = read(fd, readbuffer, 1024)) != 0) // We read from the original file
    {
        write(fd2, readbuffer, var); // And we write the read amount into the new file, until there are no more bytes to read.
    }
    printf("[OK] Copy has been successful between %s to the directory %s\n", file, path);
    close(fd);
    close(fd2);
    closedir(directory);
    return;
}
