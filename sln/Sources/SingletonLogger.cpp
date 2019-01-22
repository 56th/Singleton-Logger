#include <stdexcept>
#include <iomanip> // setprecision
#include "SingletonLogger.hpp"

SingletonLogger::SingletonLogger() : _inputCounter(0), mute(false) {
	rlutil::saveDefaultColor();
	rlutil::setBackgroundColor(0);
	//for (int i = 0; i < 16; i++) {
	//	rlutil::setColor(i);
	//	std::cout << i << " ";
	//}
}

SingletonLogger::~SingletonLogger() {
	rlutil::resetColor();
}

double SingletonLogger::_diff(clock_t t1, clock_t t2) const {
	double diff = fabs(t2 - t1) / CLOCKS_PER_SEC;
	if (timeUnits == SingletonLoggerTimeUnits::min) diff /= 60.;
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
	if (mute) return;
	if (_rec) {
		_rec = false;
		std::cout << '\n';
	}
	rlutil::setColor(9);
	std::cout  << tab() << "[beg] ";
	rlutil::setColor(15);
	std::cout << _format(message) << " . . .\n";
	rlutil::setColor(7);
	_processes.push(clock());
}

void SingletonLogger::end() {
	if (mute) return;
	if (_rec) {
		_rec = false;
		std::cout << '\n';
	}
	if (_processes.size() == 0) {
		wrn("there is nothing to end");
		return;
	}
	clock_t t = _processes.top();
	_processes.pop();
	rlutil::setColor(9);
	std::cout << tab() << "[end] ";
	rlutil::setColor(15);
	std::cout << std::fixed << std::setprecision(2) << _diff(clock(), t) << ' ' << (timeUnits == SingletonLoggerTimeUnits::sec ? 's' : 'm') << '\n';
	rlutil::setColor(7);
}

void SingletonLogger::wrn(std::string const & message) {
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
	if (_rec) {
		_rec = false;
		std::cout << '\n';
	}
	rlutil::setColor(13);
	std::cout << tab() << "[wrn] ";
	rlutil::setColor(7);
	std::cout << _format(buf.str()) << '\n';
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
	if (mute) {
		buf.str(std::string());
		buf.clear();
		return;
	}
	if (_rec) {
		_rec = false;
		std::cout << '\n';
	}
	rlutil::setColor(11);
	std::cout << tab() << "[log] ";
	rlutil::setColor(7);
	std::cout << _format(buf.str()) << '\n';
	// clear buf
	buf.str(std::string());
	buf.clear();
}

void SingletonLogger::rec() {
	if (mute) {
		buf.str(std::string());
		buf.clear();
		return;
	}
	_rec = true;
	rlutil::setColor(11);
	std::cout << '\r' << tab() << "[log] ";
	rlutil::setColor(7);
	std::cout << _format(buf.str());
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