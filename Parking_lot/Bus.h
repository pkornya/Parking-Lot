#pragma once

#include "Vehicle.h"

/// \brief Class for vehicle type Bus
class Bus : public Vehicle
{
public:
    Bus(const std::string & license_plate, double parking_duration)
        : Vehicle(license_plate, parking_duration) {}

    std::string getVehicleType() const override { return "Bus"; }
};