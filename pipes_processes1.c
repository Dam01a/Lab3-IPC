#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int fd1[2]; // Used to store two ends of the first pipe (P1 to P2)
    int fd2[2]; // Used to store two ends of the second pipe (P2 to P1)

    char fixed_str[] = "howard.edu";
    char input_str[100];
    char second_input_str[100];
    pid_t p;

    if (pipe(fd1) == -1 || pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    p = fork();

    if (p < 0)
    {
        fprintf(stderr, "fork Failed");
        return 1;
    }
    // Parent process (P1)
    else if (p > 0)
    {
        close(fd1[0]); // Close reading end of the first pipe (P1 to P2)
        close(fd2[1]); // Close writing end of the second pipe (P2 to P1)

        printf("Input: ");
        scanf("%s", input_str);

        // Write input string and close writing end of the first pipe (P1 to P2).
        write(fd1[1], input_str, strlen(input_str) + 1);

        // Read the concatenated string from P2.
        char received_str[200];
        read(fd2[0], received_str, 200);

        printf("%s\n", received_str);

        printf("Input: ");
        scanf("%s", second_input_str);

        // Write the second input string and close writing end of the first pipe (P1 to P2).
        write(fd1[1], second_input_str, strlen(second_input_str) + 1);

        // Wait for P2 to send back the concatenated string.
        wait(NULL);

        close(fd2[0]); // Close reading end of the second pipe (P2 to P1)
        close(fd1[1]); // Close writing end of the first pipe (P1 to P2)
    }
    // Child process (P2)
    else
    {
        close(fd1[1]); // Close writing end of the first pipe (P1 to P2)
        close(fd2[0]); // Close reading end of the second pipe (P2 to P1)

        // Read a string using the first pipe (P1 to P2)
        char concat_str[100];
        read(fd1[0], concat_str, 100);

        // Concatenate a fixed string with it
        int k = strlen(concat_str);
        int i;
        for (i = 0; i < strlen(fixed_str); i++)
            concat_str[k++] = fixed_str[i];

        concat_str[k] = '\0'; // string ends with '\0'

        // Write the concatenated string back to P1 through the second pipe (P2 to P1).
        write(fd2[1], concat_str, strlen(concat_str) + 1);

        // Read the second input string from P1.
        char second_input[100];
        read(fd1[0], second_input, 100);

        // Concatenate the second input string with "howard.edu."
        k = strlen(concat_str);
        for (i = 0; i < strlen(second_input); i++)
            concat_str[k++] = second_input[i];

        concat_str[k] = '\0'; // string ends with '\0'

        printf("%s\n", concat_str);

        // Close reading end of the first pipe (P1 to P2).
        close(fd1[0]);

        exit(0);
    }
}
