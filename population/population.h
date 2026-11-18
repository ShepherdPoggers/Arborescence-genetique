#ifndef POPULATION_H
#define POPULATION_H

#include "../arbre/feuille.h"

#include <limits>
#include <map>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

// Permet de gérer une population de cellules du type T.
// La population libère les cellules et les nœuds.
// Les parents pointent vers les nœuds déjà présents.
template <class T>
class Population
{
public:
    struct AncetreCommun
    {
        int id = 0; // 0 s'il n'y a aucun ancêtre commun
        int distance1 = -1;
        int distance2 = -1;
    };

    Population() = default;
    Population(const Population&) = delete;
    Population& operator=(const Population&) = delete;
    Population(Population&&) = default;
    Population& operator=(Population&&) = default;

    // Ajoute un individu avec un nouvel identifiant.
    // Les identifiants supprimés ne sont pas réutilisés.
    int ajouter(std::unique_ptr<T> cellule)
    {
        if (prochainId > std::numeric_limits<int>::max())
            throw std::overflow_error("Aucun identifiant disponible.");

        const int id = static_cast<int>(prochainId);
        inserer(id, std::move(cellule));
        return id;
    }

    // Additionne deux individus et conserve leurs parents dans le même ordre.
    int additionner(int idParent1, int idParent2)
    {
        if (idParent1 <= 0 || idParent2 <= 0)
            throw std::invalid_argument("Les identifiants doivent etre positifs.");
        if (idParent1 == idParent2)
            throw std::invalid_argument("Les deux parents doivent etre differents.");
        if (!contient(idParent1))
            throw std::out_of_range("Individu absent : " + std::to_string(idParent1));
        if (!contient(idParent2))
            throw std::out_of_range("Individu absent : " + std::to_string(idParent2));

        Feuille<T*>* parent1 = individus.at(idParent1).feuille.get();
        Feuille<T*>* parent2 = individus.at(idParent2).feuille.get();
        std::unique_ptr<T> resultat(cellule(idParent1) + cellule(idParent2));
        const int id = ajouter(std::move(resultat));

        // Relie le nouvel individu à ses parents. Le nouveau nœud ne peut pas créer de cycle.
        Feuille<T*>* enfant = individus.at(id).feuille.get();
        enfant->parent1 = parent1;
        enfant->parent2 = parent2;
        return id;
    }

    // Ajoute un individu sans parents avec l'identifiant lu dans le fichier.
    void inserer(int id, std::unique_ptr<T> cellule)
    {
        if (id <= 0)
            throw std::invalid_argument("L'identifiant doit etre positif.");
        if (!cellule)
            throw std::invalid_argument("La cellule ne peut pas etre nulle.");
        if (contient(id))
            throw std::invalid_argument("Identifiant deja present.");

        std::unique_ptr<Feuille<T*>> noeud = std::make_unique<Feuille<T*>>(cellule.get());
        individus.emplace(id, Individu{std::move(cellule), std::move(noeud)});

        // Garde le prochain identifiant dans un long long pour éviter un dépassement.
        if (id >= prochainId)
            prochainId = static_cast<long long>(id) + 1;
    }

    // Associe les parents à un individu. La valeur 0 indique un parent inconnu.
    // Vérifie les parents et les cycles avant de modifier les liens.
    void definirParents(int id, int idParent1, int idParent2)
    {
        Feuille<T*>* enfant = individus.at(id).feuille.get();
        Feuille<T*>* parent1 = idParent1 == 0 ? nullptr
                                      : individus.at(idParent1).feuille.get();
        Feuille<T*>* parent2 = idParent2 == 0 ? nullptr
                                      : individus.at(idParent2).feuille.get();

        if (parent1 != nullptr && parent1 == parent2)
            throw std::invalid_argument("Les deux parents doivent etre differents.");
        if (atteint(parent1, enfant) || atteint(parent2, enfant))
            throw std::invalid_argument("Ces liens de parente creeraient un cycle.");

        enfant->parent1 = parent1;
        enfant->parent2 = parent2;
    }

    // Supprime l'individu. Retourne false s'il n'est pas présent.
    bool supprimer(int id)
    {
        typename std::map<int, Individu>::iterator position = individus.find(id);
        if (position == individus.end())
            return false;

        Feuille<T*>* noeud = position->second.feuille.get();
        for (std::pair<const int, Individu>& entree : individus)
        {
            Feuille<T*>* enfant = entree.second.feuille.get();
            if (enfant->parent1 == noeud)
                enfant->parent1 = nullptr;
            if (enfant->parent2 == noeud)
                enfant->parent2 = nullptr;
        }

        individus.erase(position);
        return true;
    }

    // Retourne l'ancêtre commun avec la plus petite somme des distances.
    // Un individu est à 0 génération de lui-même. En cas d'égalité, le plus petit identifiant est choisi.
    AncetreCommun trouverAncetreCommun(int id1, int id2) const
    {
        if (id1 <= 0 || id2 <= 0)
            throw std::invalid_argument("Les identifiants doivent etre positifs.");
        if (!contient(id1))
            throw std::out_of_range("Individu absent : " + std::to_string(id1));
        if (!contient(id2))
            throw std::out_of_range("Individu absent : " + std::to_string(id2));

        const std::map<const Feuille<T*>*, int> distances1 = distancesDepuis(&feuille(id1));
        const std::map<const Feuille<T*>*, int> distances2 = distancesDepuis(&feuille(id2));
        AncetreCommun resultat;
        long long meilleureSomme = std::numeric_limits<long long>::max();

        // Les identifiants sont parcourus dans l'ordre croissant.
        for (const std::pair<const int, Individu>& entree : individus)
        {
            const Feuille<T*>* noeud = entree.second.feuille.get();
            typename std::map<const Feuille<T*>*, int>::const_iterator distance1 = distances1.find(noeud);
            typename std::map<const Feuille<T*>*, int>::const_iterator distance2 = distances2.find(noeud);
            if (distance1 == distances1.end() || distance2 == distances2.end())
                continue;

            const long long somme = static_cast<long long>(distance1->second) + distance2->second;
            if (somme < meilleureSomme)
            {
                meilleureSomme = somme;
                resultat = AncetreCommun{entree.first, distance1->second, distance2->second};
            }
        }
        return resultat;
    }

    bool contient(int id) const
    {
        return individus.find(id) != individus.end();
    }

    std::size_t taille() const { return individus.size(); }

    // Retourne la cellule ou le nœud. Une erreur est lancée si l'identifiant est absent.
    // La référence ne doit plus être utilisée après la suppression de l'individu ou de la population.
    const T& cellule(int id) const { return *individus.at(id).cellule; }
    const Feuille<T*>& feuille(int id) const { return *individus.at(id).feuille; }

    std::vector<int> identifiants() const
    {
        std::vector<int> resultat;
        resultat.reserve(individus.size());
        for (const std::pair<const int, Individu>& entree : individus)
            resultat.push_back(entree.first);
        return resultat;
    }

private:
    struct Individu
    {
        std::unique_ptr<T> cellule;
        std::unique_ptr<Feuille<T*>> feuille;
    };

    std::map<int, Individu> individus;
    long long prochainId = 1;

    // Retourne la distance minimale vers chaque ancêtre avec un parcours en largeur.
    // La première visite donne la distance la plus courte, même si plusieurs chemins existent.
    static std::map<const Feuille<T*>*, int> distancesDepuis(const Feuille<T*>* depart)
    {
        std::map<const Feuille<T*>*, int> distances;
        std::queue<const Feuille<T*>*> aVisiter;
        distances.emplace(depart, 0);
        aVisiter.push(depart);

        while (!aVisiter.empty())
        {
            const Feuille<T*>* courant = aVisiter.front();
            aVisiter.pop();
            const int distanceParent = distances.at(courant) + 1;
            const Feuille<T*>* parents[] = {courant->parent1, courant->parent2};
            for (const Feuille<T*>* parent : parents)
            {
                if (parent != nullptr && distances.emplace(parent, distanceParent).second)
                    aVisiter.push(parent);
            }
        }
        return distances;
    }

    // Vérifie si on peut atteindre le nœud cible à partir du nœud de départ.
    // Chaque nœud est visité une seule fois pour éviter les parcours répétés.
    static bool atteint(const Feuille<T*>* depart, const Feuille<T*>* cible)
    {
        std::vector<const Feuille<T*>*> aVisiter;
        std::unordered_set<const Feuille<T*>*> visites;
        if (depart != nullptr)
            aVisiter.push_back(depart);

        while (!aVisiter.empty())
        {
            const Feuille<T*>* courant = aVisiter.back();
            aVisiter.pop_back();
            if (courant == cible)
                return true;
            if (!visites.insert(courant).second)
                continue;
            if (courant->parent1 != nullptr)
                aVisiter.push_back(courant->parent1);
            if (courant->parent2 != nullptr)
                aVisiter.push_back(courant->parent2);
        }
        return false;
    }
};

#endif // POPULATION_H
