### Solving the Knights Tour and N-Queens problems using an Evolutionary approach 

`evolve.h` has a framework for solving problems using an iterative genetic algorithm. We start with some random specimens. Each specimen is assigned a fitness score and specimens are then combined (wherein fitness scores are used to calculate the liklihood of a particular specimen being selected) Two specimen are combined to generate two offsprings. Each offspring is then randomly mutated. The process is repeated till we have the same number of offsprings as the specimen we started with. At this point, we promote the generation so that the children are now the parent specimens and we repeat the whole process again. Eventually,  we evolve a specimen that solves the problem. 

The framework has generic concepts and concrete implentations are provided by the problems being solved.

`knights_tour.h` defines the classic chess problem of moving a knight through all the squares of a chess board without revisiting a square. `nqueens.h` defines the problem for placing N (N==8) non-attacking queens on a chessboard. Both these problems are solved using the framework in `evolve.h`.

`memoizer.h` has a generic cache used to memoize the fitness score function of specimens. 

#### Building

Tested only on linux. Uses `google-test` for unit testing. The included `CMakeLists.txt` will download and configure google-test on the fly. Also uses one header only library from boost (`boost/optional.hpp`). Expects to find boost via a CMake *find_package()* call.

#### Executing

```sh
$ ./evolve knightstour #Solve for Knights Tour
$ ./evolve nqueens     #Solve for NQueens
```

#### TODO

- Use concepts to clarify the expectations from the Specimen type
- Use a continuous evolution approach instead of discrete evolution
- Use _Warnsdorff's heuristic_ when solving for the Knight's Tour
- Solve for a circular Knight's Tour (Knight's cycle)
- Use a bounded size for the memoizing cache
- Solve both problems for any N instead of N=8

