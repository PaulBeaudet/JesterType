Gesture Based Text input device
===============================

Primary Goals
----------------------
Build a input device that has a more efficient feel than a traditional full layout keyboard and is also more portable than one. Portability of this device is key, the target use platform is a smart-watch or HID specific watch, headband or ankle band.

The device, through intuitive interaction significantly reduces typing effort thus lowering barriers in computer interaction that assist practical, pleasant and meaningful communication. Our intention is for the project to be accessible for those new to programing that need to modify the software or hardware for their own needs. 

Communicating can be a very personal thing! We all do it differently. The collaborative attitude of this project should empower that spirit of individuality. Making typing speed an efficiency available to all that want it.

Technical Overview
------------------------
JesterType software currently runs on the Arduino development platform. The arduino "language" is closely derivative of the c++ programming language. We also have interest in porting to Python for the raspberry pi. (the only barier currently is porting getDebouncedState(). which may or may not be simple)

The JesterType hardware prototype is currently a 5 button keyer that relies on the arduino internal pull-up resistant. This is an incredibly simple circuit to wire! Eventual implementation though is intended to be a subtle wrist mounted sensor array. Experimentation is going to be done with capacitance to determine the feasibility of its use as the input method. Another side experiment will be a steno-type keyboard. (likely a 10-key)

Currently Supported Devices
--------------------------
HID Supported platforms- "by the arduino language":
* Arduino Micro- working alpha (testing platform)
* Arduino USB Lilly pad- Should work, untested
* Arduino DUE- Should Work, untested
* Arduino LEONARDO- Should Work, untested

Serial Testing- On these platforms the usb programmer would need to be flashed in order become a HID

(comment out KeyboardFunctions.ino and comment in serialFunctions.ino):
* Arduino UNO- Serial testing should be semi-functional
* Arduino MEGA- Serial testing should be semi-functional

Involvement
------------
The goal is collaborative development! Open Source Hardware and Software. How the hardware developement is going to work is still pending. If you would like to getting involved please introduce your self in the google group/mailing list- 
[Google Groups](https://groups.google.com/forum/?fromgroups#!forum/jestertype)


Copy notice
----------------
 JesterType- a funny way to use an arduino to type with gestures
 Copyright (c) 2012-2013 Paul C Beaudet <inof8or@gmail.com>
 This program is free software; 
 you can redistribute it and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2 of the License

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

