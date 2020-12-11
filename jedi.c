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

#include "include/jedi.h"
#include "simple.h"

FILE *outfp;
extern int compute_weights();
extern int cluster_embedding();
extern int write_one_hot();
extern int random_embedding();
extern int opt_embedding();
int i;

FILE *outfp;
FILE  *outfp_write1;
FILE  *outfp_write2;
FILE  *infp;
FILE  *outfp_read;
jedi(THREADED)
{   
    double tenc,twt;
    outfp_write1 = fopen("output.pla", "w");
    outfp_write2 = fopen("output.blif", "w");
     /*
     * check if one hot
     */
   
   
   node_Barrier();
   outfp=stdout;
   if (hotFlag) {
   on_one_thread {
   write_one_hot(outfp);
   write_one_hot(outfp_write2);
   exit(-1);
     } }

    /*
     * compute weights
     */
   if (!sequentialFlag && !srandomFlag && !drandomFlag && !hotFlag) {
	twt = get_seconds();
   compute_weights(TH);
   twt = get_seconds()-twt;
   twt = node_Reduce_d(twt, MAX, TH);
   
    }
    
    /* 
     * solve the embedding problem 
     */
   if (srandomFlag || drandomFlag) {
	random_embedding(TH);
    } else if (clusterFlag) {
	cluster_embedding(TH);
    } else if (!sequentialFlag) {
   tenc = get_seconds();
	opt_embedding(TH);
   tenc = get_seconds()-tenc;
   tenc = node_Reduce_d(tenc, MAX, TH);
   on_one_thread {
      printf("Time taken for wt computation - %f seconds\n",twt);
      printf("Time taken for sim. annealing - %f seconds\n",tenc);
      } 
}
   node_Barrier();
   
   on_one_thread { 
   write_output(outfp); 
   write_output(outfp_write1);
   write_blif(outfp_write2);
   }
   
} 

