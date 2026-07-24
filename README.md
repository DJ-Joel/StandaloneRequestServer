# OpenKJ Standalone Request Server
Standalone basic single-venue request server implementation for use with OpenKJ.
+Adding the ability for singers to create accounts and save favorite songs
+Adding the ability for Singers to request Key Changes when submitting requests


Phase 1:  Making your Windows based computer a web server inside home or venue:

You need to install some files on your computer before you can make it a web server:
Go here https://wampserver.aviatechno.net/
Scroll to the bottom and find the sections called Best way to install Visual C++ Redistributable Packages
There will be a link to the file Visual C++ Redistributable Runtimes All-in-One
Download it
Unzip it
Double click the file INSTALL_ALL.BAT

Once that is complete you will need to install the Web Server Software called WAMP Server
Back at the same web site as before (https://wampserver.aviatechno.net/) find the section called Installers Wampserver full install version
There will be a link to the LATEST VERSION
Download and Install that file at c:\
Do not install WAMP inside a folder.  It must be in the root folder of the drive.

Run WampServer  be sure it is running for all future steps


Phase 2:  Pointing OpenKJ to your computer to see song requests

Go to the root folder of your drive and open the WAMP64 folder, then open the www folder.
Create a folder called requests    (all lower case letters)
Download the ZIP file from this repository
Extract the contents of the ZIP file to the requests folder

In the OpenKJ application:
click on Tools  >>  Settings  >>  Network
Enter this in the SERVER URL field
http://###.###.#.###:80/requests/api.php   (The ### is your computer's IP address)
Check the box to IGNORE HTTPS CERTIFICATE ERRORS
Close this window
Click on KARAOKE  >> INCOMING REQUESTS
Click on UPDATE REMOTE DB    (this could take a while depending on how many songs you have)

Open a web browser and go to 
http://###.###.#.###/requests

You should see your Request Search screen
You can search but you cannot request unless you have turned on requests in OpenKJ
Click on KARAOKE  >> INCOMING REQUESTS
Check the box to Allow Requests


Phase 3:  Running the request server when you are ready to sing.

Start WAMP64
Start OpenKJ
Click on KARAOKE  >> INCOMING REQUESTS
Click on UPDATE REMOTE DB
Check the box to Allow Requests

If you would like to change the name of your Karaoke Venue Name you can open settings.ini with Notepad and make the change



