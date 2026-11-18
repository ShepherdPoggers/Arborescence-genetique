#include "feuille.h"
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <utility>

#pragma once
#ifndef ARBRE_H
#define ARBRE_H

template <class T>
class Arbre
{
private:
    const Feuille<T> *racine;

    template<class Affichage>
    static void afficherDepuis(const Feuille<T>* feuille, std::ostream& sortie,
                               const Affichage& afficher, int indentation);
public:
    Arbre(const Feuille<T> *uneFeuille);
    // Permet l'affichage de l'arborescence au complet
    void afficherArborescence(int indentation = 0) const;
    template<class Affichage>
    void afficherArborescence(std::ostream& sortie, const Affichage& afficher,
                             int indentation = 0) const;
    // Retourne la distance minimale vers l'ancêtre, ou -1 s'il n'est pas présent.
    int trouverAncetre(const Feuille<T> *trouver) const;
    void reset();
    void setRacine(Feuille<T> &nouvellRacine);
    friend std::ostream &operator<<(std::ostream &os, const Arbre<T> &arbre)
    {
        if (arbre.racine == nullptr)
            return os << "(vide)";
        os << *arbre.racine;
        return os;
    }
};

template <class T>
Arbre<T>::Arbre(const Feuille<T> *uneFeuille)
    : racine(uneFeuille)
{
}

template <class T>
void Arbre<T>::reset()
{
    // Garde les anciens appels valides. Les parcours n'ont plus besoin d'être réinitialisés.
}

template <class T>
void Arbre<T>::setRacine(Feuille<T> &nouvelleRacine)
{
    racine = &nouvelleRacine;
}

template <class T>
void Arbre<T>::afficherArborescence(int indentation) const
{
    afficherArborescence(std::cout,
        [](std::ostream& sortie, const Feuille<T>& feuille) { sortie << feuille; },
        indentation);
}

template<class T>
template<class Affichage>
void Arbre<T>::afficherArborescence(std::ostream& sortie, const Affichage& afficher,
                                   int indentation) const
{
    afficherDepuis(racine, sortie, afficher, indentation);
}

template<class T>
template<class Affichage>
void Arbre<T>::afficherDepuis(const Feuille<T>* feuille, std::ostream& sortie,
                             const Affichage& afficher, int indentation)
{
    if (feuille == nullptr)
        return;
    for (int i = 0; i < indentation; ++i)
        sortie << ' ';
    afficher(sortie, *feuille);
    sortie << '\n';
    afficherDepuis(feuille->parent1, sortie, afficher, indentation + 4);
    afficherDepuis(feuille->parent2, sortie, afficher, indentation + 4);
}
template <class T>
int Arbre<T>::trouverAncetre(const Feuille<T> *trouver) const
{
    if (racine == nullptr || trouver == nullptr)
        return -1;

    std::queue<std::pair<const Feuille<T>*, int>> aVisiter;
    std::unordered_set<const Feuille<T>*> visites;
    aVisiter.emplace(racine, 0);
    visites.insert(racine);
    while (!aVisiter.empty())
    {
        const std::pair<const Feuille<T>*, int> courant = aVisiter.front();
        aVisiter.pop();
        if (courant.first == trouver)
            return courant.second;

        const Feuille<T>* parents[] = {courant.first->parent1, courant.first->parent2};
        for (const Feuille<T>* parent : parents)
        {
            if (parent != nullptr && visites.insert(parent).second)
                aVisiter.emplace(parent, courant.second + 1);
        }
    }
    return -1;
}

#endif
