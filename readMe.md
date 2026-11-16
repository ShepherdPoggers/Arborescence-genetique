# Arborescence génétique

## Auteurs
- SHEE15060600, Shepherd, Elliot
- DESA07010400, Desbiens, Anthony
- MARW31050600, Martel, William


## Lancements du progjets 

### Linux
Pour compiler et lancer le projet sous linux il suffit d'exécuter : 

```bash
g++ -std=c++17 -g -Wall -Wextra main.cpp cellule/*.cpp -o programme
./programme
```

### Windows
Pour compiler et lancer le projet sous linux il suffit d'exécuter : 

```powershell
g++ -std=c++17 -g -Wall -Wextra main.cpp cellule/cell.cpp cellule/ChromoCell.cpp cellule/ColorCell.cpp -o programme.exe
.\programme.exe
```

## Corrections ciblées dans les sections Feuille, Arbre et ColorCell

1. **Retrait de l'addition dans `arbre/feuille.h`.** L'ancien `Feuille::operator+`
   créait une cellule et un nœud avec `new`, mais le nœud ne possédait pas la
   cellule : supprimer le nœud ne libérait donc pas cette cellule. Cette méthode
   était redondante avec `Population::additionner`. L'addition passe maintenant
   uniquement par la population, qui possède les cellules et les nœuds avec des
   `unique_ptr`, attribue l'identifiant et raccorde les parents. Les opérateurs
   d'addition de `ColorCell` et de `ChromoCell` sont conservés.

2. **Sécurisation des méthodes dans `arbre/arbre.h`.** `trouverAncetre` utilise
   maintenant un parcours en largeur avec une file et un ensemble de nœuds
   visités, entièrement locaux. Le membre `courrant` a été retiré et la méthode
   est `const` : deux recherches successives donnent le même résultat sans
   appel à `reset()`. Le parcours en largeur garantit la distance minimale
   lorsqu'un ancêtre apparaît dans plusieurs branches. Une racine ou une cible
   nulle donne `-1`. `reset()` reste disponible pour les anciens appels, mais
   n'a plus d'état de parcours à réinitialiser. Enfin, `operator<<` vérifie la
   racine et affiche `(vide)` si elle est nulle, au lieu de la déréférencer.

3. **Validation des RGB dans `cellule/ColorCell.cpp`.** Le constructeur refuse
   désormais toute composante inférieure à `0` ou supérieure à `255` en lançant
   `std::invalid_argument`. Cette vérification protège aussi les cellules créées
   directement en C++, sans passer par les fichiers ou les transactions. Les
   bornes `0` et `255` restent acceptées et la règle d'addition ne change pas.

## Comprendre Population et execution pour la remise

### Le rôle de chaque partie

| Partie | Responsabilité |
| --- | --- |
| `ColorCell` et `ChromoCell` | Conserver les caractéristiques d'une cellule et définir son addition avec une cellule du même type. |
| `Feuille<T*>` | Conserver un pointeur vers une cellule et deux liens vers ses parents. Malgré son nom, cette classe représente aussi les nœuds ayant des parents. |
| `Population<T>` | Posséder les cellules et les nœuds, gérer leurs identifiants, leurs liens, les additions, les suppressions et les ancêtres communs. |
| `Arbre<T*>` | Parcourir l'ascendance depuis un nœud, sans posséder ni recopier les individus. |
| `execution.h` et `execution.cpp` | Lire les fichiers, choisir le type courant, interpréter les transactions et afficher les résultats. |
| `main.cpp` | Recevoir le chemin du fichier de transactions, lancer l'exécution et afficher les erreurs. |

### Les templates : écrire un algorithme pour plusieurs types

Dans `template<class T> class Population`, `T` est un paramètre représentant un
type. Le compilateur utilise le même code pour construire les versions demandées :

```cpp
Population<ColorCell> couleurs;
Population<ChromoCell> chromosomes;
```

Dans la première version, `T` représente `ColorCell`. Dans la seconde, il
représente `ChromoCell`. Ce sont deux types C++ distincts : une population de
couleurs ne peut pas recevoir une cellule de chromosomes.

Par exemple, l'instruction suivante dans `Population::additionner` reste identique :

```cpp
std::unique_ptr<T> resultat(cellule(idParent1) + cellule(idParent2));
```

Avec `T = ColorCell`, le compilateur utilise `ColorCell::operator+`, qui calcule
la moyenne entière de chaque composante RGB. Avec `T = ChromoCell`, il utilise
`ChromoCell::operator+`, qui prend le chromosome gauche du premier parent et le
chromosome droit du second. L'ordre des parents est donc conservé.

Le template utilise les opérations offertes par `T` ; aucune classe de base
`Cell`, méthode virtuelle ou conversion entre cellules n'est nécessaire.

Les définitions de `Population<T>` sont dans son en-tête pour être visibles
lorsque les autres fichiers instancient le template. Les fonctions templates
internes d'`execution.cpp` peuvent rester dans ce fichier, car leurs utilisations
avec les deux types concrets se trouvent dans le même fichier de compilation.

### Ce que possède une population

La population contient une table ordonnée par identifiant :

```cpp
std::map<int, Individu> individus;
```

Chaque entrée possède deux objets :

```cpp
struct Individu
{
    std::unique_ptr<T> cellule;
    std::unique_ptr<Feuille<T*>> feuille;
};
```

La cellule contient les caractéristiques ; le nœud contient les liens familiaux.
`std::unique_ptr` indique qui est responsable de détruire l'objet. Quand une
entrée est effacée ou que la population est détruite, ses objets sont libérés
automatiquement.

`Feuille<T*>` est un nœud dont la valeur est un pointeur vers une cellule. Ses
parents sont des pointeurs vers d'autres nœuds. Ces pointeurs ne possèdent pas
les objets : ils servent seulement à retrouver les individus déjà en mémoire.

```text
nœud 3 --parent1--> nœud 1
nœud 4 --parent1--> nœud 1
```

Dans cet exemple, les deux enfants pointent vers le même nœud 1 et sa même
cellule. Il n'y a aucune copie du parent. Techniquement, les ascendances
partagées forment un graphe orienté sans cycle ; l'affichage déroule chaque
ascendance sous forme d'arbre, en répétant les ancêtres partagés dans leurs
différentes branches.

`.get()` sur un `unique_ptr` donne un pointeur permettant de consulter l'objet,
sans transférer sa propriété. On ne doit pas faire `delete` sur ce pointeur.

### Pourquoi utiliser std::move ?

```cpp
std::unique_ptr<ColorCell> cellule =
    std::make_unique<ColorCell>("rouge", 255, 0, 0);
int id = couleurs.ajouter(std::move(cellule));
```

`std::move` autorise ici le transfert de propriété du pointeur vers la
population. Il ne copie pas la cellule. Après cet appel réussi, le pointeur
local `cellule` est nul et la population possède l'objet.

La copie de `Population` est interdite avec `= delete`. Son déplacement est
autorisé : cela permet de transférer une population chargée dans la variable
courante sans dupliquer ses individus. Les objets pointés restent aux mêmes
adresses, donc les liens vers les parents restent valides.

### Les principales méthodes de Population

| Méthode | Fonctionnement |
| --- | --- |
| `ajouter` | Utilise `prochainId`, appelle `inserer` et retourne l'identifiant d'un individu sans parents. |
| `inserer` | Conserve un identifiant fourni par un fichier, refuse les doublons et met à jour le prochain identifiant. |
| `definirParents` | Raccorde les deux parents et refuse les cycles. L'identifiant `0` représente un parent inconnu et devient un pointeur nul. |
| `additionner` | Vérifie les deux parents, calcule la nouvelle cellule, l'ajoute et raccorde ses parents dans l'ordre demandé. |
| `supprimer` | Retire d'abord tous les liens vers l'individu chez ses enfants, puis efface l'entrée et libère ses objets. |
| `trouverAncetreCommun` | Calcule les distances minimales depuis les deux individus et choisit l'ancêtre commun minimisant leur somme. |
| `identifiants` | Retourne les identifiants dans l'ordre croissant pour parcourir la population. |

`prochainId` utilise un `long long` pour pouvoir représenter une valeur dépassant
la limite des identifiants de type `int`. `ajouter` vérifie cette limite avant
la conversion. Une suppression ne diminue pas le compteur. Après un chargement,
le prochain identifiant est calculé à partir du plus grand identifiant du fichier.

Pour supprimer un parent, il faut détacher les enfants avant de libérer le
nœud, sinon leurs pointeurs désigneraient de la mémoire libérée. Les enfants
restent dans la population et leur parent supprimé devient inconnu.

### std::variant : choisir le type pendant l'exécution

Les templates définissent les types possibles à la compilation, mais le fichier
de transactions choisit lequel utiliser pendant l'exécution. C'est le rôle de :

```cpp
using PopulationCourante = std::variant<
    std::monostate,
    Population<ColorCell>,
    Population<ChromoCell>>;
```

`using` donne simplement un nom plus court à ce type. `std::variant` contient
une seule alternative active à la fois, dont il connaît le type.

| État courant | Signification |
| --- | --- |
| `std::monostate` | Aucune population n'a encore été créée ou chargée. C'est l'état initial. |
| `Population<ColorCell>` | Les transactions portent sur des couleurs. |
| `Population<ChromoCell>` | Les transactions portent sur des chromosomes. |

Une population vide de couleurs est différente de `std::monostate` : après
`C COLORCELL`, on connaît le type même s'il n'y a encore aucun individu.

Pour accéder au contenu, le code utilise des pointeurs explicitement typés :

```cpp
if (Population<ColorCell>* couleurs =
        std::get_if<Population<ColorCell>>(&courante))
{
    executerSurPopulation(transaction, *couleurs, dossier, sortie);
}
```

`std::get_if` retourne un pointeur vers la population si cette alternative est
active, sinon `nullptr`. Le `if` teste ce pointeur. `*couleurs` donne ensuite une
référence vers la population stockée, sans la copier. Le code effectue le même
test pour `Population<ChromoCell>`.

Le variant choisit donc entre les deux types à l'exécution. Dans chaque branche,
le compilateur connaît déjà le type exact utilisé pour appeler le template.

Lors de `C CHROMOCELL`, l'affectation d'une nouvelle population vide remplace
l'ancienne et libère ses individus. Lors de `O`, le fichier est chargé dans une
nouvelle population avant le remplacement : si la lecture échoue, le remplacement
n'a pas lieu et l'exécution s'arrête avec une erreur.

### Comment execution interprète les transactions

`chargerTransaction` lit toutes les lignes dans un `std::vector<Transaction>`.
Chaque transaction conserve son caractère d'opération, ses paramètres sous
forme de chaînes et son numéro de ligne. Les lignes vides sont ignorées.

`executerTransactions` traite ensuite ce vecteur dans l'ordre. Il gère `C` et
`O`, qui choisissent ou remplacent le type courant. Pour les autres opérations,
il récupère la population active avec `std::get_if` et appelle
`executerSurPopulation<T>`. Cette fonction template contient le `switch`
traitant `A`, `+`, `-`, `S`, `$` et `?` pour les deux types de cellules.

Les erreurs de paramètres, d'identifiants ou de fichiers provoquent des
exceptions. Le programme ajoute le contexte de la transaction, puis `main`
affiche l'erreur et retourne un code d'échec. Les opérations suivantes ne sont
pas exécutées. Les chemins relatifs de `O` et `S` partent du dossier du fichier
de transactions.

### Ce qui est commun et ce qui dépend de la cellule

Les algorithmes `lirePopulation<T>`, `sauvegarderPopulationTypee<T>` et
`executerSurPopulation<T>` sont communs aux deux types. La lecture des valeurs
utilise deux spécialisations de `lireCellule<T>` : l'une lit trois entiers RGB,
l'autre quatre paires de chromosomes. `nomType<T>` possède aussi deux
spécialisations pour produire le nom écrit dans l'en-tête du fichier.

L'écriture des valeurs utilise deux surcharges d'`ecrireCellule` : deux fonctions
de même nom, l'une recevant `const ColorCell&`, l'autre `const ChromoCell&`.
Le compilateur choisit celle correspondant au type du paramètre. Cela diffère
d'une spécialisation, qui précise le comportement d'un template pour un type.

Dans une expression comme `typename Population<T>::AncetreCommun`, `typename`
indique au compilateur que le nom dépendant de `T` désigne un type. Il ne crée
pas un nouvel objet et ne détermine pas le type pendant l'exécution.

### Chargement, sauvegarde et parcours

Le chargement d'une population se fait en deux passes : créer tous les individus
avec leurs identifiants, puis raccorder les parents. Ainsi, un parent peut
apparaître après son enfant dans le fichier. Le symbole `/` du fichier représente
un parent inconnu ; il est converti en identifiant `0`, puis en pointeur nul.

La sauvegarde parcourt les identifiants et construit une correspondance entre
les adresses des nœuds et leurs identifiants. Elle peut ainsi écrire les numéros
des parents au lieu de leurs adresses mémoire. Le type du fichier est déduit du
type concret de la population, même si elle est vide. Le nom d'affichage des
cellules ne fait pas partie des valeurs sauvegardées.

Pour `$`, `Arbre` effectue un parcours en profondeur : l'individu, la branche du
premier parent, puis celle du second, avec une indentation par génération.
Une fonction passée en paramètre précise comment afficher chaque nœud avec son
identifiant et ses valeurs. Les ancêtres partagés apparaissent dans chaque
branche concernée.

Pour `?`, `Population` effectue deux parcours en largeur avec une file : d'abord
l'individu à distance `0`, puis ses parents à distance `1`, ses grands-parents
à distance `2`, etc. La première visite d'un nœud donne sa distance minimale.
Chaque nœud est enregistré une seule fois par recherche, même si plusieurs
chemins y mènent. Parmi les ancêtres présents dans les deux recherches, on
retient celui dont la somme des distances est minimale. En cas d'égalité, le
plus petit identifiant est choisi. Le résultat contient cet identifiant et les
deux distances ; l'identifiant `0` indique qu'aucun ancêtre commun n'a été trouvé.

### Un exemple à expliquer oralement

```text
C COLORCELL
A 255 0 0
A 0 0 255
+ 1 2
? 3 1
$ 3
```

`C` active une population de couleurs. Les deux `A` créent les individus 1 et 2
sans parents. `+ 1 2` crée l'individu 3 de couleur `127 0 127`, dont les liens
pointent vers les nœuds 1 et 2 existants. `? 3 1` trouve l'individu 1 à une
génération de 3 et à zéro génération de lui-même. `$ 3` affiche :

```text
3 : 127 0 127
    1 : 255 0 0
    2 : 0 0 255
```

Pour présenter la conception : « Les templates permettent de partager la
gestion de la population entre les deux types de cellules. Le variant conserve
une seule population courante et permet de choisir son type à partir du fichier.
La population possède les objets ; les liens familiaux partagent uniquement
leurs adresses. »
