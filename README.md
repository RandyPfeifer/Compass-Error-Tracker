# Compass-Error-Tracker
Using a compass and small display, plot accumulated error (+/-) in your heading vs intended course

The project utilized an Arduino compatible processor with I2C services to communicate with a MC5883L compass module to measure changes in direction 
as well as a SSD1306-based 128 x 64 pixel display to output onoing assessment of accumulated error.

When initialized, the system samples the immediate / current direction.  This is then used as the intended course.
Subsequent samples on a periodic basis (1 - 2 seconds) are compared with the intended course.  If different, that "Error" is displayed and accumulatedwith subsequent samples. 

The overall accumulated Error (degrees X seconds) is displayed graphically as a growing (or shrinking) 1/4 circle. The sign (+/-) of this accumulated
error is conveyed by which half of the screen the 1/4 circle is placed:

- 1/4 circle on right indicates an accumulated error moving to the right of intended course.
- 1/4 circle on the left indicates an accumulated error moving to the left of the intended course.

As the accumulated error grows or shrinks the radius of the 1/4 circle expands or contracts. As you move from the error being positive to negative, 
the 1/4 circle flips to the other side of the display.

The intend would be for the user to adjust their steering to keep the circle as small as possible. 

This system is unaware of effects of wind on the craft in motion.  As such, on windy days, "off course" conditions can be experienced even
if the accumulated error is maintained at zero. 

Similarly, this system is unaware of the speed of the craft and assumes a constant speed. 

Under the conditions of no wind (or other forces that would move the craft off course), and a constant speed, the display will provide a faithful indication of on/off course and a relative magnitued of the current off-course condition. 

This project was undertaken to provide training experiences for canoe paddlers interested in improving their ability to paddle from Point A 
directly to Point B when they know the initial direction to B while sitting at A. 

The second purpose of this project was as a learning experience for myself as this is my first Arduino, MC5883L, SSD1306 and Github experience. 
