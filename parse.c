/*
 * Revision Control Information
 *
 * $Source$
 * $Author$
 * $Revision$
 * $Date$
 *
 */
/*
 * Symbolic encoding program for compiling a symbolic
 * description into a binary representation.  Target
 * is multi-level logic synthesis
 *
 * History:
 *
 * Bill Lin
 * University of California, Berkeley
 * Comments to billlin@ic.Berkeley.EDU
 *
 * Copyright (c) 1989 Bill Lin, UC Berkeley CAD Group
 *     Permission is granted to do anything with this
 *     code except sell it or remove this message.
 */


#include <stdio.h>
#include "include/jedi.h"

extern FILE *infp;			/* jedi.c */


usage()
{
   (void) fprintf(stderr,  "usage: jedi - t [threads] -- [encoding] [input] \n");
   (void) fprintf(stderr,  "\n");
   (void) fprintf(stderr,  "   [encoding]    specify encoding option\n" );
   (void) fprintf(stderr,  "\t\t%s\n", "   r:  random encoding" );
   (void) fprintf(stderr,  "\t\t%s\n", "   h:  one hot encoding" );
   (void) fprintf(stderr,  "\t\t%s\n", "   d:  dynamic random encoding" );
   (void) fprintf(stderr,  "\t\t%s\n", "   s:  straightforward mapping" );
   (void) fprintf(stderr,  "\t\t%s\n", "   i:  input dominant algorithm" );
   (void) fprintf(stderr,  "\t\t%s\n", "   o:  output dominant algorithm (default)" );
   (void) fprintf(stderr,  "\t\t%s\n", "   c:  coupled dominant algorithm" );
   (void) fprintf(stderr,  "\t\t%s\n", "   y:  modified output dominant algorithm" );
   (void) fprintf(stderr,  "\t\t%s\n", "   g:  group based embedding algorithm" );
   (void) fprintf(stderr,  "\t\t%s\n", "   m:  weight gen. based on Mustang" );
//   (void) fprintf(stderr,  "   -%s %s\t%s\n", "s", "length", "code length for state assignment" );
//   (void) fprintf(stderr,  "   -%s\t\t%s\n", "g", "general symbolic encoding format" );
   (void) fprintf(stderr,  "\n");
   (void) fprintf(stderr,  "   [%s]\t%s\n", "input", "file to be processed (default stdin)" );
   (void) fprintf(stderr,  "\n");
//   (void) fprintf(stderr,  "%s\n", HEADER );
   (void) fprintf(stderr,  "\n");
   (void) exit(-1);
}

parse_options(char *optarg)

{
        
    /* 
     * set defaults 
     */
    beginningStates = 1;
    endingStates = 10;
    startingTemperature = 10000;
    maximumTemperature = INFINITY;

     bitsFlag = FALSE;
     addDontCareFlag = TRUE;
     kissFlag = TRUE;
     verboseFlag = TRUE;
     sequentialFlag = FALSE;
     clusterFlag = FALSE;
     srandomFlag = FALSE;
     drandomFlag = FALSE;
     variationFlag = TRUE;
     oneplaneFlag = FALSE;
     hotFlag = FALSE;
     expandFlag = FALSE;
     plaFlag = TRUE;
     mustangFlag = FALSE;
     weightType = OUTPUT;

    /*
     * parse options
     */
     
     while (optarg != NULL) {
	   
     if (!strcmp(optarg, "s")) {
		sequentialFlag = TRUE;
		printf("sequential encoding chosen ... \n");
     break; 
       } else if (!strcmp(optarg, "r")) {
	   srandomFlag = TRUE;
     printf("random encoding chosen ... \n");
		break;
	    } else if (!strcmp(optarg, "d")) {
		drandomFlag = TRUE;
     printf("dynamic random encoding chosen ... \n");
		break;
      } else if (!strcmp(optarg, "m")) {
		mustangFlag = TRUE;
     printf("Wt. generation similar to Mustang ... \n");
		break;
      } else if (!strcmp(optarg, "g")) {
		clusterFlag = TRUE;
     printf("cluster encoding chosen ... \n");
		break;
	    } else if (!strcmp(optarg, "h")) {
		hotFlag = TRUE;
     printf("One hot encoding chosen ... \n");
		break;
	    } else if (!strcmp(optarg, "i")) {
		weightType = INPUT;
		variationFlag = TRUE;
     printf("input dominant algorithm ... \n");
		break;
	    } else if (!strcmp(optarg, "o")) {
		weightType = OUTPUT;
		variationFlag = TRUE;
     printf("output dominant algorithm ... \n");
		break;
	    } else if (!strcmp(optarg, "c")) {
		weightType = COUPLED;
		variationFlag = TRUE;
     printf("coupled algorithm ... \n");
		break;
	    } else if (!strcmp(optarg, "y")) {
		weightType = OUTPUT;
		variationFlag = FALSE;
		printf("modified output dominant algorithm ... \n");
     break;
	    } else {
		usage();
		break;
	    }
}   

    
   
    return 0;
} /* end of parse_outfpoptions*/
