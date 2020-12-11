#include "simple.h"

void *SIMPLE_main(THREADED)
{ double t;
  int i,j;
  FILE  *infp;
  FILE  *outfp_read;
    
  if (THARGC != 2) {
     fprintf(stderr,"File name or encoding type missing\n");
     fprintf(stderr,"Correct usage: jedi -t [threads] -- [enc. type] [input file]\n");
     fprintf(stderr,"\n");
     exit(-1);
  }
  
  if ((infp = fopen(THARGV[1], "r")) == NULL) {
     fprintf(stderr, "Cannot open the input file \n");       
     exit(-1);
  }  

  infp = fopen(THARGV[1], "r");
  outfp_read  = fopen("log", "w");
  
  on_one_thread {
  parse_options(THARGV[0]);
  read_kiss(infp,outfp_read,TH); }

  node_Barrier();
  
  t = get_seconds();
  jedi(TH);
  node_Barrier();
  t = get_seconds()-t;
  t = node_Reduce_d(t, MAX, TH);
  on_one_thread {
  printf("Total time taken - %f seconds\n",t); }
  node_Barrier();
  SIMPLE_done(TH);
 
}

