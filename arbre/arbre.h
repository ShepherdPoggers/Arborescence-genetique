#include "feuille.h"
#include <iostream>
#include <string>

#pragma once
#ifndef ARBRE_H
#define ARBRE_H

template <class T>
class Arbre
{
private:
    Feuille<T> *racine;
    Feuille<T> *courrant;
public:
    Arbre(Feuille<T> *uneFeuille);
    // Permet l'affichage de l'arborescence au complet
    void afficherArborescence(int indentation = 0);
    // Retourne la longueur du chemin minimal pour trouver l'ancêtre la valeur de retoure est de -1 s'il n'est pas présent.
    int trouverAncetre(const Feuille<T> *trouver);
    void reset();
    void setRacine(Feuille<T> &nouvellRacine);
    friend std::ostream &operator<<(std::ostream &os, const Arbre<T> &arbre)
    {
        os << *arbre.racine;
        return os;
    }
};

template <class T>
Arbre<T>::Arbre(Feuille<T> *uneFeuille)
    : racine(uneFeuille), courrant(uneFeuille)
{
}

template <class T>
void Arbre<T>::reset()
{
    courrant = racine;
}

template <class T>
void Arbre<T>::setRacine(Feuille<T> &nouvelleRacine)
{
    racine = nouvelleRacine;
}

template <class T>
void Arbre<T>::afficherArborescence(int indentation)
{
    std::string s = "";
    if (courrant == nullptr)
        return;
    for (int i = 0; i < indentation; i++)
        s += " ";
    std::cout << s << *courrant << std::endl;
    indentation++;
    Feuille<T> *temp = courrant;
    courrant = temp->parent1;
    afficherArborescence(indentation);
    courrant = temp->parent2;
    afficherArborescence(indentation);
    return;
}
template <class T>
int Arbre<T>::trouverAncetre(const Feuille<T> *trouver)
{

    int i = 0;
    int j = 0;

    if (courrant == nullptr)
        return -1;
    else if (courrant == trouver)
        return 0;

    Feuille<T> *temp = courrant;
    courrant = temp->parent1;
    i += (trouverAncetre(trouver));
    courrant = temp->parent2;
    j += (trouverAncetre(trouver));

    if (i == -1 && j == -1)
        return -1;

    if (i == -1)
        return j + 1;

    if (j == -1)
        return i + 1;

    return 1 + (i < j ? i : j);
}

#endif