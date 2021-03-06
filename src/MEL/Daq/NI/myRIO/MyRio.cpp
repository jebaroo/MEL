#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include "Detail/MyRio.h"
#include <MEL/Logging/Log.hpp>

namespace mel {

//==============================================================================
// CONNECTOR
//==============================================================================

MyRio::Connector::Connector(MyRio& myrio,
    MyRioConnectorType type,
    const std::vector<uint32>& ai_channels,
    const std::vector<uint32>& ao_channels,
    const std::vector<uint32>& dio_channels) :
    Device("myrio_connector_" + std::to_string(type)),
    AI(myrio, type, ai_channels),
    AO(myrio, type, ao_channels),
    DIO(myrio, type, dio_channels)
{}

bool MyRio::Connector::on_enable() {
    if (AI.enable() && AO.enable() && DIO.enable())
        return true;
    else
        return false;
}

bool MyRio::Connector::on_disable() {
    if (AI.disable() && AO.disable() && DIO.disable())
        return true;
    else
        return false;
}

bool MyRio::Connector::update_input() {
    return AI.update() && DIO.update();
}

bool MyRio::Connector::update_output() {
    return AO.update() && DIO.update();
}

//==============================================================================
// MYRIO
//==============================================================================

MyRio::MyRio(const std::string& name) :
    DaqBase(name),
    A(*this, MyRioConnectorType::MxpA, {0,1,2,3}, {0,1}, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}),
    B(*this, MyRioConnectorType::MxpB, {0,1,2,3}, {0,1}, {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}),
    C(*this, MyRioConnectorType::MspC, {0,1},     {0,1}, {0,1,2,3,4,5,6,7})
{

}

MyRio::~MyRio() {
    if (is_enabled())
        disable();
    if (is_open())
        close();
}

bool MyRio::on_open() {
    NiFpga_Status status = MyRio_Open();
    if (MyRio_IsNotSuccess(status)) {
        return false;
    }
    return true;
}

bool MyRio::on_close() {
    NiFpga_Status status = MyRio_Close();
    if (MyRio_IsNotSuccess(status)) {
        return false;
    }
    return true;
}

bool MyRio::on_enable() {
    if (!is_open()) {
        LOG(Error) << "Unable to enable myRIO " << get_name() << " because it is not open";
        return false;
    }
    // enable each connector
    if (A.enable() && B.enable() && C.enable()) {
        sleep(milliseconds(10));
        return true;
    }
    else
        return false;
}

bool MyRio::on_disable() {
    if (!is_open()) {
        LOG(Error) << "Unable to disable myRIO " << get_name() << " because it is not open";
        return false;
    }
    // disable each connect
    if (A.disable() && B.disable() && C.disable()) {
        sleep(milliseconds(10));
        return true;
    }
    else
        return false;
}

bool MyRio::update_input() {
    return (A.update_input() && B.update_input() && C.update_input());
}

bool MyRio::update_output() {
    return (A.update_output() && B.update_output() && C.update_output());
}

}  // namespace mel
