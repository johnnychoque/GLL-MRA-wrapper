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
			mgi_packet \
			wsi_packet \
			tlv \
			ari \
			ifaceTable \
			netInterface \
			gll_daemon \
			gll_im \
			gll_al \
			ifaceScan \
			gll_toolbox \
			ConfigurationFile

all_objs	=	$(addsuffix .o, $(addprefix src/, $(files)))

library		=	lib/libgll.a

CPPFLAGS	+=	-g -I./inc -Wall


.PHONY		:	bin clean

all		:	$(library) bin

$(library)	: 	$(all_objs)
			$(AR) r $@ $?

bin		:
			cd bin && gmake $(MAKECMDGOALS)

clean		: 	
			rm -rf $(targetExe) $(all_objs) $(execPos_objs) $(all_exec_objs) $(library)
			cd bin && gmake $(MAKECMDGOALS)

