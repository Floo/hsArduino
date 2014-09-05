/*
 *          fs20 sender implementation
 *
 * (c) by Alexander Neumann <alexander@bumpern.de>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (either version 2 or
 * version 3) as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#include "fs20.h"

#ifndef PD7
#define PD7 7
#endif

void fs20_send_zero(void) {
  PORTD |= (1 << PD7); //Set
  _delay_loop_2(FS20_DELAY_ZERO);
  PORTD &= ~(1 << PD7); //Clear
  _delay_loop_2(FS20_DELAY_ZERO);
}

void fs20_send_one(void) {
  PORTD |= (1 << PD7); //Set
  _delay_loop_2(FS20_DELAY_ONE);
  PORTD &= ~(1 << PD7); //Clear
  _delay_loop_2(FS20_DELAY_ONE);
}

void fs20_send_sync(void) {
    for (uint8_t i = 0; i < 12; i++)
        fs20_send_zero();

    fs20_send_one();
}

void fs20_send_bit(uint8_t bit) {
    if (bit > 0)
        fs20_send_one();
    else
        fs20_send_zero();
}

void fs20_send_byte(uint8_t byte) {
    uint8_t i = 7;

    do {
        fs20_send_bit(byte & _BV(i));
    } while (i-- > 0);

    fs20_send_bit(parity_even_bit(byte));
}

void fs20_send(uint16_t housecode, uint8_t address, uint8_t command) {
    uint8_t maxsend = 3;
    if (command == 19 || command == 20)
        maxsend = 2;

    for (uint8_t i = 0; i < maxsend; i++) {
        fs20_send_sync(); //if dimm-up or dimm-down command, repeat send sequence only twice

        uint8_t sum = 6; /* magic constant 6 from fs20 protocol definition... */

        fs20_send_byte((housecode >> 8) & 0xFF);
        sum += ((housecode >> 8) & 0xFF);
        fs20_send_byte(housecode & 0xFF);
        sum += (housecode & 0xFF);
        fs20_send_byte(address);
        sum += address;
        fs20_send_byte(command);
        sum += command;
        fs20_send_byte(sum);

        fs20_send_zero();

        _delay_loop_2(FS20_DELAY_CMD);
    }
}

//send command and erweiterungs-byte
void fs20_send_ew(uint16_t housecode, uint8_t address, uint8_t command, uint8_t ewbyte) {

    for (uint8_t i = 0; i < 3; i++) {
        fs20_send_sync();

        uint8_t sum = 6; /* magic constant 6 from fs20 protocol definition... */

        fs20_send_byte((housecode >> 8) & 0xFF);
        sum += ((housecode >> 8) & 0xFF);
        fs20_send_byte(housecode & 0xFF);
        sum += (housecode & 0xFF);
        fs20_send_byte(address);
        sum += address;
        fs20_send_byte(command);
        sum += command;
        fs20_send_byte(ewbyte);
        sum += ewbyte;
        fs20_send_byte(sum);

        fs20_send_zero();

        _delay_loop_2(FS20_DELAY_CMD);
    }
}

