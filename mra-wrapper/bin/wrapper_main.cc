/* -*- Mode: C++; c-basic-indent: 4 -*- */
/**
 *
 * Project: Ambient Networks Phase 2
 * Subproject: Workpackage C - Multi-Radio Access (GLL)
 *  
 * File: wrapper_main.cc
 *
 * Description: This file contains the mail GLL daemon that is
 *              loaded, when the GLL is initiated. It reads the
 *              different configuration parameters from the
 *              configuration file and starts the GLL.
 *
 * Authors: Ramon Aguero (ramon@tlmat.unican.es)
 *          Johnny Choque (jchoque@tlmat.unican.es)
 *
 * Organisation: University of Cantabria
 *
 * This software 
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <csignal>

#include "ConfigurationFile.h"
#include "wrapper_daemon.h"


#define MAX_PATH 200
#define DBG_LEVEL 2
#include "debug.h"

static wrapperDaemon *wrapper_Daemon_ = NULL;

void signalHandler(int sig);



int main(int argc, char *argv[])
{
    vector<string> confParameters;
    vector<double> params_d;
    ConfigFile configuration;
    string value, confFile_;
    char path_[MAX_PATH];
    unsigned int pos_;

    DBG_IN("main");

    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    if(argc != 2) {
	DBG_ERR("Configuration file not specified... Please fix!!!");
	DBG_ERR("Usage: GLL_main configuration_file");
	DBG_OUT("main");
	exit(-1);
    }
    
    value = argv[1];
    if((pos_ = value.find("configs/")) != string::npos) {
	value = value.substr(pos_ + strlen("configs/"),value.length());
    }

    getcwd(path_,MAX_PATH);
    confFile_ = path_;
    if((pos_ == confFile_.find("bin")) != string::npos) {
	confFile_ += "/configs/";
    } else {
	confFile_.replace(pos_, 3, "configs/");
    }
    confFile_ += value;
    
    DBG_INFO2("Opening configuration File %s",confFile_.c_str());

    if(configuration.LoadConfig(confFile_) == -1) {
	DBG_ERR("Could not open Configuration File... Please fix!!!");
	DBG_OUT("main");
	exit(-1);
    }
    
    wrapper_Daemon_ = new wrapperDaemon;
    wrapper_Daemon_->start(configuration);
    wrapper_Daemon_->waitForEvent();

    DBG_OUT("main");
    return(0);
}

void signalHandler(int sig)
{
    DBG_IN("signalHandler");

    switch(sig) {
    case SIGINT:
    case SIGTERM:
	DBG_INFO("Recieved SIGINT/SIGTERM signal");
	if(wrapper_Daemon_ != NULL) {
	    wrapper_Daemon_->stop();
	}
	break;
    default:
	DBG_ERR("Unknown signal");
	break;
    }
    

    DBG_OUT("signalHandler");

}
