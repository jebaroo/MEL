#pragma once

#include <MEL/Daq/InputModule.hpp>
#include <MEL/Utility/NonCopyable.hpp>


namespace mel {

    //==============================================================================
    // FORWARD DECLARATIONS
    //==============================================================================

    class QDaq;

    //==============================================================================
    // CLASS DECLARATION
    //==============================================================================

    class QDigitalInput : public InputModule<logic>, NonCopyable {

    public:

        QDigitalInput(QDaq& daq, const std::vector<uint32>& channel_numbers);

        ~QDigitalInput();

        bool enable() override;

        bool disable() override;

        bool update() override;

        bool update_channel(uint32 channel_number) override;

    private:

        QDaq& daq_;  ///< Reference to parent QDaq

    };

} // namespace mel

  //==============================================================================
  // CLASS DOCUMENTATION
  //==============================================================================