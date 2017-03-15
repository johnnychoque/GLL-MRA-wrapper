CPP     = g++
CFLAGS  = -I. -I/usr/local/include -g -O2 -Wall -Wno-deprecated -DDEBUG
LDFLAGS = -L/usr/local/lib
CLIENT	= ANifaces
SERVER	= MRAwrapper
WSDL_FILES = 	RegistrationComponent.wsdl \
		LookupComponent.wsdl \
		trg_producer.wsdl
GSOAP_PATH =	/root/gsoap/soapcpp2

#WSOBJ	= soapC.o soapClient.o stdsoap2.o
WSOBJ	= envC.o stdsoap2.o $(CLIENT)ClientLib.o $(SERVER)ServerLib.o

#library = ../lib/libWSI.a

.PHONY :  clean

#all	: $(library)

#$(library)	: $(WSOBJ)
#		$(AR) r $@ $?

#%.o: %.cpp
#	$(CXX) $(CFLAGS) -c $< -o $@

all:		$(WSOBJ)
#		$(CPP) $(CFLAGS) -o main main.cpp envC.o stdsoap2.o $(CLIENT)ClientLib.o calcServerLib.o

stdsoap2.o:	stdsoap2.cpp
		$(CPP) $(CFLAGS) -DWITH_NONAMESPACES -c stdsoap2.cpp

envC.o:		env.h
		soapcpp2 -CS -L -w -x -penv env.h
		$(CPP) $(CFLAGS) -c envC.cpp

$(CLIENT)ClientLib.cpp:	$(CLIENT).h
		soapcpp2 -I$(GSOAP_PATH)/import -C -x -w -n $(CLIENT).h -p$(CLIENT)

$(CLIENT)ClientLib.o:	$(CLIENT)ClientLib.cpp
		$(CPP) $(CFLAGS) -c $(CLIENT)ClientLib.cpp

$(SERVER)ServerLib.cpp:	$(SERVER).h
		soapcpp2 -S -n -x $(SERVER).h

$(SERVER)ServerLib.o:	$(SERVER)ServerLib.cpp
		$(CPP) $(CFLAGS) -c $(SERVER)ServerLib.cpp

clean:
	rm -rf $(library)
	rm -rf *.o 
	rm -f *~

cleanws:
	rm -f soap*
	rm -f *.xsd
	rm -f *.nsmap
	rm -f $(CLIENT).h

ws:
	wsdl2h -t /root/gsoap/soapcpp2/typemap.dat -o $(CLIENT).h $(WSDL_FILES)
	touch env.h
	soapcpp2 -C -I/root/gsoap/soapcpp2/import -x -L $(CLIENT).h
