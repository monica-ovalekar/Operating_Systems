// Monica Ovalekar	NetId: mso269	N#: N12316668
// processor.c
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
	char data[BUFSIZ];
	char *sm, *d;
	key_t key;
	int sm_id;
	int n, count;

	FILE * secrets;

	/* secrets.out file is created */
	secrets = fopen("secrets.out","w");

	/* This is the key of the shared memory segment */
	key = 660;

	/* shared memory segment is located */
	sm_id = shmget(key, SIZE, 0600);

	/* Error generated if shared memory segment is not located */
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
		/* Program runs when thee is an input */
		if (strcmp(sm, "\0") != 0)
		{
			/* % is added by processor.c to indicate that it has performed the operation.
			   So as long as sm has % it is the same data because the new data won't have this "%" symbol */
			if (*sm != '%')
			{
				/* n is the number of digits in the line of data and data is set to 0 to avoid overwriting */
				memset(data, 0, sizeof(data));
				n = 0;
				count = 0;

				/* this for loop counts the number of digits */
				for (d = sm; *d != '!'; d++)
				{
					data[count] = *d;
					count++;
					if (*d=='0' || *d=='1' || *d=='2' || *d=='3' || *d=='4' || *d=='5' || *d=='6' || *d=='7' || *d=='8' || *d=='9')
					{
						n++;
					}
				}

				/* if user enters exit, it exits from the  loop */
				if (strncmp(data, "exit", 4) == 0)
				{
					goto label;
				}

				/* secrets.out file is opened */
				secrets = fopen ("secrets.out","a");

				/* no. of digits and string is added to file */
				if (secrets != NULL)
				{
					fprintf(secrets, "%d is/are the number of digits in this string %s", n-2, data);
					fclose(secrets);
				}

				/* This "%" sign is added so that the receiver knows that processor has processed the line of data and can take in more data */
				*sm = '%';
			}
		}
	}

	label:
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
