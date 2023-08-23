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

#include <rwt/rwt_registers_common.h>
#include <rwt/common_registers.h>

#ifndef INCLUDED_RWT_CHAN_FPGA_H
#define INCLUDED_RWT_CHAN_FPGA_H

namespace gr {
namespace rwt {

#define REG_CHAN_RELOAD_DATA         19
#define REG_CHAN_RELOAD_UPDATE       20
#define REG_CHAN_RELOAD_LAST         21
#define REG_CHAN_DOWNSELECT_DATA     22
#define REG_CHAN_DOWNSELECT_UPDATE   23
#define REG_CHAN_DOWNSELECT_LAST     24
#define REG_CHAN_FFT_SIZE            25
#define REG_CHAN_AVG_LEN             26
#define REG_CHAN_PAYLOAD_LEN         27
#define REG_CHAN_BYPASS              28
#define REG_CHAN_RESET               29
#define REG_CHAN_FIRST_ENABLE        30
#define REG_CHAN_FIRST_NUM           31
#define REG_CHAN_FIRST_WAIT          32


class chan_fpga : public rwt_registers_common
{
public:
    chan_fpga(
        std::shared_ptr<base_device_common> common,
        bool debug);
    int handle_registers(
        const std::string key,
        const std::string value);
    void read_registers();
    void test_registers();
    void reset_chan();

    const char *handlers[8] = {
        "chan_bypass",
        "downselect",
        "fft_size",
        "avg_len",
        "payload_length",
        "chan_first_enable",
        "chan_first_num",
        "chan_first_wait"
    };

private:

    void set_coefficients();
    void set_downselect(const std::string value);
    void set_fft_size(uint32_t value_u32);
    void set_avg_len(uint32_t value_u32);
    void set_payload_length(uint32_t value_u32);
    void set_chan_bypass(uint32_t value_u32);
    void set_chan_first_enable(uint32_t value_u32);
    void set_chan_first_num(uint32_t value_u32);
    void set_chan_first_wait(uint32_t value_u32);

    uint32_t fft_size;
    uint32_t avg_len;
    uint32_t payload_length;
    uint32_t chan_first_num;
    uint32_t chan_first_wait;
    bool chan_bypass;
    bool chan_first_enable;

    std::vector<uint32_t> downselections;
    int down_update;
    uint32_t *coefficients;
    int reload_update;
};

}
}

#endif
