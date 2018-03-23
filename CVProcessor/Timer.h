/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Timer.h
 * Author: dean
 *
 * Created on March 17, 2018, 7:43 PM
 */

#ifndef TIMER_H
#define TIMER_H

#include "Utilities.h"
#include "Config.h"

struct Timer 
{
public:
    Timer(const std::string& header)
        : header(header)
    {
        tStart = Utilities::GetTimeMs64();
    }
    ~Timer()
    {
        tEnd = Utilities::GetTimeMs64();
        Config::output.log(header + ": " + std::to_string(tEnd-tStart) + "ms\n",
                           OutputWriter::LogLevel::Info);
    }
    
private:
    std::string header;
    Utilities::uint64 tStart, tEnd;
};

#endif /* TIMER_H */

