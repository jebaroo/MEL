#pragma once

#include <MEL/Utility/Types.hpp>
#include <string>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Utility class for creating Quanser configuration strings
class QOptions {

public:

    enum UpdateRate {
        Normal = 1, ///< 1 kHz
        Fast = 2    ///< 8 kHz
    };

    enum EncoderDirection {
        Nonreversed = 0,
        Reversed = 1
    };

    enum EncoderFilter {
        Unfiltered = 0,
        Filtered = 1
    };

    enum EncoderDetection {
        High = 0,
        Low = 1
    };

    enum EncoderReload {
        NoReload = 0,
        OnPulse = 1
    };

    enum AoMode {
        VoltageMode = 0,
        CurrentMode2 = 1,
        CurrentMode1 = 2,
        CurrentMode0 = 3,
        ControlMode2 = 4,
        ControlMode1 = 5,
        ControlMode0 = 6
    };

public:

    QOptions();

    void set_update_rate(UpdateRate update_rate);

    void set_decimation(uint32 decimation);

    void set_encoder_direction(uint32 channel_number, EncoderDirection direction);

    void set_encoder_filter(uint32 channel_number, EncoderFilter filter);

    void set_encoder_detection_a(uint32 channel_number, EncoderDetection detection);

    void set_encoder_detection_b(uint32 channel_number, EncoderDetection detection);

    void set_encoder_detection_z(uint32 channel_number, EncoderDetection detection);

    void set_encoder_reload(uint32 channel_number, EncoderReload reload);

    void set_encoder_velocity(uint32 channel_number, double velocity);

    void set_analog_output_mode(uint32 channel_number, AoMode mode, double kff,
        double a0, double a1, double a2, double b0, double b1, double post);

    void set_special_option(std::string option);

    std::string get_string();

private:

    UpdateRate update_rate_;  ///< update rate
    uint32 decimation_;       ///< decimation rate
    std::string options_;     ///< the configuration options string

};

} // namespace mel

//==============================================================================
// CLASS DECLARATION
//==============================================================================

// http://www.quanser.com/Products/quarc/documentation/q8_usb.html
