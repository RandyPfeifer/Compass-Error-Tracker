# Compass-Error-Tracker
Using a compass and small display, plot accumulated error (+/-) in your heading vs intended course


This project was undertaken to provide training experiences for canoe paddlers interested in improving their ability to paddle from Point A 
directly to Point B when they know the initial direction to B while sitting at A. It can also be used in the field when paddling in circumstances where 
visibility is impaired (i.e., fog) and a target is not readily visible while enroute.

The second purpose of this project was as a learning experience for myself as this is my first Arduino, MC5883L, SSD1306 and Github experience. 

The project utilized an Arduino compatible processor with I2C services to communicate with a MC5883L compass module to measure changes in direction 
as well as a SSD1306-based 128 x 64 pixel display to output onoing assessment of accumulated error.

When initialized (on power-up or reset), the system samples the immediate / current direction the craft is pointing (don't turn the system on until you are pointing where you want to go).  Note that all compass measurements taken are actually an average of 3 samples taken 300 ms apart to remove a bit of noise.  This bearing is then used as the intended (target) course.

Subsequent measurements on a periodic basis (1 - 2 seconds) are compared with the initially recorded target course.  
If a measurement is different from the initally captured target, that "Error" is displayed and accumulated with subsequent measurements.  
The accumulated error may grow or shrink depending on adjustments made by the person steering the craft. Ideally
they would strive to keep this accumulated error as small as possible without resorting to abrupt changes in direction (no sudden moves).

The overall accumulated Error (degree - seconds) is displayed graphically as a growing (or shrinking) 1/4 circle. The sign (+/-) of this accumulated
error is conveyed by which half of the screen the 1/4 circle is placed:

- 1/4 circle on right indicates the accumulated error has moved the craft to the right of intended course.
- 1/4 circle on the left indicates an accumulated error moving to the left of the intended course.

As the accumulated error grows or shrinks the radius of the 1/4 circle expands or contracts. As you move from the error being positive to negative, 
the 1/4 circle flips to the other side of the display.

The intent would be for the user to adjust their steering to keep the circle radius as small as possible (ideally zero). 

If the accumulated error grows beyond a certain threshold, the error number displayed is displayed in Yellow Font to signal the need for more aggressive action to correct the error.

## Notes

This system is unaware of effects of wind on the craft in motion.  As such, on windy days, "off course" conditions can be experienced even
if the accumulated error is maintained at zero. 

Similarly, this system is unaware of the speed of the craft and assumes a constant speed. 

Under the conditions of no wind (or other forces that would move the craft off course), and a constant speed, the display will provide a faithful indication of on/off course and a relative magnitued of the current off-course condition. 
