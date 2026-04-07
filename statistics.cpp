#include <iostream>
#include <limits>
#include <cmath>
#include <stdexcept>

class IStatistics {
public:
	virtual ~IStatistics() {}

	virtual void update(double next) = 0;
	virtual double eval() const = 0;
	virtual const char * name() const = 0;
};

class Min : public IStatistics {
public:
	Min() : m_min{std::numeric_limits<double>::max()} {
	}

	void update(double next) override {
		if (next < m_min) {
			m_min = next;
		}
		m_count++;
	}

	double eval() const override {
		if (!m_count) {
			throw std::invalid_argument("The sequence length must be greater than 0 \
				to calculate min");
		}
		return m_min;
	}

	const char * name() const override {
		return "min";
	}

private:
	double m_min;
	unsigned int m_count{0};
};

class Max : public IStatistics {
public:
	Max() : m_max{std::numeric_limits<double>::lowest()} {
	}

	void update(double next) override {
		if (next > m_max) {
			m_max = next;
		}
		m_count++;
	}

	double eval() const override {
		if (!m_count) {
			throw std::invalid_argument("The sequence length must be greater than 0 \
				to calculate max");
		}
		return m_max;
	}

	const char * name() const override {
		return "max";
	}

private:
	double m_max;
	unsigned int m_count{0};
};

class Mean : public IStatistics {
public:
	Mean() = default;

	void update(double next) override {
		m_sum_prev += next;
		m_num += 1;
	}

	double eval() const override {
		if (!m_num) {
			throw std::invalid_argument("The sequence length must be greater than 0 \
				to calculate mean");
		}
		return m_sum_prev / m_num;
	}

	const char * name() const override {
		return "mean";
	}

	unsigned int getNum() const {
		return m_num;
	}

private:
	double m_sum_prev{0.};
	unsigned int m_num{0};
};

class Std : public IStatistics {
public:
	Std() {
		m_meanStat = Mean();
	};

	void update(double next) override {
		m_meanStat.update(next);
		m_sum_prev_sq += ((next * next));
		m_num ++;
	}

	double eval() const override {
		if (!m_num) {
			throw std::invalid_argument("The sequence length must be greater than 0 \
				to calculate std");			
		}
		double mean = m_meanStat.eval();
		return std::sqrt(m_sum_prev_sq / m_num - (mean * mean));
	}

	const char * name() const override {
		return "std";
	}

private:
	Mean m_meanStat;
	double m_sum_prev_sq{0.};
	unsigned int m_num{0};
	double m_std{0.};
};



int main() {

	const size_t statistics_count = 4;
	IStatistics *statistics[statistics_count];

	statistics[0] = new Min{};
	statistics[1] = new Max{};
	statistics[2] = new Mean{};
	statistics[3] = new Std{};

	double val = 0;
	while (std::cin >> val) {
		for (size_t i = 0; i < statistics_count; ++i) {
			statistics[i]->update(val);
		}
	}

	// Handle invalid input data
	if (!std::cin.eof() && !std::cin.good()) {
		std::cerr << "Invalid input data\n";
		return 1;
	}

	// Print results if any
	for (size_t i = 0; i < statistics_count; ++i) {
		std::cout << statistics[i]->name() << " = " << statistics[i]->eval() << std::endl;
	}

	// Clear memory - delete all objects created by new
	for (size_t i = 0; i < statistics_count; ++i) {
		delete statistics[i];
	}

	return 0;
}