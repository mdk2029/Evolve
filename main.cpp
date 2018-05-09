#include "evolve.h"
#include "nqueens.h"
#include "knights_tour.h"
#include <iterator>

int main(int argc, char** argv) {

    if(argc > 1 && std::string(argv[1]) == "nqueens" ) {
        //Start off with 50 boards.
        std::vector<NQueens::Board> initialBoards;
        std::generate_n(std::back_inserter(initialBoards),50,NQueens::randomBoard);
        Evolve::Generation<NQueens::Board> seedGeneration{std::move(initialBoards)};
        Evolve::evolve(seedGeneration);
    } else if(argc > 1 && std::string(argv[1]) == "knightstour"){
        //Start off with 50 tours.
        std::vector<KnightsTour::Tour> initialTours;
        std::generate_n(std::back_inserter(initialTours),50,KnightsTour::randomTour);
        Evolve::Generation<KnightsTour::Tour> seedGeneration{std::move(initialTours)};
        Evolve::evolve(seedGeneration);
    } else {
        std::cout << "Usage: \n evolve [nqueens|knightstour]\n";
    }
}
