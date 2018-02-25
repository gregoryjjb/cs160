/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <memory>
#include <array>
#include "SystemHelper.h"

std::string exec(const std::string& cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}