/*
  rspo-send - Raspberry Pi Repote Switched Power Outlet TX Software
  Copyright (c) 2020 eesar.  All right reserved.

  Inspired by the github projects RCSwitch and 433Utils

*/

#include <stdio.h>
#include <map>
#include <string>
extern "C" {
#include <wiringPi.h>
}

using namespace std;

// matches columns of rspo-sniffer result
struct rspop_t { // remote switched power outlets protocol struct
    unsigned int start_h;
    unsigned int start_l;
    unsigned int zeros_h;
    unsigned int zeros_l;
    unsigned int ones_h;
    unsigned int ones_l;
    unsigned int num_of_bits;
    unsigned int num_of_packtes;
    unsigned int num_of_repeats;
};

// add found protocols with rspo-sniffer here
static map<string, rspop_t> rspop = {
    {"ELRO",      { 280, 10000, 280, 1000,  920,  360, 24, 1, 10} },
    {"Conrad",    { 750,  6900, 540, 1230, 1210,  630, 32, 1, 20} },
    {"HEITECH1",  {2830,  7060, 330, 1140,  900,  590, 24, 4,  4} },
    {"HEITECH2",  { 250,  2320, 270, 1210,  960,  540, 24, 4,  4} }
} ;

// check raspberry-pi documentation for pin name mapping to GPIO header
// or try 'gpio readall' on the command line
static const int TX_PIN = 0;

//--------------------------------------------------------------------------------------------------
// usage: rspo-send <protocol> <code packets>
// Examples:
// > rspo-send ELRO 044051
// > rspo-send HEITECH2 EE99AA CC190A BE371A 70B90A
//--------------------------------------------------------------------------------------------------
int main(int argc, char *argv[]) {

    // check if given protocol is defined
    string gp(argv[1]);
    if ( rspop.count(gp) == 0 ) {
        printf("Error: given protocol '%s' is not defined\n", gp.c_str());
        return 1;
    }

    // check if number of code packets equal protocol definition
    if ( argc < rspop[gp].num_of_packtes + 2 ) {
        printf("Error: to less code packets for given for protocol '%s'\n", gp.c_str());
        return 1;
    }

    // setup wiring lib and TX pin
    wiringPiSetup();
    pinMode(TX_PIN, OUTPUT);

    // send given code packets like defined in protocol struct
    unsigned long code;
    for (int k = 0 ; k < rspop[gp].num_of_packtes ; k++ ) {
        code = strtoul(argv[2+k],NULL,16);
        for (int j = 0; j < rspop[gp].num_of_repeats; j++ ) {
            // send start sequence
            digitalWrite(TX_PIN, HIGH); delayMicroseconds( rspop[gp].start_h);
            digitalWrite(TX_PIN, LOW ); delayMicroseconds( rspop[gp].start_l);
            for (int i = rspop[gp].num_of_bits-1; i >= 0; i--) {
                if (code & (1L << i)) {
                    digitalWrite(TX_PIN, HIGH); delayMicroseconds( rspop[gp].ones_h);
                    digitalWrite(TX_PIN, LOW ); delayMicroseconds( rspop[gp].ones_l);
                } else {
                    digitalWrite(TX_PIN, HIGH); delayMicroseconds( rspop[gp].zeros_h);
                    digitalWrite(TX_PIN, LOW ); delayMicroseconds( rspop[gp].zeros_l);
                }
            }
        }
    }
    digitalWrite(TX_PIN, LOW); // just make sure pin is low for next command

    return 0;
}
