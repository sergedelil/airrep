/*
 * SQLiteBd.h
 *
 *  Created on: Dec 21, 2017
 *      Author: sdo000
 */

#ifndef SRC_SQLITEBD_H_
#define SRC_SQLITEBD_H_



#include <string>
#include <exception>
#include <mutex>


#include <sqlite3.h>


namespace sqlite {


class SQLiteBdException : public std::exception
{
public:
	SQLiteBdException(const std::string& msg) : msg(msg) {}
    ~SQLiteBdException() throw() {}
    const char* what() const throw() { return msg.c_str(); }
private:
    std::string msg;
};


class SQLiteBd {
public:
	SQLiteBd();
    virtual ~SQLiteBd();

    /**
     *  @filename [in] filename path du fichier pointe par la BD
     *  @flags [in] flags. les flags communs sont : SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE or SQLITE_OPEN_READWRITE
     *  voir ici pour les flags :  https://sqlite.org/c3ref/open.html.
     */
    void open(const std::string& filename, const int flags);

    /** ferme la connexion sur le fichier BD. */
    void close();

    /** Debut transaction. */
    void beginTransaction();

    /** Fin transaction et commiter les modifications a' la BD. */
    void endTransaction();

    /** Annuler les modifs depuis un point de transaction. */
    void rollback();


    /** Convenience insert row into database function
     *
     * @param table [in] table to query
     * @param columns [in] columns to insert
     * @param values [in] values to insert for each column
     * @param selection [in] where column restrictions eg. "field1 = ? AND field2 = ?"
     * @param selectionArgs [in] where column binding arguments
     *
     * @return int [out] row ID if id column exists else 0, -1 on error
     */
    int insert(const std::string& table, const std::vector<std::string>& columns, const std::vector<std::string>& values,
                const std::string& selection, const std::vector<std::string>& selectionArgs);

    /** Convenience update row function
     *
     * @param table [in] table to query
     * @param columns [in] columns to update
     * @param values [in] values for columns to update
     * @param selection [in] where column restrictions eg. "field1 = ? AND field2 = ?"
     * @param selectionArgs [in] where column binding arguments
     *
     * @return int [out] number of records updated, -1 on error
     */
    int update(const std::string& table, const std::vector<std::string>& columns, const std::vector<std::string>& values,
                const std::string& selection, const std::vector<std::string>& selectionArgs);

    /** Convenience delete row function
     *
     * @param table [in] table to query
     * @param selection [in] where column restrictions eg. "field1 = ? AND field2 = ?"
     * @param selectionArgs [in] where column binding arguments
     *
     * @return int [out] number of records deleted, -1 on error
     */
    int remove(const std::string& table, const std::string& selection, const std::vector<std::string>& selectionArgs);

    /** Executes the sql and expects no results to be returned. */
    void execQuery(const std::string& sql);

    /** Executes the sql and expects no results to be returned.
     *
     * @return bool [out] true if the database connection is open
     * */
    bool isOpen();

protected:

private:
    sqlite3* db_;
    bool open_;

    std::string getStdString(const unsigned char* text);
    std::string getSQLite3ErrorMessage();

};

} /* namespace sqlite */

#endif /* SRC_SQLITEBD_H_ */
