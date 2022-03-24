# IT2R_1_Projet_ER
Projet d'IT2R du groupe 1 (année 2022).

### Utilisation de ce repository GitHub  
Afin de faciliter la prise en main de ce projet, merci d'utiliser les deux projets µVision vierges puis de développer votre programme dessus.
Etapes à suivre pour faciliter la gestion du repository github.
- `Cloner` le projet dans votre espace personnel.
- Créer une branche pour la partie du projet que vous développez
- `Merge` une fois qu'une bibliothèque constituée de fichier .c et .h a été réalisée.

Un tutoriel plus détaillé pour créer un fichier .c et .h est disponible ci-dessous.

### Création d'une libairie  
Une libairie est composé d'une fichier `.c` (programme) et d'un fichier `.h` (fichier d'en tête/header).
Afin de créer correctement des librairies de fichier il est conseillé de respecter la mise en forme suivante

1. Création du fichier .h  
Ce fichier contient deux informations primordiales cités ci-dessous. Par ailleurs son nom doit **être identique** à celui du fichier `.c` qui lui est associé.
- Le prototype des fonctions
```int getNunchuckData(void);```
- Les variables constantes
```#define nb_cote_carre 4```

Le fichier `.h` doit impérativemet commencer et finir par les lignes suivantes. Sans rentrer dans les détails du pourquoi nous allons supposer que nous créons la librairie `clignotants` qui sera dérivée en deux fichiers `clignotants.c` et `clignotants.h`
Il sera alors nécessaire de commencer le fichier `clignotants.h` par la ligne suivante :
```
#ifndef _clignotants_h_
#define _clignotants_h_

//Divers define pour les constantes
//Prototypes des fonctions

#endif
```

Plus généralement on appliquera cette convention avec `nom_du_fichier.c` et `nom_du_fichier.h` et le fichier `.h` suivant : 
```
#ifndef _nom_du_fichier_h_
#define _nom_du_fichier_h_

//Divers define pour les constantes
//Prototypes des fonctions

#endif
```

2. Créatioon d'un fichier `.c`  
Ce fichier contient le corps des fonctions. Il est **impératif** de commencer ce fichier en intégrant le fichier `.h` qui lui est associé
```
#include clignotants.h

int allume_clignotant_gauche(void){
  //Cors du programme 
  //...
  //fin du programme
}
```
