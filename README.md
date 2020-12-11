# ParJedi

This parallel code (a SIMPLE/SMP module) provides an implementation of
a parallel algorithm for state assignment of large Finite State
Machines. High performance CAD tools are necessary to overcome the
computational complexity involved in the optimization of large
sequential circuits. FSMs constitute an important class of logic
circuits and state assignment is one of the key steps in combinational
logic optimization. The SMP-based parallel algorithm, based on the
sequential program JEDI targeting multilevel logic implementation,
scales nearly linearly with the number of processors for FSMs of
varying problem sizes chosen from standard benchmark suites while
attaining quality of results comparable to the best sequential
algorithms.

References:

D.A. Bader and K. Madduri, "A Parallel State Assignment Algorithm for
Finite State Machines," The 11th International Conference on High
Performance Computing (HiPC 2004), Bangalore, India, December 2004.
