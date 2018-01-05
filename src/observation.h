/*
 * observation.h
 *
 *  Created on: Jan 4, 2018
 *      Author: sdo000
 */

#ifndef SRC_OBSERVATION_H_
#define SRC_OBSERVATION_H_


#include <string>

class Observation{

public:

	int getElement() const;
	std::string getDescription() const;
	double getValeur() const;
	std::string getUnit() const;

	void setElement(int);
	void setDescription(std::string&);
	void setValeur(double);
	void setUnit(std::string&);

private:
	int element;
	std::string description;
	double valeur;
	std::string unit;

	friend std::istream& operator >> (std::istream&, Observation&);
	//friend std::ostream& operator << (std::ostream&, const RappObs&);

};



#endif /* SRC_OBSERVATION_H_ */
