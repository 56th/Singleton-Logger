#pragma once
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <stack>
#include <vector>
#include <cmath>
#include "rlutil.h" // https://github.com/tapio/rlutil
// for cross–platform terminal text coloring

enum class SingletonLoggerTimeUnits { min, sec };

class SingletonLogger {
	std::stack<time_t> _processes; // stack of started proccesses
	bool _rec = false;
	double _diff(clock_t t1, clock_t t2) const;
	std::string _format(std::string const &) const;
	// for input history
	std::ostringstream _inputValues, _inputDescriptions;
	size_t _inputCounter;
	// singleton
	SingletonLogger();
	SingletonLogger(SingletonLogger const &);
	SingletonLogger& operator=(SingletonLogger const &);
public:
	SingletonLoggerTimeUnits timeUnits = SingletonLoggerTimeUnits::sec;
	bool mute;
	std::ostringstream buf;
	~SingletonLogger();
	static SingletonLogger& instance();
	void beg(std::string const &); // begin new process
	void end(); // terminate last process
	void log(std::string const &); // print message w/ "[log]" prefix
	void log(); // print formatted buf 
	void rec(); // print formatted buf w/ '\r'
	void wrn(std::string const &); // print warning
	void wrn(); // print formatted buf 
	void err(std::string const &); // print error
	void pro(size_t, size_t); // progress bar
	template <typename T>
	SingletonLogger& inp(std::string const &, T&); // print input invite and get input value
	bool yes(std::string const &); // true if get 'y' from stdin
	size_t opt(std::string const &, std::vector<std::string> const &); // choose vector element (get index from stdin), return its index
	std::string tab() const { // tabulations
		return std::string(_processes.size(), '\t'); 
	} 
	void exp(std::string const &); // export inp commands
};

template <typename T>
SingletonLogger& SingletonLogger::inp(std::string const & message, T& val) {
	rlutil::setColor(10);
	std::cout << tab() << "[inp] ";
	rlutil::setColor(7);
	std::cout << _format(message) << ":\n" << tab() << "      ";
	std::cin >> val;
	// echo
	std::cout << tab() << "   -> value = " << val << '\n';
	// input buffer
	++_inputCounter;
	_inputValues << val << '\n';
	_inputDescriptions << _inputCounter << ". " << message << '\n';
	return *this;
}