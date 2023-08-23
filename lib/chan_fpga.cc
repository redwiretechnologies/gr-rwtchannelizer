/* -*- c++ -*- */
/*
 * Copyright 2023 Red Wire Technologies.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "chan_fpga.h"
#include "taps.h"

namespace gr {
namespace rwt {

chan_fpga::chan_fpga(
    std::shared_ptr<base_device_common> common,
    bool debug) :
    rwt_registers_common(common, debug)
{}

void
chan_fpga::set_coefficients() {
    //Send the coefficients one at a time
    for(int i=0; i<LEN_COEFF; i++) {
        m_common->write_reg(REG_BLK_USER, REG_CHAN_RELOAD_DATA, coefficients[i]);
        if(i==LEN_COEFF-1)
            m_common->write_reg(REG_BLK_USER, REG_CHAN_RELOAD_LAST, 1);
        if(reload_update != 0)
            reload_update = 0;
        else
            reload_update = 1;
        m_common->write_reg(REG_BLK_USER, REG_CHAN_RELOAD_UPDATE, reload_update);
    }
    m_common->write_reg(REG_BLK_USER, REG_CHAN_RELOAD_LAST, 0);
}

void
chan_fpga::set_downselect(const std::string value) {
    //Convert the string into a vector of ints with the mask of the channels you want
    parse_list(value, "hex", (void *) &downselections);

    //Send the masked ints one at a time
    for(int i=downselections.size()-1; i>=0; i--) {
        m_common->write_reg(REG_BLK_USER, REG_CHAN_DOWNSELECT_DATA, downselections[i]);
        if(i==0)
            m_common->write_reg(REG_BLK_USER, REG_CHAN_DOWNSELECT_LAST, 1);
        if(down_update != 0)
            down_update = 0;
        else
            down_update = 1;
        m_common->write_reg(REG_BLK_USER, REG_CHAN_DOWNSELECT_UPDATE, down_update);
    }
    m_common->write_reg(REG_BLK_USER, REG_CHAN_DOWNSELECT_LAST, 0);
}

void
chan_fpga::reset_chan() {
    m_common->write_reg(REG_BLK_USER, REG_CHAN_RESET, 0);
    m_common->write_reg(REG_BLK_USER, REG_CHAN_RESET, 1);
}

void
chan_fpga::read_registers() {
	read_help(REG_BLK_USER, REG_CHAN_FFT_SIZE, "fft size");
    read_help(REG_BLK_USER, REG_CHAN_AVG_LEN, "averaging length");
    read_help(REG_BLK_USER, REG_CHAN_PAYLOAD_LEN, "payload length");
    read_help(REG_BLK_USER, REG_CHAN_BYPASS, "channelizer bypass");
    read_help(REG_BLK_USER, REG_CHAN_RESET, "channelizer reset");
    read_help(REG_BLK_USER, REG_CHAN_FIRST_ENABLE, "channelizer first enable");
    read_help(REG_BLK_USER, REG_CHAN_FIRST_NUM, "channelizer first number");
    read_help(REG_BLK_USER, REG_CHAN_FIRST_WAIT, "channelizer first wait");
}

void
chan_fpga::test_registers() {
	reg_test_help(REG_BLK_USER, REG_CHAN_FFT_SIZE, 128, "fft size");
    reg_test_help(REG_BLK_USER, REG_CHAN_AVG_LEN, 64, "averaging length");
    reg_test_help(REG_BLK_USER, REG_CHAN_PAYLOAD_LEN, 64, "payload length");
    reg_test_help(REG_BLK_USER, REG_CHAN_BYPASS, 1, "channelizer bypass");
    reg_test_help(REG_BLK_USER, REG_CHAN_RESET, 1, "channelizer reset");
    reg_test_help(REG_BLK_USER, REG_CHAN_FIRST_ENABLE, 1, "channelizer first enable");
    reg_test_help(REG_BLK_USER, REG_CHAN_FIRST_NUM, 10, "channelizer first number");
    reg_test_help(REG_BLK_USER, REG_CHAN_FIRST_WAIT, 30, "channelizer first wait");
}

void
chan_fpga::set_fft_size(uint32_t value_u32) {
    fft_size = value_u32;
    m_common->write_reg(REG_BLK_USER, REG_CHAN_FFT_SIZE, fft_size);
    if (value_u32 == 8)
        coefficients = taps_8;
    else if (value_u32 == 16)
        coefficients = taps_16;
    else if (value_u32 == 32)
        coefficients = taps_32;
    else if (value_u32 == 64)
        coefficients = taps_64;
    else
        coefficients = taps_128;
    reset_chan();
    set_coefficients();
}

void
chan_fpga::set_avg_len(uint32_t value_u32) {
    avg_len = value_u32;
    m_common->write_reg(REG_BLK_USER, REG_CHAN_AVG_LEN, avg_len);
}

void
chan_fpga::set_payload_length(uint32_t value_u32) {
    payload_length = value_u32;
    m_common->write_reg(REG_BLK_USER, REG_CHAN_PAYLOAD_LEN, payload_length);
}

void
chan_fpga::set_chan_bypass(uint32_t value_u32) {
    chan_bypass = (value_u32 == 1);
    if (chan_bypass)
        value_u32 = 1;
    else
        value_u32 = 0;
    m_common->write_reg(REG_BLK_USER, REG_CHAN_BYPASS, value_u32);
}

void
chan_fpga::set_chan_first_enable(uint32_t value_u32) {
    chan_first_enable = (value_u32 == 1);
    if (chan_first_enable)
        value_u32 = 1;
    else
        value_u32 = 0;
    m_common->write_reg(REG_BLK_USER, REG_CHAN_FIRST_ENABLE, value_u32);
}

void
chan_fpga::set_chan_first_num(uint32_t value_u32) {
    chan_first_num = value_u32;
    m_common->write_reg(REG_BLK_USER, REG_CHAN_FIRST_NUM, chan_first_num);
}

void
chan_fpga::set_chan_first_wait(uint32_t value_u32) {
    chan_first_wait = value_u32;
    m_common->write_reg(REG_BLK_USER, REG_CHAN_FIRST_WAIT, chan_first_wait);
}

int
chan_fpga::handle_registers(const std::string key, const std::string value) {
    uint32_t value_u32;

    if (key == "downselect") {
        set_downselect(value);
    }else if (key == "fft_size") {
        value_u32 = (uint32_t)strtoul(value.c_str(), NULL, 0);
        set_fft_size(value_u32);
    }else if (key == "avg_len") {
        value_u32 = (uint32_t)strtoul(value.c_str(), NULL, 0);
        set_avg_len(value_u32);
    }else if (key == "payload_length") {
        value_u32 = (uint32_t)strtoul(value.c_str(), NULL, 0);
        set_payload_length(value_u32);
    }else if (key == "chan_bypass") {
        value_u32 = (uint32_t)strtoul(value.c_str(), NULL, 0);
        set_chan_bypass(value_u32);
    }else if (key == "chan_first_enable") {
        value_u32 = (uint32_t)strtoul(value.c_str(), NULL, 0);
        set_chan_first_enable(value_u32);
    }else if (key == "chan_first_num") {
        value_u32 = (uint32_t)strtoul(value.c_str(), NULL, 0);
        set_chan_first_num(value_u32);
    }else if (key == "chan_first_wait") {
        value_u32 = (uint32_t)strtoul(value.c_str(), NULL, 0);
        set_chan_first_wait(value_u32);
    }
    else {
        return -1;
    }
    return 0;
}

}
}
