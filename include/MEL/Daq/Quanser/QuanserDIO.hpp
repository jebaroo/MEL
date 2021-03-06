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

#ifndef MEL_QUANSER_DIO_HPP
#define MEL_QUANSER_DIO_HPP

#include <MEL/Daq/InputOutput.hpp>
#include <MEL/Core/NonCopyable.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QuanserDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class MEL_API QuanserDIO : public DigitalInputOutput, NonCopyable {
public:
    QuanserDIO(QuanserDaq& daq);

    ~QuanserDIO();

    bool update() override;

    bool update_channel(uint32 channel_number) override;

    bool set_directions(const std::vector<Direction>& directions) override;

    bool set_direction(uint32 channel_number, Direction direction) override;

    bool set_expire_values(const std::vector<Logic>& expire_values) override;

    bool set_expire_value(uint32 channel_number, Logic expire_value) override;

private:

    bool on_enable() override;

    bool on_disable() override;

private:
    QuanserDaq& daq_;  ///< Reference to parent QDaq
};

}  // namespace mel

#endif  // MEL_QUANSER_DIO_HPP
