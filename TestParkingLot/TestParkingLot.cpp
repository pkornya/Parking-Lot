#include "gtest/gtest.h"

#include "ParkingLot.h"
#include "Car.h"
#include "Motorcycle.h"
#include "Bus.h"
#include "ParkingLot.cpp"

#include <iostream>
#include <memory>

TEST(ParkingLotTest, GetInstanceReturnsValidInstance)
{
    std::shared_ptr<ParkingLot> parkingLotInstance = ParkingLot::getInstance(200, 200, 200);

	ASSERT_NE(parkingLotInstance, nullptr);
}

TEST(ParkingLotTest, GetInstanceReturnsSameInstance) 
{
    std::shared_ptr<ParkingLot> parkingLotInstance = ParkingLot::getInstance();
    std::shared_ptr<ParkingLot> anotherInstance = ParkingLot::getInstance(20, 30, 10);

    ASSERT_EQ(parkingLotInstance, anotherInstance);
}

TEST(ParkingLotTest, ParkVehicleSuccess) 
{
    std::shared_ptr<ParkingLot> parkingLotInstance = ParkingLot::getInstance();

    // Park car
    Car car("CAR123", 2.0);
    std::shared_ptr<Vehicle> car_pointer = std::make_shared<Car>(car);

    bool parked = parkingLotInstance->parkVehicle(car_pointer);
    ASSERT_TRUE(parked);

    // Park motorcycle
    Motorcycle moto("MOTO123", 3.0);
    std::shared_ptr<Vehicle> moto_pointer = std::make_shared<Motorcycle>(moto);

    parked = parkingLotInstance->parkVehicle(moto_pointer);
    ASSERT_TRUE(parked);

    // Park bus
    Bus bus("BUS123", 4.0);
    std::shared_ptr<Vehicle> bus_pointer = std::make_shared<Bus>(bus);

    parked = parkingLotInstance->parkVehicle(bus_pointer);
    ASSERT_TRUE(parked);
}

TEST(ParkingLotTest, ParkVehicleDuplicateParking) 
{
    std::shared_ptr<ParkingLot> parkingLotInstance = ParkingLot::getInstance();

    // Create a vehicle (e.g., Bus)
    Bus bus("BUS789", 2.5);
    std::shared_ptr<Vehicle> busPointer = std::make_shared<Bus>(bus);

    // Park the vehicle for the first time
    bool parked1 = parkingLotInstance->parkVehicle(busPointer);

    // Park the same vehicle again and expect it to return false
    bool parked2 = parkingLotInstance->parkVehicle(busPointer);

    // Check if the vehicle is parked successfully the first time and not parked the second time
    ASSERT_TRUE(parked1);
    ASSERT_FALSE(parked2);
}

TEST(ParkingLotTest, ParkDifferentTypesOfVehicles)
{
    std::shared_ptr<ParkingLot> parkingLotInstance = ParkingLot::getInstance();

    std::shared_ptr<Car> car = std::make_shared<Car>("CAR001", 2.0);
    std::shared_ptr<Motorcycle> motorcycle = std::make_shared<Motorcycle>("MOTO001", 1.5);
    std::shared_ptr<Bus> bus = std::make_shared<Bus>("BUS001", 3.5);

    EXPECT_TRUE(parkingLotInstance->parkVehicle(car));
    EXPECT_TRUE(parkingLotInstance->parkVehicle(motorcycle));
    EXPECT_TRUE(parkingLotInstance->parkVehicle(bus));
}

TEST(ParkingLotTest, ParkMultipleVehiclesOfTheSameType)
{
    std::shared_ptr<ParkingLot> parkingLot = ParkingLot::getInstance();

    std::shared_ptr<Car> car1 = std::make_shared<Car>("CAR0012", 2.0);
    std::shared_ptr<Car> car2 = std::make_shared<Car>("CAR0013", 1.5);

    EXPECT_TRUE(parkingLot->parkVehicle(car1));
    EXPECT_TRUE(parkingLot->parkVehicle(car2));
}

TEST(ParkingLotTest, ParkMultipleVehiclesOfDifferentTypes)
{
    std::shared_ptr<ParkingLot> parkingLot = ParkingLot::getInstance();

    std::shared_ptr<Car> car = std::make_shared<Car>("CAR0014", 2.0);
    std::shared_ptr<Motorcycle> motorcycle = std::make_shared<Motorcycle>("MOTO0014", 1.5);
    std::shared_ptr<Bus> bus = std::make_shared<Bus>("BUS0014", 3.5);

    EXPECT_TRUE(parkingLot->parkVehicle(car));
    EXPECT_TRUE(parkingLot->parkVehicle(motorcycle));
    EXPECT_TRUE(parkingLot->parkVehicle(bus));
}

TEST(ParkingLotTest, ReleaseVehicleByTicketID)
{
    std::shared_ptr<ParkingLot> parkingLot = ParkingLot::getInstance();

    // Park a car and get its ticket ID
    std::shared_ptr<Car> car = std::make_shared<Car>("CAR0015", 2.0);
    EXPECT_TRUE(parkingLot->parkVehicle(car));
    int ticket_id = parkingLot->getTicketIDByLicensePlate("CAR0015");

    // Release the car by ticket ID
    EXPECT_TRUE(parkingLot->releaseVehicleByTicketID(ticket_id));
}

TEST(ParkingLotTest, ReleaseVehicleByInvalidTicketID)
{
    std::shared_ptr<ParkingLot> parkingLot = ParkingLot::getInstance();

    // Attempt to release a vehicle with an invalid ticket ID
    int invalid_ticket_id = 9999;
    EXPECT_THROW(parkingLot->releaseVehicleByTicketID(invalid_ticket_id), VehicleNotFoundException);
}

TEST(ParkingLotTest, ReleaseVehicleByLicensePlate)
{
    std::shared_ptr<ParkingLot> parkingLot = ParkingLot::getInstance();

    // Park a car and get its license plate
    std::shared_ptr<Car> car = std::make_shared<Car>("CAR0016", 2.0);
    EXPECT_TRUE(parkingLot->parkVehicle(car));
    std::string license_plate = car->getLicensePlate();

    // Release the car by license plate
    EXPECT_TRUE(parkingLot->releaseVehicleByLicensePlate(license_plate));
}

TEST(ParkingLotTest, ReleaseVehicleByInvalidLicensePlate)
{
    std::shared_ptr<ParkingLot> parkingLot = ParkingLot::getInstance();

    // Attempt to release a vehicle with an invalid license plate
    std::string invalid_license_plate = "INVALID";
    EXPECT_THROW(parkingLot->releaseVehicleByLicensePlate(invalid_license_plate), VehicleNotFoundException);
}

TEST(ParkingLotConcurrentTest, ConcurrentParking)
{
    std::shared_ptr<ParkingLot> parkingLot = ParkingLot::getInstance();

    // Create multiple threads for concurrent parking of cars
    std::vector<std::thread> threads;
    for (int i = 17; i < 37; ++i)
    {
        threads.emplace_back([parkingLot, i]() 
        {
            std::shared_ptr<Vehicle> car = std::make_shared<Car>("CAR00" + std::to_string(i), 2.0);
            try
            {
                EXPECT_TRUE(parkingLot->parkVehicle(car));
            }
            catch (const std::exception& e)
            {
                FAIL() << "Exception thrown while parking: " << e.what();
            }
        });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
}

TEST(ParkingLotConcurrentTest, ConcurrentReleasing)
{
    std::shared_ptr<ParkingLot> parkingLot = ParkingLot::getInstance();

    // Create and park multiple cars
    for (int i = 0; i < 5; ++i)
    {
        std::shared_ptr<Vehicle> car = std::make_shared<Car>("CAR0000" + std::to_string(i), 2.0);
        parkingLot->parkVehicle(car);
    }

    // Create multiple threads for concurrent releasing of cars
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i)
    {
        threads.emplace_back([parkingLot, i]() {
            try
            {
                EXPECT_TRUE(parkingLot->releaseVehicleByLicensePlate("CAR0000" + std::to_string(i)));
            }
            catch (const std::exception& e)
            {
                FAIL() << "Exception thrown while releasing: " << e.what();
            }
            });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
}

TEST(ParkingLotConcurrentTest, MixedParkingAndReleasing)
{
    std::shared_ptr<ParkingLot> parkingLot = ParkingLot::getInstance();

    // Create multiple threads for mixed parking and releasing
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i)
    {
        if (i % 2 == 0)
        {
            threads.emplace_back([parkingLot, i]() 
            {
                std::shared_ptr<Vehicle> car = std::make_shared<Car>("CAR00000" + std::to_string(i), 2.0);
                try
                {
                    EXPECT_TRUE(parkingLot->parkVehicle(car));
                    // Simulate parking
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    EXPECT_TRUE(parkingLot->releaseVehicleByLicensePlate(car->getLicensePlate()));
                    
                }
                catch (const std::exception& e)
                {
                    FAIL() << "Exception thrown while parking/releasing: " << e.what();
                }
            });
        }
        else
        {
            threads.emplace_back([parkingLot, i]() 
            {
                std::shared_ptr<Vehicle> motorcycle = std::make_shared<Motorcycle>("MOTO00000" + std::to_string(i), 1.5);
                try
                {
                    EXPECT_TRUE(parkingLot->parkVehicle(motorcycle));
                    // Simulate parking
                    std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
                    EXPECT_TRUE(parkingLot->releaseVehicleByLicensePlate(motorcycle->getLicensePlate()));
                }
                catch (const std::exception& e)
                {
                    FAIL() << "Exception thrown while parking/releasing: " << e.what();
                }
            });
        }
    }

    // Join the threads
    for (auto & thread : threads)
    {
        thread.join();
    }
}