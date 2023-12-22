#pragma once

#include <exception>
#include <string>

// Custom exception for invalid vehicle type
class InvalidVehicleTypeException : public std::exception
{
public:
    InvalidVehicleTypeException(const std::string & message) : m_message(message) {}
    const char* what() const noexcept override { return m_message.c_str(); }

private:
    std::string m_message;
};