/*
 * Copyright 2022 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(rwt_source_chan.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(730bae76ffd628ae613de0ea919cd692)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/rwtchannelizer/rwt_source_chan.h>
// pydoc.h is automatically generated in the build directory
#include <rwt_source_chan_pydoc.h>

void bind_rwt_source_chan(py::module& m)
{

    using rwt_source_chan    = gr::rwt::rwt_source_chan;


    py::class_<rwt_source_chan, gr::sync_block, gr::block, gr::basic_block,
        std::shared_ptr<rwt_source_chan>>(m, "rwt_source_chan", D(rwt_source_chan))

        .def(py::init(&rwt_source_chan::make),
           D(rwt_source_chan,make)
        )
        



        ;




}








