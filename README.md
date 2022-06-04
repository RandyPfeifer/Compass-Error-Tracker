# Compass-Error-Tracker
Using a compass, GPS and small display, plot accumulated error (+/-) in your heading vs intended course


This project was undertaken to provide training experiences for canoe paddlers interested in improving their ability to paddle from Point A 
directly to Point B when they know the initial direction to B while sitting at A. It can also be used in the field when paddling in circumstances where 
visibility is impaired (i.e., fog) and a target is not readily visible while enroute.

The second purpose of this project was as a learning experience for myself as this is my first Arduino, MC5883L, SSD1306, NEO-6M and Github experience. 

The project utilized an Arduino compatible processor with I2C services to communicate with a MC5883L compass module to measure changes in direction 
as well as a SSD1306-based 128 x 64 pixel display to output onoing assessment of accumulated error.  The GPS module in play is the NEO-6M module using software serial for UART communication.Two specfic processors have been tested with this software:  Arduino Nano Every and Seeeduino XIAO.  The Nano was used during early development but most all testing and later development (GPS in particular) was done on the Seeeduino XIAO. 

When initialized (on power-up or reset or when the push_button is released), the system samples the immediate / current direction the craft is pointing (don't turn the system on until you are pointing where you want to go).  Note that all compass measurements taken are actually an average of 3 samples taken 300 ms apart to remove a bit of noise.  This bearing formed from 9 samples (3 reads of 3 samples each averaged) is then used as the intended (target) course.  The display will signal "TARGET ACQUIRED" when this is accomplished. 

Subsequent measurements on a periodic basis (2 - 3 seconds) are compared with the initially recorded target course.  
If a measurement is different from the initally captured target, that "Error" is displayed in the middle of the screen and accumulated with subsequent measurements.  
The accumulated error may grow or shrink depending on adjustments made by the person steering the craft. Ideally
they would strive to keep this accumulated error as small as possible without resorting to abrupt changes in direction (no sudden moves).

The overall accumulated Error (degree - seconds) is displayed graphically as a growing (or shrinking) 1/4 circle. The sign (+/-) of this accumulated
error is conveyed by which half of the screen the 1/4 circle is placed:

- 1/4 circle on right indicates the accumulated error has moved the craft to the right of intended course.
- 1/4 circle on the left indicates an accumulated error moving to the left of the intended course.

As the accumulated error grows or shrinks the radius of the 1/4 circle expands or contracts. As you move from the accumulated error being positive to negative, 
the 1/4 circle of interest flips to the other side of the display.

The intent would be for the user to adjust their steering to keep the circle radius as small as possible (ideally near zero). 

If the accumulated error grows beyond a defined threshold, the screen is updated to add a "turn signal" to the blank side of the screen.  For example, if the craft has strayed off to the right and a large 1/4 circle has grown on the right side of the screen, the left side of the screen will display a left turn signal.  Similarly for the opposite condition.  


Also, if the accumulated error grows beyond a certain (larger) threshold, the current cycle measured error number is displayed in Yellow and large Font to signal the need for more aggressive action to correct the error.

Further, if the accumulated error grows to an enormous threshold, the side of the screen reflecting that error will display the actual accumulated error to be "worked off" by steering in the appropriate direction indicated on the other side of the display. 

At any time, the button on the unit can be pressed which will effectively restart the process, taking a new target bearing and zeroing out the accumulated error. 

A short video of the program in operation (inside my house) can be found here:  https://youtu.be/wzSbLun69U0

## Notes

This system is unaware of effects of wind on the craft in motion.  As such, on windy days, "off course" conditions can be experienced even
if the accumulated error is maintained at zero. 

Similarly, this system is unaware of the speed of the craft and assumes a constant speed. 

Under the conditions of no wind (or other forces that would move the craft off course), and a constant speed, the display will provide a faithful indication of on/off course and a relative magnitued of the current off-course condition. 

There is some  code in there to account for the 32bit vs 16 bit differences between the processors and the fact that the measured samples from the compass module are 16 bit signed integers.

Schematic and board designs included in /src (created with Autodesk - Eagle).
A few pictures in /Examples. including a photo of a hand etched board and first assembled prototype and a link to a utube video of working system.

## GPS services

The unit also includes a GPS module.  Once it is in service and has a lock with sufficient number of satellites, it will begin to deliver sentences with location, heading, speed, etc.  The software will switch to navigating using the GPS once certain criteria are met:

1. The paddler is within a certain cumulative error level (we want them to be close to their original intended line of travel)
2. The current heading is close to the intended heading identified at the beginning of the session (i.e., pointing in the right direction)

These criteria ensure that the craft is on the correct path (ignoring wind conditions referenced above) and GPS navigation can continue from there. 
After maintaining this criteria for a period of about 45 seconds, the GPS will derive a heading from its own readings and determine the heading target that the GPS will follow.
This avoids the need for the compass to produce a heading that is true to the real world (we only needed relative changes in direction from the compass to do all of our navigation so far). 

With that GPS-derived heading the software calculates the deviation from a line drawn from the point the GPS came into service (point A') in the direction of the GPS-derived target heading.  This deviation is now displayed as the cumulative error in the same fashion cumulative errors were displayed under compass navigation. The error displayed in the middle of the screen, which used to be the difference in current compass angle and the intended heading, is replaced with a new error which is the difference in current heading and the intended heading. Hence the meaning and interpretation of the display are consistent in Compass and GPS modes. 

In the upper right corner of the display will be an indicator of the status of the GPS module:
!G - no GPS services are available
~G - the system is currently using valid GPS data and is in the 45+ second training cycle with the above criteria being met (this cycle may fail and need to be extended until the above criteria are met - a flip back to !G).
G! - the system has acheived all criteria for switching to GPS navigation and has made the transition. No further us of the compass is needed. 

When G! has been acheived, the upper left corner of the display shows the current heading reported by the GPS. 

Every 2-3 seconds, the GPS is polled for new information and the error calculation is repeated and the display updated.  The paddler can now be confident that no additional error can be introduced by effects of wind or current.  As long as GPS services remain, the system will continue to deliver accurate assessment of the overall divergence from the GPS derived heading.  

In order to eliminate any error in the GPS training cycle, the user can choose to explicitly provide the intended heading (in addition to pointing the craft in the proper direction when the system is powered up).   Even if GPS services are not available, the user can press the (reset) button on the unit and hold it down for an excess of 3 seconds.  This will signal the system to enter an "input" mode and it will display the current compass reading on the display.  The user can then rotate the unit to get the compass direction they are intending to travel on the display.  Once this is acheived, simply releasing the button will cause this last reading to be recorded for future use by the GPS.  Once the GPS comes online, the training process is circumvented and the system begins to immediatley navigate using the "input" target heading. 

The user could of course wait at their starting point for the GPS to be in service and perform all of their paddling under the guidance of the GPS. 


