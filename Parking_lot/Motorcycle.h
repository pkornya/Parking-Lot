#pragma once

#include "Vehicle.h"

/// \brief Class for vehicle type Motorcycle
class Motorcycle : public Vehicle
{
public:
    Motorcycle(const std::string& license_plate, double parking_duration)
        : Vehicle(license_plate, parking_duration) {}

    std::string getVehicleType() const override { return "Motorcycle"; }
};