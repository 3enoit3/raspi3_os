/*
 * Copyright (C) 2018
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "uart.hpp"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void Uart::init()
{
    /* initialize UART */
    set(AUXENB::en_mini_uart);
    reset<AUX_MU_IER_REG>();
    reset<AUX_MU_CNTL_REG>();
    reset(AUX_MU_LCR_REG::data_size);
    reset<AUX_MU_MCR_REG>();
    reset<AUX_MU_IER_REG>();
    rset<AUX_MU_IIR_REG>(0xC6);
    write(AUX_MU_BAUD::baudrate, 270);

    /* map UART1 to GPIO pins */
    write(GPFSEL1::fsel14, 2);
    write(GPFSEL1::fsel15, 2);

    reset<GPPUD>();
    sleep(150);

    reset(GPPUDCLK0::pudclk14 + GPPUDCLK0::pudclk15);
    sleep(150);

    reset<GPPUDCLK0>(); // flush GPIO setup
    rset<AUX_MU_CNTL_REG>(3); // enable Tx, Rx
}

/**
 * Send a character
 */
void Uart::send(char c) {
    /* wait until we can send */
    do{asm volatile("nop");}while(!read(AUX_MU_LSR_REG::tx_empty));
    /* write the character to the buffer */
    rset<AUX_MU_IO_REG>(c);
}

/**
 * Receive a character
 */
char Uart::getc() {
    char r;
    /* wait until something is in the buffer */
    do{asm volatile("nop");}while(!read(AUX_MU_LSR_REG::data_ready));
    /* read it and return */
    r=(char)(read(AUX_MU_IO_REG::rx_read));
    /* convert carriage return to newline */
    return r=='\r'?'\n':r;
}

/**
 * Display a string
 */
void Uart::puts(const char *s) {
    while(*s) {
        /* convert newline to carriage return + newline */
        if(*s=='\n')
            send('\r');
        send(*s++);
    }
}

