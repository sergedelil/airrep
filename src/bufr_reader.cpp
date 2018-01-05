#include <iostream>
#include <sstream>
#include <fstream>

#include <sqlite3.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <libecbufr/bufr_api.h>
#include "bufr_reader.h"

#define TABLE_VERSION_REJECTION_LIMIT 12
#define TABLE_B_PATH "constants/table_b_iasi"
#define TABLE_D_PATH "constants/table_d_iasi"

using namespace std;

string OBSDB_PATH = "/home/can000/repos/eccc-ppp2/obsDB/";

string trim_header(char *header) {
    #define MAX_STR 256
    string header_trim(header);
    int pos = 0;

    while ( (pos = header_trim.find("\\040")) < header_trim.size() ) {
        header_trim.replace(pos, 4, " ");
    }
    while ( (pos = header_trim.find("\\012")) < header_trim.size() ) {
        header_trim.replace(pos, 4, " ");
    }

    return header_trim;
}


/* ===========  BufrReader functions ===========*/
BufrReader::BufrReader() {
   bufr_begin_api();
}

BufrReader::~BufrReader() {
   bufr_end_api();
}

int BufrReader::get_table_version_limit() {
   return TABLE_VERSION_REJECTION_LIMIT;
}

void BufrReader::read(list<msg_in>* mylist, FILE *bufr_file) {
   // ecbufr objects
   BUFR_Message *msg;
   BUFR_Dataset *dts;

   // tables B et D
   int tablenos[1];
   BUFR_Tables *tables = NULL;
   LinkedList  *tables_list=NULL;

   // Gestion des Tables B et D
   tables = bufr_create_tables();
   bufr_load_m_tableB(tables, (OBSDB_PATH + tableB).c_str());
   bufr_load_m_tableD(tables, (OBSDB_PATH + tableD).c_str());

   tablenos[0] = 13;
   tables_list = bufr_load_tables_list( getenv("BUFR_TABLES"), tablenos, sizeof(tablenos)/sizeof(int) );
   lst_addfirst( tables_list, lst_newnode( tables ) );

   list<vector<pair<string,double>>> dataset;

   int count = 0;
   while (bufr_read_message(bufr_file, &msg) > 0) {
      ++count;

      if (msg->s1.master_table_version < get_table_version_limit()){ //TABLE_VERSION_REJECTION_LIMIT) {
         cout << msg->s1.master_table_version << endl;
         bufr_free_message(msg);
         continue;
      }

      msg_in m;
      m.header = trim_header(msg->header_string);

      map<string,int> s1;
      s1["master_table"] = msg->s1.bufr_master_table;
      s1["orig_centre"] = msg->s1.orig_centre;
      s1["orig_sub_centre"] = msg->s1.orig_sub_centre;
      s1["upd_seq_no"] = msg->s1.upd_seq_no;
      s1["flag"] = msg->s1.flag;
      s1["msg_type"] = msg->s1.msg_type;
      s1["msg_inter_subtype"] = msg->s1.msg_inter_subtype;
      s1["msg_local_subtype"] = msg->s1.msg_local_subtype;
      s1["master_table_version"] = msg->s1.master_table_version;
      s1["local_table_version"] = msg->s1.local_table_version;
      s1["year"] = msg->s1.year;
      s1["month"] = msg->s1.month;
      s1["day"] = msg->s1.day;
      s1["hour"] = msg->s1.hour;
      s1["minute"] = msg->s1.minute;
      s1["second"] = msg->s1.second;
      m.section1 = s1;

      dts = bufr_decode_message(msg, tables);
      if (dts == NULL || BUFR_IS_INVALID(dts)) {
         if (BUFR_IS_INVALID(dts)) bufr_free_dataset(dts);
         bufr_free_message(msg);
         continue;
      }
      
      //list to store the decoded subsets
      list<vector<pair<int,string>>> dataset;

      int nb_subsets = bufr_count_datasubset(dts);
      for (int i=0; i<nb_subsets; i++) {
         vector<pair<int,string>> subset;

         // Create a list of BUFR descriptors
         DataSubset *dtss = bufr_get_datasubset(dts,i);

         int descriptorCount = bufr_datasubset_count_descriptor(dtss);
         list<BufrDescriptor *> dtssDescriptors;

         for ( int j=0 ; j< descriptorCount ; j++ ) {
            BufrDescriptor *desc;
            desc = bufr_datasubset_get_descriptor(dtss, j);

            int descriptor = desc->descriptor;

            string val_str;
            ostringstream oss;
            if (desc->value) {
               if( bufr_value_is_missing( desc->value ) ) {
                  oss << "NONE";
               } else if (desc->value->type == VALTYPE_INT32) {
                  int32_t value = bufr_descriptor_get_ivalue( desc );
                  oss << value;
               } else if (desc->value->type == VALTYPE_INT64) {
                  int64_t value = bufr_descriptor_get_ivalue( desc );
                  oss << value;
               } else if (desc->value->type == VALTYPE_FLT32) {
                  float value = bufr_descriptor_get_fvalue( desc );
                  oss << value;
               } else if (desc->value->type == VALTYPE_FLT64) {
                  double value = bufr_descriptor_get_dvalue( desc );
                  oss << value;
               } else if (desc->value->type == VALTYPE_STRING) {
                  int len;
                  char *value = bufr_descriptor_get_svalue( desc, &len );
                  oss << value;
               }
            }
            subset.emplace(subset.end(), descriptor, oss.str());
         }

         dataset.insert(dataset.end(),subset);

      }

      m.dataset = dataset;
      mylist->insert(mylist->end(),m);

      bufr_free_message(msg);
      bufr_free_dataset(dts);
   }
}

/*===========================================================================*/
Writer::Writer(){
}

Writer::~Writer(){

}

void Writer::write_to_sqlite(list<msg_out>* data, string f_out) {//, string f_schema) {
   const int codtyp = 1;
   int id_rpt, id_obs, id_info_rpt, id_info_obs; // counts... IDs to be used as primary key in DB

   /* SQLITE elements */
   sqlite3 *db; // DATABASE in memory
   sqlite3 *db_final; // final persistent database
   sqlite3_backup *pBackup;
   sqlite3_stmt *stmt;
   sqlite3_stmt *stmt_rpt; //statement for table 'rapport'
   sqlite3_stmt *stmt_meta; //statement for table 'meta_donnee' 
   sqlite3_stmt *stmt_obs; //statement for table 'observation'
   sqlite3_stmt *stmt_info; //statement for table 'info_obs'

   char *err = 0; // error message
   int rc; // result of the execution of a statement

   /* datetime elements */
   time_t now;
   struct tm *dtm;
   char date_exec[20];
   
   now = time(0);
   dtm = gmtime(&now);
   strftime(date_exec,20,"%Y-%m-%d %H:%M:%S",dtm); // format can also be "%F %R"

   ifstream schema_infile((OBSDB_PATH + schema_fname));

   /* Load schema into string buffer */
   schema_infile.seekg (0, schema_infile.end);
   int length = schema_infile.tellg();
   schema_infile.seekg (0, schema_infile.beg);

   char *schema_buf = new char[length];
   schema_infile.read(schema_buf,length);  

   /* Open DB file */
   rc = sqlite3_open(":memory:", &db);
//   rc = sqlite3_open(f_out.c_str(), &db);
   if (rc != SQLITE_OK) {
      cerr << "ERROR : cannot open database file" << endl;
      return;
   }

   /* Loading schema into db */
   rc = sqlite3_exec(db,schema_buf,NULL,NULL,&err);
   if (!rc) { // if rc is non-zero
      cerr << "ERROR : cannot upload schema" << endl;
      return;
   }
   delete[] schema_buf;

   char dt_valid[20];

   const char* query_rpt = "insert into rapport " \
                    "(id_rapport,id_stn,date_validite,date_creation,lat,lon,alt,codtyp,no_correction) " \
                    "values (?1,?2,?3,?4,?5,?6,?7,?8,0);";
   sqlite3_prepare_v2(db,query_rpt,-1,&stmt_rpt,NULL);
   const char* query_meta = "insert into meta_donnee (id_meta_donnee,id_rapport,description,valeur) " \
                    "values (?1,?2,?3,?4);";
   sqlite3_prepare_v2(db,query_meta,-1,&stmt_meta,NULL);
   const char* query_obs = "insert into observation (id_obs,id_rapport,id_elem,valeur,qc,unit,precision) " \
                    "values (?1,?2,?3,?4,0,?5,0);";
   sqlite3_prepare_v2(db,query_obs,-1,&stmt_obs,NULL);
   const char* query_obs_info = "insert into info_obs (id_info_obs,id_obs,description,valeur) " \
                       "values (?1,?2,?3,?4);";
   sqlite3_prepare_v2(db,query_obs_info,-1,&stmt_info,NULL);

   /* Loop over messages */
   id_rpt = id_obs = id_info_rpt = id_info_obs = 0;
   for (msg_out msg : (*data) ) {
      /* One transaction per message */
      sqlite3_exec(db,"BEGIN TRANSACTION;",NULL,NULL,NULL);
      for (report rpt : msg.reports) {
         sqlite3_bind_int(stmt_rpt,1,++id_rpt);
         sqlite3_bind_text(stmt_rpt,2,rpt.id_stn.c_str(),-1,SQLITE_STATIC);
         sqlite3_bind_text(stmt_rpt,3,dt_valid,-1,SQLITE_STATIC);
         sqlite3_bind_text(stmt_rpt,4,date_exec,-1,SQLITE_STATIC);
         sqlite3_bind_double(stmt_rpt,5,rpt.latitude);
         sqlite3_bind_double(stmt_rpt,6,rpt.longitude);
         sqlite3_bind_double(stmt_rpt,7,rpt.altitude);
         sqlite3_bind_int(stmt_rpt,8,codtyp);

         rc = sqlite3_step(stmt_rpt);
         if (rc != SQLITE_DONE) {
            cerr << "ERROR : cannot insert report" << endl;
            return;
         }
         sqlite3_reset(stmt_rpt);
//         sqlite3_clear_bindings(stmt_rpt);

         for (pair<int,string> meta : rpt.info) {
            string description = to_string(meta.first);

            sqlite3_bind_int(stmt_meta,1,++id_info_rpt);
            sqlite3_bind_int(stmt_meta,2,id_rpt);
            sqlite3_bind_text(stmt_meta,3,description.c_str(),-1,SQLITE_STATIC);
            sqlite3_bind_text(stmt_meta,4,meta.second.c_str(),-1,SQLITE_STATIC);

            rc = sqlite3_step(stmt_meta);
            if (rc != SQLITE_DONE) {
               cerr << "ERROR : cannot insert metadata" << endl;
               return;
            }
            sqlite3_reset(stmt_meta);
//            sqlite3_clear_bindings(stmt_meta);
         }

         for (observation o : rpt.obs) {
            sqlite3_bind_int(stmt_obs,1,++id_obs);
            sqlite3_bind_int(stmt_obs,2,id_rpt);
            sqlite3_bind_int(stmt_obs,3,o.id_elem);
            sqlite3_bind_double(stmt_obs,4,o.value);
            sqlite3_bind_text(stmt_obs,5,"",-1,SQLITE_STATIC);

            rc = sqlite3_step(stmt_obs);
            if (rc != SQLITE_DONE) {
               cerr << "ERROR : cannot insert obs" << endl;
               return;
            }
            sqlite3_reset(stmt_obs);
//            sqlite3_clear_bindings(stmt_obs);

            for (pair<string,string> o_info : o.info) {
               sqlite3_bind_int(stmt_info,1,++id_info_obs);
               sqlite3_bind_int(stmt_info,2,id_obs);
               sqlite3_bind_text(stmt_info,3,o_info.first.c_str(),-1,SQLITE_STATIC);
               sqlite3_bind_text(stmt_info,4,o_info.second.c_str(),-1,SQLITE_STATIC);
               rc = sqlite3_step(stmt_info);
               if (rc != SQLITE_DONE) {
                  cerr << "ERROR : cannot insert obs_info" << endl;
                  return;
               }
               sqlite3_reset(stmt_info);
//               sqlite3_clear_bindings(stmt_info);               
            }
         }

      }
      sqlite3_exec(db,"COMMIT;",NULL,NULL,NULL);
   }

   sqlite3_finalize(stmt_rpt);
   sqlite3_finalize(stmt_meta);
   sqlite3_finalize(stmt_obs);
   sqlite3_finalize(stmt_info);

   rc = sqlite3_open(f_out.c_str(), &db_final);
   if (rc != SQLITE_OK) {
      cerr << "ERROR : cannot open database file" << endl;
      return;
   }

   pBackup = sqlite3_backup_init(db_final, "main", db, "main");
   if( pBackup ){
      (void)sqlite3_backup_step(pBackup, -1);
      (void)sqlite3_backup_finish(pBackup);
   }
   sqlite3_close(db_final);

   sqlite3_close(db);
}
