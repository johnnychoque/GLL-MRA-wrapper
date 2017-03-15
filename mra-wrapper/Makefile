#
# Project: Ambient Networks Phase 2
# Subproject: Workpackage C - Multi-Radio Access (GLL)
#   
# File: Makefile
# 
# Description: This is the Makefile for the GLL FE
#
# Authors: Ramon Aguero (ramon@tlmat.unican.es)
#          Johnny Choque (jchoque@tlmat.unican.es)
#  
# Organisation: University of Cantabria
# 

files		=	timer \
			scheduler \
			tlv \
			mra_wrapper \
			wrapper_daemon \
			wsi_packet \
			ConfigurationFile

all_objs	=	$(addsuffix .o, $(addprefix src/, $(files)))

library		=	lib/libMRAwrapper.a

CPPFLAGS	+=	-g -I./inc -I./wsi -I/usr/local/include -Wall -DDEBUG


.PHONY		:	bin wsi clean 

all		:	$(library) wsi bin

$(library)	: 	$(all_objs)
			$(AR) r $@ $?

bin		:
			cd bin && gmake $(MAKECMDGOALS)

wsi		:
			cd wsi && gmake $(MAKECMDGOALS)

clean		: 	
			rm -rf $(targetExe) $(all_objs) $(execPos_objs) $(all_exec_objs) $(library)
			cd bin && gmake $(MAKECMDGOALS)
			cd wsi && gmake $(MAKECMDGOALS)
