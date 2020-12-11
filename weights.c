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

int     compute_weights();		/* forward declaration */
int     add_input_weights();		/* forward declaration */
int     add_output_weights();		/* forward declaration */
int     add_normal_inputs();		/* forward declaration */
int     add_normal_outputs();		/* forward declaration */
int     add_variation_inputs();		/* forward declaration */
int     add_variation_outputs();	/* forward declaration */
int     normal_input_proximity();	/* forward declaration */
int     normal_output_proximity();	/* forward declaration */

compute_weights(THREADED)
{
    int i, e;
       
	 on_one_thread { (void) fprintf(stderr, "computing weights ...\n"); }
    
    if (weightType == OUTPUT) {
    add_output_weights(0,TH);

   } else if (weightType == INPUT) {
	 add_input_weights(ni-1,TH); 
    
   } else {
   add_output_weights(0,TH);
   add_input_weights(ni-1,TH);
   }
	return 0;
} /* end of compute_weights */


add_input_weights(int i, THREADED)

{
    int e;
    e = inputs[i].enumtype_ptr;
    enumtypes[e].input_flag = TRUE;
    node_Barrier();
    add_variation_inputs(i,TH);
	 return 0;
} /* end of add_input_weights */


add_output_weights(int i,THREADED)

{
    int e;
    e = outputs[i].enumtype_ptr;
    enumtypes[e].output_flag = TRUE;
    node_Barrier();
    add_variation_outputs(i,TH);
    return 0;
} /* end of add_output_weights */



add_variation_inputs(int i, THREADED)

{
    int si, sj;
    int e;
    int ie;
    int n;
    int we;
    int nb;
    int nw_si, nw_sj;
    int k, l;

    e = inputs[i].enumtype_ptr;
    node_Barrier();
    on_one_thread printf("No. of states -- %d\n",enumtypes[e].ns);
//    pardo(si,0,enumtypes[e].ns,1)   {
      for (si=0; si<enumtypes[e].ns; si++) {
      pardo(sj,0,si,1)   {
  
     	for (n=0; n<no; n++) {
		    if (outputs[n].boolean_flag) {
			
			nw_si = nw_sj = 0;
			for (k=0; k<np; k++) {
			    if (!strcmp(outputs[n].entries[k].token, "1")) {
				if (!strcmp(inputs[i].entries[k].token,
				  enumtypes[e].symbols[si].token)) {
				    nw_si++;
				} else if (!strcmp(inputs[i].entries[k].token,
				  enumtypes[e].symbols[sj].token)) {
				    nw_sj++;
				}
			    }
			}
			we = nw_si * nw_sj;
			enumtypes[e].links[si][sj].weight += we;
			enumtypes[e].links[sj][si].weight += we;
		    } else {
			ie = outputs[n].enumtype_ptr;
			nb = enumtypes[ie].nb;

			/*
			 * for each symbol
			 */
			for (l=0; l<enumtypes[ie].ns; l++) {
			    nw_si = nw_sj = 0;
			    for (k=0; k<np; k++) {
				if (!strcmp(outputs[n].entries[k].token, 
				  enumtypes[ie].symbols[l].token)) {
				    if (!strcmp(inputs[i].entries[k].token,
				      enumtypes[e].symbols[si].token)) {
					nw_si++;
				    } else if 
				      (!strcmp(inputs[i].entries[k].token,
				      enumtypes[e].symbols[sj].token)) {
					nw_sj++;
				    }
				}
			    }
			    we = nw_si * nw_sj * nb;
			    enumtypes[e].links[si][sj].weight += we;
			    enumtypes[e].links[sj][si].weight += we;
			}
		    }
		}
	    }
    }
   node_Barrier();
	return 0;
} /* end of add_variation_inputs */


add_variation_outputs(int i,THREADED)

{
    int si, sj;
    int e;
    int ie;
    int n;
    int we;
    int nb;
    int nw_si, nw_sj;
    int k, l;

    e = outputs[i].enumtype_ptr;
    on_one_thread printf("No. of states -- %d\n",enumtypes[e].ns);
    node_Barrier();  
     for (si=0; si<enumtypes[e].ns; si++) {
//   pardo(si,0,enumtypes[e].ns,1)  { 
     pardo(sj,0,si,1)  {
   		for (n=0; n<ni; n++) {
		    if (inputs[n].boolean_flag) {
			/*
			 * I-on
			 */
			nw_si = nw_sj = 0;
			for (k=0; k<np; k++) {
			    if (!strcmp(inputs[n].entries[k].token, "1")) {
      			if (!strcmp(outputs[i].entries[k].token,
				  enumtypes[e].symbols[si].token)) {
				    nw_si++;
				} else if (!strcmp(outputs[i].entries[k].token,
				  enumtypes[e].symbols[sj].token)) {
				    nw_sj++;
				}
			    }
			}
			we = nw_si * nw_sj;
			enumtypes[e].links[si][sj].weight += we;
			enumtypes[e].links[sj][si].weight += we;

			/*
			 * I-off
			 */
			nw_si = nw_sj = 0;
			for (k=0; k<np; k++) {
			    if (!strcmp(inputs[n].entries[k].token, "0")) {
				if (!strcmp(outputs[i].entries[k].token,
				  enumtypes[e].symbols[si].token)) {
				    nw_si++;
				} else if (!strcmp(outputs[i].entries[k].token,
				  enumtypes[e].symbols[sj].token)) {
				    nw_sj++;
				}
			    }
			}
			we = nw_si * nw_sj;
			enumtypes[e].links[si][sj].weight += we;
			enumtypes[e].links[sj][si].weight += we;
		    } else {
			ie = inputs[n].enumtype_ptr;
			nb = enumtypes[ie].nb;

			/*
			 * for each symbol
			 */
			for (l=0; l<enumtypes[ie].ns; l++) {
			    nw_si = nw_sj = 0;
			    for (k=0; k<np; k++) {
				if (!strcmp(inputs[n].entries[k].token, 
				  enumtypes[ie].symbols[l].token)) {
				    if (!strcmp(outputs[i].entries[k].token,
				      enumtypes[e].symbols[si].token)) {
					nw_si++;
				    } else if 
				      (!strcmp(outputs[i].entries[k].token,
				      enumtypes[e].symbols[sj].token)) {
					nw_sj++;
				    }
				}
			    }
			    we = nw_si * nw_sj * nb;
			    enumtypes[e].links[si][sj].weight += we;
			    enumtypes[e].links[sj][si].weight += we;
			}
		    }
		}
	    }
    }
   node_Barrier();
	return 0;
} /* end of add_variation_outputs */
