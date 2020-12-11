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
 * Symbolic stencoding program for compiling a symbolic
 * description into a binary representation.  Target
 * is multi-level logic synthesis
 *
 * History:
 *     3/20/90: recognize ANY or * on input
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
int read_kiss();                        /* forward declaration */
char *mk_var_name();                    /* forward declaration */


/* *  util_strsav -- save a copy of a string
 */

char *s;
util_strsav(s)
{
   if (s == NIL(char)) {
   return s;
    }
    else {
   return strcpy(ALLOC(char, strlen(s)+1), s);
    }
}



/*
 * global variable
 */

int dummy_count = 0;

read_kiss(FILE *fin,FILE *fout,THREADED)

{   
    Boolean iflag = FALSE;
    Boolean oflag = FALSE;
    Boolean sflag = FALSE;
    Boolean ilbflag = FALSE;
    Boolean obflag = FALSE;
    Boolean found;
    char tline[BUFSIZE];
    char line[BUFSIZE];
    int i, j;
    int k, l, m;
    int nc, nb;
    int t;

    np = 0;
    
    
    
    reset_state = NIL(char);
    
    while (fgets(line, BUFSIZE, fin) != NULL) {
        (void) strcpy(tline, line);
        parse_line(tline);
        if (targc == 0) {
            /* skip */
        } else if (targv[0][0] == 0) {
            /* skip */
        } else if (targv[0][0] == '#') {
            /*
             * pass on comments
             */
            (void) fprintf(fout, "%s", line);
        } else if (!strcmp(targv[0], ".i")) {
            if (iflag) {
                (void) fprintf(stderr,
                  "warning: .i has been declared more than once\n");
                continue;
            }

            iflag = TRUE;
            ni = atoi(targv[1]) + 1;
            inputs = ALLOC(struct Variable, ni);
            for (i=0; i<ni; i++) {
                inputs[i].name = mk_var_name();                     /* default */
                inputs[i].boolean_flag = TRUE;
                inputs[i].enumtype_ptr = 0;
                inputs[i].entries = ALLOC(struct Entry, 1);        /* undefined */
            }
            inputs[ni-1].boolean_flag = FALSE;        /* presentState input */
        } else if (!strcmp(targv[0], ".o")) {
            if (oflag) {
                (void) fprintf(stderr,
                  "warning: .o has been declared more than once\n");
                continue;
            }

            oflag = TRUE;
            no = atoi(targv[1]) + 1;
            outputs = ALLOC(struct Variable, no);
            for (i=0; i<no; i++) {
                outputs[i].name = mk_var_name();                /* default */
                outputs[i].boolean_flag = TRUE;
                outputs[i].enumtype_ptr = 0;
                outputs[i].entries = ALLOC(struct Entry, 1);        /* undefined */
            }
            outputs[0].boolean_flag = FALSE;                /* nextState output */
        } else if (!strcmp(targv[0], ".ilb")) {
            if (!iflag) {
                (void) fprintf(stderr,
                  "fatal error: .i has not been defined\n");
                exit(1);
            }

            if (ilbflag) {
                (void) fprintf(stderr,
                  "warning: .ilb has been declared more than once\n");
                continue;
            }

            ilbflag = TRUE;
            for (i=0; i<ni; i++) {
                FREE(inputs[i].name);                        /* free previous ptr */
                inputs[i].name = util_strsav(targv[i+1]);        /* assign new name */
            }
        } else if (!strcmp(targv[0], ".ob")) {
            if (!oflag) {
                (void) fprintf(stderr,
                  "fatal error: .o has not been defined\n");
                exit(1);
            }

            if (obflag) {
                (void) fprintf(stderr,
                  "warning: .ob has been declared more than once\n");
                continue;
            }

            obflag = TRUE;
            for (i=0; i<no; i++) {
                FREE(outputs[i].name);                        /* free previous ptr */
                outputs[i].name = util_strsav(targv[i+1]);        /* assign new name */
            }
        } else if (!strcmp(targv[0], ".s")) {
            if (sflag) {
                (void) fprintf(stderr,
                  "warning: .s has been declared more than once\n");
                continue;
            }

            sflag = TRUE;
            ne = 1;
            enumtypes = ALLOC(struct Enumtype, 1);
            enumtypes[0].name = "States";
            enumtypes[0].ns = 0;                        /* undefined */
            t = atoi(targv[1]);
	    if (t <= 1) {
		t = 1;
	    } else {
		t = (int) ceil(log((double) t)/ log(2.0));
	    }

            if (bitsFlag) {
                if (code_length < t) {
                    (void) fprintf(stderr, 
                      "warning: code length %d is not long enough: ",
                      code_length);
                    (void) fprintf(stderr, "using default length %d.\n", t);
                    nb = enumtypes[0].nb = t;
                } else {
                    nb = enumtypes[0].nb = code_length;
                }
            } else {
                nb = enumtypes[0].nb = t;
            }

            if (nb > MAXSPACE) {
                (void) fprintf(stderr, 
                    "panic: type (%s) code length %d exceeds limit of %d.\n",
                    "States", nb, MAXSPACE);
            }

            nc = enumtypes[0].nc = pow_of_2(nb);
            enumtypes[0].input_flag = FALSE;
            enumtypes[0].output_flag = FALSE;

            enumtypes[0].dont_care = ALLOC(char, nb + 1);
            for (i=0; i<nb; i++) {
                enumtypes[0].dont_care[i] = '-';
            }
            enumtypes[0].dont_care[nb] = '\0';
        
            enumtypes[0].codes = ALLOC(struct Code, nc);
            for (i=0; i<nc; i++) {
                enumtypes[0].codes[i].assigned = FALSE;                /* undefined */
                enumtypes[0].codes[i].bit_vector = 
                  int_to_binary(i, nb);
                enumtypes[0].codes[i].symbol_ptr = 0;                /* undefined */
            }

            enumtypes[0].symbols = ALLOC(struct Symbol, 1);        /* undefined */
            enumtypes[0].links = ALLOC(struct Link *, 1);        /* undefined */

            /*
             * compute code distances
             */
            enumtypes[0].distances = ALLOC(int *, nc);
            for (i=0; i<nc; i++) {
                enumtypes[0].distances[i] = ALLOC(int, nc);
            }
            for (i=0; i<nc; i++) {
                for (j=0; j<nc; j++) {
                    enumtypes[0].distances[i][j] =
                      distance(enumtypes[0].codes[i].bit_vector,
                        enumtypes[0].codes[j].bit_vector, nb);
                }
            }
        } else if (!strcmp(targv[0], ".r")) {
           reset_state = util_strsav(targv[1]);
        } else if (!strcmp(targv[0], ".e")) {
           /* ignore end command */
        } else if (targv[0][0] == '.') {
            /* skip */
        } else if (targc == 4) {
            if (!iflag || !oflag || !sflag) {
                (void) fprintf(stderr,
                  "fatal error: either .i, .o, or .s has not been defined\n");
                exit(1);
            }

            np++;
            k = np-1;
            l = ni-1;

            /*
             * parse primary inputs
             */
            for (i=0; i<l; i++) {
                inputs[i].entries = 
                  REALLOC(struct Entry, inputs[i].entries, np);
                inputs[i].entries[k].token = ALLOC(char, 2);
                inputs[i].entries[k].token[0] = targv[0][i];
                inputs[i].entries[k].token[1] = '\0';
                inputs[i].entries[k].enumtype_ptr = 0;
            }

            /*
             * parse presentState
             */
            inputs[l].entries = 
              REALLOC(struct Entry, inputs[l].entries, np);
          inputs[l].entries[k].token = util_strsav(targv[1]);
            inputs[l].entries[k].enumtype_ptr = 0;

            if (!check_dont_care(inputs[l].entries[k].token)) {
                found = FALSE;
                for (j=0; j<enumtypes[0].ns; j++) {
                    if (!strcmp(inputs[l].entries[k].token,
                     enumtypes[0].symbols[j].token)) {
                        inputs[l].entries[k].symbol_ptr = j;
                        found = TRUE;
                        break;
                    }
                }
                if (!found) {
                    enumtypes[0].ns++;
                    enumtypes[0].symbols = REALLOC(struct Symbol, 
		      enumtypes[0].symbols, enumtypes[0].ns);
                    m = enumtypes[0].ns-1;

                    inputs[l].entries[k].symbol_ptr = m;
                    enumtypes[0].symbols[m].token =
                      util_strsav(inputs[l].entries[k].token);
                    enumtypes[0].symbols[m].code_ptr = m;

                    /*
                     * assign codes
                     */
                    enumtypes[0].codes[m].assigned = TRUE;
                    enumtypes[0].codes[m].symbol_ptr = m;
                }
            }

            /*
             * parse primary outputs 
             */
            for (i=0; i<no-1; i++) {
                outputs[i+1].entries = 
                  REALLOC(struct Entry, outputs[i+1].entries, np);
                outputs[i+1].entries[k].token = ALLOC(char, 2);
                outputs[i+1].entries[k].token[0] = targv[3][i];
                outputs[i+1].entries[k].token[1] = '\0';
                outputs[i+1].entries[k].enumtype_ptr = 0;
            }

            /*
             * parse nextState
             */
            outputs[0].entries = 
              REALLOC(struct Entry, outputs[0].entries, np);
            outputs[0].entries[k].token = util_strsav(targv[2]);
            outputs[0].entries[k].enumtype_ptr = 0;

            if (!check_dont_care(outputs[0].entries[k].token)) {
		found = FALSE;
		for (j=0; j<enumtypes[0].ns; j++) {
		    if (!strcmp(outputs[0].entries[k].token,
		     enumtypes[0].symbols[j].token)) {
			outputs[0].entries[k].symbol_ptr = j;
			found = TRUE;
			break;
		    }
		}
		if (!found) {
		    enumtypes[0].ns++;
		    enumtypes[0].symbols = REALLOC(struct Symbol, 
		      enumtypes[0].symbols, enumtypes[0].ns);
		    m = enumtypes[0].ns-1;

		    outputs[0].entries[k].symbol_ptr = m;
		    enumtypes[0].symbols[m].token =
		      util_strsav(outputs[0].entries[k].token);
		    enumtypes[0].symbols[m].code_ptr = m;

		    /*
		     * assign codes
		     */
		    enumtypes[0].codes[m].assigned = TRUE;
		    enumtypes[0].codes[m].symbol_ptr = m;
		}
	    }
        } else {
            (void) fprintf(stderr, "warning: ignored incorrect input ...\n");
            (void) fprintf(stderr, "----> %s", line);
        }
    }

    /*
     * check to make sure a truth table was read in
     */
    if (!iflag || !oflag || !sflag) {
        (void) fprintf(stderr, "jedi: unable to find input truth table\n");
        exit(1);
    }

    /*
     * initialize connectivity matrix to be
     * computed when the weights are computed
     */
    enumtypes[0].links = ALLOC(struct Link *, enumtypes[0].ns);
    for (i=0; i<enumtypes[0].ns; i++) {
        enumtypes[0].links[i] = 
          ALLOC(struct Link, enumtypes[0].ns);
    }

    for (i=0; i<enumtypes[0].ns; i++) {
        for (j=0; j<enumtypes[0].ns; j++) {
            enumtypes[0].links[i][j].weight = 0;        /* undefined */
        }
    }

    /*
     * set reset state
     */
    if (reset_state == NIL(char)) {
        reset_state = util_strsav(enumtypes[0].symbols[0].token);
    }

    /*
     * determine total inputs and total outputs
     */
    tni = ni + enumtypes[0].nb - 1;
    tno = no + enumtypes[0].nb - 1;
	return 0;
} /* end of read_kiss */


char *
mk_var_name()
{
    char buffer[BUFSIZE];

    (void) sprintf(buffer, "v.%d", dummy_count++);
   return util_strsav(buffer);
} /* end of mk_var_name */
