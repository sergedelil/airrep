/*
 * rappObs.h
 *
 *  Created on: Dec 12, 2017
 *      Author: sdo000
 */

#ifndef SRC_RAPPOBS_H_
#define SRC_RAPPOBS_H_


#include <string>
#include <vector>
#include "observation.h"

class RappObs{
public:
	std::string getType() const;
	std::string getIdFly() const;
	double getLatitude() const;
	double getLongitude() const;
	std::string gethhmm() const;
	//double getAltitude() const;
	//double getPressure() const;
	//double getTemperature() const;
	//double getWindDir() const;
	//double getWindSpeed() const;
	//std::string getTurbulence() const;
	std::vector<Observation> getObservations();
	void ajouterObs(Observation&);
	void setType(std::string&);
	void setIdFly(std::string&);
	void setLatitude(double);
	void setLongitude(double);
	void sethhmm(std::string&);
	//void setPressure(double);
	//void setAltitude(double);
	//void setTemperature(double);
	//void setWindDir(double);
	//void setWindSpeed(double);
	//void setTurbulence(std::string&);


private:
	std::string type;
	std::string idFly;
	double latitude;
	double longitude;
	std::string hhmm;
	//double altitude;
	//double pressure;
	//double temperature;
	//double windDir;
	//double windSpeed;
	//std::string turbulence;
	std::vector<Observation> observations;

friend std::istream& operator >> (std::istream&, RappObs&);
//friend std::ostream& operator << (std::ostream&, const RappObs&);

};


#endif /* SRC_RAPPOBS_H_ */
