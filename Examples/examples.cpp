#include <iostream>
#include <boost/program_options.hpp>
#include "mel_util.h"
#include "MelShare.h"
#include "Clock.h"
#include "Integrator.h"
#include "ExternalApp.h"
#include "Input.h"
#include <cmath>
#include <random>
#include "mel_math.h"
#include "Q8Usb.h"

// This is the MEL Examples program. It is divided in sections by comment headers.
// With the exception of PROGRAM OPTIONS, each section is self contained and 
// does not require any variables from other sections. To run a particular secition,
// use the appropriate program option argument when running the exe from the 
// command line.

int main(int argc, char * argv[]) {

    //-------------------------------------------------------------------------
    // PROGRAM OPTIONS:    >Examples.exe --help
    //-------------------------------------------------------------------------

    // use boost set up your program options 
    // this allows us to pass arguments through the command line like
    boost::program_options::options_description desc("Available Options");
    desc.add_options()
        ("help", "produces help message")
        ("melshare-data", "example demonstrating how to use MELShare for two way data sharing with C# or Python")
        ("melshare-msg", "example demonstrating how to send and recieve string messages over MELShare")
        ("melscope", "another MELShare demo that produces test data for also introducing in MELScope")
        ("external", "example of how to launch an external app or game from C++")
        ("q8", "example demonstrating how to set up a Q8 USB and becnhmark it's read/write speed")
        ("clock", "clock testing");

    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), var_map);
    boost::program_options::notify(var_map);

    // running "Examples.exe --help" at the command line will produce a nicely
    // formatted list of program options
    if (var_map.count("help")) {
        mel::util::print(desc);
        return 0;
    }

    //-------------------------------------------------------------------------
    // MELSHARE DATA EXAMPLE:    >Examples.exe --melshare
    //-------------------------------------------------------------------------

    if (var_map.count("melshare-data")) {

        // create shared memory maps (optionally define size in bytes, default = 256 bytes)
        // note that on Windows, the shared memory maps will only stay active for as long as
        // these instances stay in scope, so create them inside of your main loop or inside of a
        // class isntance that you expect to stay alive for the duration of the program (e.g. OpenWrist or ME-II).
        mel::comm::MelShare map0("map0");
        mel::comm::MelShare map1("map1");
        mel::comm::MelShare map2("map2", 80); // 10 doubles * 8 bytes/double

        // create new data containers to write to map (vectors, STL arrays, and C-style arrays can be used)
        std::vector<char> my_chars = { 'a','b','c' }; // STL vector
        std::array<int, 5> my_ints = { 1,1,2,3,5 };    // STL array
        double my_doubles[10] = { 0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9 }; // C-style array

        std::cout << "Press ENTER to write values.";
        getchar();

        // write the data to the maps
        map0.write(my_chars); // non-static version called using dot operator on a MelShare instance (fastest method)
        mel::comm::write_map("map1", my_ints); // alternately, static version that can be called anywhere if you know the map name (slightly slower)
        map2.write(my_doubles, 10); // C-style arrays can be used with both non-static and static versions, but you must input size manually

        mel::util::print("Wrote: ");
        std::cout << "map0:    ";  mel::util::print(my_chars);
        std::cout << "map1:    ";  mel::util::print(my_ints);
        std::cout << "map2:    ";  mel::util::print(my_doubles, 10, true);

        std::cout << "Run Python or C# code, then press ENTER to receive new values.";
        getchar();

        // Data in a map can change type and size, so long as the data stored does not exceed the initial bytes
        // size. The only caveat is that corresponding writer-reader arguement types must match, e.g. if you
        // last wrote a double type vector/array to a map, you must read the map back into a double type vector/array
        // for any meaningful interpretation.

        // Here we expect the size of my_ints to increase by two (see Python or C#). If you require this sort of behavior, 
        // consider using a single vector instead of the multiple arrays like this example shows.
        std::array<int, 7> my_ints_bigger{ 0,1,0,0,0,0,0 };

        // read the altered data from the maps
        map0.read(my_chars);
        map1.read(my_doubles, 10); // this demonstrates that we can read doubles from a map that previously wrote ints
        mel::comm::read_map("map2", my_ints_bigger); // and vice-versa

        mel::util::print("Read: ");
        std::cout << "map0:    ";  mel::util::print(my_chars);
        std::cout << "map1:    ";  mel::util::print(my_doubles, 10, true);
        std::cout << "map2:    ";  mel::util::print(my_ints_bigger);

        return 0;
    }

    //-------------------------------------------------------------------------
    // MELSHARE MESSAGING EXAMPLE:    >Examples-exe --melshare-msg
    //-------------------------------------------------------------------------

    if (var_map.count("melshare-msg")) {
        // create a MELShare
        mel::comm::MelShare messenger("messenger");
        messenger.write_message("this is a message being sent to unity");
        getchar();
        mel::util::print(messenger.read_message());
    }

    //-------------------------------------------------------------------------
    // MELSCOPE EXAMPLE:    >Examples.exe --melscope
    //-------------------------------------------------------------------------

    if (var_map.count("melscope")) {

        // **Open example.scope in MELScope to see this example in action**

        // Create our MELShare maps. We will make one map, "integrals", to demonstate
        // MEL's Integrator class. We will also make three other maps, "cpp2py", 
        // "py2cpp_ampl", and "py2cpp_freq" to deomstate bidirectional communication in 
        // MELShare. While bidirectional communcation over a single MELShare map is possible 
        // with the read_write() functions, it can be tricky to get the timing right. 
        // It's best to just keep two maps open for this purpose.
        mel::comm::MelShare integrals("integrals");
        mel::comm::MelShare cpp2py("cpp2py");
        mel::comm::MelShare py2cpp_ampl("py2cpp_ampl");
        mel::comm::MelShare py2cpp_freq("py2cpp_freq");

        // create the data buffers for each map
        std::array<double, 4> integrals_data = { 0, 0, 0, 0 };
        std::array<double, 4> cpp2py_data = { 0, 0, 0, 0 };
        std::array<double, 4> py2cpp_ampl_data = { 10, 8, 6, 4 };
        std::array<double, 4> py2cpp_freq_data = { 0.4, 0.6, 0.8, 1.0 };

        // create a MEL Integrator for doing some integration. note the initial value 5
        mel::math::Integrator integrator = mel::math::Integrator(5, mel::math::Integrator::Technique::Trapezoidal);

        // create a MEL Clock to control the rate of our data generating loop
        mel::util::Clock clock(1000); // 1000 Hz clock

        // In this example, we will have the data in cpp2py be a funcion of the data
        // from py2cpp. Specifically, we will generate some periodic data, and use
        // scalers from py2cpp to change the amplitudes and frequencies. For that 
        // reason, we will start off by actually writing py2cpp so that it has some 
        // intial values. 
        py2cpp_ampl.write(py2cpp_ampl_data);
        py2cpp_freq.write(py2cpp_freq_data);

        // start the clock
        clock.start();

        // run the data loop for 60 seconds
        while (clock.time() < 60) {

            // generate new data for integrals
            integrals_data[0] = clock.time();                                          //     t
            integrals_data[1] = 0.5 * (cos(clock.time()) + 7 * cos(7 * clock.time())); // dx/dt = 1/2 * [ cos(t) + 7 * cos(7t) ] 
            integrals_data[2] = sin(4 * clock.time()) * cos(3 * clock.time()) + 5;     //     x = sin(4t) * cos(3t) + 5
            integrals_data[3] = integrator.integrate(integrals_data[1], clock.time()); //     x ~ integrate(dx/dt)

            // write integrals
            integrals.write(integrals_data);
            
            // read data from py2cpp_ampl and py2cpp_freq
            py2cpp_ampl.read(py2cpp_ampl_data);
            py2cpp_freq.read(py2cpp_freq_data);

            // generate new data for cpp2py
            cpp2py_data[0] = mel::math::sin_wave(py2cpp_ampl_data[0], py2cpp_freq_data[0], clock.time());
            cpp2py_data[1] = mel::math::square_wave(py2cpp_ampl_data[1], py2cpp_freq_data[1], clock.time());;
            cpp2py_data[2] = mel::math::triangle_wave(py2cpp_ampl_data[2], py2cpp_freq_data[2], clock.time());
            cpp2py_data[3] = mel::math::sawtooth_wave(py2cpp_ampl_data[3], py2cpp_freq_data[3], clock.time());

            // write cpp2py
            cpp2py.write(cpp2py_data);

            // wait the clock
            clock.wait();
        }
    }

    //-------------------------------------------------------------------------
    // EXTERNAL APPLICATION EXAMPLE:    >Examples.exe --external
    //-------------------------------------------------------------------------

    if (var_map.count("external")) {
        mel::util::ExternalApp my_app("my_python_shell", "C:\\dev\\Python27\\python.exe");
        my_app.launch();
    }

    //-------------------------------------------------------------------------
    // Q8 USB EXAMPLE:    >Examples.exe --q8
    //-------------------------------------------------------------------------

    if (var_map.count("q8")) {

        mel::uint32 id = 0;

        mel::channel_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
        mel::channel_vec  ao_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
        mel::channel_vec  di_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
        mel::channel_vec  do_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
        mel::channel_vec enc_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };

        mel::core::Daq* q8 = new mel::dev::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels);

        mel::util::print("Press ENTER to enable this Q8 USB", false);
        getchar();
        q8->enable();

        mel::util::print("Press ENTER to benchmark this Q8 USB", false);
        getchar();
        q8->benchmark(1000000);

        q8->disable();
    }

    if (var_map.count("clock")) {

        std::chrono::high_resolution_clock::time_point start, stop;

        mel::util::enable_realtime();
       
        for (int i = 0; i < 1000; i++) {
            mel::util::Clock clock(1000);
            clock.start();
            start = std::chrono::high_resolution_clock::now();
            clock.wait();
            stop = std::chrono::high_resolution_clock::now();
            auto duration = stop - start;
            mel::util::print(duration.count() * 1.0 / 1000000000.0);
        }

    }
}


