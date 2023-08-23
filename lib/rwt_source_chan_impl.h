/* -*- c++ -*- */
/*
 * Copyright 2022,2023 Red Wire Technologies.
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

#ifndef INCLUDED_RWTCHANNELIZER_RWT_SOURCE_CHAN_IMPL_H
#define INCLUDED_RWTCHANNELIZER_RWT_SOURCE_CHAN_IMPL_H

#include <list>

#include <gnuradio/rwtchannelizer/rwt_source_chan.h>
#include <rwt/rwt_base_block_impl.h>
#include <rwt/rwt_registers_common.h>
#include <rwt/cic_filter_fpga.h>
#include <rwt/time_helper.h>
#include "chan_fpga.h"


namespace gr {
  namespace rwt {


class rwt_source_chan_impl : public rwt_source_chan, public rwt_base_block_impl
{
public:
    rwt_source_chan_impl(
        const pmt::pmt_t config,
        unsigned int reg_base_addr,
        const char *filter,
        bool use_tags,
        bool auto_filter,
        bool force_reload,
        unsigned int buffer_size,
        const char *phy_name,
        const char *rx_name,
        const char *tx_name);

    ~rwt_source_chan_impl();

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items);

    void ctrl_reg_handler(
        const std::string key,
        const std::string value);

	void set_samplerate(const std::string value);

    bool start();
    bool stop();

    pmt::pmt_t get_time();
    uint64_t get_sample_idx();

private:
    void alloc_volk_buffers(int nitems);

    void free_volk_buffers() {
        if (m_pkt_data) volk_free(m_pkt_data);
        m_pkt_data = NULL;
        m_alloc_len = 0;
    }

    uint64_t m_escape;
    bool m_in_constructor;
    bool m_use_tags;
    bool m_autofilter;
    unsigned long m_samplerate;
    int debug = 0;

    size_t m_alloc_len;
    short *m_pkt_data;

    time_helper m_time_helper;

    rwt_registers_common *common_registers_rwt;
    cic_filter_fpga *cic_frontend;
    chan_fpga *channelizer;

    std::list< gr::tag_t > m_tags;

};

} // namespace rwt
} // namespace gr

#endif /* INCLUDED_RWTCHANNELIZER_RWT_SOURCE_CHAN_IMPL_H */
