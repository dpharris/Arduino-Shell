/**
 * @file ShellBlinkFrame.ino
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2016, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * @section Description
 * This Arduino sketch shows how to use the Shell library to
 * implement the classical blink sketch as a script using parameter
 * frame and element access.
 */

#include <Shell.h>

Shell<16,16> shell(Serial);

void setup()
{
  Serial.begin(57600);
  while (!Serial);
  Serial.println(F("ShellBlinkFrame: started"));

  // : blink ( ms pin -- )
  //   pin output
  //   {
  //      pin high ms delay
  //      pin low ms delay
  //      true
  //   } while ;
  const char* blink = "2$2_@O{2_@H1_@D2_@L1_@DT}w";

  // 1000 13 blink
  shell.trace(true);
  shell.push(1000);
  shell.push(13);
  shell.execute(blink);
}

void loop()
{
}