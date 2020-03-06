#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3
#define BUFSIZE 80

int available[NUMBER_OF_RESOURCES];

int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

bool is_safe();

/* Set initial values for all arrays using the following:
Customer	Allocation	Need		Maximum	
0		0 0 0 		7 1 3 		7 1 3 
1		0 0 0 		4 3 5 		4 3 5 		
2		0 0 0 		6 2 1 		6 2 1 		
3		0 0 0 		5 3 3 		5 3 3 		
4		0 0 0 		1 3 4 		1 3 4 		
*/
/* The above initial values only work when NUMBER_OF_CUSTOMERS is 3 */
void *initialize_resource_arrays(char* argv[])
{
	/* Loop control variables */
	int c,r;

	/* Initialize available resources to command-line parameters */
	for (r=0; r<NUMBER_OF_RESOURCES; ++r) available[r]=atoi(argv[r+1]);

	maximum[0][0]=7;
	maximum[0][1]=1;
	maximum[0][2]=3;

	maximum[1][0]=4;
	maximum[1][1]=3;
	maximum[1][2]=5;

	maximum[2][0]=6;
	maximum[2][1]=2;
	maximum[2][2]=1;

	maximum[3][0]=5;
	maximum[3][1]=3;
	maximum[3][2]=3;

	maximum[4][0]=1;
	maximum[4][1]=3;
	maximum[4][2]=4;

	/* Initialize need and allocation */
	for (c=0; c<NUMBER_OF_CUSTOMERS; ++c)
		for (r=0; r<NUMBER_OF_RESOURCES; ++r)
		{
			/* Initialize need = maximum */
			need[c][r]=maximum[c][r];

			/* Nothing is allocated */
			allocation[c][r]=0;
		}
}

/* Output the current state */
/* Only use this for debugging purposes. */
/* Do not call this function in your submitted solution. */
print_state()
{
	int c,r;
	printf("\n");
	printf("Customer\tAllocation\tNeed\t\tMaximum\t\tAvailable\n");
	for (c=0; c<NUMBER_OF_CUSTOMERS; ++c)
	{
		printf("%d\t\t",c);
		for (r=0; r<NUMBER_OF_RESOURCES; ++r) printf("%d ",allocation[c][r]);
		printf("\t\t");
		for (r=0; r<NUMBER_OF_RESOURCES; ++r) printf("%d ",need[c][r]);
		printf("\t\t");
		for (r=0; r<NUMBER_OF_RESOURCES; ++r) printf("%d ",maximum[c][r]);
		printf("\t\t");
		if (c==0)
			for (r=0; r<NUMBER_OF_RESOURCES; ++r) printf("%d ",available[r]);
		printf("\n");
	}
	if (is_safe())
		printf("SAFE!\n");
	else
		printf("UNSAFE!\n");
	printf("\n");
}

/* Adds arr1's elements by arr2's elements and store the results in arr1 */
void addArray(int arr1[], int arr2[], int n)
{
	int i;

	for (i = 0; i < n; i++)
		arr1[i] += arr2[i];
}

/* Subtracts arr1's elements by arr2's elements and store the results in arr1 */
void subArray(int arr1[], int arr2[], int n)
{
        int i;

        for (i = 0; i < n; i++)
                arr1[i] -= arr2[i];
}

/* Determines if the system is in a safe state. Returns true if the system is safe, false if it is unsafe. */
bool is_safe()
{
	bool finish[NUMBER_OF_CUSTOMERS]; // Array of booleans to determine if a customer is finished.
	bool flag, flag2; // Using these insteads of breaks because they're "bad".
	int work[NUMBER_OF_RESOURCES], i, j, k; // Work is a temp version of available. i, j, k are all used for iteration.
	
	// Initialize the finish and work arrays.
	memset(finish, false, NUMBER_OF_CUSTOMERS * sizeof(bool));
	memcpy(work, available, NUMBER_OF_RESOURCES * sizeof(int));

	// Need to keep going until we reach every process.
	for (k = 0; k < NUMBER_OF_CUSTOMERS; k++)
	{
		flag = true;

		for (i = 0; i < NUMBER_OF_CUSTOMERS && flag; i++)
		{
			// Check if this customer has already been completed.
			if (!finish[i])
			{
				flag2 = true;

				// Check to see if a process is asking for more than the system has available.
				for (j = 0; j < NUMBER_OF_RESOURCES && flag2; j++)
					if (need[i][j] > work[j])
						flag2 = false;

				// If j made it to it's upper bound, we need to allocate the resources and finish the customer.
				if(flag2)
				{
					finish[i] = true;
					flag = false;
					addArray(work, allocation[i], NUMBER_OF_RESOURCES);
				}
			}
		}
	}	
	
	// If all the customers were finished, the system is in a safe state.
	for (i = 0; i < NUMBER_OF_CUSTOMERS; i++)
		if (!finish[i])
			return false;
	return true;
}

/* Requests reqource(s). Returns customer number */
int request_resources(int c, int request[])
{
	int i;

	for (i = 0; i < NUMBER_OF_RESOURCES; i++)
	{
		// Test if resource can be allocated based on the total number of resources and maximum available.
		if (request[i] > need[c][i] || request[i] > available[i])
		{
			printf("UNAVAILABLE\n");
			return c;
		}
	}

	// Pretends to allocate the resource to check if this will place the system in an unsafe state.
	subArray(available, request, NUMBER_OF_RESOURCES);
	addArray(allocation[c], request, NUMBER_OF_RESOURCES);
	subArray(need[c], request, NUMBER_OF_RESOURCES);

	// If the system state is unsafe, restore the old state and indicate the process must wait.
	if (!is_safe())
	{
		addArray(available, request, NUMBER_OF_RESOURCES);
		subArray(allocation[c], request, NUMBER_OF_RESOURCES);
		addArray(need[c], request, NUMBER_OF_RESOURCES);

		printf("UNSAFE\n");
		return c;
	}		

	// If the system is in a safe state, then we will keep the matrices and arrays intact and indicate the request was successful.
	printf("ACCEPTED\n");
	return c;
}

/* Releases resource(s). Returns customer number */
int release_resources(int c, int release[])
{	
	int i;
	
	printf("\n");

	// Adds the released resources back to the available array and need matrix. Also removed that from the allocation matrix.
	addArray(available, release, NUMBER_OF_RESOURCES);
	subArray(allocation[c], release, NUMBER_OF_RESOURCES);
	addArray(need[c], release, NUMBER_OF_RESOURCES);

	return c;
}

int main(int argc, char *argv[])
{
	char str[BUFSIZE];	// Input buffer
	char *pch;		// This is for use by strtok
	int custnum;		// This is for holding the customer number
	char reqresp;  		// This is for holding the + or the -
	int i;			// Loop variable
	int rarray[NUMBER_OF_RESOURCES];  // For requests or releases

	/* Initialize resource arrays */
	initialize_resource_arrays(argv);

	/* Parse input and call appropriate function */
	while (fgets(str, BUFSIZE, stdin))
	{
		pch = strtok (str," ");
		custnum = atoi(pch);
		pch = strtok (NULL," ");
		reqresp=*pch;
		printf("%d %c ", custnum, reqresp);
		for (i=0; i<NUMBER_OF_RESOURCES; ++i)
		{
			rarray[i]=atoi(strtok (NULL, " "));
			printf("%d ", rarray[i]);
		}
			
		if (reqresp == '+')
			request_resources(custnum, rarray);
		else
			release_resources(custnum, rarray);
	}
}
