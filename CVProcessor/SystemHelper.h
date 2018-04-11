/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SystemHelper.h
 * Author: dean
 *
 * Created on February 23, 2018, 5:56 PM
 */

#ifndef SYSTEMHELPER_H
#define SYSTEMHELPER_H

#include <string>

// Executes the given command and returns anything 
// output to stdout by the command
std::string execAndGetOutput(const std::string& cmd);

// Creates a fifo of the given name, returning the status indicator
// from mkfifo
int createFIFO(const std::string& fifoName);

#endif /* SYSTEMHELPER_H */
