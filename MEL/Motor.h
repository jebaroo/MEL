#pragma once
#include "Actuator.h"
#include "Integrator.h"
#include "Clock.h"

namespace mel {

    class Motor : public Actuator {

    public:

        //---------------------------------------------------------------------
        // CONSTRUCTOR(S) / DESTRUCTOR(S)
        //---------------------------------------------------------------------

        /*
        /// Default constructor
        Motor();

        /// Constructor for motor with DO enable/disable channel
        Motor(std::string name, double kt, double amp_gain, 
            Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode);
        /// Constructor for hard current limitied motor with DO enable/disable channel
        Motor(std::string name, double kt, double amp_gain, 
            Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode,
            double hard_current_limit);
        /// Constructor for I2T current limited motor with DO enable/disable channels
        Motor(std::string name, double kt, double amp_gain,
            Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode,
            double continuous_current_limit, double peak_current_limit, double i2t_time);

        /// Constructor for motor with DO enable/disable channel and current sensing
        Motor(std::string name, double kt, double amp_gain, 
            Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode, Daq::Ai ai_channel);
        /// Constructor for hard current limitied motor with DO enable/disable channel and current sensing
        Motor(std::string name, double kt, double amp_gain, 
            Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode, Daq::Ai ai_channel,
            double hard_current_limit);

            */
        /// Constructor for I2T current limitied motor with DO enable/disable channel and current sensing
        Motor(std::string name, double kt, double amp_gain,
            Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode, Daq::Ai ai_channel,
            double continuous_current_limit, double peak_current_limit, double i2t_time);

        //---------------------------------------------------------------------
        // PUBLIC FUNCTIONS
        //---------------------------------------------------------------------

        /// Enables the Motor using the associated digital output channel
        void enable() override;
        /// Disables the Motor using using the associated digital output channel and writes zero to the associated analog output channel
        void disable() override;


        /// Set the desired current to be generated at the Motor, calls limit_current(), converts from current to amplifier voltage command, and sets voltage on associated analog output channel
        void set_current(double new_current);        
        /// Sets the desired torque to be generated at the Motor, converts from torque to current, and calls set_current()
        void set_torque(double new_torque) override; 

        /// Returns the commanded current resulting from the commanded current
        double get_current_command();
        /// Returns the limited current resulting from the current limit mode operation
        double get_current_limited();
        /// Gets the current measured by the current sensor of the current amplifier
        double get_current_sense();

        /// Returns the attempted command current since the last call to set_current()
        double get_torque_command() override;
        /// Returns the limited command current since the last call to set_current()
        double get_torque_limited() override;

        
        //---------------------------------------------------------------------
        // PUBLIC VARIABLES
        //---------------------------------------------------------------------

        const double kt_; ///< torque constant of the Motor
        const double amp_gain_; ///< the conversion rate between voltage sent to the analog output channel and the current generated by the amplifier [A/V]
        
        double continuous_current_limit_;
        double peak_current_limit_;
        double i2t_time_;   

        enum class CurrentLimitMode { None, Saturate, I2T };
        CurrentLimitMode current_limit_mode_; ///< the current limiting mode

    private:      


        //---------------------------------------------------------------------
        // PRIVATE FUNCTIONS
        //---------------------------------------------------------------------   

        void limit_current_saturate();
        void limit_current_i2t();

        //---------------------------------------------------------------------
        // PRIVATE VARIABLES
        //---------------------------------------------------------------------   

        double i2t_integrand_;
        double i2t_integral_;
        double i2t_time_now_;
        double i2t_time_last_;
        
        double current_; ///< stores the desired Motor current since the last call to set_current()
        double limited_current_;
        bool has_current_limit_; ///< whether or not the Motor should enforce a current limit
       
        double current_sense_; ///< store the measured current since the last call to get_current_sense()       
        bool has_current_sense_; ///< whether or not the Motor comes with current sensing, i.e., whether or not the ai_channel is provided 

        Daq::Ao ao_channel_; ///< the DAQ analog output channel bound to this Motor (for commanding torque/current)
        Daq::Do do_channel_; ///< the DAQ digital output channel bound to this Motor (for enable/disable)
        Daq::Ai ai_channel_; ///< the DAQ analog input channel bound to this Motor (for current sensing)

    };
}