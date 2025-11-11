// Created by Nathan on 2025-11-10.
// Observer pattern that allows for logging. Any class that inherits from ILoggable can produce a string that gets written to gamelog.txt.

#include "LoggingObserver.h"
#include <fstream>
#include <utility>

void Subject::attach(Observer* o) {
    if (!o) return;
    for (auto* p : observers_) if (p == o) return;
    observers_.push_back(o);
}

void Subject::detach(Observer* o) {
    for (auto it = observers_.begin(); it != observers_.end(); ++it) {
        if (*it == o) { observers_.erase(it); break; }
    }
}

void Subject::notify(const ILoggable& l) {
    for (auto* o : observers_) if (o) o->update(l);
}

//stream insertion 
std::ostream& operator<<(std::ostream& os, const Subject& s) {
    os << "Subject{observers=" << s.observers_.size() << "}";
    return os;
}

LogObserver::LogObserver(std::string filename) : file_(std::move(filename)) {}

LogObserver::LogObserver(const LogObserver& other) : file_(other.file_) {}

LogObserver& LogObserver::operator=(const LogObserver& other) {
    if (this != &other) {
        file_ = other.file_;
    }
    return *this;
}

void LogObserver::update(const ILoggable& loggable) {
    std::ofstream out(file_, std::ios::app);
    if (!out) return;
    out << loggable.stringToLog() << "\n";
}
