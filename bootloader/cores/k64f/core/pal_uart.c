/*
 * Copyright (c) 2017 Bob Cousins bobcousins42@googlemail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "PinNames.h"
#include "gpio_api.h"
#include "serial_api.h"

#include "pal_uart.h"

#include "board.h"

serial_t serial;
gpio_t pin_tx = {.pinName.W = PTC17};
gpio_t pin_rx = {.pinName.W = PTC16};



void uart_init(void)
{
  pin_tx.pinName.function = 3;
  pin_rx.pinName.function = 3;

  serial_init (&serial, BL_UART, pin_tx.pinName, pin_rx.pinName);
  serial_baud (&serial, BL_BAUD_RATE);
  serial_format(&serial, 8, ParityNone, 1);
}

bool uart_data_available(void)
{
	return serial_readable(&serial);
}

char uart_receive(void)
{
	return serial_getc(&serial);
}

char uart_receive_wait(void)
{
  while (!serial_readable(&serial));
  return serial_getc(&serial);
}

void uart_send_byte (char byte)
{
  while (!serial_writable(&serial));
  serial_putc (&serial, byte);
}

void uart_send_data(unsigned char *data, unsigned int count)
{
  while (count--)
    uart_send_byte (*data++);
}

int uart_get_data(unsigned char *data, unsigned int count)
{
  while (count--) {
      *data++ = uart_receive_wait ();
  }
  return 0;
}
