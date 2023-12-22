#include <mutex>
#include <iostream>
#include <fstream>

#include "ParkingLot.h"
#include "ParkingLotFullException.h"
#include "VehicleNotFoundException.h"
#include "InvalidVehicleTypeException.h"

std::shared_ptr<ParkingLot> ParkingLot::instance_ = nullptr;
std::mutex ParkingLot::instance_mutex_;
std::mutex ParkingLot::count_mutex_;

ParkingLot::ParkingLot() 
    : m_car_capacity(0), m_motorcycle_capacity(0), m_bus_capacity(0) 
{
    
}

ParkingLot::ParkingLot(const int car_capacity, const int motorcycle_capacity, const int bus_capacity)
    : m_car_capacity(car_capacity), m_motorcycle_capacity(motorcycle_capacity), m_bus_capacity(motorcycle_capacity)
{

}

std::shared_ptr<ParkingLot> ParkingLot::getInstance(int car_capacity, int motorcycle_capacity, int bus_capacity)
{
    std::lock_guard<std::mutex> lock(instance_mutex_);

    if (!instance_)
    {
        instance_ = std::shared_ptr<ParkingLot>(new ParkingLot(car_capacity, motorcycle_capacity, bus_capacity));
    }
    return instance_;
}

bool ParkingLot::parkVehicle(const std::shared_ptr<Vehicle> & vehicle)
{
    const std::string& license_plate = vehicle->getLicensePlate();
    std::lock_guard<std::mutex> lock(instance_mutex_);

    if (m_parked_vehicles.find(license_plate) == m_parked_vehicles.end())
    {
        if (isParkingFull(vehicle->getVehicleType()))
        {
            throw ParkingLotFullException("Parking lot is full for " + vehicle->getVehicleType());
        }

        // Generate a unique ticket ID for the parked vehicle
        int ticket_id = generateTicketID();
        m_parked_vehicles[vehicle->getLicensePlate()] = std::make_pair(vehicle, ticket_id);

        updateCount(vehicle->getVehicleType(), 1);

        std::cout << vehicle->getVehicleType() << " with license plate " << license_plate << " parked. Ticket ID: " << ticket_id << std::endl;

        // Log the vehicle entry
        logEntry("Entry", vehicle, ticket_id);

        return true;
    }
    else
    {
        std::cout << vehicle->getVehicleType() << " with license plate " << license_plate << " is already parked." << std::endl;
        return false;
    }
}

bool ParkingLot::releaseVehicleByTicketID(const int ticket_id)
{
    std::lock_guard<std::mutex> lock(instance_mutex_);
    for (auto it = m_parked_vehicles.begin(); it != m_parked_vehicles.end(); ++it)
    {
        if (it->second.second == ticket_id)
        {
            auto vehicle = it->second.first;
            double charge = calculateCharge(vehicle);
            std::string license_plate = it->first;
            std::cout << vehicle->getVehicleType() << " with license plate " << license_plate << " released. Charge: $" << charge << std::endl;
            m_parked_vehicles.erase(it);

            updateCount(vehicle->getVehicleType(), -1);

            // Log the vehicle exit
            logEntry("Exit", vehicle, ticket_id);
            return true;
        }
    }
    throw VehicleNotFoundException("Vehicle with ticket ID " + std::to_string(ticket_id) + " is not found in the parking lot.");
}

bool ParkingLot::releaseVehicleByLicensePlate(const std::string& license_plate)
{
    std::lock_guard<std::mutex> lock(instance_mutex_);
    auto it = m_parked_vehicles.find(license_plate);
    if (it != m_parked_vehicles.end())
    {
        auto vehicle = it->second.first;
        int ticket_id = it->second.second;
        double charge = calculateCharge(vehicle);
        std::cout << vehicle->getVehicleType() << " with license plate " << license_plate << " released. Charge: $" << charge << std::endl;
        m_parked_vehicles.erase(it);

        updateCount(vehicle->getVehicleType(), -1);

        // Log the vehicle exit
        logEntry("Exit", vehicle, ticket_id);
        return true;
    }
    throw VehicleNotFoundException("Vehicle with license plate " + license_plate + " is not found in the parking lot.");
}

void ParkingLot::queryAvailableCarsSlots()
{
    std::cout << "Available Cars slots: " << m_car_capacity - m_car_count << " out of " << m_car_capacity << std::endl;
}

void ParkingLot::queryAvailableMotorcyclesSlots()
{
    std::cout << "Available Motorcycles slots: " << m_motorcycle_capacity - m_motorcycle_count << " out of " << m_motorcycle_capacity << std::endl;
}

void ParkingLot::queryAvailableBusesSlots()
{
    std::cout << "Available Buses slots: " << m_bus_capacity - m_bus_count << " out of " << m_bus_capacity << std::endl;
}

bool ParkingLot::isParkingFull(const std::string & vehicle_type)
{
    if (vehicle_type == "Car")
    {
        return m_car_count >= m_car_capacity;
    }
    else if (vehicle_type == "Motorcycle")
    {
        return m_motorcycle_count >= m_motorcycle_capacity;
    }
    else if (vehicle_type == "Bus")
    {
        return m_bus_count >= m_bus_capacity;
    }
    throw InvalidVehicleTypeException("Invalid vehicle type: " + vehicle_type);
}

double ParkingLot::calculateCharge(const std::shared_ptr<Vehicle>& vehicle)
{
    double charge = 0.0;

    double charge_cars_first_hour = 2.0;
    double charge_cars_subsequent_hours = 1.0;
    double charge_motorcycles_first_hour = 1.0;
    double charge_motorcycles_subsequent_hours = 0.5;
    double charge_buses_first_hour = 5.0;
    double charge_buses_subsequent_hours = 3.0;

    if (vehicle->getVehicleType() == "Car")
    {
        charge = charge_cars_first_hour + std::max(0.0, vehicle->getParkingDuration() - 1.0) * charge_cars_subsequent_hours;
    }
    else if (vehicle->getVehicleType() == "Motorcycle")
    {
        charge = charge_motorcycles_first_hour + std::max(0.0, vehicle->getParkingDuration() - 1.0) * charge_motorcycles_subsequent_hours;
    }
    else if (vehicle->getVehicleType() == "Bus")
    {
        charge = charge_buses_first_hour + std::max(0.0, vehicle->getParkingDuration() - 1.0) * charge_buses_subsequent_hours;
    }
    else
    {
        throw InvalidVehicleTypeException("Invalid vehicle type: " + vehicle->getVehicleType());
    }
    return charge;
}

void ParkingLot::logEntry(const std::string& action, const std::shared_ptr<Vehicle>& vehicle, const int ticket_id)
{
    std::ofstream log_file("parking_log.txt", std::ios_base::app);
    if (log_file.is_open())
    {
        log_file << action << ": Ticket ID " << ticket_id << ", " << vehicle->getVehicleType() << " with license plate " << vehicle->getLicensePlate() << std::endl;
        log_file.close();
    }
}

int ParkingLot::generateTicketID()
{
    // Is initialized once
    static int ticket_counter = 1;
    return ticket_counter++;
}

void ParkingLot::updateCount(const std::string& vehicle_type, int change)
{
    std::lock_guard<std::mutex> lock(count_mutex_);
    if (vehicle_type == "Car")
    {
        m_car_count += change;
    }
    else if (vehicle_type == "Motorcycle")
    {
        m_motorcycle_count += change;
    }
    else if (vehicle_type == "Bus")
    {
        m_bus_count += change;
    }
}

int ParkingLot::getTicketIDByLicensePlate(const std::string & license_plate)
{
    std::lock_guard<std::mutex> lock(instance_mutex_);

    auto it = m_parked_vehicles.find(license_plate);
    if (it != m_parked_vehicles.end())
    {
        return it->second.second;
    }
    else
    {
        throw VehicleNotFoundException("Vehicle with license plate " + license_plate + " is not found in the parking lot.");
    } 
}