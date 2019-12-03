/***************************************************************************
*                 Lempel, Ziv, Storer, and Szymanski Encoding
****************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mpi.h>
#include "lzlocal.h"
#include "bitfile/bitfile.h"

unsigned char slidingWindow[WINDOW_SIZE];
unsigned char uncodedLookahead[MAX_CODED];

/****************************************************************************
*   Description: This function will read an input file and write an output
*   Function   : EncodeLZSS
*                file encoded according to the traditional LZSS algorithm.
*                This algorithm encodes strings as 16 bits (a 12 bit offset
*                + a 4 bit length).
*   Returned   : 0 for success, -1 for failure.  errno will be set in the
*                event of a failure.
****************************************************************************/
int EncodeLZSS(FILE *fpIn, FILE *fpOut, int argc, char *argv[])
{
    int nproc;
    int my_rank;
    int *sendcounts;
    int *displs;
    MPI_Comm comm;
    MPI_Status status;

    MPI_Init(&argc,&argv);
    comm=MPI_COMM_WORLD;
    MPI_Comm_size(comm,&nproc);
    MPI_Comm_rank(comm,&my_rank);
    
    bit_file_t *bfpOut;
    encoded_string_t matchData;
    int c;
    unsigned int i;
    unsigned int len;                       /* length of string */

    /* head of sliding window and lookahead */
    unsigned int windowHead, uncodedHead;
    windowHead = 0;
    uncodedHead = 0;
    int tlen;
    int sum=0;

    if(my_rank==0){

    	/* validate arguments */

    	/* convert output file to bitfile */
    	bfpOut = MakeBitFile(fpOut, BF_WRITE);

    	if (NULL == bfpOut)
    	{
        	perror("Making Output File a BitFile");
        	return -1;
    	}


    	/************************************************************************
    	* Fill the sliding window buffer with some known vales.  DecodeLZSS must
    	* use the same values.  If common characters are used, there's an
    	* increased chance of matching to the earlier strings.
    	************************************************************************/
    	memset(slidingWindow, ' ', WINDOW_SIZE * sizeof(unsigned char));

    	/************************************************************************
    	* Copy MAX_CODED bytes from the input file into the uncoded lookahead
    	* buffer.
    	************************************************************************/
    	for (len = 0; len < MAX_CODED && (c = getc(fpIn)) != EOF; len++)
    	{
        	uncodedLookahead[len] = c;
    	}

    	if (0 == len)
    	{
        	return 0;   /* inFile was empty */
    	}

    	/* Look for matching string in sliding window */
    	i = InitializeSearchStructures();

    	if (0 != i)
    	{
        	return i;       /* InitializeSearchStructures returned an error */
    	}
	tlen = (len/nproc)+1; 
	int rem = (len)%nproc; // elements remaining after division among processes
    	sendcounts = malloc(sizeof(int)*nproc);
    	displs = malloc(sizeof(int)*nproc);
	for ( i = 0; i < nproc; i++) {
       	 	sendcounts[i] = len/nproc;
        	if (rem > 0) {
            		sendcounts[i]++;
            		rem--;
        	}

        	displs[i] = sum;
        	sum += sendcounts[i];
		//printf("\n %d-%d \n", displs[i], sendcounts[i]);
    	}
    }
    char local_Look[tlen];
    MPI_Scatterv(&uncodedLookahead, sendcounts, displs, MPI_CHAR, &local_Look, tlen, MPI_CHAR, 0, comm);

    matchData = FindMatch(windowHead, uncodedHead);

    /* now encoded the rest of the file until an EOF is read */
    while (tlen > 0)
    {
        if (matchData.length > tlen)
        {
            /* garbage beyond last data happened to extend match length */
            matchData.length = tlen;
        }

        if (matchData.length <= MAX_UNCODED)
        {
            /* not long enough match.  write uncoded flag and character */
            BitFilePutBit(UNCODED, bfpOut);
            BitFilePutChar(local_Look[uncodedHead], bfpOut);

            matchData.length = 1;   /* set to 1 for 1 byte uncoded */
        }
        else
        {
            unsigned int adjustedLen;

            /* adjust the length of the match so minimun encoded len is 0*/
            adjustedLen = matchData.length - (MAX_UNCODED + 1);

            /* match length > MAX_UNCODED.  Encode as offset and length. */
            BitFilePutBit(ENCODED, bfpOut);
            BitFilePutBitsNum(bfpOut, &matchData.offset, OFFSET_BITS,
                sizeof(unsigned int));
            BitFilePutBitsNum(bfpOut, &adjustedLen, LENGTH_BITS,
                sizeof(unsigned int));
        }

        /********************************************************************
        * Replace the matchData.length worth of bytes we've matched in the
        * sliding window with new bytes from the input file.
        ********************************************************************/
        i = 0;
        while ((i < matchData.length) && ((c = getc(fpIn)) != EOF))
        {
            /* add old byte into sliding window and new into lookahead */
            ReplaceChar(windowHead, local_Look[uncodedHead]);
            local_Look[uncodedHead] = c;
            windowHead = Wrap((windowHead + 1), WINDOW_SIZE);
            uncodedHead = Wrap((uncodedHead + 1), MAX_CODED);
            i++;
        }

        /* handle case where we hit EOF before filling lookahead */
        while (i < matchData.length)
        {
            ReplaceChar(windowHead, local_Look[uncodedHead]);
            /* nothing to add to lookahead here */
            windowHead = Wrap((windowHead + 1), WINDOW_SIZE);
            uncodedHead = Wrap((uncodedHead + 1), MAX_CODED);
            tlen--;
            i++;
        }

        /* find match for the remaining characters */
        matchData = FindMatch(windowHead, uncodedHead);
    }

    /* we've encoded everything, free bitfile structure */
    	
    if(my_rank ==0){
    	BitFileToFILE(bfpOut);
    }
    	MPI_Finalize();

   return 0;
}
