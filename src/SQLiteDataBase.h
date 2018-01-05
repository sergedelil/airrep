/*
 * SQLiteDataBase.h
 *
 *  Created on: Jan 4, 2018
 *      Author: sdo000
 */

#ifndef SRC_SQLITEDATABASE_H_
#define SRC_SQLITEDATABASE_H_

#include <exception>
#include "sqlite3.h"
#include "fichier.h"

class SQLiteDataBaseException : public std::exception {
	public:

		SQLiteDataBaseException(const std::string& msg) : msg(msg) {}
	    ~SQLiteDataBaseException() throw() {}
	    const char* what() const throw() { return msg.c_str(); }

	private:
	    std::string msg;
};

class SQLiteDataBase {

	private:

		sqlite3* db_;
		bool open_;
		std::string getErrorMessage();

	public:

		SQLiteDataBase();
		~SQLiteDataBase();

		sqlite3* getDb_();
		bool getOpen_();
		bool open(char mode, const std::string& filename = "");
		void copy(SQLiteDataBase&);
		void insert(std::vector<Bulletin>&);
		void loadSchema(std::string&);
		void close();

};




#endif /* SRC_SQLITEDATABASE_H_ */
