/*
 * lib.h
 *
 *  Created on: Dec 6, 2017
 *      Author: sdo000
 */

#ifndef LIB_H_
#define LIB_H_

#include <iostream>
#include <vector>
#include <string>
#include "fichier.h"
#include <boost/regex.hpp>
#include <ctime>
#include "SQLiteDataBase.h"


std::string trim(std::string);
std::vector<std::string>& stringToArrayString(std::string&, std::vector<std::string>&);
bool validerHeader(std::vector<std::string>);
std::vector<std::string>& formaterRapportsObs(std::vector<std::string>&, unsigned int);
bool validerPropriete(std::string&, boost::regex);
bool validerIdFly(std::string&);
bool validerLatLon(std::string&);
bool validerHourMinute(std::string&);
bool validerFlightLevel(std::string&);
bool validerTemperature(std::string&);
bool validerWind(std::string&);
bool validerTurbulence(std::string&);
std::vector<std::string>& filtrerRapportsObs(std::vector<std::string>&, int);
std::vector<std::string>& eliminerDoublonsRapportsObs(std::vector<std::string>&, int);
bool analyserBulletin(std::vector<std::string>&, int);
bool validerBlocBulletin(std::vector<std::string>&, std::vector<std::string>&);
double convertirLatitude(std::string&);
double convertirLongitude(std::string&);
double convertirPression(std::string&);
double convertirTemperature(std::string&);
double convertirWinDir(std::string&);
double convertirWindSpeed(std::string&);
std::string concertirTurbulence(std::string&);
RappObs& construireRappObs(std::string&, RappObs&);
Bulletin& creerRappObs(std::vector<std::string>& blocBull, Bulletin&, std::string&);
Bulletin& creerBulletin(std::vector<std::string>&, Bulletin&, std::string&);
Bulletin& construireBulletin(std::vector<std::string>&, Bulletin&);
//std::string& datetodayToString();
const std::string currentDateTime();

#endif /* LIB_H_ */
