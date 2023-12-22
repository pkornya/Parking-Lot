# Parking Lot Simulation

This is a C++ program that simulates a parking lot system with multi-threading support. It allows vehicles to be parked and released concurrently and tracks the available parking slots for different vehicle types (Car, Motorcycle, Bus).

## How to Run
1. To download and run the code, particularly open Command Prompt, navigate to the location where you'd like to store the repository, and type the following command: git clone https://github.com/pkornya/Parking-Lot.git.
2. After downloading is complete, if you have Visual Studio, you can navigate to the Parking-Lot folder and execute the Parking_lot.sln file.
3. In case you don't have Visual Studio, please navigate to the following path: your_path_to_repo\Parking-Lot\x64\Debug. 
You can find the Parking_lot.exe file there; it has a simple main function that uses the implemented functionality, but there is no interaction with the user. 
You will also find a static Google Test library and the TestParkingLot.exe file; the second one implements unit tests for the core functionality.
4. In your_path_to_repo\Parking-Lot\GoogleTest, you will find a Google Test static library.
5. In your_path_to_repo\Parking-Lot\Parking_lot, you will find the source code for the core functionality.
6. In your_path_to_repo\Parking-Lot\TestParkingLot, you will find the source code for unit tests.

## Assumptions Made
- The program assumes that the user specifies the capacity of the parking lot for each vehicle type (Car, Motorcycle, Bus) when creating the `ParkingLot` instance.
- It assumes that vehicles have unique license plates, and license plates are used as a unique identifier for parked vehicles.
- The parking charges are calculated based on the parking duration for each vehicle type, as mentioned in the code.

## Design Choices
- The code uses C++11 features, including multi-threading using std::thread, smart pointers (e.g., std::shared_ptr) for managing objects, and mutexes (e.g., std::mutex) for ensuring thread safety.
- The program implements a Singleton design pattern for the ParkingLot class to ensure that there is only one instance of the parking lot.
- It uses a Factory Method pattern for creating different types of vehicles (Car, Motorcycle, Bus) with a common base class (Vehicle).
- The code includes exception handling for scenarios such as parking lot full, vehicle not found, and invalid vehicle types.
- Log entries for vehicle entry and exit are written to a file named "parking_log.txt."
- The program provides options for querying available parking slots and releasing vehicles by both ticket ID and license plate.
- Multi-threading is supported for simulating concurrent parking and releasing of vehicles.
