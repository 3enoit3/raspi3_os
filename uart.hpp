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

#ifndef UART_20180914
#define UART_20180914

#include "mmio.hpp"

// UART
struct AUXENB : public Register<0x3F215004> {
    static decltype(auto) doc() { return "Enable UART, SPI1, SPI2"; }
    static constexpr auto en_mini_uart = Field(0, RW, "Enable mini UART");
    static constexpr auto en_spi2 = Field(1, RW, "Enable SPI 1");
    static constexpr auto en_spi1 = Field(2, RW, "Enable SPI 2");
};

struct AUX_MU_IO_REG : public Register<0x3F215040> {
    static decltype(auto) doc() { return "Read and write data from UART FIFOs (and baudrate if DLAB=1)"; }
    static constexpr auto ls8_baudrate = Field(0, 8, RW, "LS 8 bits of 16 bits baudrate register if DLAB=1");
    static constexpr auto tx_write = Field(0, 8, W, "Data is put in FIFO if DLAB=0");
    static constexpr auto rx_read = Field(0, 8, R, "Data is read from FIFO if DLAB=0");
};

struct AUX_MU_IER_REG : public Register<0x3F215044> {
    static decltype(auto) doc() { return "Interrupts and baudrate"; }
    static constexpr auto en_rx_int = Field(0, RW, "Enable receive intterrupts if DLAB=0");
    static constexpr auto en_tx_int = Field(1, RW, "Enable transmit intterrupts if DLAB=0");
    static constexpr auto ms8_baudrate = Field(0, 8, RW, "MS 8 bits of 16 bits baudrate register if DLAB=1");
};

struct AUX_MU_IIR_REG : public Register<0x3F215048> {
    static decltype(auto) doc() { return "Interrupt status"; }
    static constexpr auto int_pending = Field(0, R, "Clear when an interrupt is pending");
    static constexpr auto int_id = Field(1, 2, R, "Shows the interrupt id");
    static constexpr auto clear_fifo = Field(1, 2, W, "Clear the FIFO (bit 1:rx, bit2:tx)");
};

struct AUX_MU_LCR_REG : public Register<0x3F21504C> {
    static decltype(auto) doc() { return "Line data format and baudrate"; }
    static constexpr auto data_size = Field(0, RW, "7/8 bits UART mode");
    static constexpr auto brk = Field(6, RW, "UART1_TX line");
    static constexpr auto dlab = Field(7, RW, "Gives access to the baudrate");
};

struct AUX_MU_MCR_REG : public Register<0x3F215050> {
    static decltype(auto) doc() { return "Modem signals"; }
    static constexpr auto rts = Field(1, RW, "UART1_RTS line");
};

struct AUX_MU_LSR_REG : public Register<0x3F215054> {
    static decltype(auto) doc() { return "Data status"; }
    static constexpr auto data_ready = Field(0, R, "Set if rx FIFO hold at least 1 symbol");
    static constexpr auto rx_overrun = Field(1, R, "Set if rx overrun");
    static constexpr auto tx_empty = Field(5, R, "Set if tx FIFO is empy");
    static constexpr auto tx_idle = Field(6, R, "Set if tx can accept at least 1 byte");
};

struct AUX_MU_SCRATCH : public Register<0x3F21505C> {
    static decltype(auto) doc() { return "Single byte storage"; }
    static constexpr auto scratch = Field(0, 8, RW, "Extra byte on top of SDC");
};

struct AUX_MU_CNTL_REG : public Register<0x3F215060> {
    static decltype(auto) doc() { return "Extra features on 16550 UART"; }
    static constexpr auto en_rx = Field(0, RW, "Rx enable");
    static constexpr auto en_tx = Field(1, RW, "Tx enable");
    static constexpr auto rx_auto_rts = Field(2, RW, "Rx autoflow");
    static constexpr auto tx_auto_rts = Field(2, RW, "Tx autoflow");
    static constexpr auto rts_auto_level = Field(4, 2, RW, "FIFO level for RTS de-assertion");
    static constexpr auto rts_level = Field(6, RW, "Invert RTS");
    static constexpr auto cts_level = Field(7, RW, "Invert CTS");
};

struct AUX_MU_STAT_REG : public Register<0x3F215064> {
    static decltype(auto) doc() { return "Internal status of the mini UART"; }
    // Not done
};

struct AUX_MU_BAUD : public Register<0x3F215068> {
    static decltype(auto) doc() { return "Baudrate counter"; }
    static constexpr auto baudrate = Field(0, 16, RW, "Mini UART baudrate counter");
};

// GPIO
struct GPFSEL1 : public Register<0x3F200004> {
    static decltype(auto) doc() { return "GPIO Alternate function select 1"; }
    static constexpr auto fsel14 = Field(12, 3, RW, "FSEL14");
    static constexpr auto fsel15 = Field(15, 3, RW, "FSEL15");
};

struct GPPUD : public Register<0x3F200094> {
    static decltype(auto) doc() { return "GPIO Pull-up/down"; }
    static constexpr auto pud = Field(0, 2, RW, "PUD");
};

struct GPPUDCLK0 : public Register<0x3F200098> {
    static decltype(auto) doc() { return "GPIO Pull-up/down Clock Registers 0"; }
    static constexpr auto pudclk14 = Field(14,  RW, "PUDCLK14");
    static constexpr auto pudclk15 = Field(15,  RW, "PUDCLK15");
};

// UART
class Uart {
public:
    void init();
    void send(char c);
    char getc();
    void puts(const char *s);
};

#endif

