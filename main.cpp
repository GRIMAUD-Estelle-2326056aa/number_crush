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
    param.mapParamChar["toucheBas"] = 's';
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
    for (size_t numLigne (0); numLigne < mat.size(); ++numLigne){
        for (size_t numCol (0); numCol < mat[numLigne].size(); ++numCol){
            if(mat[numLigne][numCol] == KAIgnorer){//si val = ID trois horiz.
                if(numLigne==0){//si cest la premiere ligne
                    mat[numLigne][numCol] = 1+rand()%(nbMax);
                }else{
                    mat[numLigne][numCol] = mat[numLigne-1][numCol];
                }
            }
        }
    }
}

//***********************************************************************************/
//**************************   Explosion Horizontale   ******************************/
//***********************************************************************************/

//fait descendre toutes les cases d'une unité suite à une explosition
void explositionUneBombeHorizontale (CMatrice & mat, const size_t & numLigne, const size_t & numColonne, const size_t & combien){
    for (size_t j (numColonne); j < numColonne + combien; ++j){
        for (size_t i (numLigne); i>0; --i){
            mat [i][j] = mat[i-1][j];
        }
        mat [0][j] =  KAIgnorer;
    }
}

bool detectionExplositionUneBombeHorizontale (CMatrice & mat, int & score, int & nbCoups, int & niveau){
    bool auMoinsUneExplosion (false);
    //on parcourt la matrice case / case
    for (size_t numLigne (0); numLigne < mat.size(); ++numLigne){
        for (size_t numCol (0); numCol < mat[numLigne].size(); ++numCol){
            // si on tombe sur la valeur KAIgnorer, on passe à la case suivante
            if (KAIgnorer == mat [numLigne][numCol]) continue;
            // sinon on compte combien de fois on a la même valeur
            size_t combienALaSuite (1);
            while (numCol + combienALaSuite < mat[numLigne].size() &&
                   mat[numLigne][numCol] == mat[numLigne][numCol + combienALaSuite])
                ++combienALaSuite;
            // cas initialisation de matrice niveau 2
            if (nbCoups == 0 && niveau == 2){
                if (combienALaSuite >=2){
                    auMoinsUneExplosion = true;
                    explositionUneBombeHorizontale(mat, numLigne, numCol, combienALaSuite);
                    remplirMatrice(mat);
                }
            }
            //si on a au moins 3 chiffres identiques a la suite
            if (combienALaSuite >= 3){
                auMoinsUneExplosion = true;
                //cout << "on a une suite en position numLigne = " << numLigne
                //     << "; colonne = " << numCol
                //     << "; sur  " << combienALaSuite << " cases" << endl;
                //cout << string (20, '-') << endl << "matrice avant suppresion" << endl;
                //afficheMatriceV2(mat);
                explositionUneBombeHorizontale (mat, numLigne, numCol, combienALaSuite);
                remplirMatrice(mat);
                if (nbCoups>0){
                    score += 1;
                }
                //cout << string (20, '-') << endl << "matrice après suppresion" << endl;
                //afficheMatriceV2(mat);
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

bool detectionExplositionUneBombeVerticale (CMatrice & mat, int & score, int & nbCoups, int & niveau){
    bool auMoinsUneExplosion (false);
    // on parcourt la matrice case / case
    for (size_t numCol (0); numCol < /*10*/ mat[0].size(); ++numCol){
        for (size_t numLigne (0); numLigne < mat.size(); ++numLigne){
            // si on tombe sur la valeur KAIgnorer, on passe a la case suivante
            if (KAIgnorer == mat [numLigne][numCol]) continue;
            // sinon on compte combien de fois on a la même valeur
            size_t combienALaSuite (1);
            while (numLigne + combienALaSuite < mat.size() &&
                   mat[numLigne][numCol] == mat[numLigne + combienALaSuite][numCol])
                            ++combienALaSuite;
            // cas initialisation de matrice niveau 2
            if (nbCoups == 0 && niveau == 2){
                if (combienALaSuite >=2){
                    auMoinsUneExplosion = true;
                    explositionUneBombeVerticale(mat, numLigne, numCol, combienALaSuite);
                    remplirMatrice(mat);
                }
            }
            //on a aun moins 3 chiffres identiques a la suite
            if (combienALaSuite >= 3){
                auMoinsUneExplosion = true;
                //cout << "on a une suite en position numLigne = " << numLigne
                //     << "; colonne = " << numCol
                //     << "; sur  " << combienALaSuite << " cases" << endl;
                //cout << string (20, '-') << endl << "matrice avant suppresion" << endl;
                //afficheMatriceV2(mat);
                explositionUneBombeVerticale(mat, numLigne, numCol, combienALaSuite);
                remplirMatrice(mat);
                if (nbCoups>0){
                    score += 1;
                }
                //cout << string (20, '-') << endl << "matrice après suppresion" << endl;
                //afficheMatriceV2(mat);
            }
        }
    }
    return auMoinsUneExplosion;
}

//***********************************************************************************/
//********************************   Mouvements   ***********************************/
//***********************************************************************************/

void faitUnMouvement (CMatrice & mat) {
    cout << "Entrez les coordonnees du numero a deplacer" << endl;
    cout << "Numero de ligne : ";
    unsigned numLigne;
    cin >> numLigne;
    cout << "Numero de colonne : ";
    unsigned numCol;
    cin >> numCol;
    cout << "Choisir z, q, d ou s pour se deplacer : ";
    char deplacement;
    cin >> deplacement;
    switch (tolower(deplacement)) {
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
    case 's':
        if (numLigne==mat.size()-1){
            cout << "erreur" << endl;
            break;
        }
        swap(mat[numLigne][numCol],mat[numLigne+1][numCol]);
        break;
    default:
        cout<<"Choisir z, q, d ou s"<<endl;
        break;
    }
}

//***********************************************************************************/
//****************************   Choix configuration   ******************************/
//***********************************************************************************/


// choix taille matrice (min 4, max 20) et choix nombre de coups
/*
void creeConfig(){
    CMatrice mat;
    cout << "quel taille de matrice : ";
    unsigned taille;
    cin >> taille;
    mat.resize(taille);
    for (CVLigne & uneLigne : mat)
        uneLigne.resize(taille);
    for (size_t numLigne (0); numLigne < mat.size(); ++numLigne)
        for (size_t numCol (0); numCol < mat[numLigne].size(); ++numCol){
            mat [numLigne][numCol] = 1;}
    for (size_t numLigne (0); numLigne < mat.size(); ++numLigne){
        for (size_t numCol (0); numCol < mat[numLigne].size(); ++numCol){
            unsigned remplir;
            mat [numLigne][numCol] = 0;
            afficheMatriceV2(mat);
            couleur (KReset);
            cin >> remplir;
            mat [numLigne][numCol] = remplir;
        }
    }
    afficheMatriceV2(mat);
    while (detectionExplositionUneBombeVerticale(mat) || detectionExplositionUneBombeHorizontale(mat)) {
        //showMatrixV2 (mat);
        detectionExplositionUneBombeHorizontale (mat);
        detectionExplositionUneBombeVerticale (mat);
    }
    afficheMatriceV2(mat);
    faitUnMouvement (mat);
    afficheMatriceV2(mat);
}
*/

int ppal ();

void afficheMenu(){
    unsigned navigation = 1;
    cout << "------------------------" << endl;
    cout << "|     Candy Crush     |" << endl;
    cout << "------------------------" << endl;
    cout << "| 1. Config 1         |" << endl;
    cout << "| 2. Config 2         |" << endl;
    cout << "| 3. Config 3         |" << endl;
    cout << "| 4. Personnalise     |" << endl;
    cout << "| 5. Creation A a Z   |" << endl;
    cout << "------------------------" << endl;

    cin >> navigation;
    switch (navigation) {
    case 1:
        clearScreen();
        srand(1);
        ppal();
        break;
    case 2:
        clearScreen();
        srand(2);
        ppal();
        break;
    case 3:
        clearScreen();
        srand(4);
        ppal();
        break;
    case 4:
        clearScreen();
        cout << "------------------------" << endl;
        cout << "|     Candy Crush     |" << endl;
        cout << "------------------------" << endl;
        cout << " Rentrer la graine : " << endl;
        unsigned seed;
        cin >> seed;
        srand(seed);
        ppal();
        break;
    case 5:
        clearScreen();
        //creeConfig();
        break;
    }
}

//***********************************************************************************/
//****************************   Choix configuration   ******************************/
//***********************************************************************************/

// Niveau 2 : matrice de départ avec aucun chiffres identiques à côté

void MenuNiveaux(){
    cout << "------------------------" << endl;
    cout << "|     Candy Crush     |" << endl;
    cout << "------------------------" << endl;
    cout << "| 1. Niveau simple    |" << endl;
    cout << "| 2. Niveau expert    |" << endl;
    cout << "| 3. Personnalise     |" << endl;
    cout << "------------------------" << endl;
}

//***********************************************************************************/
//*************************************   Jeu   *************************************/
//***********************************************************************************/

int ppal (){
    CMatrice mat;
    CMyParam params;
    int nbCoups (0);
    int nbCoupsMax (10);
    int score (0);
    int niveau (0);
    initParams(params);
    chargerParametre(params, "../number_crush/config.yaml");
    initMat(mat,params);
    MenuNiveaux();
    cin >> niveau;
    if (niveau == 3){
        cout << "Choisissez le nombre de coups maximal : ";
        cin >> nbCoupsMax;
        cout << "Choisissez entre niveau simple (1) et niveau expert (2) : ";
        cin >> niveau;
    }
    while((nbCoupsMax - nbCoups)>0){
        while (detectionExplositionUneBombeVerticale(mat, score, nbCoups, niveau) || detectionExplositionUneBombeHorizontale(mat, score, nbCoups, niveau)) {
            detectionExplositionUneBombeHorizontale (mat, score, nbCoups, niveau);
            detectionExplositionUneBombeVerticale (mat, score, nbCoups, niveau);

        }
        afficheMatriceV2(mat);
        cout << "Vous avez " << (nbCoupsMax - nbCoups) << " coups a jouer" << endl;
        cout << "Votre score est de " << score << " points" << endl;
        faitUnMouvement (mat);
        nbCoups += 1;
    }
    cout << "C'est la fin de la partie !" << endl;
    cout << "Votre score final est de " << score << " points" << endl;
    return 0;
}

//***********************************************************************************/
//*************************************   Main   ************************************/
//***********************************************************************************/

int main() {
    afficheMenu();
}
