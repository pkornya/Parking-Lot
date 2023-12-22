#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <random>

#include "Vehicle.h"
#include "Car.h"
#include "Motorcycle.h"
#include "Bus.h"
#include "ParkingLot.h"

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
                
                std::shared_ptr<Vehicle> motorcycle_pointer = Vehicle::makeVehicle(2, "MOTO" + std::to_string(dist(rng)), 1.5);

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