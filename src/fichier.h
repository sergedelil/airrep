/*
 * fichier.h
 *
 *  Created on: Dec 1, 2017
 *      Author: sdo000
 */

#ifndef FICHIER_H_
#define FICHIER_H_

#include <string>
#include <vector>
#include <iostream>
#include "rappObs.h"


class Bulletin {
	public:
	    const std::string& getHour() const;
		const std::string& getHeader() const;
		std::vector<RappObs> getRappsObs();
		void ajouterRappObs(RappObs&);
		void setHour(std::string&);
		void setHeader(std::string&);

	private:
		std::string hour;
		std::string header;
		std::vector<RappObs> rappsObs;

	friend std::istream& operator >> (std::istream&, Bulletin&);
};


class Fichier{
	public:
		int nbBulletin;
		std::vector<Bulletin> getBulletins();
		void ajouterBulletin(Bulletin&);
		void afficherFichier();
	private:
		std::vector<Bulletin> bulletins;

	friend std::istream& operator >> (std::istream&, Fichier&);
};

#endif /* FICHIER_H_ */
