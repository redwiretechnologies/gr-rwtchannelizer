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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include "rwt_source_chan_impl.h"
#include <rwt/common_tag_types.h>
#include <rwt/common_registers.h>
#include "chan_tags.h"

namespace gr {
  namespace rwt{

#define BLK_ID 0x0002
#define BLK_VER_MIN 0x0002
#define BLK_VER_MAX 0x0002

rwt_source_chan::sptr
rwt_source_chan::make(
    const pmt::pmt_t config,
    unsigned int reg_base_addr,
    const char *filter,
    bool use_tags,
    bool auto_filter,
    bool force_reload,
    unsigned int buffer_size,
    const char *phy_name,
    const char *rx_name,
    const char *tx_name)
{
  return gnuradio::get_initial_sptr
    (new rwt_source_chan_impl(
        config,
        reg_base_addr,
        filter,
        use_tags,
        auto_filter,
        force_reload,
        buffer_size,
        phy_name,
        rx_name,
        tx_name));
}

/*
 * The private constructor
 */
rwt_source_chan_impl::rwt_source_chan_impl(
    const pmt::pmt_t config,
    unsigned int reg_base_addr,
    const char *filter,
    bool use_tags,
    bool auto_filter,
    bool force_reload,
    unsigned int buffer_size,
    const char *phy_name,
    const char *rx_name,
    const char *tx_name) :
    rwt_base_block(
        "rwt_source_chan",
        gr::io_signature::make(0, 0, 0),
        gr::io_signature::make(
            1,
            1,
            sizeof(gr_complex))),
    rwt_base_block_impl(
        "rwt_source_chan",
        gr::io_signature::make(0, 0, 0),
        gr::io_signature::make(
            1,
            1,
            sizeof(gr_complex)),
        buffer_size,
        0,
        reg_base_addr,
        true,
        false,
        "default-chan",
        force_reload,
        phy_name,
        rx_name,
        tx_name),
    m_escape(0xaaaaaaaaaaaaaaaa),
    m_in_constructor(true),
    m_use_tags(use_tags),
    m_autofilter(auto_filter),
    m_samplerate(5000000),
    m_alloc_len(0),
    m_pkt_data(NULL)
{

    common_registers_rwt = new rwt_registers_common(m_common, false);
    cic_frontend = new cic_filter_fpga(m_common, 0, false, true);
    channelizer = new chan_fpga(m_common, false);

    const char *handlers[] = {
        "escape",
        "samplerate",
        "debug"
    };

    const int alignment_multiple =
        volk_get_alignment() / sizeof(short);
    set_alignment(std::max(1, alignment_multiple));
    set_output_multiple(4);

    channelizer->reset_chan();

    for (uint32_t i = 0; i < (sizeof(handlers)/sizeof(char *)); i++) {
        set_config_handler(
            handlers[i],
            std::bind(&rwt_source_chan_impl::ctrl_reg_handler, this, std::placeholders::_1, std::placeholders::_2));
    }
    for (uint32_t i = 0; i < (sizeof(common_registers_rwt->handlers)/sizeof(char *)); i++) {
        set_config_handler(
            common_registers_rwt->handlers[i],
            std::bind(&rwt_source_chan_impl::ctrl_reg_handler, this, std::placeholders::_1, std::placeholders::_2));
    }
    for (uint32_t i = 0; i < (sizeof(cic_frontend->handlers)/sizeof(char *)); i++) {
        set_config_handler(
            cic_frontend->handlers[i],
            std::bind(&rwt_source_chan_impl::ctrl_reg_handler, this, std::placeholders::_1, std::placeholders::_2));
    }
    for (uint32_t i = 0; i < (sizeof(channelizer->handlers)/sizeof(char *)); i++) {
        set_config_handler(
            channelizer->handlers[i],
            std::bind(&rwt_source_chan_impl::ctrl_reg_handler, this, std::placeholders::_1, std::placeholders::_2));
    }

    m_common->check_user_blkid(
        BLK_ID,
        BLK_VER_MIN,
        BLK_VER_MAX,
        true);

    m_time_helper.setup(m_common, m_use_tags);
    m_source->setup_tags(m_use_tags, m_escape);
    config_msg_handler(config);

    if (!m_common->setup_filter(filter, m_samplerate, m_autofilter))
        throw std::runtime_error("Unable to set filter");
    cic_frontend->reset();
    m_in_constructor = false;
}

/*
 * Our virtual destructor.
 */
rwt_source_chan_impl::~rwt_source_chan_impl()
{
    free_volk_buffers();
    free(common_registers_rwt);
    free(cic_frontend);
    free(channelizer);
}

void
rwt_source_chan_impl::set_samplerate(const std::string value)
{
    unsigned long val;
    int count;
    std::string s = "";
    val = (unsigned long)strtoul(value.c_str(), NULL, 0);

    for(count=0; count<8; count++) {
        if(val < MIN_SAMPLE_RATE) {
            val *= 10;
            if(count == 3)
                count = 5;
            s += "0";
        }
        else
            break;
    }

    if(count == 8) {
        printf("Can't set samplerate to %s!!!\n", value.c_str());
        return;
    }

    const std::string val2 = value + s;
    m_common->set_attr("samplerate", val2);
    if(debug) printf("  Set samplerate to %ld\n", val);
    cic_frontend->set_decimation(count);

    m_samplerate = val;
    if (m_autofilter && !m_in_constructor) {
        if(!m_common->setup_filter(NULL, m_samplerate, m_autofilter))
            std::cerr << "Error setting filter for AD9361.\n";
    }
}

void
rwt_source_chan_impl::ctrl_reg_handler(
    const std::string key,
    const std::string value)
{

    uint32_t value_u32;

    if (key == "samplerate") {
        set_samplerate(value);
    }else if (key == "escape") {
        m_escape = (uint64_t)strtoull(value.c_str(), NULL, 0);;
        m_source->setup_tags(m_use_tags, m_escape);
    }else if (key == "debug") {
        debug = (uint32_t)strtoul(value.c_str(), NULL, 0);
    }else if (cic_frontend->handle_registers(key, value) == 0) {
        return;
    }else if (channelizer->handle_registers(key, value) == 0) {
        return;
    } else {
        m_time_helper.ctrl_reg_handler(key, value, m_samplerate);
    }

    if(debug == 3) {
        cic_frontend->read_registers();
        channelizer->read_registers();
    }
}

pmt::pmt_t
rwt_source_chan_impl::get_time()
{
    return m_time_helper.get_time_pmt(
        time_helper::INDEX_ADC,
        m_samplerate);
}


uint64_t
rwt_source_chan_impl::get_sample_idx()
{
    return m_time_helper.get_sample_idx(time_helper::INDEX_ADC);
}


void
rwt_source_chan_impl::alloc_volk_buffers(int nitems)
{
    const unsigned int alignment = volk_get_alignment();

    free_volk_buffers();

    m_pkt_data = (short *)volk_malloc(sizeof(short) * nitems, alignment);

    if (!m_pkt_data) {
        throw std::runtime_error(
            "rwt_source_chan: Failed to allocate volk buffers");
    }

    m_alloc_len = nitems;
}


int
rwt_source_chan_impl::work(
    int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items)
{
    gr_complex *out0 = (gr_complex *) output_items[0];
    bool next_tag_valid;
    uint8_t next_tag_type;
    uint64_t next_tag_value;
    size_t max_pkt_size;
    size_t pkt_idx = 0;
    size_t pkt_read_count;
    int ret;
    gr::tag_t tag;

    (void)input_items;

    max_pkt_size = noutput_items * 2;

    if (m_alloc_len < max_pkt_size) {
        alloc_volk_buffers(max_pkt_size);
    }

    while (pkt_idx < max_pkt_size) {
        ret = m_source->read_until_next_tag_or_end(
            false,
            &m_pkt_data[pkt_idx],
            2,
            max_pkt_size - pkt_idx,
            &next_tag_valid,
            &next_tag_type,
            &next_tag_value,
            &pkt_read_count);

        if (ret == -ETIMEDOUT) {
            std::cerr << "Warning: rwt_source_chan: timed out.\n";
            break;
        } else if (ret < 0) {
            std::cerr << "Warning: iio:source Buffer refill failed.\n";
            return -1;
        }

        if (!pkt_read_count && pkt_idx) {
            /* Didn't read anything last try, but data is already available.
               So go ahead and send it. */
            break;
        }

        pkt_idx += pkt_read_count;

        if (next_tag_valid) {
            switch (next_tag_type) {
            case RWT_TAG_IDX_TIME:
                tag.key = RWT_TAG_PMT_TIME;
                break;
            case RWT_TAG_IDX_PPS:
                tag.key = RWT_TAG_PMT_PPS;
                break;
            case RWT_TAG_IDX_OVERFLOW:
                tag.key = RWT_TAG_PMT_OVERFLOW;
                break;
            case RWT_TAG_IDX_CHAN_FIRST:
                tag.key = RWT_TAG_PMT_CHAN_FIRST;
                break;
            default:
                next_tag_valid = false;
            }
        }

        if (next_tag_valid) {
            tag.offset = nitems_written(0) + pkt_idx / 2;

            tag.value = pmt::from_uint64(next_tag_value);
            tag.srcid = pmt::PMT_F;
            m_tags.push_back(tag);
        }
    }

    if (!pkt_idx)
        return 0;

    /* Convert the types from short to complex numbers.

       @todo: Is it worth creating a custom volk kernel that optimizes with
              neon intrinsics. */

	/* pkt_idx is # float samples, whereas noutput_items needs to be #
	   complex samples. */
	noutput_items = pkt_idx / 2;

	/* Short to complex.

	   This works because std::complex is a literal type. The data is
	   stored such that you can do a memcpy() from an array of floats
	   to an array of complex values. */
	volk_16i_s32f_convert_32f((float *)out0, m_pkt_data, 2048.0, pkt_idx);


    while (!m_tags.empty()) {
        tag = m_tags.front();

        if (tag.offset < (nitems_written(0) + noutput_items)) {
            add_item_tag(0, tag);

            m_tags.pop_front();
        } else {
            /* The list is sorted by offset, so when a tag past what's been
               sent we are done.*/
            break;
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}


bool
rwt_source_chan_impl::start()
{
    bool ret;

    ret = rwt_base_block::start();
    if (!ret)
        return ret;

    ret = m_source->start(true, false);
    if (!ret)
        return ret;

    return ret;
}


bool
rwt_source_chan_impl::stop()
{
    m_source->stop();
    rwt_base_block::stop();
    free_volk_buffers();
    return true;
}

} /* namespace rwt */
} /* namespace gr */
