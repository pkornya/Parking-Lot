#pragma once

#include <exception>
#include <string>

// Custom exception for vehicles not found in the parking lot
class VehicleNotFoundException : public std::exception
{
public:
    VehicleNotFoundException(const std::string & message) : m_message(message) {}
    const char* what() const noexcept override { return m_message.c_str(); }

private:
    std::string m_message;
};