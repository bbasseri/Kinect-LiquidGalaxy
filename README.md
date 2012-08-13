Kinect-LiquidGalaxy
===================

Kinect controller for liquid galaxy

Description:
This application uses Fakenav to connect and send data to Google earth. To setup Fakenav follow the instructions:
Download and install proper version of GoogleEarth from http://www.google.com/earth/download/ge/agree.html
Edit driver.ini and add the code for spacenavigator to the settings. http://code.google.com/p/liquid-galaxy/wiki/LinuxSpaceNavigator “Enable Multi-axis support in Google Earth”
Change “SpaceNavigator/device = /dev/input/spacenavigator” to “SpaceNavigator/device = /tmp/fakenav”. This line lets GoogleEarth know to read the control inputs from /tmp/fakenav.
Open GoogleEarth and go to “tools>options” select the “Navigation” tab and select the “Enable Controller” from the “Non-mouse controller” Section. Apply and close GoogleEarth.

Running the application: 
Go to the “SOC>fakenav” folder, make and then run the “run.sh” script.
Open another terminal and navigate to “SOC>kinect” folder and run make on the files.
While “run.sh” is running open GoogleEarth, if GoogleEarth is reading properly from the /tmp/fakenave, the msg will change to “waiting for clients”
Make sure the kinect is connected, and run “./kinect localhost 23457” from “SOC>kinect” folder.
To change the port goto “/fakenav/include/fakenav.h”, and change the port number.

Interacting with the application:
To calibrate and detect a user wave at the kinect
To move forward move the hand towards the kinect.
The further your hand gets from the original location the speed is increased
To move Left and Right you are required to move the hand left and right
To move Up and Down move the hand up and down (Fix)
To stop move hand behind the body, so the kinect can’t detect.

Code:
The code for gesture recognition is located in “SOC/main.cpp”
The code for controlling the GoogleEarth is located in “SOC/control.cpp” which writes the data to “/tmp/fakenav”
Variables:
y:

Rotates around the earth, depending on the direction of the camera.
Forward “-”
Backward “+”
x

Moves on the X axis to left/right
Left “-”
Right “+”
z

Moves towards the earth or away from the center of the earth
Towards “+”
Outward “-”
yaw

Rotates on the Y-Axis
Left “-”
Right “+”
pitch

Rotates on the X-axis
Up “+”
Down “-”
roll

At the moment no movement.

*** NOTE: The data are multiplied by a negative scale. If in GoogleEarth option you have selected “Inverted controls” you should remove the negatives. ***

Connecting to google earth
Connecting to GE occurs through control.cpp .
Data are written to the fakenav file, and GE reads through them
In order to use fakenav, GE has to be set on space nav controller.
The code for the Space nav has to be included within drivers.ini (code can be found on liquidgalaxy)

The data that are sent to GE should not be too small, because of the scaling 
