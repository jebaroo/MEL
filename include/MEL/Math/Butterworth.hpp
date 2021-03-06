// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_BUTTERWORTH_HPP
#define MEL_BUTTERWORTH_HPP

#include <MEL/Config.hpp>
#include <MEL/Math/Filter.hpp>
#include <MEL/Core/Frequency.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// A digital Butterworth filter implementation
class MEL_API Butterworth : public Filter {
public:
    enum Type {
        Lowpass,  ///< Lowpass filter
        Highpass  ///< Highpass filter
    };

public:
    /// Designs an n-th order lowpass or highpass digital Butterworth filter
    /// with normalized cutoff frequency Wn
    Butterworth(std::size_t n,
                double Wn,
                Type type      = Lowpass,
                uint32 seeding = 0);

    /// Designs an n-th order lowpass or highpass digital Butterworth filter
    /// with specified cutoff and sample frequencies
    Butterworth(std::size_t n,
                Frequency cutoff,
                Frequency sample,
                Type type      = Lowpass,
                uint32 seeding = 0);
};

};  // namespace mel

#endif  // MEL_BUTTERWORTH_HPP
