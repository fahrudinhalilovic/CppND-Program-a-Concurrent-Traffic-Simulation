#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

/* 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
}
*/

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(&TrafficLight::cycleThroughPhases, this);
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(4, 6);
    auto cycleDuration = std::chrono::seconds(distr(eng));
    auto lastUpdate = std::chrono::system_clock::now();

    while (true) {
        auto diff = std::chrono::system_clock::now() - lastUpdate;

        if ( std::chrono::duration_cast<std::chrono::seconds>(diff) >= cycleDuration ) {
            if ( _currentPhase == TrafficLightPhase::red ) {
                _currentPhase = TrafficLightPhase::green;
            }
            else {
                _currentPhase = TrafficLightPhase::red;
            }

            // FP.4 send update to the message queue

            lastUpdate = std::chrono::system_clock::now();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
