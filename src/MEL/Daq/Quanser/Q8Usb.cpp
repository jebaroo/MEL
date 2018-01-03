#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/System.hpp>
#include <hil.h>

namespace mel {

//==============================================================================
// STATIC MEMBER VARIABLES
//==============================================================================

 uint32 Q8Usb::next_id_ = 0;

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Q8Usb::Q8Usb(uint32 id, QOptions options) :
    Q8Usb(std::vector<uint32>{0, 1, 2, 3, 4, 5, 6, 7},
          std::vector<uint32>{0, 1, 2, 3, 4, 5, 6, 7},
          std::vector<uint32>{0, 1, 2, 3, 4, 5, 6, 7},
          std::vector<uint32>{0, 1, 2, 3, 4, 5, 6, 7},
          std::vector<uint32>{0, 1, 2, 3, 4, 5, 6, 7},
          id,
          options)
{
}

Q8Usb::Q8Usb(std::vector<uint32> ai_channels,
             std::vector<uint32> ao_channels,
             std::vector<uint32> di_channels,
             std::vector<uint32> do_channels,
             std::vector<uint32> enc_channels,
             uint32 id,
             QOptions options) :
    QDaq("q8_usb", id, options),
    analog_input(*this, ai_channels),
    analog_output(*this, ao_channels),
    digital_input(*this, di_channels),
    digital_output(*this, do_channels),
    encoder(*this, enc_channels),
    velocity(*this, enc_channels),
    watchdog(*this, milliseconds(100))
{
    ++next_id_;
}

Q8Usb::~Q8Usb() {
    if (enabled_)
        disable();
    if (open_)
        close();
    --next_id_;
}

bool Q8Usb::enable() {
    // open if not already
    if (!open_)
        if (!open())
            return false;
    print("Enabling " + namify(name_) + " ... ");
    // clear watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();    
    // set options
    if (!set_options())
        return false;
    // enable each module
    if (!analog_input.enable())
        return false;
    if (!analog_output.enable())
        return false;
    if (!digital_input.enable())
        return false;
    if (!digital_output.enable())
        return false;
    if (!encoder.enable())
        return false;
    if (!velocity.enable())
        return false;
    // allow changes to take effect
    sleep(milliseconds(10));
    return Device::enable();
}

bool Q8Usb::disable() {
    print("Disabling " + namify(name_) + " ... ");
    // disable each module
    if (!analog_input.disable())
        return false;
    if (!analog_output.disable())
        return false;
    if (!digital_input.disable())
        return false;
    if (!digital_output.disable())
        return false;
    if (!encoder.disable())
        return false;
    if (!velocity.disable())
        return false;
    // stop watchdog (precautionary, ok if fails)
    watchdog.stop();
    // clear the watchdog (precautionary, ok if fails)
    watchdog.clear();
    // allow changes to take effect
    sleep(milliseconds(10));
    return Device::disable();
}

bool Q8Usb::reset() {
    return false;
}

bool Q8Usb::update_input() {
    if (open_) {
        t_error result;
        result = hil_read(handle_,
            analog_input.get_channel_count() > 0 ? &(analog_input.get_channel_numbers())[0] : NULL, 
            static_cast<uint32>(analog_input.get_channel_count()),
            encoder.get_channel_count() > 0 ? &(encoder.get_channel_numbers())[0] : NULL, 
            static_cast<uint32>(encoder.get_channel_count()),
            digital_input.get_channel_count() > 0 ? &(digital_input.get_channel_numbers())[0] : NULL, 
            static_cast<uint32>(digital_input.get_channel_count()),
            velocity.get_channel_count() > 0 ? &(velocity.get_converted_channel_numbers())[0] : NULL, 
            static_cast<uint32>(velocity.get_channel_count()),
            analog_input.get_channel_count() > 0 ? &(analog_input.get_values())[0] : NULL,
            encoder.get_channel_count() > 0 ? &(encoder.get_values())[0] : NULL,
            digital_input.get_channel_count() > 0 ? &(digital_input.get_values())[0] : NULL,
            velocity.get_channel_count() > 0 ? &(velocity.get_values())[0] : NULL);
        if (result == 0)
            return true;
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

bool Q8Usb::update_output() {
    if (open_) {
        t_error result;
        result = hil_write(handle_,
            analog_output.get_channel_count() > 0 ? &(analog_output.get_channel_numbers())[0] : NULL, 
            static_cast<uint32>(analog_output.get_channel_count()),
            NULL, 0,
            digital_output.get_channel_count() > 0 ? &(digital_output.get_channel_numbers())[0] : NULL, 
            static_cast<uint32>(digital_output.get_channel_count()),
            NULL, 0,
            analog_output.get_channel_count() > 0 ? &(analog_output.get_values())[0] : NULL,
            NULL,
            digital_output.get_channel_count() > 0 ? &(digital_output.get_values())[0] : NULL,
            NULL);
        if (result == 0)
            return true;
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print(namify(get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

bool Q8Usb::identify(uint32 channel_number) {
    if (open_) {
        InputModule<logic>::Channel di_ch = digital_input.get_channel(channel_number);
        OutputModule<logic>::Channel do_ch = digital_output.get_channel(channel_number);
        bool loopback_detected = true;
        for (int i = 0; i < 5; ++i) {
            do_ch.set_value(HIGH);
            do_ch.update();
            sleep(milliseconds(10));
            di_ch.update();
            if (di_ch.get_value() != HIGH) {
                return false;
            }
            do_ch.set_value(LOW);
            do_ch.update();
            sleep(milliseconds(10));
            di_ch.update();
            if (di_ch.get_value() != LOW) {
                return false;
            }
        }        
        return true;
    }
    else {
        print(namify(name_) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

int Q8Usb::identify() {
    for (uint32 channel_number = 0; channel_number < 8; ++channel_number) {
        if (identify(channel_number))
            return (int)channel_number;
    }
    return -1;
}

std::size_t Q8Usb::get_q8_usb_count() {
    return QDaq::get_qdaq_count("q8_usb");
}

} // namespace mel

