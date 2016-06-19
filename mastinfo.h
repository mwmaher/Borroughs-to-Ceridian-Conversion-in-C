/* mastinfo.h

   By Michael W. Maher

   Description: Header information for Burroughs to Ceridian payroll
		conversion programs.


*/

/***************************** defines *************************************/

#define MC_EMP_TABLE 100               /* the number of table entries */


/**************************** type definitions *****************************/

typedef struct employee_rec
  {
  char *name;                          /* employee name */
  char *old_clock;                     /* employee clock number on Burroughs*/
  char *new_clock;                     /* employee clock number of Ceridian */
  char *special;                       /* code for handling special cases */
  } employee_rec;


/*************************** function prototypes ***************************/

/* This function will convert the alpha prefix to a 2 char number */
char *atonumstr(char inchar, char *outbuf);

/* This function contains a hardcoded "MC" employee number conversion table */
int load_employee_table(employee_rec *e_table);

