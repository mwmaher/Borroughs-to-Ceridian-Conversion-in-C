/* mastinfo.c

   By Michael W. Maher

   Description: This file contains C source code functions for use with
		the Burroughs to Ceridian payroll conversion programs.

*/

/* function: atonumstr()

   returns:  char * - a ptr to the converted string of characters

   synopsis: This function will convert the leading prefix from a single
	     letter to a two-character numeric representation.
*/

char *atonumstr(char inchar, char *outbuf)
  {

  switch (inchar)
    {
    case 'A': strcpy(outbuf, "01");
	      break;
    case 'B': strcpy(outbuf, "02");
	      break;
    case 'C': strcpy(outbuf, "03");
	      break;
    case 'D': strcpy(outbuf, "04");
	      break;
    case 'E': strcpy(outbuf, "05");
	      break;
    case 'F': strcpy(outbuf, "06");
	      break;
    case 'G': strcpy(outbuf, "07");
	      break;
    case 'H': strcpy(outbuf, "08");
	      break;
    case 'I': strcpy(outbuf, "09");
	      break;
    case 'J': strcpy(outbuf, "10");
	      break;
    case 'K': strcpy(outbuf, "11");
	      break;
    case 'L': strcpy(outbuf, "12");
	      break;
    case 'M': strcpy(outbuf, "13");
	      break;
    case 'N': strcpy(outbuf, "14");
	      break;
    case 'O': strcpy(outbuf, "15");
	      break;
    case 'P': strcpy(outbuf, "16");
	      break;
    case 'Q': strcpy(outbuf, "17");
	      break;
    case 'R': strcpy(outbuf, "18");
	      break;
    case 'S': strcpy(outbuf, "19");
	      break;
    case 'T': strcpy(outbuf, "20");
	      break;
    case 'U': strcpy(outbuf, "21");
	      break;
    case 'V': strcpy(outbuf, "22");
	      break;
    case 'W': strcpy(outbuf, "23");
	      break;
    case 'X': strcpy(outbuf, "24");
	      break;
    case 'Y': strcpy(outbuf, "25");
	      break;
    case 'Z': strcpy(outbuf, "26");
	      break;
    default:  strcpy(outbuf, "00");
    }
  return(outbuf);
  }



/* function: load_employee_table()

   returns:  int - the size of the table

   synopsis: This function builds a conversion table for those employees
	     with an "MC" prefix in their employee number.
*/

int load_employee_table(employee_rec *e_table)
  {
  int i = 0;

  e_table[i].name = "Smith, John J.";
  e_table[i].old_clock = "MC01";
  e_table[i].new_clock = "11111";
  i++;
  /* 9/29/94 - Add D */
  e_table[i].name = "Smyth, John J.";
  e_table[i].old_clock = "MC102";
  e_table[i].new_clock = "11112";
  e_table[i].special   = "D";
  i++;
  return(i);
  }

