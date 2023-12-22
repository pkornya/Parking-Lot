#include <iostream>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <memory> // Added for smart pointers
#include <random>

class Vehicle
{
public:
    Vehicle(const std::string & license_plate, double parking_duration)
        : m_license_plate(license_plate), m_parking_duration(parking_duration) {}
    virtual ~Vehicle() {}
    virtual std::string getLicensePlate() const { return m_license_plate; }
    virtual std::string getVehicleType() const = 0;
    virtual double getParkingDuration() const { return m_parking_duration; }
    static std::shared_ptr<Vehicle> makeVehicle(const int choice, const std::string & license_plate, double parking_duration);
private:
    std::string m_license_plate;
    double m_parking_duration;
};

class Car : public Vehicle
{
public:
    Car(const std::string& license_plate, double parking_duration)
        : Vehicle(license_plate, parking_duration) {}

    std::string getVehicleType() const override { return "Car"; }
};

class Motorcycle : public Vehicle
{
public:
    Motorcycle(const std::string & license_plate, double parking_duration)
        : Vehicle(license_plate, parking_duration) {}

    std::string getVehicleType() const override { return "Motorcycle"; }
};

class Bus : public Vehicle
{
public:
    Bus(const std::string& license_plate, double parking_duration)
        : Vehicle(license_plate, parking_duration) {}

    std::string getVehicleType() const override { return "Bus"; }
};

// Custom exception for vehicles not found in the parking lot
class VehicleNotFoundException : public std::exception
{
public:
    VehicleNotFoundException(const std::string& message) : m_message(message) {}
    const char* what() const noexcept override { return m_message.c_str(); }

private:
    std::string m_message;
};

// Custom exception for parking lot full
class ParkingLotFullException : public std::exception
{
public:
    ParkingLotFullException(const std::string& message) : m_message(message) {}
    const char* what() const noexcept override { return m_message.c_str(); }

private:
    std::string m_message;
};

// Custom exception for invalid vehicle type
class InvalidVehicleTypeException : public std::exception
{
public:
    InvalidVehicleTypeException(const std::string& message) : m_message(message) {}
    const char* what() const noexcept override { return m_message.c_str(); }

private:
    std::string m_message;
};

class ParkingLot
{
public:
    static std::shared_ptr<ParkingLot> getInstance(int car_capacity = 10, int motorcycle_capacity = 15, int bus_capacity = 5)
    {
        std::lock_guard<std::mutex> lock(instance_mutex_);

        if (!instance_)
        {
            instance_ = std::shared_ptr<ParkingLot>(new ParkingLot(car_capacity, motorcycle_capacity, bus_capacity));
        }
        return instance_;
    }

    bool parkVehicle(const std::shared_ptr<Vehicle>& vehicle)
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

    bool releaseVehicleByTicketID(int ticket_id)
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

    bool releaseVehicleByLicensePlate(const std::string& license_plate)
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

    void queryAvailableCarsSlots()
    {
        std::cout << "Available Cars slots: " << m_car_capacity - m_car_count << " out of " << m_car_capacity << std::endl;
    }

    void queryAvailableMotorcyclesSlots()
    {
        std::cout << "Available Motorcycles slots: " << m_motorcycle_capacity - m_motorcycle_count << " out of " << m_motorcycle_capacity << std::endl;
    }

    void queryAvailableBusesSlots()
    {
        std::cout << "Available Buses slots: " << m_bus_capacity - m_bus_count << " out of " << m_bus_capacity << std::endl;
    }

private:

    ParkingLot() : m_car_capacity(0), m_motorcycle_capacity(0), m_bus_capacity(0) {}
    ParkingLot(int car_capacity, int motorcycle_capacity, int bus_capacity)
        : m_car_capacity(car_capacity), m_motorcycle_capacity(motorcycle_capacity), m_bus_capacity(motorcycle_capacity) {}

    bool isParkingFull(const std::string& vehicle_type)
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

    double calculateCharge(const std::shared_ptr<Vehicle> & vehicle)
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

    void logEntry(const std::string& action, const std::shared_ptr<Vehicle>& vehicle, int ticket_id)
    {
        std::ofstream log_file("parking_log.txt", std::ios_base::app);
        if (log_file.is_open())
        {
            log_file << action << ": Ticket ID " << ticket_id << ", " << vehicle->getVehicleType() << " with license plate " << vehicle->getLicensePlate() << std::endl;
            log_file.close();
        }
    }

    int generateTicketID()
    {
        // Is initialized once
        static int ticket_counter = 1;
        return ticket_counter++;
    }

    void updateCount(const std::string& vehicle_type, int change)
    {
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

private:
    std::unordered_map<std::string, std::pair<std::shared_ptr<Vehicle>, int>> m_parked_vehicles;

    int m_car_capacity;
    int m_motorcycle_capacity;
    int m_bus_capacity;
    int m_car_count = 0;
    int m_motorcycle_count = 0;
    int m_bus_count = 0;

    static std::shared_ptr<ParkingLot> instance_; // Singleton instance
    static std::mutex instance_mutex_; // Static mutex for thread safety
};

std::shared_ptr<ParkingLot> ParkingLot::instance_ = nullptr;
std::mutex ParkingLot::instance_mutex_;

 std::shared_ptr<Vehicle> Vehicle::makeVehicle(const int choice, const std::string& license_plate, double parking_duration)
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

int main()
{
    // Use the Singleton instance getter to access the ParkingLot
    std::shared_ptr<ParkingLot> parkingLot = ParkingLot::getInstance();

    // Create multiple threads to simulate concurrent parking and releasing
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i)
    {
        if (i % 2 == 0)
        {
            threads.emplace_back([&parkingLot, i]() 
            {
                std::mt19937 rng(i); // Use a different seed for each thread
                std::uniform_int_distribution<int> dist(0, 999);

                int randomValue = dist(rng);

                std::shared_ptr<Vehicle> car_pointer = Vehicle::makeVehicle(1, "CAR" + std::to_string(dist(rng)), 2.0);
                try
                {
                    if (parkingLot->parkVehicle(car_pointer))
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate vehicle parking
                    }
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            });
        }
        else
        {
            threads.emplace_back([&parkingLot, i]() 
            {
                std::mt19937 rng(i); // Use a different seed for each thread
                std::uniform_int_distribution<int> dist(0, 999);

                int randomValue = dist(rng);

                Motorcycle motorcycle("MOTO" + std::to_string(dist(rng)), 1.5);
                std::shared_ptr<Vehicle> motorcycle_pointer = std::make_shared<Motorcycle>(motorcycle);
                try
                {
                    if (parkingLot->parkVehicle(motorcycle_pointer))
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate vehicle parking
                    }
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            });
        }
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

    parkingLot->queryAvailableCarsSlots();
    parkingLot->queryAvailableMotorcyclesSlots();
    parkingLot->queryAvailableBusesSlots();

    try
    {
        // Example of releasing a vehicle by ticket ID
        int ticket_to_release = 3; // Replace with the actual ticket ID
        parkingLot->releaseVehicleByTicketID(ticket_to_release);

        // Example of releasing a vehicle by license plate
        std::string license_plate_to_release = "CAR123"; // Replace with the actual license plate
        parkingLot->releaseVehicleByLicensePlate(license_plate_to_release);

        // Example of attempting to release a vehicle not in the parking lot
        std::string non_existent_license_plate = "NONEXIST"; // Replace with a non-existent license plate
        parkingLot->releaseVehicleByLicensePlate(non_existent_license_plate);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}