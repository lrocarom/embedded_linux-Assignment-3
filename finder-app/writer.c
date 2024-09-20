#include <stdio.h>
#include <string.h>
#include <syslog.h>


int main(int argc, char *argv[]) {
	openlog(NULL,0,LOG_USER);
	syslog(LOG_USER,"The number of parameters is %d", argc );

	int numberParams = argc;
	
	if (numberParams != 3) {
	
	    syslog(LOG_ERR,"Number of params not valid");
		return 1;
	
	} 

	char *routeName = argv[1];
	char *inputName = argv[2];

	syslog(LOG_INFO,"The route name is  %s", routeName );

	syslog(LOG_INFO,"The input name is  %s", inputName );

    FILE *fp = fopen(routeName, "w");

    if (fp == NULL)

    {
        fclose(fp); // close the file
        syslog(LOG_ERR,"The input file don't exists ");

           return 1;
     }
     syslog(LOG_DEBUG,"Writting to the input file ... ");

     fprintf(fp, inputName, 1);

     fclose(fp); // close the file


	return 0;





}
