// Created by Nathan on 2025-11-10.
// Observer pattern that allows for logging. Any class that inherits from ILoggable can produce a string that gets written to gamelog.txt.

#ifndef LOGGING_OBSERVER_H
#define LOGGING_OBSERVER_H

#include <string>
#include <vector>
#include <ostream>

class ILoggable {
public:
    virtual ~ILoggable() = default;
    virtual std::string stringToLog() const = 0;
};

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const ILoggable& loggable) = 0;
};

class Subject {
public:

    virtual ~Subject() = default;

    void attach(Observer* o);
    void detach(Observer* o);
    void notify(const ILoggable& l);

    // Stream insertion: shows how many observers are attached
    friend std::ostream& operator<<(std::ostream& os, const Subject& s);

private:
    std::vector<Observer*> observers_;
};

class LogObserver : public Observer {
public:
    explicit LogObserver(std::string filename = "gamelog.txt");
    LogObserver(const LogObserver& other);              //copy constructor
    LogObserver& operator=(const LogObserver& other);   //assignment operator
    ~LogObserver() override = default;
    void update(const ILoggable& loggable) override;

    // Stream insertion: prints destination file
    friend std::ostream& operator<<(std::ostream& os, const LogObserver& lo);

private:
    std::string file_;
};

#endif // LOGGING_OBSERVER_H
