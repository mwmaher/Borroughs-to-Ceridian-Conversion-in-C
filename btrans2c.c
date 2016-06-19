/* BTrans2C.C

   By Michael W. Maher
   Ver 1.0: 11/23/92
   Ver 3.0: 10/27/93 - added header file

   DESCRIPTION:
	The following program will change the format of the transaction
	data records generated from the Burroughs payroll system to a
	Ceridian Payroll Services format.

	SYNOPSIS:
	    - For payroll transaction records only
		1. each record having an employee ID is changed such that
		   the alpha portion of the ID is changed to a numeric
		   string. for example: 'A' becomes "01" and 'Z'
		   becomes "26".

		   Employees having clock numbers of the format: MC__ will
		   be assigned new Ceridian clock numbers through a table
		   lookup.

		2. a new-line character is added to the end of every record.

	NOTES:
*/

#include<memory.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"mastinfo.h"
#include"mastinfo.c"

/******************************* constants *********************************/
#define OUT_REC_SIZE 80
#define FALSE 0
#define TRUE 1

/****************************** global data ********************************/


void main(int argc, char **argv);
void convert_empl(char *infilename, char *outfilename);
void convert(char *outbuf, char *inbuf,
	     employee_rec *employee_table, int table_size);

void main(int argc, char **argv)
  {
  fprintf(stdout, "\n"
		  "btrans2c.c By Michael W. Maher\n"
		  "Ver 3.0: 10/27/93\n"
		  "This program will convert Burroughs payroll transaction\n"
		  "file into a Ceridian acceptable format.\n"
		  "  - adds a newline char every 80 chars.\n"
		  "  - converts clock numbers.\n");
  if(argc > 2)
    convert_empl(argv[1], argv[2]);
  else
    fprintf(stdout, "\nUsage: btrans2c infile outfile\n");
  return;
  }


void convert_empl(char *infilename, char *outfilename)
  {
  employee_rec employee_table[MC_EMP_TABLE];
  int  table_size;
  char inbuf[OUT_REC_SIZE+1],
       outbuf[OUT_REC_SIZE+1];
  FILE *infile,
       *outfile;

  memset(inbuf, '\0', OUT_REC_SIZE+1);
  fprintf(stdout, "\nConverting file...\n\n");
  if ((infile = fopen(infilename, "rt")) == NULL)
    fprintf(stderr, "ERROR: can not open source file.\n");
  else
    {
    if ((outfile = fopen(outfilename, "wt")) == NULL)
      fprintf(stderr, "ERROR: can not open destination file.\n");
    else
      {
      table_size = load_employee_table(employee_table);
      fprintf(stdout, "Writing to the destination file...\n");
      while (fread(inbuf, sizeof(char), OUT_REC_SIZE, infile) == OUT_REC_SIZE)
	{
	/* convert the record format */
	convert(outbuf, inbuf, employee_table, table_size);
	/* write out the converted record */
	fwrite(outbuf, sizeof(char), OUT_REC_SIZE, outfile);
	/* add a newline character to the end of the record */
	fwrite("\n", sizeof(char), 1, outfile);
	}
      }
    }
  fclose(infile);
  fclose(outfile);
  fprintf(stdout, "The file conversion is completed.\n\n");
  return;
  }


void convert(char *outbuf, char *inbuf,
	     employee_rec *employee_table, int table_size)
  {
  char numstr[3],
       temp_clock[5],
       found;
  int  i,
       j;

  strcpy(outbuf, inbuf);
  /* convert payroll transaction records 50 and 51*/
  if (((inbuf[13] == '5') && (inbuf[14] == '0')) ||
      ((inbuf[13] == '5') && (inbuf[14] == '1')))
    {
    for (i = 0; i < 4; i++)
      temp_clock[i] = inbuf[(9+i)];
    temp_clock[4] = '\0';
    /* convert the employee ID with a table substitution */
    found = FALSE;
    for (i = 0; i < table_size; i++)
      {
      if (strcmp(employee_table[i].old_clock, temp_clock) == 0)
	{
	/* copy the new clock number over the old */
	for (j = 0; j < 5; j++)
	  outbuf[8+j] = employee_table[i].new_clock[j];
	printf("Converted %s to %s\n",
	       temp_clock, employee_table[i].new_clock);
	found = TRUE;
	}
      }
    if ((found == FALSE) && (inbuf[9] == 'M') && (inbuf[10] == 'C'))
      {
      printf("ERROR: %s not found in conversion table!!!\n", temp_clock);
      printf("Strike any key to continue...\n");
      getch();
      }
    if (found == FALSE)
      {
      /* convert the employee ID with standard conversion procedure */
      atonumstr(inbuf[9], numstr);
      outbuf[8] = numstr[0];
      outbuf[9] = numstr[1];
      }
    }
  return;
  }


