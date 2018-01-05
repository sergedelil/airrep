/*
 * fichier.cpp
 *
 *  Created on: Dec 1, 2017
 *      Author: sdo000
 */


#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include "fichier.h"
#include "lib.h"
#include "SQLiteDataBase.h"


/*
 * Definition de la class Bulletin
 * */

const std::string& Bulletin::getHour() const{
	return hour;
}

const std::string& Bulletin::getHeader() const{
	return header;
}

std::vector<RappObs> Bulletin::getRappsObs(){
	return rappsObs;
}

void Bulletin::ajouterRappObs(RappObs& rappObs){
	rappsObs.push_back(rappObs);
}

void Bulletin::setHeader(std::string& _header){
	header = _header;
}

void Bulletin::setHour(std::string& _hour){
	hour = _hour;
}

std::istream& operator >> (std::istream& monFlux, Bulletin& bulletin){

	std::string ligne;
	std::vector<std::string> blocBull;
	while ( getline(monFlux, ligne) ){
		ligne = trim(ligne);
		if (ligne.size() != 0){
			if (!iscntrl(ligne[0])){
				blocBull.push_back(ligne);
			}else{
				bulletin = construireBulletin( blocBull, bulletin);
				break;
			}
		}
	}
	return monFlux;
}

/*
 * Definition de la class Fichier
 * */

std::vector<Bulletin> Fichier::getBulletins(){
	return bulletins;
}

void Fichier::ajouterBulletin(Bulletin& bulletin){
	if (bulletin.getRappsObs().size() != 0)
		bulletins.push_back(bulletin);
}

void Fichier::ecrireEnSqlite(std::string& filname, std::string& schema){

	std::vector<Bulletin> bulletins = this-> getBulletins();
	SQLiteDataBase db_;
	db_.open('V');
	db_.loadSchema(schema);
	db_.insert(bulletins);

	SQLiteDataBase db;
	db.open('P', filname);
	db.loadSchema(schema);

	db_.copy(db);
	db_.close();


}

void Fichier::afficherFichier(){

	for (unsigned int i = 0; i < this -> getBulletins().size(); i++){

		std::cout << "=================================================================" << std::endl;
		std::cout << "header = "<< this -> getBulletins()[i].getHeader() << std::endl;
		std::cout << "heure = "<< this -> getBulletins()[i].getHour() << std::endl;

		for (unsigned int j = 0; j < this -> getBulletins()[i].getRappsObs().size(); j++){
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << "type = "<< this -> getBulletins()[i].getRappsObs()[j].getType() << std::endl;
			std::cout << "idFly = "<< this -> getBulletins()[i].getRappsObs()[j].getIdFly() << std::endl;
			std::cout << "latitude = "<< this -> getBulletins()[i].getRappsObs()[j].getLatitude() << std::endl;
			std::cout << "longitude = "<< this -> getBulletins()[i].getRappsObs()[j].getLongitude() << std::endl;
			std::cout << "hhmm = "<< this -> getBulletins()[i].getRappsObs()[j].gethhmm() << std::endl;
			for (unsigned int k = 0; k < this -> getBulletins()[i].getRappsObs()[j].getObservations().size(); j++){
				std::cout << "***************************" << std::endl;
				std::cout <<  this -> getBulletins()[i].getRappsObs()[j].getObservations()[k].getElement()
						  <<  this -> getBulletins()[i].getRappsObs()[j].getObservations()[k].getDescription()
						  << this -> getBulletins()[i].getRappsObs()[j].getObservations()[k].getValeur()
						  << this -> getBulletins()[i].getRappsObs()[j].getObservations()[k].getUnit()<< std::endl;
			}
		}
	}
}


std::istream& operator >> (std::istream& monFlux, Fichier& fichier){

	while (monFlux){
		Bulletin bulletin;
		monFlux >> bulletin;
		fichier.ajouterBulletin(bulletin);
	}
	return monFlux;

}
