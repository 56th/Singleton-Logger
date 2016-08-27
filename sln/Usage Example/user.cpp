#include "SingletonLogger.hpp"

int main() {
	SingletonLogger& logger = SingletonLogger::instance();
	logger.beg("main() started");
		double x, y;
		logger.beg("division module");
			logger << "enter reals x, y: ";
			std::cin >> x >> y;
			if (y == 0.) logger.err("y cannot equal to zero");
			else logger << "x / y = " + std::to_string(x / y);
		logger.end();
	logger.end();
	return 0;
}