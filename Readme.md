## plexAuto
Automatic updates for Plex server on Linux systems.

#### Description
This applicaiton will check for Plex server updates on an interval provided by the user in the configuration file. When there is an update available the application will check if there is any media playing, then download the update and install. If the install is sucessfull then it will send a push notification informing the user of the update via PushBullet if configured to do so. The push notification feature requires an IFTTT account with a webhook event enabled.

#### Package Requirements
* libcurl
* jsoncpp

#### Building
Run make in the cloned directory
````bash
make

````

#### Manual Building
To manually build the application using gcc.
````bash
g++ -std=c++11 -Wall src/main.cpp src/settings.cpp src/updateChecker.cpp src/downloader.cpp src/sender.cpp src/utilities.cpp -lcurl -ljsoncpp -o bin/plexauto

````

#### Installing the application
This will enable the plexauto.service with systemd and install the required components.
binary and config file are installed in /opt/plexAuto.
````bash
sudo make install

````

#### Uninstalling the application
````bash
sudo make uninstall

````

#### Starting the application
This application will automatically start at boot after installation or can manually be
started using the following command.
````bash
sudo systemctl start plexauto.service

````
