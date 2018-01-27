#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Exoskeletons/MahiExoII/MahiExoII.hpp>
#include <MEL/Utility/System.hpp>
#include <vector>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/DataLog.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/RingBuffer.hpp>

using namespace mel;

static bool stop = false;
static void handler(int var) {
    stop = true;
}

int main(int argc, char *argv[]) {

    // make options
    Options options("ex_mahiexoii_q8usb.exe", "MahiExoII Q8 USB Demo");
    options.add_options()
        ("c,calibrate", "Calibrates the MAHI Exo-II")
        ("s,setpoint", "Runs the MAHI Exo-II MelScope set-point demo")
        ("h,help", "Prints this help message");

    auto result = options.parse(argc, argv);

    if (result.count("help") > 0) {
        print(options.help());
        return 0;
    }

    // register ctrl-c handler
    register_ctrl_c_handler(handler);

    // make MelShares
    MelShare ms_pos("melscope_pos");
    MelShare ms_vel("melscope_vel");
    MelShare ms_trq("melscope_trq");
    MelShare ms_sp("melscope_sp");

    // enable Windows realtime
    enable_realtime();

    // make Q8 USB and configure
    Q8Usb q8;
    q8.digital_output.set_enable_values(std::vector<logic>(8, HIGH));
    q8.digital_output.set_disable_values(std::vector<logic>(8, HIGH));
    q8.digital_output.set_expire_values(std::vector<logic>(8, HIGH));

    // create MahiExoII and bind Q8 channels to it
    std::vector<Amplifier> amplifiers;
    std::vector<double> amp_gains;
    for (uint32 i = 0; i < 2; ++i) {
        amplifiers.push_back(
            Amplifier("meii_amp_" + std::to_string(i),
                Amplifier::TtlLevel::Low,
                q8.digital_output[i + 1],
                1.8,
                q8.analog_output[i + 1])
        );
    }
    for (uint32 i = 2; i < 5; ++i) {
        amplifiers.push_back(
            Amplifier("meii_amp_" + std::to_string(i),
                Amplifier::TtlLevel::Low,
                q8.digital_output[i + 1],
                0.184,
                q8.analog_output[i + 1])
        );
    }
    MeiiConfiguration config(q8, q8.watchdog, q8.encoder[{1, 2, 3, 4, 5}], q8.velocity[{1, 2, 3, 4, 5}], amplifiers);
    MahiExoII meii(config);

    // calibrate - manually zero the encoders (right arm supinated)
    if (result.count("calibrate") > 0) {
        meii.calibrate(stop);
        return 0;
    }

    // setpoint control with MelScope
    if (result.count("setpoint") > 0) {

        // create initial setpoint and ranges
        std::vector<double> setpoint = { -35 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD,  0.10 };
        const std::vector<std::vector<double>> setpoint_ranges = { { -90 * DEG2RAD,  0 * DEG2RAD },
                                                                   { -90 * DEG2RAD, 90 * DEG2RAD },
                                                                   { -15 * DEG2RAD, 15 * DEG2RAD },
                                                                   { -15 * DEG2RAD, 15 * DEG2RAD },
                                                                   {          0.08,        0.115 } };

        // set up state machine
        uint16 state = 0;
        Time backdrive_time = seconds(3);
        //Time test_enter_time;
        //Time test_wait_time = seconds(3);

        // create data containers
        std::vector<double> rj_positions(meii.N_rj_);
        std::vector<double> rj_velocities(meii.N_rj_);
        std::vector<double> aj_positions(meii.N_aj_);
        std::vector<double> aj_velocities(meii.N_aj_);
        std::vector<double> command_torques(meii.N_aj_);
        std::vector<double> rps_command_torques(meii.N_qs_);

        // enable DAQ and exo
        q8.enable();        
        meii.enable();

        // initialize controller
        meii.set_rps_control_mode(0);

        // construct timer in hybrid mode to avoid using %100 CPU
        Timer timer(milliseconds(1), Timer::Hybrid);

        // start loop
        q8.watchdog.start();
        while (!stop) {
            
            // update all DAQ input channels
            q8.update_input();

            // update MahiExoII kinematics
            meii.update_kinematics();

            // store most recent readings from DAQ
            for (int i = 0; i < meii.N_rj_; ++i) {
                rj_positions[i] = meii[i].get_position();
                rj_velocities[i] = meii[i].get_velocity();
            }
            for (int i = 0; i < meii.N_aj_; ++i) {
                aj_positions[i] = meii.get_anatomical_joint_position(i);
                aj_velocities[i] = meii.get_anatomical_joint_velocity(i);
            }

            


            switch (state) {
            case 0: // backdrive

                // command zero torque
                meii.set_joint_torques(command_torques);

                // check for wait period to end
                if (timer.get_elapsed_time() >= backdrive_time) {
                    meii.rps_init_par_ref_.start(meii.get_wrist_parallel_positions(), timer.get_elapsed_time());
                    state = 1;
                }
                break;

            case 1: // initialize rps

                // calculate commanded torques
                rps_command_torques = meii.set_rps_pos_ctrl_torques(meii.rps_init_par_ref_, timer.get_elapsed_time());
                std::copy(rps_command_torques.begin(), rps_command_torques.end(), command_torques.begin() + 2);

                // check for RPS Initialization target reached
                if (meii.check_rps_init()) {
                    print("RPS Mechanism Initialized");
                    meii.set_rps_control_mode(2);
                    meii.anat_ref_.start(setpoint, meii.get_anatomical_joint_positions(), timer.get_elapsed_time());
                    state = 2;
                    //test_enter_time = timer.get_elapsed_time(); // testing
                }
                break;

            case 2: // setpoint control

                // read from MelShare
                setpoint = ms_sp.read_data();

                command_torques = meii.set_anat_pos_ctrl_torques(meii.anat_ref_, timer.get_elapsed_time());

                /*if (timer.get_elapsed_time() >= (test_enter_time + test_wait_time)) {
                    setpoint = { -35 * DEG2RAD, -30 * DEG2RAD, 5 * DEG2RAD, 0 * DEG2RAD,  0.10 };
                    meii.anat_ref_.set_ref(setpoint, timer.get_elapsed_time());
                }*/

                break;
            }

            // write to MelShares
            ms_pos.write_data(aj_positions);
            ms_vel.write_data(aj_velocities);
            ms_trq.write_data(command_torques);
            
            // update all DAQ output channels
            q8.update_output();

            // kick watchdog
            if (!q8.watchdog.kick() || meii.check_all_joint_limits())
                stop = true;

            // wait for remainder of sample period
            timer.wait();
            
        }
        
    }

    disable_realtime();
    return 0;
}

