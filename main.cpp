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

// initialisation des paramètres par défaut
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

// récupération des paramètres à partir du fichier yaml
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

// initialisation de la grille de jeu avec la configuration choisie dans le fichier yaml
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

// fonction permettant de remplir la matrice après une explosion
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
                explositionUneBombeHorizontale (mat, numLigne, numCol, combienALaSuite);
                remplirMatrice(mat);
                // mise à jour du score
                if (nbCoups>0){
                    score += 1;
                }
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
    for (size_t i (numLigne); i < numLigne + combien; ++i){
        mat [i][numColonne] = KAIgnorer;
    }
    for (size_t i (numLigne); i>0; --i){
        swap(mat [i-1][numColonne], mat [i-1+combien][numColonne]);
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
            // si on a au moins 3 chiffres identiques a la suite
            if (combienALaSuite >= 3){
                auMoinsUneExplosion = true;
                explositionUneBombeVerticale(mat, numLigne, numCol, combienALaSuite);
                remplirMatrice(mat);
                // mise à jour du score
                if (nbCoups>0){
                    score += 1;
                }
            }
        }
    }
    return auMoinsUneExplosion;
}


//***********************************************************************************/
//********************************   Mouvements   ***********************************/
//***********************************************************************************/

// fonction pour les mouvements
// demande en entrées les informations du mouvement puis réalise le mouvement correspondant
void faitUnMouvement (CMatrice & mat, CMyParam & params, int & nbCoups) {
    // Entrées des informations de déplacement : coordonnées de la case et mouvement
    cout << "Entrez les coordonnees du numero a deplacer" << endl;
    cout << "Numero de ligne : ";
    unsigned numLigne;
    cin >> numLigne;
    cout << "Numero de colonne : ";
    unsigned numCol;
    cin >> numCol;
    cout << "Appuyez sur une touche de deplacement pour vous deplacer : ";
    char deplacement;
    cin >> deplacement;
    // mouvement effectué en fonction de la touche choisie
    // correspondance entre la valeur entrée dans la variable déplacement et les valeurs renseignées dans les paramètres
    if (params.mapParamChar["toucheHaut"] == tolower(deplacement)){
        // si mouvement vers l'extérieur de la matrice alors affichage erreur
        if (numLigne==0){
            couleur(KRouge);
            cout << "-------------------------------------------------------" << endl;
            cout << "Erreur, veuillez faire un deplacement dans la matrice !" << endl;
            cout << "-------------------------------------------------------" << endl;
            couleur(KReset);
        }
        // sinon application du mouvement avec un swap des 2 cases concernées
        else {
            swap(mat[numLigne][numCol],mat[numLigne-1][numCol]);
            nbCoups += 1;
        }
    }
    if (params.mapParamChar["toucheGauche"] == tolower(deplacement)){
        // si mouvement vers l'extérieur de la matrice alors affichage erreur
        if (numCol==0){
            couleur(KRouge);
            cout << "-------------------------------------------------------" << endl;
            cout << "Erreur, veuillez faire un deplacement dans la matrice !" << endl;
            cout << "-------------------------------------------------------" << endl;
            couleur(KReset);
        }
        // sinon application du mouvement avec un swap des 2 cases concernées
        else{
            swap(mat[numLigne][numCol],mat[numLigne][numCol-1]);
            nbCoups += 1;
        }
    }
    if (params.mapParamChar["toucheDroite"] == tolower(deplacement)){
        // si mouvement vers l'extérieur de la matrice alors affichage erreur
        if (numCol==mat[numLigne].size()-1){
            couleur(KRouge);
            cout << "-------------------------------------------------------" << endl;
            cout << "Erreur, veuillez faire un deplacement dans la matrice !" << endl;
            cout << "-------------------------------------------------------" << endl;
            couleur(KReset);
        }
        // sinon application du mouvement avec un swap des 2 cases concernées
        else {
            swap(mat[numLigne][numCol],mat[numLigne][numCol+1]);
            nbCoups += 1;
        }
    }
    if (params.mapParamChar["toucheBas"] == tolower(deplacement)){
        // si mouvement vers l'extérieur de la matrice alors affichage erreur
        if (numLigne==mat.size()-1){
            couleur(KRouge);
            cout << "-------------------------------------------------------" << endl;
            cout << "Erreur, veuillez faire un deplacement dans la matrice !" << endl;
            cout << "-------------------------------------------------------" << endl;
            couleur(KReset);
        }
        // sinon application du mouvement avec un swap des 2 cases concernées
        else {
            swap(mat[numLigne][numCol],mat[numLigne+1][numCol]);
            nbCoups += 1;
        }
    }
    // si aucune correspondance entre la valeur entrée et celles des paramètres, affichage erreur
    else {
        couleur(KRouge);
        cout << "-----------------------------------------------------------------" << endl;
        cout << "Erreur, veuillez utiliser les touches de deplacements parametrees" << endl;
        cout << "-----------------------------------------------------------------" << endl;
        couleur(KReset);
    }
}


//***********************************************************************************/
//************************   Choix configuration et niveaux   **************************/
//***********************************************************************************/

// signature de la fonction ppal() pour l'utiliser plus bas
int ppal();

// fonction pour faire la configuration de la matrice désirée appelée par menuConfig si sélection du choix 5
CMatrice creeConfig(){
    CMatrice mat;
    size_t nbLignes;
    size_t nbColonnes;
    // demande de la taille de la matrice voulue en entrées
    cout << "Quelle taille de matrice voulez-vous ?" << endl;
    cout << "Nombre de lignes : ";
    cin >> nbLignes;
    cout << "Nombre de colonnes : ";
    cin >> nbColonnes;
    // redimension de la matrice avec les nouveaux paramètres
    mat.resize(nbLignes, vector<contenueDUneCase>(nbColonnes));
    // remplissage par défaut de la matrice avec des 0
    for (size_t numLigne (0); numLigne < mat.size(); ++numLigne)
        for (size_t numCol (0); numCol < mat[numLigne].size(); ++numCol){
            mat [numLigne][numCol] = 0;}
    // remplissage de la matrice case par case avec les valeurs demandées en entrées
    for (size_t numLigne (0); numLigne < mat.size(); ++numLigne){
        for (size_t numCol (0); numCol < mat[numLigne].size(); ++numCol){
            unsigned remplir;
            afficheMatriceV2(mat);
            cout << "Remplissez la case a la ligne " << numLigne << " et a la colonne " << numCol << endl;
            cin >> remplir;
            mat [numLigne][numCol] = remplir;
        }
    }
    afficheMatriceV2(mat);
    return mat;
}

// fonction affichant un menu permettant de choisir la configuration de la matrice voulue
int menuConfig(){
    int config = 1;
    // affichage menu
    cout << "------------------------" << endl;
    cout << "|     Candy Crush     |" << endl;
    cout << "------------------------" << endl;
    cout << "| 1. Config 1         |" << endl;
    cout << "| 2. Config 2         |" << endl;
    cout << "| 3. Config aleatoire |" << endl;
    cout << "| 4. Personnalise     |" << endl;
    cout << "| 5. Creation A a Z   |" << endl;
    cout << "------------------------" << endl;
    // demande entrée pour choisir la configuration
    cin >> config;
    // application de la configuration en fonction de l'entrée
    switch (config) {
        // configuration pseudo aléatoire avec la graine 1
        case 1:
            clearScreen();
            srand(1);
            break;
        // configuration pseudo aléatoire avec la graine 2
        case 2:
            clearScreen();
            srand(2);
            break;
        // configuration aléatoire
        case 3:
            clearScreen();
            srand(time(NULL));
            break;
        // configuration avec choix de la graine : demande entrée puis application
        case 4:
            clearScreen();
            cout << "------------------------" << endl;
            cout << "|     Candy Crush     |" << endl;
            cout << "------------------------" << endl;
            cout << " Rentrer la graine : ";
            unsigned seed;
            cin >> seed;
            srand(seed);
            break;
        // configuation personnalisée appliquée lors de l'appel de la fonction creeConfig
        case 5:
            clearScreen();
            break;
        // affichage message d'erreur si l'entrée ne correspond à aucune des valeurs proposées
        default:
            couleur(KRouge);
            cout << "------------------------------------------------------------------------" << endl;
            cout << "Erreur, veuillez rentrer le chiffre d'une des configurations proposées !" << endl;
            cout << "------------------------------------------------------------------------" << endl;
            couleur(KReset);
    }
    return config;
}

// affichage menu pour choix niveau
void MenuNiveaux(){
    clearScreen();
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
    // déclarations et initialisations des variables
    CMatrice mat;
    int nbCoups (0);
    int nbCoupsMax (10);
    int score (0);
    int niveau (0);
    int config (0);
    // initialisation des paramètres par défaut et récupération de ceux renseignés dans le fichier config.yaml
    CMyParam params;
    initParams(params);
    chargerParametre(params, "../number_crush/config.yaml");
    // choix configuration matrice
    // affichage menu et récupération du choix
    config = menuConfig();
    // cas particulier du choix 5
    if (config==5){
        mat = creeConfig();
    }
    // sinon initialisation de la matrice avec les paramètres récupérés précédemment
    else{
        initMat(mat,params);
    }
    // choix niveau
    // affichage menu
    MenuNiveaux();
    // demande entrée
    cin >> niveau;
    // cas particulier du niveau 3, demande et récupération des valeurs (nbCoupsMax et niveau)
    if (niveau == 3){
        cout << "Choisissez le nombre de coups maximal : ";
        cin >> nbCoupsMax;
        cout << "Choisissez entre niveau simple (1) et niveau expert (2) : ";
        cin >> niveau;
    }
    // jeu
    // tant qu'il reste des coups à jouer
    while((nbCoupsMax - nbCoups)>0){
        // explosions avant le début de la partie et entre chaque coup en fonction du niveau choisi
        while (detectionExplositionUneBombeVerticale(mat, score, nbCoups, niveau) || detectionExplositionUneBombeHorizontale(mat, score, nbCoups, niveau)) {
            detectionExplositionUneBombeHorizontale (mat, score, nbCoups, niveau);
            detectionExplositionUneBombeVerticale (mat, score, nbCoups, niveau);

        }
        // affichage de la matrice et des informations pratiques
        afficheMatriceV2(mat);
        cout << "Vous avez " << (nbCoupsMax - nbCoups) << " coup(s) a jouer" << endl;
        cout << "Votre score est de " << score << " point(s)" << endl;
        // appel fonction pour réaliser un mouvement
        faitUnMouvement(mat, params, nbCoups);
    }
    // fin jeu = nbCoupsMax atteint
    cout << "C'est la fin de la partie !" << endl;
    // affichage score final
    cout << "Votre score final est de " << score << " point(s)" << endl;
    return 0;
}

//***********************************************************************************/
//*************************************   Main   ************************************/
//***********************************************************************************/

int main() {
    ppal();
}
