/* BMast2c.C

   By Michael W. Maher
   Ver 1.0 11/23/92
       2.0 12/03/92
       3.0 10/27/93  - add to the MC table
       4.0 11/2/93   - Change record 30 status from "N" to "D" for 
		       all people hire before 1993.
		       Change record 34 position #19 to a blank for flat
		       dollar withholding.
       4.5 11/2/93   - Change record 34 position 19 to a blank if 0.
       4.6 11/2/93   - Add a conversion table for location codes, from
		       Burroughs hourly codes to Ceridian salary codes.
       4.7 11/15/93  - Convert employee number to contain the division
		       number.  This requires changing the 30 record.

   DESCRIPTION:
	The following program will change the format of the data records
	generated from the Burroughs payroll system.  The program will
	operate on both ytd total files and master employee data files.

	changes:
		1. all dates are changed from DDMMYY to MMDDYY

		2. each record having an employee ID is changed such that
		   the alpha portion of the ID is changed to a numeric
		   string. for example: 'A' becomes "01" and 'Z'
		   becomes "26".

		   Employees having clock numbers of the format: MC__ will
		   be assigned new Ceridian clock numbers through a table
		   lookup.

		3. a new-line character is added to the end of every record.

		4. all active employees have 000000 for a terminiation date
		   this is converted to blanks

   NOTES:
*/

/******************************* includes **********************************/
#include<memory.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"mastinfo.h"
#include"mastinfo.c"

/******************************* constants *********************************/
#define OUT_REC_SIZE 80
#define DEFAULT_DIV "00"
#define FALSE 0
#define TRUE 1

void main(int argc, char **argv);
void convert_empl(char *infilename, char *outfilename, char *division);
void convert(char *outbuf,
	     char *inbuf,
	     employee_rec *employee_table,
	     int table_size,
	     FILE *infile,
	     char *division);

void main(int argc, char **argv)
  {
  fprintf(stdout, "\n"
		  "bmast2c By Michael W. Maher\n"
		  "Ver 4.7: 11/15/93\n"
		  "This program will convert Burroughs payroll employee\n"
		  "master file into a Ceridian acceptable format.\n"
		  "  - add a newline char every 80 chars\n"
		  "  - convert clock numbers\n"
		  "  - convert various data record formats\n"
		  "  - add the employee's division to his record\n");
  if (argc == 3)
    convert_empl(argv[1], argv[2], DEFAULT_DIV);
  else if (argc == 4)
    convert_empl(argv[1], argv[2], argv[3]);
  else
    fprintf(stdout, "\nUsage: bmast2c infile outfile [division]\n");
  return;
  }


void convert_empl(char *infilename, char *outfilename, char *division)
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
	convert(outbuf, inbuf, employee_table, table_size, infile, division);
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

void convert(char *outbuf,
	     char *inbuf,
	     employee_rec *employee_table,
	     int table_size,
	     FILE *infile,
	     char *division)
  {
  char numstr[3],
       temp_clock[5],
       hire_date[7],
       special,
       temp_buf[OUT_REC_SIZE+1],
       found;
  int  i,
       j;

  strcpy(outbuf, inbuf);

  /* Convert the employee number in records 30, 32, 33, 34, and 61. */
  if (((inbuf[13] == '3') && (inbuf[14] == '2')) ||
      ((inbuf[13] == '3') && (inbuf[14] == '3')) ||
      ((inbuf[13] == '3') && (inbuf[14] == '4')) ||
      ((inbuf[13] == '3') && (inbuf[14] == '0')) ||
      ((inbuf[13] == '6') && (inbuf[14] == '1')))
    {
    for (i = 0; i < 4; i++)
      temp_clock[i] = inbuf[(9+i)];
    temp_clock[4] = '\0';
    /* convert the employee ID with a table substitution */
    found   = FALSE;
    special = '\0';
    for (i = 0; i < table_size; i++)
      {
      if (strcmp(employee_table[i].old_clock, temp_clock) == 0)
	{
	/* copy the new clock number over the old */
	for (j = 0; j < 5; j++)
	  outbuf[8+j] = employee_table[i].new_clock[j];
	printf("Converted %s to %s\n",
	       temp_clock, employee_table[i].new_clock);
	found   = TRUE;
	if (employee_table[i].special[0] == 'N')
	  special = 'N';
	if (employee_table[i].special[0] == 'D')
	  special = 'D';
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

  /* convert hire status field of record 30 */
  /* All employee previously converted during the first attempt
     (i.e. hired before 12/7/92) must have a status code of 'D' instead
     of 'N' in order to be accepted into Ceridian.  Except for a few
     special cases.
  */
  if ((inbuf[13] == '3') && (inbuf[14] == '0') && (inbuf[15] == 'N'))
    {
    /* read in record number 32 to find the hire date */
    fread(temp_buf, sizeof(char), OUT_REC_SIZE, infile);
    fseek(infile, -1L * OUT_REC_SIZE, SEEK_CUR);
    /* year */
    hire_date[0]  = temp_buf[69];
    hire_date[1]  = temp_buf[70];
    /* month */
    hire_date[2]  = temp_buf[67];
    hire_date[3]  = temp_buf[68];
    /* day */
    hire_date[4]  = temp_buf[65];
    hire_date[5]  = temp_buf[66];
    hire_date[6] = '\0';
    if ((strcmp(hire_date, "921207") <= 0) && (special != 'N'))
      {
      outbuf[15] = 'D';
      printf("Convert #30: %s\n", hire_date);
      /* If the employee is 'D' then use new employee id field to assign
	 the employee to a division. */
      outbuf[17] = division[0];
      outbuf[18] = division[1];
      outbuf[19] = inbuf[5];
      outbuf[20] = inbuf[6];
      outbuf[21] = inbuf[7];
      outbuf[22] = outbuf[8];
      outbuf[23] = outbuf[9];
      outbuf[24] = outbuf[10];
      outbuf[25] = outbuf[11];
      outbuf[26] = outbuf[12];
      }
    else
      {
      /* Assign a new employee to a division via the employee id field. */
      outbuf[3] = division[0];
      outbuf[4] = division[1];
      }
    /* change the DT status to TD */
    if ((outbuf[16] == 'T') && (outbuf[15] == 'D'))
      {
      outbuf[16] = 'D';
      outbuf[15] = 'T';
      }
    /* change the DI status to ID */
    if ((outbuf[16] == 'I') && (outbuf[15] == 'D'))
      {
      outbuf[16] = 'D';
      outbuf[15] = 'I';
      }
    }

  /* convert dates of record 32 */
  if ((inbuf[13] == '3') && (inbuf[14] == '2'))
    {
    /* termination date */
    if ((inbuf[61] == '0') && (inbuf[62] == '0') && (inbuf[63] == '0'))
      {
      outbuf[59] = ' ';
      outbuf[60] = ' ';
      outbuf[61] = ' ';
      outbuf[62] = ' ';
      outbuf[63] = ' ';
      outbuf[64] = ' ';
      }
    else
      {
      outbuf[59] = inbuf[61];
      outbuf[60] = inbuf[62];
      outbuf[61] = inbuf[59];
      outbuf[62] = inbuf[60];
      }
    /* hire date */
    outbuf[65] = inbuf[67];
    outbuf[66] = inbuf[68];
    outbuf[67] = inbuf[65];
    outbuf[68] = inbuf[66];
    }

  /* convert record 34 */
  /* Record #34 position 19 has a '0' where a blank should be when an
     employee has a flat adjustment amount.  This is cause for Ceridian
     to reject the record.
  */
  /* The location code must be converted from Burroughs hourly to
     Ceridian salary.
  */
  if ((inbuf[13] == '3') && (inbuf[14] == '4'))
    {
    /* convert position 19  (Fed. Adj. code) */
    if (inbuf[18] == '0')
      outbuf[18] = '$';
    else if (inbuf[18] != ' ')
      printf("ERROR: record 34 position 19, unkown flag");

    /* convert position 25  (State. Adj. code) */
    if (inbuf[24] == '0')
      outbuf[24] = '$';
    else if (inbuf[24] != ' ')
      printf("ERROR: record 34 position 25, unkown flag");

    /* convert location codes */
    printf("code: %c%c\n", inbuf[32], inbuf[33]);
    if ((inbuf[32] == '0') && (inbuf[33] == '1'))
      {
      /* Hudson Resident */
      outbuf[32] = 'H';
      outbuf[33] = 'R';
      }
    else if ((inbuf[32] == '0') && (inbuf[33] == '5'))
      {
      /* Hudson Non-resident */
      outbuf[32] = 'H';
      outbuf[33] = 'N';
      }
    else if ((inbuf[32] == '1') && (inbuf[33] == '0'))
      {
      /* Steuben Resident */
      outbuf[32] = '7';
      outbuf[33] = '6';
      }
    else if ((inbuf[32] == '2') && (inbuf[33] == '5'))
      {
      /* Steuben Non-Resident */
      outbuf[32] = '9';
      outbuf[33] = '9';
      }
    }
  return;
  }
