#include <stdexcept>
#include <iomanip> // setprecision
#include "SingletonLogger.hpp"

SingletonLogger::SingletonLogger() : _inputCounter(0), mute(false) {
	rlutil::saveDefaultColor();
	// rlutil::setBackgroundColor(0);
	//	for (int i = 0; i < 16; i++) {
	//		rlutil::setColor(i);
	//		std::cout << i << " ";
	//	}
}

SingletonLogger::~SingletonLogger() {
	rlutil::resetColor();
}

double SingletonLogger::_diff(timePoint t1, timePoint t2) const {
	using namespace std::chrono;
	auto diff = duration_cast<duration<double>>(t2 - t1).count();
	if (timeUnits == TimeUnits::min) diff /= 60.;
	return diff;
}

std::string SingletonLogger::_format(std::string const & s) const {
	std::string res(s), newLine('\n' + tab() + "      ");
	// trim
	res.erase(res.find_last_not_of(" \t\n\r\f\v") + 1);
	res.erase(0, res.find_first_not_of(" \t\n\r\f\v"));
	// add tabulations
	size_t i = 0;
	while (true) {
		i = res.find('\n', i);
		if (i == std::string::npos) break;
		res.replace(i, 1, newLine);
		i += newLine.size();
	}
	return res;
}

SingletonLogger& SingletonLogger::instance() {
	static SingletonLogger single;
	return single;
}

void SingletonLogger::beg(std::string const & message) {
	if (!mute) {
        if (_rec) {
            _rec = false;
            std::cout << '\n';
        }
        rlutil::setColor(9);
        std::cout << tab() << "[beg] ";
        rlutil::setColor(15);
        std::cout << _format(message) << " . . .\n";
        rlutil::setColor(7);
    }
	_processes.push(std::chrono::high_resolution_clock::now());
}

double SingletonLogger::end() {
	if (_processes.size() == 0) {
		wrn("there is nothing to end");
		return 0.;
	}
	auto t = _processes.top();
	_processes.pop();
	auto res = _diff(t, std::chrono::high_resolution_clock::now());
    if (mute) return res;
    if (_rec) {
        _rec = false;
        std::cout << '\n';
    }
	rlutil::setColor(9);
	std::cout << tab() << "[end] ";
	rlutil::setColor(15);
	std::cout << std::fixed << std::setprecision(2) << res << ' ' << (timeUnits == TimeUnits::sec ? 's' : 'm') << '\n';
	rlutil::setColor(7);
	return res;
}

void SingletonLogger::wrn(std::string const & message) {
    if (mute) return;
	if (_rec) {
		_rec = false;
		std::cout << '\n';
	}
	rlutil::setColor(13); // color
	std::cout << tab() << "[wrn] ";
	rlutil::setColor(7);
	std::cout << _format(message) << '\n';
}

void SingletonLogger::wrn() {
    if (!mute) {
        if (_rec) {
            _rec = false;
            std::cout << '\n';
        }
        rlutil::setColor(13);
        std::cout << tab() << "[wrn] ";
        rlutil::setColor(7);
        std::cout << _format(buf.str()) << '\n';
    }
    // clear buf
	buf.str(std::string());
	buf.clear();
}

void SingletonLogger::err(std::string const & message) {
	if (_rec) {
		_rec = false;
		std::cout << '\n';
	}
	rlutil::setColor(12);
	std::cout << tab() << "[err] ";
	rlutil::setColor(7);
	std::cout << _format(message) << '\n';
}

void SingletonLogger::log(std::string const & message) {
	if (mute) return;
	if (_rec) {
		_rec = false;
		std::cout << '\n';
	}
	rlutil::setColor(11);
	std::cout << tab() << "[log] ";
	rlutil::setColor(7);
	std::cout << _format(message) << '\n';
}

void SingletonLogger::log() {
	if (!mute) {
        if (_rec) {
            _rec = false;
            std::cout << '\n';
        }
        rlutil::setColor(11);
        std::cout << tab() << "[log] ";
        rlutil::setColor(7);
        std::cout << _format(buf.str()) << '\n';
    }
	// clear buf
	buf.str(std::string());
	buf.clear();
}

void SingletonLogger::rec() {
	if (!mute) {
        _rec = true;
        rlutil::setColor(11);
        std::cout << '\r' << tab() << "[log] ";
        rlutil::setColor(7);
        std::cout << _format(buf.str());
    }
	// clear buf
	buf.str(std::string());
	buf.clear();
}

bool SingletonLogger::yes(std::string const & message) {
	if (_rec) {
		_rec = false;
		std::cout << '\n';
	}
	rlutil::setColor(10);
	std::cout << tab() << "[inp] ";
	rlutil::setColor(7);
	std::cout << _format(message);
	rlutil::setColor(15);
	std::cout  << " [y / n]";
	rlutil::setColor(7);
	std::cout << ": ";
	char flag;
	std::cin >> flag;
	// echo
	std::string choice = flag == 'y' ? "yes" : "no";
	std::cout << tab() << "   -> " << choice << '\n';
	// input buffer
	++_inputCounter;
	_inputValues << flag << '\n';
	_inputDescriptions << _inputCounter << ". " << message << '\n';
	if (flag == 'y') return true;
	return false;
}

size_t SingletonLogger::opt(std::string const & message, std::vector<std::string> const & choices) {
	if (choices.size() == 0) throw std::invalid_argument("empty option vector");
	if (choices.size() == 1) return 0;
	if (_rec) {
		_rec = false;
		std::cout << '\n';
	}
	size_t i;
	for (i = 0; i < choices.size(); ++i)
		buf << '(' << i << ") " << choices[i] << '\n';
	log();
	rlutil::setColor(10);
	std::cout << tab() << "[inp] ";
	rlutil::setColor(7);
	std::cout << _format(message);
	rlutil::setColor(15);
	buf << " [0, 1";
	if (choices.size() == 2) buf << "]";
	else if (choices.size() == 3) buf << ", 2]";
	else if (choices.size() == 4) buf << ", 2, 3]";
	else buf << ", ..., " << choices.size() - 1 << "]";
	std::cout << buf.str();
	// clear buf
	buf.str(std::string());
	buf.clear();
	rlutil::setColor(7);
	std::cout << ": ";
	std::cin >> i;
	if (i >= choices.size()) i = choices.size() - 1;
	// echo
	std::cout << tab() << "   -> " << choices[i] << '\n';
	// input buffer
	++_inputCounter;
	_inputValues << i << '\n';
	_inputDescriptions << _inputCounter << ". " << message << '\n';
	for (size_t j = 0; j < choices.size(); ++j) _inputDescriptions << "\t(" << j << ") " << choices[j] << '\n';
	return i;
}

void SingletonLogger::exp(std::string const & fname) {
	std::ofstream output(fname);
	output << _inputValues.str() << '\n' << _inputDescriptions.str();
	// clear
	_inputCounter = 0;
	_inputValues.str(std::string());
	_inputValues.clear();
	_inputDescriptions.str(std::string());
	_inputDescriptions.clear();
}

void SingletonLogger::pro(size_t currentState, size_t lastState) {
	if (mute) return;
	// if (currentState == 0)
	if (currentState == 1)
		_proStartingTime = std::chrono::high_resolution_clock::now();
	size_t percent = round((100. * currentState) / lastState);
	size_t numbOfBars = round(percent / 10.);
	std::stringstream bar;
	rlutil::setColor(14);
	std::cout << '\r' << tab() << "[pro] ";
	rlutil::setColor(7);
	std::cout << "[ ";
	size_t i;
	for (i = 0; i < numbOfBars; ++i) std::cout << "| ";
	for (; i < 10; ++i) std::cout << "  ";
	std::cout << "] " << std::setfill(' ') << std::setw(3) << percent << '%';

//	auto proCurrentTime = std::chrono::high_resolution_clock::now();
//	auto currentTimeDiff = _diff(_proStartingTime, proCurrentTime);
//	auto estEndingTime = 100. * currentTimeDiff/ percent;

	if (currentState == lastState) std::cout << '\n';
}