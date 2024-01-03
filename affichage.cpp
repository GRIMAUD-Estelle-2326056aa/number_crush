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

// affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
void  afficheMatriceV0 (const CMatrice & Mat) {
    //TODO
    for (size_t i = 0; i < 10; ++i) {
        cout << "|";
        for (size_t j = 0; j < 10; ++j) {
            cout << Mat[i][j] << "|";
        }
        cout << endl;
    }
}

// affichage de la matrice avec les numéros de lignes / colonnes en haut / à gauche
// avec un fond de couleur
void  afficheMatriceV1 (const CMatrice & Mat) {
    //TODO
    for (size_t i = 0; i < 10; ++i) {
        cout << "|";
        for (size_t j = 0; j < 10; ++j) {
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

// affichage de la matrice avec les numéros de lignes / colonnes en haut / à gauche
// avec un fond de couleur
// et mis en forme
void  afficheMatriceV2 (const CMatrice & mat) {
    //TODO
    cout << "  ";
    for (size_t k = 0; k < mat.size(); ++k){
        cout << setw(4) << k+1;
    }
    cout << endl;
    for (size_t i = 0; i < 10; ++i) {
        cout << setw(2) << i+1 << " ";
        cout << "|";
        for (size_t j = 0; j < 10; ++j) {
            if (mat[i][j] == 1){
                couleur(KNoir);
            }
            if (mat[i][j] == 2){
                couleur(KRouge);
            }
            if (mat[i][j] == 3){
                couleur(KBleu);
            }
            if (mat[i][j] == 4){
                couleur(KCyan);
            }
            if (mat[i][j] == KAIgnorer){
                couleur(KBGCyan);
                couleur(KCyan);
            }
            cout << " " << mat[i][j] << " ";
            couleur(KReset);
            cout << "|";
        }
        cout << endl;
    }
}
