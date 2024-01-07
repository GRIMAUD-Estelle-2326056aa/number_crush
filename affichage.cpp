#include <affichage.h>
#include <cst.h>
#include <type.h>
#include <iomanip>
#include<iostream>
using namespace std;

void clearScreen () {
    cout << "\033[H\033[2J";
}

void couleur (const unsigned & coul) {
    cout << "\033[" << coul <<"m";
}

// affichage de la matrice sans les numéros de lignes et colonnes
void  afficheMatriceV0 (const CMatrice & Mat) {
    for (size_t i = 0; i < Mat.size(); ++i) {
        cout << "|";
        for (size_t j = 0; j < Mat[0].size(); ++j) {
            cout << Mat[i][j] << "|";
        }
        cout << endl;
    }
}


// affichage de la matrice sans les numéros de lignes et colonnes
// avec un fond de couleur
void  afficheMatriceV1 (const CMatrice & Mat) {
    for (size_t i = 0; i < Mat.size(); ++i) {
        cout << "|";
        for (size_t j = 0; j < Mat[0].size(); ++j) {
            if (Mat[i][j] == 1){
                couleur(KBGNoir);
            }
            if (Mat[i][j] == 2){
                couleur(KBGRouge);
            }
            if (Mat[i][j] == 3){
                couleur(KGBBleu);
            }
            if (Mat[i][j] == 4){
                couleur(KBGCyan);;
            }
            cout << Mat[i][j];
            couleur(KReset);
            cout << "|";
        }
        cout << endl;
    }
}

// affichage de la matrice avec les numéros de lignes et colonnes
// avec un fond de couleur
// et mise en forme
void  afficheMatriceV2 (const CMatrice & mat) {
    unsigned leSetw (3);
    // déclaration et initialisation d'un séparateur horizontal pour mettre en forme la matrice
    string uneLigneHorizontalePleine (2* (mat[0].size()-1)*(leSetw)+3, '-');
    // affichage séparateur horizontal
    cout << uneLigneHorizontalePleine << endl;
    // affichage numéros de colonnes séparés d'un pipe
    //sur fond magenta
    cout << setw(leSetw + 3) << ' ' << '|';
    for (size_t i (0); i < mat[0].size(); ++i){
        couleur (KBGMagenta);
        cout << setw(leSetw) << i;
        cout << " ";
        couleur (KReset);
        cout << "|";
    }
    cout << endl;
    // affichage séparateur horizontal
    cout << uneLigneHorizontalePleine << endl;
    // pour chaque ligne : affichage du numéro de ligne
    for (size_t numLigne (0); numLigne < mat.size(); ++numLigne){
        couleur (KBGMagenta);
        cout << setw(leSetw +1) << numLigne;
        cout << " ";
        couleur (KReset);
        cout << " | ";
        // pour chaque case de la ligne : affichage du contenu avec une couleur puis d'un pipe
        for (const int & cell : mat[numLigne]){
            switch (cell){
            case KAIgnorer :
                couleur (KBGNoir);
                break;
            case 1:
                couleur (KRouge);
                break;
            case 2:
                couleur (KBleu);
                break;
            case 3:
                couleur (KJaune);
                break;
            case 4:
                couleur (KVert);
                break;
            case 5:
                couleur (KCyan);
                break;
            case 6:
                couleur (KMAgenta);
                break;
            case 7:
                couleur (KNoir);
                break;
            }
            cout << setw(leSetw-2);
            cout << ((cell == KAIgnorer) ? " " : to_string(cell));
            couleur (KReset);
            cout << "  | " ;
        }
        couleur (KReset);
        cout << endl;
    }
    // affichage séparateur horizontal
    cout << uneLigneHorizontalePleine << endl;
}
