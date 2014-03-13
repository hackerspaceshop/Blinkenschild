This file is part of the BlinkenSchild Android App.

The BlinkenSchild Android App is free software: you can redistribute
it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

The BlinkenSchild Android App is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with the BlinkenSchild Android App. If not, see
<http://www.gnu.org/licenses/>.

Author: Chris Hager, March 2014


Commands
========

All commands have to be terminated with 0x0D (Carriage Return)

                      Client                            Arduino
                      ------                            -------
    List animations:  +list:  ---------------------->
                              <----------------------  +list:<file>\n+list:<file2>\n...\n-list

    Set animation:    +anim:<file> ----------------->
                                   <-----------------  -anim

    Set text:         +text:line1;line2;line3;... -->
                                                  <--  -text

    Set text color:   +text-color:RRGGBB  ---------->
                                          <----------  -text-color