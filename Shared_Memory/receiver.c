// Monica Ovalekar	NetId: mso269	N#: N12316668
// receiver.c
/* receiver.c takes input from the user.
If the input has the secret code "C00L" in the beginning then this line of data is transferred to shared memory segment.
From shared memory segment, processor.c takes the line of data counts its digits and dumps the data (alongwith the codeword) and the count (which does not include the two zeroes of the code word) in the file secrets.out
*/

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 1024

int main(void)
{
	char *input = malloc(sizeof(char)*BUFSIZ);
	char *sm, *d;
	key_t key;
	int sm_id;

	/* This is the key of the shared memory segment */
    key = 660;

	/* Shared memory segment is created */
	sm_id = shmget(key, SIZE, IPC_CREAT | 0600);

	/* Error generated if shared memory segment is not created */
	if (sm_id == -1)
	{
		perror("shmget failed");
		exit(55);
	}

	/* Shared memory is attached to data space */
	sm = shmat(sm_id, NULL, 0);

	/* Error generated if shared memory segment is not attached */
	if (sm == (char *) -1)
	{
		perror("shmat failed");
		exit(66);
	}

	/* This is an infinite loop in which shared memory is used for IPC between two C programs */
	while (1)
	{
		/* An alphanumeric is to be entered by the user */
		printf("Enter an alphanumeric string and type exit to get out of the loop: ");
		fgets(input, BUFSIZ, stdin);

		/* if user enters exit, it exits from the  loop */
		if (strncmp(input, "exit", 4) == 0)
		{
			break;
		}

		/* This if loop checks if the entered input has code "C00L" in the starting or not.
		   If input does not start with this code then it is not given to the shared memory */
		if (strncmp(input, "C00L", 4) == 0)
		{
			/* Input copied to shared memory */
			memcpy(sm, input, strlen(input));
			d = sm;
			d += strlen(input);
			*d = '!';
			/* This "!" sign is added so that the processor knows where the input ends */

			/* processor.c adds "% sign to show that it has finished processing and receiver.c can take in the next line of input. */
			while (*sm != '%')
			{
				sleep(1);
			}
		}

		else
		{
			/* If the input does not have secret code "C00L" in the start then this statement is printed */
			printf("The data entered does not have secret code so it cannot be processed.\n");
		}
	}

	/* shared memory segment is detached and error is generated if it isn't detached */
	if (shmdt(sm) == -1)
	{
		perror("shmdt failed");
		exit(88);
	}

	/* shared memory IPC is closed and error is generated if it isn't closed */
	if (shmctl(sm_id, IPC_RMID, 0) == -1)
	{
		perror("shmctl failed");
		exit(99);
	}
}