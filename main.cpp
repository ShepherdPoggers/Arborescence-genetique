#include "execution.h"

#include <cstdlib>
#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Utilisation : " << argv[0] << " TRANSACTIONS.txt\n";
        return EXIT_FAILURE;
    }

    try
    {
        executerTransactions(argv[1], std::cout);
    }
    catch (const std::exception& erreur)
    {
        std::cerr << "Erreur : " << erreur.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
