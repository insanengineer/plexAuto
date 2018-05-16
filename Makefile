APPNAME=plexauto
FILES=src/main.cpp	src/settings.cpp	src/updateChecker.cpp	src/downloader.cpp	src/sender.cpp	src/utilities.cpp
CC=g++	-std=c++11	-Wall

all:	app

app:	$(FILES)
	$(CC)	$(FILES)	-lcurl	-ljsoncpp	-o	bin/$(APPNAME)

clean:
	/bin/rm -rf bin/$(APPNAME)

install: app
	mkdir -p /opt/plexAuto
	cp bin/$(APPNAME) /opt/plexAuto
	cp plexauto.conf /opt/plexAuto
	cp plexauto.service /etc/systemd/system
	systemctl enable plexauto.service

uninstall:
	systemctl disable plexauto.service
	rm -rf /opt/plexAuto
	rm -f /etc/systemd/system/plexauto.service
