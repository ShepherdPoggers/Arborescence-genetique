#ifndef ARBORESCENCE_GENETIQUE_EXECUTION_H
#define ARBORESCENCE_GENETIQUE_EXECUTION_H

#include "population/population.h"
#include "cellule/ColorCell.h"
#include "cellule/ChromoCell.h"

#include <ostream>
#include <string>
#include <vector>
#include <variant>

struct Transaction
{
    char operation;
    std::vector<std::string> parametres;
    std::size_t numeroLigne = 0;
};

// Contient la population courante. monostate indique qu'aucune population n'a été créée ou chargée.
using PopulationCourante = std::variant<std::monostate,
                                       Population<ColorCell>, Population<ChromoCell>>;

std::vector<Transaction> chargerTransaction(
    const std::string& fichierTransaction);

// Exécute les transactions dans l'ordre et arrête à la première erreur.
// Les chemins relatifs sont lus à partir du dossier du fichier de transactions.
void executerTransactions(const std::string& fichierTransaction, std::ostream& sortie);

// Charge les individus, puis leurs parents. Une erreur est lancée si le fichier est invalide.
// Le nom de chaque cellule correspond à son identifiant.
PopulationCourante chargerPopulation(const std::string& fichierPopulation);

// Enregistre les individus et leurs parents, sans les noms des cellules.
// Le type du fichier correspond à celui de la population, même si elle est vide.
void sauvegarderPopulation(const std::string& fichierPopulation,
                           const PopulationCourante& population);
void sauvegarderPopulation(const std::string& fichierPopulation,
                           const Population<ColorCell>& population);
void sauvegarderPopulation(const std::string& fichierPopulation,
                           const Population<ChromoCell>& population);

#endif //ARBORESCENCE_GENETIQUE_EXECUTION_H
