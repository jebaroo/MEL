#pragma once
#include <MEL/Core/Device.hpp>
#include <MEL/Daq/InputModule.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class EmgElectrode : public Device {

public:

    /// Default constructor
    EmgElectrode(std::string name, InputModule<voltage>::Channel ai_channel);

    /// Gets the voltage measured by the EMG electrode
    double get_voltage();

private:

    InputModule<voltage>::Channel ai_channel_; ///< the DAQ analog input channel bound to this electrode
    double voltage_;

};

} // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
