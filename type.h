#ifndef TYPE_H
#define TYPE_H

#include <vector>
#include <string>

typedef unsigned short contenueDUneCase;
typedef std::vector <contenueDUneCase> CVLigne; // un type représentant une ligne de la grille
typedef std::vector <CVLigne> CMatrice; // un type représentant la grille

#include <map>
struct CMyParam {
    std::map <std::string, char> mapParamChar;
    std::map <std::string, unsigned> mapParamUnsigned;
};

#endif // TYPE_H
