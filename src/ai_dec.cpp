/*
 * main.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: sdo000
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include "fichier.h"


using namespace std;

int main(int argc, const char** argv){

	if (argc < 2){
		cout << "Aucun argument present" << endl;
		return 1;
	}
	ifstream monFlux(argv[1]);
	if (!monFlux){
		cout << "Erreur ouverture fichier d entree" << endl;
		return 2;
	}

	Fichier fichier;
	std::string fichierSqlite = argv[2];
	std::string schemea = argv[3];

	monFlux >> fichier;
	//fichier.afficherFichier();

	fichier.ecrireEnSqlite(fichierSqlite, schemea);

	return 0;
} // fin main
