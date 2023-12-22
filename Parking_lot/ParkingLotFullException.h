#pragma once

#include <exception>
#include <string>

// Custom exception for parking lot full
class ParkingLotFullException : public std::exception
{
public:
    ParkingLotFullException(const std::string & message) : m_message(message) {}
    const char* what() const noexcept override { return m_message.c_str(); }

private:
    std::string m_message;
};