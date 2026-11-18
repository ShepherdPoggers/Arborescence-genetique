#include "execution.h"
#include "arbre/arbre.h"
#include "cellule/ColorCell.h"
#include "cellule/ChromoCell.h"

#include <filesystem>
#include <fstream>
#include <utility>
#include <map>
#include <sstream>
#include <stdexcept>

namespace
{
    struct ParentsALier
    {
        int id;
        int parent1;
        int parent2;
        std::size_t ligne;
    };

    void verifierType(const std::string& type)
    {
        if (type != "COLORCELL" && type != "CHROMOCELL")
            throw std::invalid_argument("Type de population inconnu : " + type);
    }

    bool lireLigneNonVide(std::istream& fichier, std::string& ligne,
                         std::size_t& numeroLigne)
    {
        while (std::getline(fichier, ligne))
        {
            ++numeroLigne;
            if (ligne.find_first_not_of(" \t\r\n") != std::string::npos)
                return true;
        }
        return false;
    }

    void verifierFinLigne(std::istream& flux)
    {
        std::string surplus;
        if (flux >> surplus)
            throw std::invalid_argument("Donnees en trop sur la ligne.");
    }

    int lireEntier(const std::string& texte)
    {
        std::istringstream flux(texte);
        int valeur;
        if (!(flux >> valeur))
            throw std::invalid_argument("Entier invalide : " + texte);
        verifierFinLigne(flux);
        return valeur;
    }

    int lireParent(const std::string& texte)
    {
        if (texte == "/")
            return 0;
        const int id = lireEntier(texte);
        if (id <= 0)
            throw std::invalid_argument("Identifiant de parent invalide : " + texte);
        return id;
    }

    void verifierCouleur(int rouge, int vert, int bleu)
    {
        if (rouge < 0 || rouge > 255 || vert < 0 || vert > 255 ||
            bleu < 0 || bleu > 255)
        {
            throw std::invalid_argument("Les composantes RGB doivent etre entre 0 et 255.");
        }
    }

    template<class T>
    const char* nomType();

    template<>
    const char* nomType<ColorCell>() { return "COLORCELL"; }

    template<>
    const char* nomType<ChromoCell>() { return "CHROMOCELL"; }

    template<class T>
    std::unique_ptr<T> lireCellule(std::istream& flux, const std::string& nom);

    template<>
    std::unique_ptr<ColorCell> lireCellule<ColorCell>(std::istream& flux, const std::string& nom)
    {
        std::string texteRouge, texteVert, texteBleu;
        if (!(flux >> texteRouge >> texteVert >> texteBleu))
            throw std::invalid_argument("Trois entiers RGB sont attendus.");
        verifierFinLigne(flux);
        const int rouge = lireEntier(texteRouge);
        const int vert = lireEntier(texteVert);
        const int bleu = lireEntier(texteBleu);
        verifierCouleur(rouge, vert, bleu);
        return std::make_unique<ColorCell>(nom, rouge, vert, bleu);
    }

    template<>
    std::unique_ptr<ChromoCell> lireCellule<ChromoCell>(std::istream& flux, const std::string& nom)
    {
        std::string paire1, paire2, paire3, paire4;
        if (!(flux >> paire1 >> paire2 >> paire3 >> paire4))
            throw std::invalid_argument("Quatre paires de chromosomes sont attendues.");
        verifierFinLigne(flux);
        return std::make_unique<ChromoCell>(nom, paire1, paire2, paire3, paire4);
    }

    void ecrireCellule(std::ostream& flux, const ColorCell& cellule)
    {
        verifierCouleur(cellule.getRouge(), cellule.getVert(), cellule.getBleu());
        flux << cellule.getRouge() << ' ' << cellule.getVert() << ' ' << cellule.getBleu();
    }

    void ecrireCellule(std::ostream& flux, const ChromoCell& cellule)
    {
        flux << cellule.getPaire1() << ' ' << cellule.getPaire2()
             << ' ' << cellule.getPaire3() << ' ' << cellule.getPaire4();
    }

    template<class T>
    void ecrireParent(std::ostream& flux, const Feuille<T*>* parent,
                       const std::map<const Feuille<T*>*, int>& identifiants)
    {
        if (parent == nullptr)
            flux << '/';
        else
            flux << identifiants.at(parent);
    }

    void verifierNombreParametres(const Transaction& transaction, std::size_t attendu)
    {
        if (transaction.parametres.size() != attendu)
            throw std::invalid_argument("Nombre de parametres attendu : " + std::to_string(attendu));
    }

    std::string cheminPopulation(const std::filesystem::path& dossier, const std::string& nom)
    {
        return (dossier / std::filesystem::path(nom)).lexically_normal().string();
    }

    template<class T>
    Population<T> lirePopulation(std::istream& fichier, int nombreIndividus, std::size_t& numeroLigne)
    {
        Population<T> population;
        std::vector<ParentsALier> parents;
        std::string ligne;

        // Crée tous les individus avant d'associer leurs parents.
        while (lireLigneNonVide(fichier, ligne, numeroLigne))
        {
            if (population.taille() >= static_cast<std::size_t>(nombreIndividus))
                throw std::invalid_argument("Plus d'individus que le nombre annonce.");

            std::istringstream flux(ligne);
            std::string texteId, parent1, parent2;
            if (!(flux >> texteId >> parent1 >> parent2))
                throw std::invalid_argument("Ligne attendue : id parent1 parent2 valeurs.");

            const int id = lireEntier(texteId);
            const ParentsALier liens = {id, lireParent(parent1), lireParent(parent2), numeroLigne};
            std::unique_ptr<T> cellule = lireCellule<T>(flux, std::to_string(id));
            population.inserer(id, std::move(cellule));
            parents.push_back(liens);
        }

        if (fichier.bad())
            throw std::runtime_error("Erreur pendant la lecture du fichier.");
        if (population.taille() != static_cast<std::size_t>(nombreIndividus))
            throw std::invalid_argument("Moins d'individus que le nombre annonce.");

        // Associe les parents une fois tous les individus chargés.
        for (const ParentsALier& liens : parents)
        {
            numeroLigne = liens.ligne;
            if ((liens.parent1 != 0 && !population.contient(liens.parent1)) ||
                (liens.parent2 != 0 && !population.contient(liens.parent2)))
            {
                throw std::invalid_argument("Un parent reference est absent du fichier.");
            }
            population.definirParents(liens.id, liens.parent1, liens.parent2);
        }
        return population;
    }

    template<class T>
    void sauvegarderPopulationTypee(const std::string& fichierPopulation, const Population<T>& population)
    {
        const std::vector<int> ids = population.identifiants();
        std::map<const Feuille<T*>*, int> identifiants;
        for (int id : ids)
            identifiants.emplace(&population.feuille(id), id);

        // Prépare et vérifie le contenu avant d'écraser le fichier.
        std::ostringstream contenu;
        contenu << nomType<T>() << ' ' << population.taille() << '\n';
        for (int id : ids)
        {
            const Feuille<T*>& feuille = population.feuille(id);
            contenu << id << ' ';
            ecrireParent(contenu, feuille.getParent1(), identifiants);
            contenu << ' ';
            ecrireParent(contenu, feuille.getParent2(), identifiants);
            contenu << ' ';
            ecrireCellule(contenu, population.cellule(id));
            contenu << '\n';
        }

        std::ofstream fichier(fichierPopulation);
        if (!fichier)
            throw std::runtime_error("Impossible d'ouvrir le fichier : " + fichierPopulation);
        fichier << contenu.str();
        fichier.close();
        if (!fichier)
            throw std::runtime_error("Erreur d'ecriture du fichier : " + fichierPopulation);
    }

    template<class T>
    void afficherArbre(const Population<T>& population, int id, std::ostream& sortie)
    {
        if (id <= 0)
            throw std::invalid_argument("L'identifiant doit etre positif.");
        if (!population.contient(id))
            throw std::out_of_range("Individu absent : " + std::to_string(id));

        std::map<const Feuille<T*>*, int> identifiants;
        for (int identifiant : population.identifiants())
            identifiants.emplace(&population.feuille(identifiant), identifiant);

        const Arbre<T*> arbre(&population.feuille(id));
        arbre.afficherArborescence(sortie,
            [&population, &identifiants](std::ostream& flux, const Feuille<T*>& feuille)
            {
                const int identifiant = identifiants.at(&feuille);
                flux << identifiant << " : ";
                ecrireCellule(flux, population.cellule(identifiant));
            });
    }

    template<class T>
    void executerSurPopulation(const Transaction& transaction, Population<T>& population,
                               const std::filesystem::path& dossier, std::ostream& sortie)
    {
        switch (transaction.operation)
        {
        case 'A':
        {
            std::ostringstream valeurs;
            for (const std::string& parametre : transaction.parametres)
                valeurs << parametre << ' ';
            std::istringstream flux(valeurs.str());
            std::unique_ptr<T> cellule = lireCellule<T>(flux, "individu");
            const int id = population.ajouter(std::move(cellule));
            sortie << "Individu " << id << " ajoute : ";
            ecrireCellule(sortie, population.cellule(id));
            sortie << '\n';
            break;
        }
        case '+':
        {
            verifierNombreParametres(transaction, 2);
            const int parent1 = lireEntier(transaction.parametres[0]);
            const int parent2 = lireEntier(transaction.parametres[1]);
            const int id = population.additionner(parent1, parent2);
            sortie << "Individu " << id << " cree par " << parent1 << " + " << parent2 << " : ";
            ecrireCellule(sortie, population.cellule(id));
            sortie << '\n';
            break;
        }
        case '?':
        {
            verifierNombreParametres(transaction, 2);
            const int id1 = lireEntier(transaction.parametres[0]);
            const int id2 = lireEntier(transaction.parametres[1]);
            const typename Population<T>::AncetreCommun ancetre = population.trouverAncetreCommun(id1, id2);
            if (ancetre.id == 0)
            {
                sortie << "Aucun ancetre commun entre " << id1 << " et " << id2 << ".\n";
                break;
            }
            sortie << "Ancetre commun de " << id1 << " et " << id2 << " : " << ancetre.id << " (";
            ecrireCellule(sortie, population.cellule(ancetre.id));
            sortie << ")\n";
            sortie << id1 << " est a " << ancetre.distance1 << " generation(s) de " << ancetre.id << ".\n";
            sortie << id2 << " est a " << ancetre.distance2 << " generation(s) de " << ancetre.id << ".\n";
            break;
        }
        case '$':
        {
            verifierNombreParametres(transaction, 1);
            const int id = lireEntier(transaction.parametres[0]);
            afficherArbre(population, id, sortie);
            break;
        }
        case 'S':
        {
            verifierNombreParametres(transaction, 1);
            const std::string chemin = cheminPopulation(dossier, transaction.parametres[0]);
            sauvegarderPopulationTypee(chemin, population);
            sortie << "Population sauvegardee dans " << chemin << ".\n";
            break;
        }
        case '-':
        {
            verifierNombreParametres(transaction, 1);
            const int id = lireEntier(transaction.parametres[0]);
            if (id <= 0)
                throw std::invalid_argument("L'identifiant doit etre positif.");
            if (!population.supprimer(id))
                throw std::invalid_argument("Individu absent : " + std::to_string(id));
            sortie << "Individu " << id << " supprime.\n";
            break;
        }
        default:
            throw std::invalid_argument(std::string("Operation non implementee : ") + transaction.operation);
        }
    }
}

PopulationCourante chargerPopulation(const std::string& fichierPopulation)
{
    std::ifstream fichier(fichierPopulation);
    if (!fichier)
        throw std::runtime_error("Impossible d'ouvrir le fichier : " + fichierPopulation);

    std::string ligne;
    std::size_t numeroLigne = 0;
    try
    {
        if (!lireLigneNonVide(fichier, ligne, numeroLigne))
            throw std::invalid_argument("En-tete de population manquant.");

        std::istringstream entete(ligne);
        std::string type, texteNombre;
        if (!(entete >> type >> texteNombre))
            throw std::invalid_argument("En-tete attendu : TYPE nombreIndividus (positif ou nul).");
        const int nombreIndividus = lireEntier(texteNombre);
        if (nombreIndividus < 0)
            throw std::invalid_argument("Le nombre d'individus ne peut pas etre negatif.");
        verifierType(type);
        verifierFinLigne(entete);

        if (type == "COLORCELL")
            return lirePopulation<ColorCell>(fichier, nombreIndividus, numeroLigne);
        return lirePopulation<ChromoCell>(fichier, nombreIndividus, numeroLigne);
    }
    catch (const std::exception& erreur)
    {
        throw std::runtime_error(fichierPopulation + ", ligne " +
                                 std::to_string(numeroLigne) + " : " + erreur.what());
    }
}

void sauvegarderPopulation(const std::string& fichierPopulation, const Population<ColorCell>& population)
{
    sauvegarderPopulationTypee(fichierPopulation, population);
}

void sauvegarderPopulation(const std::string& fichierPopulation, const Population<ChromoCell>& population)
{
    sauvegarderPopulationTypee(fichierPopulation, population);
}

void sauvegarderPopulation(const std::string& fichierPopulation, const PopulationCourante& population)
{
    if (const Population<ColorCell>* couleurs = std::get_if<Population<ColorCell>>(&population))
        sauvegarderPopulationTypee(fichierPopulation, *couleurs);
    else if (const Population<ChromoCell>* chromosomes = std::get_if<Population<ChromoCell>>(&population))
        sauvegarderPopulationTypee(fichierPopulation, *chromosomes);
    else
        throw std::invalid_argument("Aucune population courante. Utiliser C ou O d'abord.");
}

std::vector<Transaction> chargerTransaction(const std::string& fichierTransaction)
{
    std::ifstream fichier(fichierTransaction);

    if (!fichier)
        throw std::runtime_error("Impossible d'ouvrir le fichier : " + fichierTransaction);

    std::vector<Transaction> transactions;
    std::string ligne;
    std::size_t numeroLigne = 0;

    while (std::getline(fichier, ligne))
    {
        ++numeroLigne;
        std::istringstream flux(ligne);
        std::string operation;

        // Ignore les lignes vides.
        if (!(flux >> operation))
            continue;

        if (operation.size() != 1 ||
            std::string("CAOS+-?$").find(operation[0]) == std::string::npos)
        {
            throw std::runtime_error(
                fichierTransaction + ", ligne " + std::to_string(numeroLigne) +
                " : operation invalide : " + operation);
        }

        Transaction transaction;
        transaction.operation = operation[0];
        transaction.numeroLigne = numeroLigne;

        std::string parametre;
        while (flux >> parametre)
            transaction.parametres.push_back(parametre);

        transactions.push_back(std::move(transaction));
    }

    if (fichier.bad())
        throw std::runtime_error(
            "Erreur pendant la lecture : " + fichierTransaction);

    return transactions;
}

void executerTransactions(const std::string& fichierTransaction, std::ostream& sortie)
{
    const std::vector<Transaction> transactions = chargerTransaction(fichierTransaction);
    const std::filesystem::path dossier = std::filesystem::path(fichierTransaction).parent_path();
    PopulationCourante courante;

    for (const Transaction& transaction : transactions)
    {
        try
        {
            if (transaction.operation == 'C')
            {
                verifierNombreParametres(transaction, 1);
                verifierType(transaction.parametres[0]);
                if (transaction.parametres[0] == "COLORCELL")
                    courante = Population<ColorCell>();
                else
                    courante = Population<ChromoCell>();
                sortie << "Population " << transaction.parametres[0] << " creee.\n";
            }
            else if (transaction.operation == 'O')
            {
                verifierNombreParametres(transaction, 1);
                const std::string chemin = cheminPopulation(dossier, transaction.parametres[0]);
                // Remplace la population si le chargement réussit.
                courante = chargerPopulation(chemin);
                sortie << "Population chargee depuis " << chemin << ".\n";
            }
            else if (Population<ColorCell>* couleurs = std::get_if<Population<ColorCell>>(&courante))
                executerSurPopulation(transaction, *couleurs, dossier, sortie);
            else if (Population<ChromoCell>* chromosomes = std::get_if<Population<ChromoCell>>(&courante))
                executerSurPopulation(transaction, *chromosomes, dossier, sortie);
            else
                throw std::invalid_argument("Aucune population courante. Utiliser C ou O d'abord.");
        }
        catch (const std::exception& erreur)
        {
            throw std::runtime_error(fichierTransaction + ", ligne " +
                                     std::to_string(transaction.numeroLigne) + " (" +
                                     transaction.operation + ") : " + erreur.what());
        }
    }
}
