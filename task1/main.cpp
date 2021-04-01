#include <iostream>
#include <random>
#include <vector>
#include <future>

int in_circle(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    int points_in_circle = 0;
    for (auto i = 0; i < n; ++i) {
        double x = dist(gen);
        double y = dist(gen);
        if (x * x + y * y <= 1.0) {
            ++points_in_circle;
        }
    }
    return points_in_circle;
}

double r_pi_sequential(int p) {
    double all_points_inside = in_circle(p);
    double pi = ((double)all_points_inside / (double)p) * 4.0;
    return pi;
}

double r_pi_parallel(int p, int n) {
    int per_thread = p / n;
    std::vector<std::future<int>> futures;
    for (int t = 0; t < n; ++t) {
        futures.emplace_back(async(std::launch::async, in_circle, per_thread));
    }

    int all_points_inside = 0;
    for (std::future<int>& f : futures) {
        all_points_inside += f.get();
    }

    double pi = 4.0 * (double)all_points_inside / (double)p;
    return pi;

}

int main() {

    int p = 1000000;
    int n = 8;

    double pi_sequental = r_pi_sequential(p);
    double pi_parallel = r_pi_parallel(p, n);

    std::cout << "result in sequential version of the algorithm: " << pi_sequental << std::endl;
    std::cout << "result in parallel version of the algorithm: " << pi_parallel << std::endl;

    return EXIT_SUCCESS;
}