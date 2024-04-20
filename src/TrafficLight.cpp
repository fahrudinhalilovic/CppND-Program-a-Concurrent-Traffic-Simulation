#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lck { _mutex };
    _condition.wait(lck, [this]() { return !_queue.empty(); });
    // FIFO - first object is at the beggining of the queue thus we take from front
    auto msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lck { _mutex };
    _queue.emplace_back(std::move(msg));
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while (true) {
        if ( _queue.receive() == TrafficLightPhase::green ) {
            return;
        }
    }
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
            auto newPhase = TrafficLightPhase::red;
            if ( _currentPhase ==  TrafficLightPhase::red ) {
                newPhase = TrafficLightPhase::green;
            }

            _currentPhase = newPhase;
            _queue.send(std::move(newPhase));

            lastUpdate = std::chrono::system_clock::now();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
