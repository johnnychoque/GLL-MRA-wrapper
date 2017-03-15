exec		=	wrapper_main

lib		= 	MRAwrapper

CPPFLAGS 	+=	-g -I../inc -Wall -DDEBUG 
LDFLAGS		+=	-L../lib -L/usr/local/lib

.PHONY		:	clean

all		:	$(exec)

clean           :	
			rm -rf $(exec) 

$(exec)	: % : %.cc
#		$(CXX) $(CPPFLAGS) $(LDFLAGS) $<  -o $@ -lpthread -lMRAwrapper -lWSI -lTrg -lgsoap++	
			$(CXX) $(CPPFLAGS) $(LDFLAGS) ../wsi/envC.o ../wsi/ANifacesClientLib.o ../wsi/WSwrapperServerLib.o ../wsi/stdsoap2.o $<  -o $@ -lpthread -lMRAwrapper -lTrg

$(exec)	: ../lib/libMRAwrapper.a ../lib/libWSI.a


