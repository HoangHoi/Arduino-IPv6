/*  
 *  GPS Module
 *  
 *  Copyright (C) Libelium Comunicaciones Distribuidas S.L. 
 *  http://www.libelium.com 
 *  
 *  This program is free software: you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation, either version 3 of the License, or 
 *  (at your option) any later version. 
 *  a
 *  This program is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License 
 *  along with this program.  If not, see http://www.gnu.org/licenses/. 
 *  
 *  Version:           2.0
 *  Design:            David Gascón 
 *  Implementation:    Marcos Yarza, Jorge Casanova & Marcos Martinez
 */

// Include the SoftwareSerial library
#include "SoftwareSerial.h"

// Constants
#define txPin 8      //tx pin in GPS connection
#define rxPin 9      //rx pin in GPS connection

// Set up the GPS serial port
SoftwareSerial gps = SoftwareSerial(rxPin, txPin);


// Variables
byte byteGPS = 0;
int i = 0;
int state = 0;
char dataGPG[100] = "";
char *pch;
char *GGA[15];
int sat = 0;

void setup() {
    //setup for Serial Port
    Serial.begin(9600);
    Serial.flush();

    //setup for GPS Serial Port  
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    gps.begin(4800);
    gps.flush();
    Serial.println(_SS_MAX_RX_BUFF);
    //setup satellites signal
    pinMode(13, OUTPUT);
    digitalWrite(13, LOW); // Turn off the led until a satellite signal
}

void loop() {
    gps.flush();
    Serial.flush();

    Serial.println("Waiting GPS data...");

    // Prepare all for reading GPS Serial Port
    memset(dataGPG, 0, sizeof (dataGPG)); // Remove previous readings
    byteGPS = 0; // Remove data
    byteGPS = gps.read(); // Read the byte that is in the GPS serial port
    delay(1000);

    // Find the desired string
    while (byteGPS != '$') {
        byteGPS = gps.read();
    }

    // Save the string in an array
    i = 1;
    dataGPG[0] = '$';

    while (byteGPS != '*') {
        byteGPS = gps.read();
        dataGPG[i] = byteGPS;
        i++;
    }

    dataGPG[i] = '\0';
    string(); // Call to the function that manipulates our string

}

/*
This function will allow us to identify the data we need to get the longitude, latitude ...
 */

void string() {
    i = 0;
    memset(GGA, 0, sizeof (GGA)); // Remove previous readings

    pch = strtok(dataGPG, ",");

    // Analyze the saved interval in pch to see if it the needed string
    if (strcmp(pch, "$GPGGA") == 0) {
        while (pch != NULL) {
            pch = strtok(NULL, ",");
            GGA[i] = pch;
            i++;
        }

        plot(GGA, "$GPGGA"); // Call to the function that is going to display the data
    }
}

/*
This function organize the gps data received for printing in the serial monitor.
 */

void plot(char **GGAPrint, char *trama) {
    state = atoi(GGAPrint[5]);
    sat = atoi(GGAPrint[6]);

    if (trama == "$GPGGA" && state == 1) {
        digitalWrite(13, HIGH); // Then there are satellites, the LED switch ON

        Serial.println("");
        Serial.println("----------------------------------------------");
        Serial.print("UTC Hour -> ");
        Serial.println(GGAPrint[0]);
        Serial.print("Latitude -> ");
        Serial.print(GGAPrint[1]);
        Serial.println(GGAPrint[2]);
        Serial.print("Longitude -> ");
        Serial.print(GGAPrint[3]);
        Serial.println(GGAPrint[4]);
        Serial.print("GPS quality: 0=null; 1=GPS fixed -> ");
        Serial.println(GGAPrint[5]);
        Serial.print("Number of satellites -> ");
        Serial.println(sat);
        Serial.print("Horizontal Dilution of Precision -> ");
        Serial.println(GGAPrint[7]);
        Serial.print("Antenna altitude -> ");
        Serial.print(GGAPrint[8]);
        Serial.println(GGAPrint[9]);
        Serial.print("Geoid Separation -> ");
        Serial.print(GGAPrint[10]);
        Serial.println(GGAPrint[11]);
        Serial.println("----------------------------------------------");
        Serial.println("");

    }
    else // If no satellite connection
    {
        digitalWrite(13, LOW); // Turn off the LED
        Serial.println("");
        Serial.println("-----------------------------");
        Serial.print("|--- Satellites Used -->");
        Serial.print(sat);
        Serial.println(" |");
        Serial.println("|----Waiting location----|");
        Serial.println("-----------------------------");
        Serial.println("");
    }
}
