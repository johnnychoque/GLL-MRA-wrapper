CPP     = g++
CFLAGS  = -I. -I/usr/local/include -g -O2 -Wall -Wno-deprecated -DDEBUG
LDFLAGS = -L/usr/local/lib
CLIENT	= ANifaces
SERVER	= WSwrapper
SERVER_HDR = MRAwrapper.h 
WSDL_FILES = 	RegistrationComponent.wsdl \
		LookupComponent.wsdl \
		trg_producer.wsdl
GSOAP_PATH =	/root/gsoap/soapcpp2

WSOBJ	= envC.o stdsoap2.o $(CLIENT)ClientLib.o $(SERVER)ServerLib.o

.PHONY :  clean

all:		$(WSOBJ)

stdsoap2.o:	stdsoap2.cpp
		$(CPP) $(CFLAGS) -DWITH_NONAMESPACES -c stdsoap2.cpp

envC.o:		env.h
		soapcpp2 -CS -L -w -x -penv env.h
		$(CPP) $(CFLAGS) -c envC.cpp

$(CLIENT)ClientLib.cpp:	$(CLIENT).h
		soapcpp2 -I$(GSOAP_PATH)/import -C -x -w -n $(CLIENT).h -p$(CLIENT)

$(CLIENT)ClientLib.o:	$(CLIENT)ClientLib.cpp
		$(CPP) $(CFLAGS) -c $(CLIENT)ClientLib.cpp

$(SERVER)ServerLib.cpp:	$(SERVER_HDR)
		soapcpp2 -S -n -x $(SERVER_HDR)

$(SERVER)ServerLib.o:	$(SERVER)ServerLib.cpp
		$(CPP) $(CFLAGS) -c $(SERVER)ServerLib.cpp

clean:
	rm -f *.o *~

cleanws:
	rm -f stdsoap2.cpp stdsoap2.h
	rm -f soap* *.xsd *.nsmap $(CLIENT)* $(SERVER)* env* MRAservices.wsdl

ws:
	wsdl2h -t /root/gsoap/soapcpp2/typemap.dat -o $(CLIENT).h $(WSDL_FILES)
	touch env.h
	soapcpp2 -C -I/root/gsoap/soapcpp2/import -x -L $(CLIENT).h
	cp $(GSOAP_PATH)/stdsoap2.cpp $(GSOAP_PATH)/stdsoap2.h ./
	echo "namespace $(CLIENT) {" > tmp; cat $(CLIENT).h >> tmp; mv -f tmp $(CLIENT).h; echo "}" >> $(CLIENT).h
