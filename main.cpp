#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>

#include <type.h>
#include <cst.h>
#include <affichage.h>

using namespace std;

//***********************************************************************************/
//************************   Initialisation paramètres   ****************************/
//***********************************************************************************/

void initParams (CMyParam & param){

    //touche du joueur
    param.mapParamChar["toucheHaut"] = 'z';
    param.mapParamChar["toucheGauche"] = 'q';
    param.mapParamChar["toucheBas"] = 'x';
    param.mapParamChar["toucheDroite"] = 'd';

    //taille de la grille - on suppose que c'est un rectangle
    param.mapParamUnsigned["nbColonnes"] = 7;
    param.mapParamUnsigned["nbLignes"] = 7;
    param.mapParamUnsigned["nbMax"] = 5;
}

void chargerParametre(CMyParam & params, const string & fichier){
    ifstream ifs (fichier);
    if (!ifs) return;
    string cle;
    while (ifs>>cle){
        if(params.mapParamUnsigned.find(cle) != params.mapParamUnsigned.end()){
            char deuxPts;
            ifs >> deuxPts;
            unsigned val;
            ifs >> val;
            params.mapParamUnsigned[cle]=val;
        }
        else if (params.mapParamChar.find(cle) != params.mapParamChar.end()){
            char deuxPts;
            ifs >> deuxPts;
            char val;
            ifs >> val;
            params.mapParamChar[cle]=val;
        }
        else {
            string tmp;
            getline(ifs, tmp);
        }
    }
}

//***********************************************************************************/
//******************   Initialisation et remplissage matrice   **********************/
//***********************************************************************************/


//initialisation de la grille de jeu
void initMat (CMatrice & mat, const size_t & nbLignes = 10, const size_t & nbColonnes = 10, const unsigned & nbMax= KPlusGrandNombreDansLaMatrice){
    mat.resize(nbLignes, vector<contenueDUneCase>(nbColonnes));
    for (size_t i = 0; i < nbLignes; ++i) {
        for (size_t j = 0; j < nbColonnes; ++j) {
            contenueDUneCase nb;
            nb = rand() % nbMax + 1;
            mat[i][j] = nb;
        }
    }
}


void initMat(CMatrice & mat, const CMyParam & params){
    auto it = params.mapParamUnsigned.find("nbLignes");
    if (it == params.mapParamUnsigned.end())
        exit(1);
    size_t nbLignes = it->second;
    it = params.mapParamUnsigned.find("nbColonnes");
    if (it == params.mapParamUnsigned.end())
        exit(2);
    size_t nbColonnes = it->second;
    it = params.mapParamUnsigned.find("nbMax");
    if (it == params.mapParamUnsigned.end())
        exit(3);
    size_t nbMax = it->second;
    initMat(mat, nbLignes, nbColonnes, nbMax);
}

void remplirMatrice(CMatrice & mat, const unsigned & nbMax= KPlusGrandNombreDansLaMatrice){
    for (size_t numLigne = 0; numLigne < mat.size(); ++numLigne) {
        for (size_t numCol = 0; numCol < mat[numLigne].size(); ++numCol) {
            if (mat[numLigne][numCol]== KAIgnorer){
                mat[numLigne][numCol] = rand() % nbMax + 1;
            }
        }
    }
}

//***********************************************************************************/
//**************************   Explosion Horizontale   ******************************/
//***********************************************************************************/

//fait descendre toutes les cases d'une unité suite à une explosition
void explositionUneBombeHorizontale (CMatrice & mat, const size_t & numLigne,
                                    const size_t & numColonne, const size_t & combien){
    for (size_t j (numColonne); j < numColonne + combien; ++j){
        for (size_t i (numLigne); i>0; --i){
            mat [i][j] = mat[i-1][j];
        }
        mat [0][j] =  KAIgnorer;
    }
}

//
bool detectionExplositionUneBombeHorizontale (CMatrice & mat){
    bool auMoinsUneExplosion (false);
    //size_t numLigne;
    //size_t numCol;
    //on parcours la matrice case / case
    for (size_t numLigne (0); numLigne < mat.size(); ++numLigne){
        for (size_t numCol (0); numCol < mat[numLigne].size(); ++numCol){
            // si on tombe sur la valeur KAIgnorer, on passe a la case suivante
            if (KAIgnorer == mat [numLigne][numCol]) continue;
            // sinon on compte combien de fois on a la même valeur
            size_t combienALaSuite (1);
            while (numCol + combienALaSuite < mat[numLigne].size() &&
                   mat[numLigne][numCol] == mat[numLigne][numCol + combienALaSuite])
                ++combienALaSuite;
            //si on a au moins 3 chiffres identiques a la suite
            if (combienALaSuite >= 3){
                auMoinsUneExplosion = true;
                cout << "on a une suite en position numLigne = " << numLigne
                     << "; colonne = " << numCol
                     << "; sur  " << combienALaSuite << " cases" << endl;
                cout << string (20, '-') << endl << "matrice avant suppresion" << endl;
                afficheMatriceV2(mat);
                explositionUneBombeHorizontale (mat, numLigne, numCol, combienALaSuite);
                remplirMatrice(mat);
                cout << string (20, '-') << endl << "matrice après suppresion" << endl;
                            afficheMatriceV2(mat);
            }
        }
    }
    return auMoinsUneExplosion;
}

//***********************************************************************************/
//*****************************   Explosion verticale   *****************************/
//***********************************************************************************/

void explositionUneBombeVerticale (CMatrice & mat, const size_t & numLigne,
                                  const size_t & numColonne, const size_t & combien){
    //for (size_t j (numLigne); j < numLigne + combien; ++j){
    for (size_t i (numLigne); i < numLigne + combien; ++i){
        mat [i][numColonne] = KAIgnorer;
    }
    for (size_t i (numLigne); i>0; --i){
        swap(mat [i-1][numColonne], mat [i-1+combien][numColonne]);
        //mat [0][numColonne] =  KAIgnorer;
    }
}

bool detectionExplositionUneBombeVerticale (CMatrice & mat){
    bool auMoinsUneExplosion (false);
    //size_t numCol;
    //on parcours la matrice case / case
    for (size_t numCol (0); numCol < /*10*/ mat[0].size(); ++numCol){
        for (size_t numLigne (0); numLigne < mat.size(); ++numLigne){
            // si on tombe sur la valeur KAIgnorer, on passe a la case suivante
            if (KAIgnorer == mat [numLigne][numCol]) continue;
            // sinon on compte combien de fois on a la même valeur
            size_t combienALaSuite (1);
            while (numLigne + combienALaSuite < mat.size() &&
                   mat[numLigne][numCol] == mat[numLigne + combienALaSuite][numCol])
                            ++combienALaSuite;
            //si on a aun moins 3 chiffres identiques a la suite
            if (combienALaSuite >= 3){
                            auMoinsUneExplosion = true;
                            cout << "on a une suite en position numLigne = " << numLigne
                                 << "; colonne = " << numCol
                                 << "; sur  " << combienALaSuite << " cases" << endl;
                            cout << string (20, '-') << endl << "matrice avant suppresion" << endl;
                            afficheMatriceV1(mat);
                            explositionUneBombeVerticale(mat, numLigne, numCol, combienALaSuite);
                            remplirMatrice(mat);
                            cout << string (20, '-') << endl << "matrice après suppresion" << endl;
                            afficheMatriceV1(mat);
            }
        }
    }
    return auMoinsUneExplosion;
}

//***********************************************************************************/
//********************************   Mouvements   ***********************************/
//***********************************************************************************/

void faitUnMouvement (CMatrice & mat, const char & deplacment, size_t & numLigne, size_t & numCol) {
    numLigne = numLigne - 1;
    numCol = numCol - 1;
    //size_t nouvellePositionLigne (numLigne), nouvellePositionColonne (numCol);
    switch (tolower(deplacment)) {
    case 'z':
        if (numLigne==0){
            cout << "erreur" << endl;
            break;
        }
        swap(mat[numLigne][numCol],mat[numLigne-1][numCol]);
        break;
    case 'q':
        if (numCol==0){
            cout << "erreur" << endl;
            break;
        }
        swap(mat[numLigne][numCol],mat[numLigne][numCol-1]);
        break;
    case 'd':
        if (numCol==mat[numLigne].size()-1){
            cout << "erreur" << endl;
            break;
        };
        swap(mat[numLigne][numCol],mat[numLigne][numCol+1]);
        break;
    case 'x':
        if (numLigne==mat.size()-1){
            cout << "erreur" << endl;
            break;
        }
        swap(mat[numLigne][numCol],mat[numLigne+1][numCol]);
        break;
    default:
        cout<<"Tu choisis A ou Z ou E  ou Q ou D ou X ou C ou V"<<endl;
        break;
    }
}

//***********************************************************************************/
//************************************   Tests   ************************************/
//***********************************************************************************/

int ppalExo01 (){
    CMatrice mat = {};
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    afficheMatriceV2 (mat);
    //detectionExplositionUneBombeHorizontale (mat);
    return 0;
}

int ppalExo02 (){
    CMatrice mat;
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    afficheMatriceV2 (mat);
    while (detectionExplositionUneBombeHorizontale(mat)){
        detectionExplositionUneBombeHorizontale(mat);
    }
    afficheMatriceV2 (mat);
    return 0;
}
int ppalExo02bis (){
    CMatrice mat;
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    afficheMatriceV2 (mat);
    while (detectionExplositionUneBombeVerticale(mat)){
        detectionExplositionUneBombeVerticale(mat);
    }
    afficheMatriceV2 (mat);
    return 0;
}

int ppalExo03 (){
    CMatrice mat;
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    afficheMatriceV1 (mat);
    while (detectionExplositionUneBombeHorizontale (mat) && detectionExplositionUneBombeVerticale(mat)) {
        detectionExplositionUneBombeHorizontale(mat);
        detectionExplositionUneBombeVerticale(mat);
    }
    detectionExplositionUneBombeVerticale(mat);
    afficheMatriceV2 (mat);
    while (true) {
        cout << "Fait un mouvement ";
        cout << "numero de ligne : ";
        size_t numLigne;
        cin >> numLigne;
        cout << "numero de colonne : ";
        size_t numCol;
        cin >> numCol;
        cout << "Sens du deplacement : (A|Z|E|Q|D|W|X|C) : " << endl;
        char deplacement;
        cin >> deplacement;
        faitUnMouvement (mat, deplacement, numLigne, numCol);
        afficheMatriceV2 (mat);
    }
    return 0;
}

/*
int ppalExo04 (){
    CMatrice mat;
    initMat(mat);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    detectionExplositionUneBombeHorizontale (mat);
    afficheMatriceV2 (mat);
    //condition de victoire a trouver
    while (true) {
        cout << "Fait un mouvement ";
        cout << "numero de ligne : ";
        size_t numLigne;
        cout << numLigne;
        cout << "numero de colonne : ";
        size_t numCol;
        cout << numCol;
        cout << "Sens du deplacement : (A|Z|E|Q|D|W|X|C) : " << endl;
        char deplacement;
        cin >> deplacement;
        faitUnMouvement (mat, deplacement, numLigne, numCol);
        detectionExplositionUneBombeHorizontale (mat);
        afficheMatriceV2 (mat);
    }
    return 0;
}
*/

int ppalExo06 (){
    CMatrice mat;
    CMyParam params;
    initParams(params);
    chargerParametre(params, "../number_crush/config.yaml");
    //initMat(mat, 10, 10);
    initMat(mat,params);
    // affichage de la matrice sans les numéros de lignes / colonnes en haut / à gauche
    afficheMatriceV2 (mat);
    return 0;
}

//***********************************************************************************/
//*************************************   Main   ************************************/
//***********************************************************************************/

int main() {

    ppalExo06();
    srand (time(NULL));
    // ---------Exercice 2 -----------------//
    //    clearScreen();

    //    CMatrix mat (10, CVLine (10, kEmpty));
    //    mat [0][mat.size()-1] = kTokenPlayer1;
    //    mat [mat.size()-1][0] = kTokenPlayer2;
    //    showMatrix(mat);
    //-------------------------------------//
    //return 0;

    // ---------Exercice 2 -----------------//
    //return ppalExo02();
    //-------------------------------------//

    // ---------Exercice 3 -----------------//
    //return ppalExo03();
    //-------------------------------------//

    // ---------Exercice 3 -----------------//
    //return ppalExo04();
    //-------------------------------------//
}
