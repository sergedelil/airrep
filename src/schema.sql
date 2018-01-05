CREATE TABLE IF NOT EXISTS Rapport (
  id_rapport     INTEGER      NOT NULL   PRIMARY KEY, 
  id_stn         varchar(50)  NOT NULL, 
  date_validite  timestamp    NOT NULL, 
  date_creation  timestamp    NOT NULL, 
  lat            double       NOT NULL, 
  lon            double       NOT NULL, 
  alt            double       NOT NULL, 
  codtyp         integer      NOT NULL, 
  no_correction  integer      NOT NULL
);

CREATE TABLE IF NOT EXISTS Observation (
  id_obs         INTEGER      PRIMARY KEY, 
  id_rapport     integer      NOT NULL, 
  id_elem        integer      NOT NULL, 
  valeur         double, 
  qc             integer, 
  unit           varchar(10), 
  precision      integer, 

  CONSTRAINT fk_obs FOREIGN KEY(id_rapport) REFERENCES Rapport(id_rapport)
);

CREATE TABLE IF NOT EXISTS Info_Obs (
  id_info_obs    INTEGER      PRIMARY KEY, 
  id_obs         integer      NOT NULL, 
  description    varchar(255) NOT NULL, 
  valeur         varchar(255) NOT NULL, 

  CONSTRAINT fk_info FOREIGN KEY(id_obs) REFERENCES Observation(id_obs)
);

CREATE TABLE IF NOT EXISTS Meta_Donnee (
  id_meta_donnee INTEGER      PRIMARY KEY, 
  id_rapport     integer      NOT NULL, 
  description    varchar(255) NOT NULL, 
  valeur         varchar(255) NOT NULL, 

  CONSTRAINT fk_meta FOREIGN KEY(id_rapport) REFERENCES Rapport(id_rapport)
);

