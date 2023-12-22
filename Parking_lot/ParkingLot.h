#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>

#include "Vehicle.h"

/// \brief Singleton class representing a parking lot
/// The Singleton pattern ensures that there is only one instance of the ParkingLot class throughout the application.
/// This is important for managing a centralized parking lot system where all vehicles are parked and released from a single location.
/// The Singleton pattern ensures that all threads and parts of the program access the same parking lot instance, preventing inconsistencies and resource conflicts.
class ParkingLot
{
public:
    /// \brief Get the instance of the ParkingLot (Singleton)
    /// \param[in] car_capacity Capacity of the vehicles of type Car
    /// \param[in] motorcycle_capacity Capacity of the vehicles of type Motorcycle
    /// \param[in] bus_capacity Capacity of the vehicles of type Bus
    /// \return Returns shared pointer to the ParkingLot instance
    static std::shared_ptr<ParkingLot> getInstance(int car_capacity = 10, int motorcycle_capacity = 15, int bus_capacity = 5);

    /// \brief Parks a vehicle in the parking lot
    /// \param[in] vehicle Shared pointer to a certain vehicle to park
    /// \return Returns true if the vehicle was successfully parked, false otherwise.
    /// \throw Throws ParkingLotFullException if the parking lot is full for the vehicle type
    bool parkVehicle(const std::shared_ptr<Vehicle> & vehicle);

    /// \brief Releases a vehicle from the parking lot by ticket ID
    /// \param[in] ticket_id Ticket ID of the vehicle to be released
    /// \return Returns true if the vehicle was successfully released, false otherwise.
    /// \throw Throws VehicleNotFoundException if the vehicle with the given ticket ID is not found.
    bool releaseVehicleByTicketID(const int ticket_id);

    /// \brief Releases a vehicle from the parking lot by license plate
    /// \param[in] license_plate License plate of the vehicle to be released
    /// \return Returns true if the vehicle was successfully released, false otherwise.
    /// \throw Throws VehicleNotFoundException if the vehicle with the given ticket ID is not found.
    bool releaseVehicleByLicensePlate(const std::string & license_plate);

    /// \brief Query and print the available parking slots for Cars
    void queryAvailableCarsSlots();

    /// \brief Query and print the available parking slots for CMotorcycles
    void queryAvailableMotorcyclesSlots();

    /// \brief Query and print the available parking slots for Buses
    void queryAvailableBusesSlots();

    /// \brief Serches for ticket ID by License Plate
    /// \param[in] license_plate License plate of the vehicle to be released
    /// \return Returns ticket ID as int
    /// \throw Throws VehicleNotFoundException if license plate is not found
    int getTicketIDByLicensePlate(const std::string & license_plate);

private:
    /// \brief Constructor with default values
    ParkingLot();

    /// \brief Constructor
    /// \param[in] car_capacity Capacity of the cars
    /// \param[in] motorcycle_capacity Capacity of the motorcycles
    /// \param[in] bus_capacity Capacity of the buses
    ParkingLot(const int car_capacity, const int motorcycle_capacity, const int bus_capacity);

    /// \brief Checks if parking is full for a specific vehicle type
    /// \param[in] vehicle_type Cpecific vehicle type (Car, Motorcycle, Bus)
    /// \return Returns True if parking is full for the given vehicle type, false otherwise
    /// \throw Throws InvalidVehicleTypeException if an invalid vehicle type is provided
    bool isParkingFull(const std::string & vehicle_type);

    /// \brief Calculates the parking charge for a vehicle
    /// \param[in] vehicle Shared pointer to the Vehicle for which to calculate the charge
    /// \return Returns a parking charge as a double
    /// \throw Throws InvalidVehicleTypeException if an invalid vehicle type is encountered
    double calculateCharge(const std::shared_ptr<Vehicle> & vehicle);

    /// \brief Logs a vehicle entry or exit in the parking lot
    /// \param[in] action Action (Entry or Exit)
    /// \param[in] vehicle Shared pointer to the Vehicle being parked or released
    /// \param[in] ticket_id Ticket ID of the vehicle
    void logEntry(const std::string& action, const std::shared_ptr<Vehicle>& vehicle, const int ticket_id);

    /// \brief Generates a unique ticket ID
    /// \returns Returns unique ticket ID as an integer
    int generateTicketID();

    /// \brief Updates number of certain vehicle after it is parked or released
    /// \param[in] vehicle_type Type of the vehicle for which the count should be updated
    /// \param[in] change Whether a value should be incremented or decremented, 1: incremented, -1: decremented
    void updateCount(const std::string& vehicle_type, int change);

private:
    std::unordered_map<std::string, std::pair<std::shared_ptr<Vehicle>, int>> m_parked_vehicles;

    int m_car_capacity;
    int m_motorcycle_capacity;
    int m_bus_capacity;
    int m_car_count = 0;
    int m_motorcycle_count = 0;
    int m_bus_count = 0;

    static std::shared_ptr<ParkingLot> instance_;
    static std::mutex instance_mutex_;
    static std::mutex count_mutex_;
};