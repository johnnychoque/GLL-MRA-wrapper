/* -*- Mode: C++; c-basic-indent: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: ConfigurationFile.h
 *
 * Description: This file contains the definition of the
 *              configuration file class, which is used to pass
 *              the different configuration parameters to the
 *              GLL entities
 *
 * Authors: Ramon Aguero (ramon@tlmat.unican.es)
 *          Johnny Choque (jchoque@tlmat.unican.es)
 *
 * Organisation: University of Cantabria
 *
 * This software 
 *
 */

#ifndef ConfigurationFile_h_
#define ConfigurationFile_h_

#include <vector>
#include <string>
#include <map>
using namespace std;

#define MAXLINE 200

#define CONF_COMMENT '#'
#define CONF_SECTION_BEGIN '['
#define CONF_SECTION_END ']'
#define CONF_KEY_ASSIGN '='

enum {
  SECTION,
  KEY,
  LIST,
};

typedef vector<string> listEntry_;
typedef map<string,listEntry_> listEntries;
typedef map<string,string> keyEntries;
typedef struct {
  keyEntries keyEntry;
  listEntries listEntry;
} entries;
typedef map<string,entries *> section;

class ConfigFile;

class ConfigFile {
 public:
  ConfigFile();
  
  int LoadConfig(string fileName);
  int getListValues(char *section, char *key, vector<string> &value);
  int getKeyValue(char *section, char *key, string &value);
  
 private:
  int suppressSpaces(char *line);
  int readLine(char *line, string &section, string &key, vector<string> &values);
  section sectionEntries;
  
};

#endif  // ConfigurationFile_h_
