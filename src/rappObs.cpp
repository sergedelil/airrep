/*
 * rappObs.cpp
 *
 *  Created on: Dec 12, 2017
 *      Author: sdo000
 */



#include <assert.h>
#include <string>
#include "rappObs.h"


std::string RappObs::getType() const{
	return type;
}

std::string RappObs::getIdFly() const{
	return idFly;
}

double RappObs::getLatitude() const{
	return latitude;
}

double RappObs::getLongitude() const{
	return longitude;
}

std::string RappObs::gethhmm() const{
	return hhmm;
}

std::vector<Observation> RappObs::getObservations(){
	return observations;
}

void RappObs::ajouterObs(Observation& obs){
	observations.push_back(obs);

}
/*double RappObs::getAltitude() const{
	return altitude;
}

double RappObs::getPressure() const{
	return pressure;
}

double RappObs::getTemperature() const{
	return temperature;
}

double RappObs::getWindDir() const{
	return windDir;
}

double RappObs::getWindSpeed() const{
	return windSpeed;
}

std::string RappObs::getTurbulence() const{
	return turbulence;
}*/

void RappObs::setType(std::string& _type){
	type = _type;
}
void RappObs::setIdFly(std::string& _idFly){
	idFly = _idFly;
}
void RappObs::setLatitude(double _latitude){
	latitude = _latitude;
}
void RappObs::setLongitude(double _longitude){
	longitude = _longitude;
}
void RappObs::sethhmm(std::string& _dateHour){
	hhmm = _dateHour;
}
/*void RappObs::setAltitude(double _altitude){
	altitude = _altitude;
}

void RappObs::setPressure(double _pressure){
	pressure = _pressure;
}
void RappObs::setTemperature(double _temperature){
	temperature = _temperature;
}
void RappObs::setWindDir(double _windDir){
	windDir = _windDir;
}
void RappObs::setWindSpeed(double _windSpeed){
	windSpeed = _windSpeed;
}
void RappObs::setTurbulence(std::string& _turbulence){
	turbulence = _turbulence;
}*/

std::istream& operator >> (std::istream& monFlux, RappObs&){
	return monFlux;
}
