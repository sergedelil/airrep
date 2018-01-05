/*
 * lib.cpp
 *
 *  Created on: Dec 6, 2017
 *      Author: sdo000
 */

#include <cctype>
//#include <iostream>
//#include <vector>
#include <fstream>
//#include <string>
#include <cmath>
#include "lib.h"
#include <stdlib.h>     /* exit, EXIT_FAILURE */
#include <algorithm>
#include <boost/algorithm/string.hpp>



const boost::regex header("([A-Z0-9]+\\s[A-Z]+\\s(0[1-9]|1[0-9]|2[0-9]|3[0-1])(0[0-9]|1[0-9]|2[0-3])[0-5][0-9].*)");
const boost::regex typId("^([A-Z0-9]+)\\s");
const boost::regex idFly("\\s([A-Z0-9]+)\\s");
const boost::regex latLon("(\\s([0-9]{2,4}[NS]{1}[0-9]{2,5}[EW]{1})\\s|\\s([0-9]{2,4}[NS]{1}\\s[0-9]{2,5}[EW]{1})\\s)");
const boost::regex hhmm("\\s((0[0-9]|1[0-9]|2[0-3])[0-5][0-9])\\s");
const boost::regex flightLevel("\\s(F[0-9]{3})\\s");
const boost::regex temperature("\\s((MS|PS)[0-9]{2})\\s");
const boost::regex wind("\\s([0-9]{3}/?[0-9]{2,3}(KT)?)[=\\s]{1}");
const boost::regex turbulence("\\s((TURB|TB)\\s[A-Z]+)[=\\s]{1}");

std::string trim(std::string str){

	size_t debut = str.find_first_not_of(' ');
	size_t fin = str.find_last_not_of(' ');
	size_t taille = str.size();
	if (taille != (debut + fin + 1)){
		str = str.substr(debut, (fin - debut + 1));
	}
	return str;
}

std::vector<std::string>& stringToArrayString(std::string& str, std::vector<std::string>& tab){

	boost::split(tab,str,boost::is_any_of(" "));
	return tab;
}

std::vector<std::string>& formaterRapportsObs(std::vector<std::string>& blocBull, unsigned int ind){

	while(ind < blocBull.size()){
		// verifie dernier carac de chaq ligne
		char fin = blocBull[ind].at(blocBull[ind].length() - 1);

		if ((fin != '=') and  ( ind < blocBull.size() - 1)){// si le carc de fin est != de "=" et non fin du blocBull
			blocBull[ind] = blocBull[ind] + " " + blocBull[ind + 1];
			blocBull.erase(blocBull.begin() + ind + 1);
			ind--;
		}else if ((fin != '=') and  ( ind == blocBull.size() - 1)){//si le carc de fin est != de "=" et fin du blocBull
			blocBull[ind] = blocBull[ind] + "=";
			//blocBull.erase(blocBull.begin() + ind);
			//ind--;
		}else if ((fin == '=') and  ( blocBull[ind].length() == 1)){ // si le signe egale est seul sur la ligne
			blocBull[ind - 1] = blocBull[ind - 1] + "=";
			blocBull.erase(blocBull.begin() + ind);
		}
		ind++;
	}

	return blocBull;
}

bool validerPropriete(std::string& ligne, boost::regex rgx){
	boost::smatch match;
	return (boost::regex_search (ligne,match,rgx));
}

std::vector<std::string>& filtrerRapportsObs(std::vector<std::string>& blocBull, int position){

	for (unsigned int i = position; i < blocBull.size(); i++){
		std::string ligne = blocBull[i];
		if (!(validerPropriete(ligne, idFly) and validerPropriete(ligne, latLon) and validerPropriete(ligne, hhmm) and
				(validerPropriete(ligne, flightLevel) or validerPropriete(ligne, temperature) or validerPropriete(ligne, wind)))){

			blocBull.erase(blocBull.begin() + i);
		}
	}
	return blocBull;
}

std::vector<std::string>& eliminerDoublonsRapportsObs(std::vector<std::string>& blocBull, int position){


	std::vector<std::string> listeLatlon;
	for (unsigned int i = position; i < blocBull.size(); i++){

		std::string ligne = blocBull[i];
		boost::smatch match;
		if (boost::regex_search(ligne, match, latLon)){
			std::string result(match[1]);
			result.erase(std::remove(result.begin(), result.end(), ' '), result.end());
			if (std::find(listeLatlon.begin(), listeLatlon.end(), result) != listeLatlon.end()){
				blocBull.erase(blocBull.begin() + i);
				i--;
			}else{
				listeLatlon.push_back(result);
			}
		}
	}
	return blocBull;
}

bool analyserBulletin(std::vector<std::string>& blocBull, int indice){

	blocBull = formaterRapportsObs(blocBull, indice);
	blocBull = filtrerRapportsObs(blocBull, indice);
	blocBull = eliminerDoublonsRapportsObs(blocBull, indice);
	return blocBull.size() >= 3;
}

bool validerBlocBulletin(std::vector<std::string>& blocBull, std::vector<std::string>& listeType){

	bool resultat = false;
	if (blocBull.size() < 3 || blocBull[2] == "NIL") return false;

	std::vector<std::string> tab;
	tab = stringToArrayString(blocBull[2], tab);
	std::string leType = tab[0];

	if (std::find(listeType.begin(), listeType.end(), leType) == listeType.end()) return false;
	if (!validerPropriete(blocBull[1], header)) return false;

	if (leType == "AIREP"){
		resultat = analyserBulletin(blocBull, 3);
	}else if (leType == "ARP" or leType == "ARS"){
		resultat = analyserBulletin(blocBull, 2);
	}
	return resultat;
}

int extraireChiffre2Chaine(std::string chaine){
	std::string str = "";
	for(unsigned int i = 0; i < chaine.length(); i++){
		if (isdigit(chaine.at(i))){
			str += chaine.at(i);
		}
	}
	return atoi(str.c_str());
}

double convertirLatitude(std::string& champ){

	boost::smatch match;
	boost::regex rgx ("([0-9]+[NS]{1}).*");
	if (boost::regex_search (champ,match,rgx)){
		champ = match[1];
	}
	int coord = extraireChiffre2Chaine(champ);
	int factor = -1;
	if(champ.at(champ.length() - 1) == 'N'){
		factor = 1;
	}
	double lat = ( int(coord/100)*100 + ((coord%100)*100/60) ) * factor;
	return lat + 9000;
}

double convertirLongitude(std::string& champ){

	boost::smatch match;
	boost::regex rgx (".([0-9]+[EW]{1}).*");
	if (boost::regex_search (champ,match,rgx)){
		champ = match[1];
	}
	int coord = extraireChiffre2Chaine(champ);
	int factor = -1;
	if(champ.at(champ.length() - 1) == 'E'){
		factor = 1;
	}
	double lon = ( int(coord/100)*100 + ((coord%100)*100/60) ) * factor;
	if (lon < 0){
		lon = lon + 36000;
	}
	return lon;
}

double convertirPression(double alt){
	double pression = 10133 * pow((1 - alt * 0.0000068756), 5.2559);
	return pression;
}

double convertirAltitude(std::string& champ){
	double alt = atoi(champ.substr(1).c_str()) * 30.48 + 0.5;
	return alt;
}

double convertirTemperature(std::string& champ){

	int temperature = atoi(champ.substr(2).c_str());
	int factor = -1;
	if(champ.substr(0,2) == "PS"){
		factor = 1;
	}
	return temperature * factor ;
}

double convertirWinDir(std::string& champ){

	boost::smatch match;
	boost::regex rgx ("(^[0-9]{3}).*");
	if (boost::regex_search (champ,match,rgx)){
		champ = match[1];
	}
	int windDir = atoi(champ.c_str()) * 1.0;
	return windDir;
}

double convertirWindSpeed(std::string& champ){

	champ = champ.substr(2).c_str();
	int windS = extraireChiffre2Chaine(champ);
	double windSpeed = 0.5144 * (windS * 1.0) * 10 + 0.5;
	return windSpeed;
}

std::string concertirTurbulence(std::string& champ){

	std::string code = " ";
	return code;
}

std::string extraireChamp(std::string ligne, boost::regex rgx){

	std::string resultat = "";
	boost::smatch match;
	if (boost::regex_search (ligne, match,rgx)){
		resultat =  match[1];
	}
	return resultat;
}

Observation& initStaticObservation(Observation& obs, int codeElem, std::string desc, double val, std::string _unit ){
	obs.setElement(codeElem);
	obs.setDescription(desc);
	obs.setValeur(val);
	obs.setUnit(_unit);
	return obs;
}

RappObs& construireRappObs(std::string& ligne, RappObs& leRappObs){

	std::string _typId = extraireChamp(ligne, typId);
	leRappObs.setType(_typId);

	std::string _idFly = extraireChamp(ligne, idFly);
	leRappObs.setIdFly(_idFly);

	std::string _latLon = extraireChamp(ligne, latLon);
	double latitude = convertirLatitude(_latLon);
	leRappObs.setLatitude(latitude);

	double longitude = convertirLongitude(_latLon);
	leRappObs.setLongitude(longitude);

	std::string _hhmm = extraireChamp(ligne, hhmm);
	leRappObs.sethhmm(_hhmm);

	std::string _flyLvl = extraireChamp(ligne, flightLevel);
	if (_flyLvl != ""){
		double alt = convertirAltitude(_flyLvl);
		double pression = convertirPression(alt);
		Observation pres;
		pres = initStaticObservation(pres, 7001, "PRESSION", pression, "PA");
		leRappObs.ajouterObs(pres);
		Observation alti;
		alti = initStaticObservation(alti, 7002, "ALTITUDE", alt, "M");
		leRappObs.ajouterObs(alti);
	}

	std::string _temp = extraireChamp(ligne, temperature);
	if ( _temp != ""){
		double temp = convertirTemperature(_temp);
		Observation tempe;
		tempe = initStaticObservation(tempe, 12001, "TEMPERATURE", temp, "K");
		leRappObs.ajouterObs(tempe);
	}

	std::string vent = extraireChamp(ligne, wind);
	if ( vent != ""){
		double windD = convertirWinDir(vent);
		Observation ventD;
		ventD = initStaticObservation(ventD,11001, "DIRECTION DU VENT", windD, "DEG");
		leRappObs.ajouterObs(ventD);

		double winds = convertirWindSpeed(vent);
		Observation ventS;
		ventS = initStaticObservation(ventS, 11002, "VITESSE DU VENT", winds, "M/S");
		leRappObs.ajouterObs(ventS);
	}

	std::string _turb = extraireChamp(ligne, turbulence);
	if (_turb != ""){
		Observation tub;
		tub = initStaticObservation(tub, 11031, _turb, 0.0, "");
		leRappObs.ajouterObs(tub);
	}
	return leRappObs;
}

Bulletin& creerRappObs(std::vector<std::string>& blocBull, Bulletin& bull, std::string& leType){

	RappObs obsRapp;
	if (leType == "AIREP"){

		for(unsigned int i = 3; i < blocBull.size(); i++){
			std::string ligne = leType + " " + blocBull[i];
			obsRapp = construireRappObs(ligne, obsRapp);
			bull.ajouterRappObs(obsRapp);
		}
	}else{
		for(unsigned int i = 2; i < blocBull.size(); i++){
			obsRapp = construireRappObs(blocBull[i], obsRapp);
			bull.ajouterRappObs(obsRapp);
		}
	}
	return bull;
}

Bulletin& creerBulletin(std::vector<std::string>& blocBull, Bulletin& bull, std::string& leType){

	bull.setHour(blocBull[0]);
	bull.setHeader(blocBull[1]);
	bull = creerRappObs(blocBull, bull, leType);

	return bull;
}

Bulletin& construireBulletin(std::vector<std::string>& blocBull, Bulletin& bull){

	std::vector<std::string> listeType;
	listeType.push_back("AIREP");
	listeType.push_back("ARP");
	listeType.push_back("ARS");

	if (! validerBlocBulletin(blocBull, listeType)) return bull;
	std::vector<std::string> tab;
	tab = stringToArrayString(blocBull[2], tab);
	std::string leType = tab[0];
	bull = creerBulletin(blocBull, bull, leType);

	return bull;
}

// Get current date/time in string, format is YYYY-MM-DD HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}
