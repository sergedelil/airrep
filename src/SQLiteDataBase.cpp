/*
 * SQLiteDataBase.cpp
 *
 *  Created on: Jan 4, 2018
 *      Author: sdo000
 */


#include "SQLiteDataBase.h"
#include <fstream>
#include <streambuf>
//#include "fichier.h"
//#include "observation.h"
#include "lib.h"

SQLiteDataBase::SQLiteDataBase() : db_(nullptr), open_(false) { }

SQLiteDataBase::~SQLiteDataBase(){ }

sqlite3* SQLiteDataBase::getDb_(){ return db_; }

bool SQLiteDataBase::getOpen_(){ return open_; }



// flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
// 'V' = mode volatile  'P' = mode persistence
bool SQLiteDataBase::open(char mode, const std::string& filename) {

	open_ = false;
	if (mode == 'V'){

		auto rc = sqlite3_open(":memory:", &db_);
		if (rc) {
			std::string errorMsg = "Can't open database: " + std::string(sqlite3_errstr(rc));
			sqlite3_close(db_);
			throw SQLiteDataBaseException(errorMsg);
		}

	}else if (mode == 'P'){

		auto rc = sqlite3_open_v2(filename.c_str(), &db_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
		if (rc) {
			std::string errorMsg = "Can't open database: " + std::string(sqlite3_errstr(rc));
			sqlite3_close(db_);
			throw SQLiteDataBaseException(errorMsg);
		}
	}
    open_ = true;
    return open_;
}

void SQLiteDataBase::close() {

	if (open_){

		auto rc = sqlite3_close(db_);
		if (rc) {
			std::string errorMsg = "Can't close database: " + std::string(sqlite3_errstr(rc));
			throw SQLiteDataBaseException(errorMsg);
		}
		db_ = nullptr;
		open_ = false;
	}
}

void SQLiteDataBase::loadSchema(std::string& schema){

	char * sErrMsg = 0;
	std::ifstream in (schema);
	std::string tables((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());//file to string
	const char *table = tables.c_str();

	auto rc = sqlite3_exec(this->db_, table, NULL, NULL, &sErrMsg);
	if (rc) {
		std::string errorMsg = "Can't upload schema: " + std::string(sqlite3_errstr(rc));
		this->close();
		throw SQLiteDataBaseException(errorMsg);
	}
}

void SQLiteDataBase::insert(std::vector<Bulletin>& bulletins){

	sqlite3_stmt *rapStmt;
	sqlite3_stmt *obsStmt;
	//sqlite3_stmt *infoStmt;
	//sqlite3_stmt *metaStmt;

	char * sErrMsg = 0;

	char *rapSQL = (char *)"INSERT INTO RAPPORT VALUES (?1, ?2, ?3, ?4, ?5, ?6, 0, 128, 0)";
	char *obsSQL = (char *)"INSERT INTO OBSERVATION VALUES (?1, ?2, ?3, ?4, 0, ?5, 0)";
	//char *infoSQL = "INSERT INTO INFO_OBS VALUES (@IINF, @IOBS, @DESC, @VAL)";
	//char *metaSQL = "INSERT INTO META_DONNEE VALUES (@IMETA, @IRAP, @DES, @VAL)";
	sqlite3_prepare_v2(this->db_,  rapSQL, -1, &rapStmt, NULL);
	sqlite3_prepare_v2(this->db_,  obsSQL, -1, &obsStmt, NULL);
	//sqlite3_prepare_v2(this->db_,  infoSQL, -1, &infoStmt, NULL);
	//sqlite3_prepare_v2(this->db_,  metaSQL, -1, &metaStmt, NULL);


	//auto rc = sqlite3_open(":memory:", &db);

	int id_rapp, id_obs, id_info_obs, id_meta;
	id_rapp = id_obs = id_info_obs = id_meta = 0;

	const std::string dateDec = currentDateTime();

	sqlite3_exec(this->db_, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);

	for(Bulletin bulletin: bulletins){

		std::vector<RappObs> rappsObs = bulletin.getRappsObs();

		for(RappObs  rappObs : rappsObs){

			sqlite3_bind_int(rapStmt, 1, ++id_rapp);
			sqlite3_bind_text(rapStmt, 2, rappObs.getIdFly().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(rapStmt, 3, rappObs.gethhmm().c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_text(rapStmt, 4, dateDec.c_str(), -1, SQLITE_TRANSIENT);
			sqlite3_bind_double(rapStmt, 5, rappObs.getLatitude());
			sqlite3_bind_double(rapStmt, 6, rappObs.getLongitude());

			sqlite3_step(rapStmt);
			sqlite3_clear_bindings(rapStmt);
			sqlite3_reset(rapStmt);

			std::vector<Observation> observations = rappObs.getObservations();
			for (Observation obs : observations){
				sqlite3_bind_int(obsStmt, 1, ++id_obs);
				sqlite3_bind_int(obsStmt, 2, id_rapp);
				sqlite3_bind_int(obsStmt, 3, obs.getElement());
				sqlite3_bind_double(obsStmt, 4, obs.getValeur());
				sqlite3_bind_text(obsStmt, 5, obs.getUnit().c_str(), -1, SQLITE_TRANSIENT);
			}

			sqlite3_step(obsStmt);
			sqlite3_clear_bindings(obsStmt);
			sqlite3_reset(obsStmt);

		}

	}
	sqlite3_exec(this->db_, "COMMIT", NULL, NULL, &sErrMsg);
	//sqlite3_exec(this->db_, "END TRANSACTION", NULL, NULL, &sErrMsg);
	sqlite3_finalize(rapStmt);
	sqlite3_finalize(obsStmt);

}

void SQLiteDataBase::copy(SQLiteDataBase& db){

	auto rc = sqlite3_backup_init(db.getDb_(), "main", this->db_, "main");
	if( rc ){
	  (void)sqlite3_backup_step(rc, -1);
	  (void)sqlite3_backup_finish(rc);
	}
	db.close();
}
