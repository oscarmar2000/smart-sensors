# smart-sensors
## Intro
This C++ project implements a template based strategy to handle the adquisition, processing and sharing of multiple sensor readings using the Arduino platform.

## Strategy
The implemented solution for generically handling of the different types of sensors is to use variant types to handle the data in a generic manner. Also a CRTP strategy is used to define the basic structure of the sensor classes. 

## Current status
At the moment the system supports 3 kinds of basic sensor types: Digital, which returns a boolen value, Analog, which returns floating point within a specified range and resolution, and finally a Discrete sensor which returns a predefined range of integer values based on an analog input.

## Getting started
The main file sensor-server.ino sets up the main service object with the sensor configuration according to the physical configuration. It also starts the web service to listen for subscriptions and send back data updates.

## TODOS
  + Add more sensor types
    - Virtual sensors
    - Remote sensors
    - fused data sensors
    - complex logic sensors
  + Add different subscription types
    - change-only updates
    - polling time
    - condition based trigger
  + Appliance class
    - logic/action based on sensor input
    - control outputs
    - generate messages/alerts
    - chain actions