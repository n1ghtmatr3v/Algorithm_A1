#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <fstream>

struct Circle {
    double x, y, r;
};

bool isInside(double x, double y, const Circle& c) {
    double dx = x - c.x;
    double dy = y - c.y;
    return dx * dx + dy * dy <= c.r * c.r;
}

double monteCarloArea(const std::vector<Circle>& circles, double min_x, double max_x,
    double min_y, double max_y, int N, std::mt19937& gen) {
    std::uniform_real_distribution<double> dist_x(min_x, max_x);
    std::uniform_real_distribution<double> dist_y(min_y, max_y);

    int count = 0;
    for (int i = 0; i < N; i++) {
        double x = dist_x(gen);
        double y = dist_y(gen);
        bool inside = true;
        for (const auto& c : circles) {
            if (!isInside(x, y, c)) {
                inside = false;
                break;
            }
        }
        if (inside) count++;
    }

    double area_rect = (max_x - min_x) * (max_y - min_y);
    return area_rect * count / N;
}

int main() {
    std::vector<Circle> circles = {
        {1.0, 1.0, 1.0},
        {1.5, 2.0, 1.118033988749895},
        {2.0, 1.5, 1.118033988749895}
    };

    const double pi = 3.14159265358979323846;
    const double exact_area = 0.25 * pi + 1.25 * asin(0.8) - 1.0;

    double wide_min_x = 0.0, wide_max_x = 3.0;
    double wide_min_y = 0.0, wide_max_y = 3.0;

    double narrow_min_x = 1.0, narrow_max_x = 2.0;
    double narrow_min_y = 1.0, narrow_max_y = 2.0;

    double large_min_x = -1.0, large_max_x = 4.0;
    double large_min_y = -1.0, large_max_y = 4.0;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::ofstream outfile("monte_carlo_results_large.csv");
    outfile << "N,wide_area,wide_relative_error,narrow_area,narrow_relative_error,large_area,large_relative_error\n";

    std::vector<int> test_points;

    for (int N = 100; N <= 100000; N += 500) {
        test_points.push_back(N);
    }

    int total_tests = test_points.size();
    int current_test = 0;

    for (int N : test_points) {
        double wide_area = monteCarloArea(circles, wide_min_x, wide_max_x,
            wide_min_y, wide_max_y, N, gen);
        double wide_error = std::abs(wide_area - exact_area) / exact_area;

        double narrow_area = monteCarloArea(circles, narrow_min_x, narrow_max_x,
            narrow_min_y, narrow_max_y, N, gen);
        double narrow_error = std::abs(narrow_area - exact_area) / exact_area;

        double large_area = monteCarloArea(circles, large_min_x, large_max_x,
            large_min_y, large_max_y, N, gen);
        double large_error = std::abs(large_area - exact_area) / exact_area;

        outfile << N << "," << wide_area << "," << wide_error << ","
            << narrow_area << "," << narrow_error << ","
            << large_area << "," << large_error << "\n";

        current_test++;
        if (current_test % 10 == 0) {
            std::cout << "Progress: " << current_test << "/" << total_tests
                << " (N = " << N << ")\n";
        }
    }

    outfile.close();
    std::cout << "Large dataset saved to monte_carlo_results_large.csv\n";
    std::cout << "Total tests: " << total_tests << "\n";
    std::cout << "Exact area: " << exact_area << "\n";

    return 0;
}