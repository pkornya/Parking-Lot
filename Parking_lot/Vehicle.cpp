#include "Vehicle.h"
#include "Car.h"
#include "Motorcycle.h"
#include "Bus.h"
#include "InvalidVehicleTypeException.h"

std::shared_ptr<Vehicle> Vehicle::makeVehicle(const int choice, const std::string& license_plate, const double parking_duration)
{
    if (choice == 1)
    {
        return std::shared_ptr<Car>(new Car(license_plate, parking_duration));
    }
    else if (choice == 2)
    {
        return std::shared_ptr<Motorcycle>(new Motorcycle(license_plate, parking_duration));
    }
    else if (choice == 3)
    {
        return std::shared_ptr<Bus>(new Bus(license_plate, parking_duration));
    }
    else
    {
        throw InvalidVehicleTypeException("Invalid vehicle type");
    }
}