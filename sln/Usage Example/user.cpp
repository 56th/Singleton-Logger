#include "SingletonLogger.hpp"

int main() {
	SingletonLogger& logger = SingletonLogger::instance();
	logger.beg("main() started");
		logger.beg("division module");
			double x, y;
			logger.inp("enter real x", x);
			logger.inp("enter real y", y);
			if (y == 0.) logger.err("y cannot equal to zero");
			else logger.log("x / y = " + std::to_string(x / y));
		logger.end();
	logger.end();
	return 0;
}