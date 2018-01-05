/*
 * observation.cpp
 *
 *  Created on: Jan 4, 2018
 *      Author: sdo000
 */



#include <assert.h>
#include <string>
#include "observation.h"


int Observation::getElement() const{
	return element;
}

std::string Observation::getDescription() const{
	return description;
}

double Observation::getValeur() const{
	return valeur;
}

std::string Observation::getUnit() const{
	return unit;
}

void Observation::setElement(int elem){
	element = elem;
}

void Observation::setDescription(std::string& desc){
	description = desc;
}

void Observation::setValeur(double val){
	valeur = val;
}

void Observation::setUnit(std::string& un){
	unit = un;
}

std::istream& operator >> (std::istream& monFlux, Observation&){
	return monFlux;
}
