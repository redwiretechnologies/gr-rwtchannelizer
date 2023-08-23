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

#ifndef INCLUDED_TAPS_H
#define INCLUDED_TAPS_H

#include <stdint.h>

#define LEN_COEFF 4096

// Taps for FFT Size 8
extern uint32_t taps_8[4096];

// Taps for FFT Size 16
extern uint32_t taps_16[4096];

// Taps for FFT Size 32
extern uint32_t taps_32[4096];

// Taps for FFT Size 64
extern uint32_t taps_64[4096];

// Taps for FFT Size 128
extern uint32_t taps_128[4096];

#endif