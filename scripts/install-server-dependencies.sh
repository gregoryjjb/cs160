#!/bin/bash

# These should eventually be merged into install-essential-dependncies.sh
# I'm tired of waiting hours for docker to reimage everything whenever need to add some trivial package
# TODO: split dependency installations up strictly based on time-consuming builds needing them, then install the remainder

apt-get install -y python3-pip
