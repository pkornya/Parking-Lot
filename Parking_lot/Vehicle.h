#pragma once
#include <string>
#include <memory>

/// \brief Abstract base class for all vehicle types
class Vehicle
{
public:
    /// Constructor
    /// \param[in] license_plate Unique license plate of the vehicle
    /// \param[in] parking_duration The dureation of the parking
    Vehicle(const std::string& license_plate, const double parking_duration)
        : m_license_plate(license_plate), m_parking_duration(parking_duration) {}

    /// Destructor
    virtual ~Vehicle() {}

    /// Get the license plate of the vehicle.
    /// \return Returns license plate as a string.
    virtual std::string getLicensePlate() const { return m_license_plate; }

    /// Abstract method to get the type of the vehicle
    /// \return Returns vehicle type as a string
    virtual std::string getVehicleType() const = 0;

    /// Get the parking duration of the vehicle.
    /// \return Returns parking duration in hours as a double.
    virtual double getParkingDuration() const { return m_parking_duration; }

    /// Creates certain vehicle according to the first argument
    /// \param[in] choice Could have value from 1 to 3 and according to this value certain vehicle is created
    /// \param[in] license_plate Unique license plate of the vehicle
    /// \param[in] parking_duration Parking duration of the vehicle
    /// \return Returns shared_ptr object of that point to a certain derived class
    /// \throw Throws InvalidVehicleTypeException exception in case first argument is invalid
    static std::shared_ptr<Vehicle> makeVehicle(const int choice, const std::string& license_plate, const double parking_duration);
private:
    std::string m_license_plate;
    double m_parking_duration;
};