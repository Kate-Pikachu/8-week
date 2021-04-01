#include <iostream>
#include <random>
#include <vector>
#include <mutex>
#include <thread>



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

void Ppi(int n, double& W) {

    double a, b;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);
    std::uniform_real_distribution<> dist_2(a, b);

    int points_in_circle = 0;

    for (auto i = 0; i < n; ++i) {

        double x = dist_2(gen);
        double y = dist(gen);

        if (x * x + y * y <= 1.0) {

            ++points_in_circle;
        }

    }

    W += points_in_circle;

}

double r_pi_parallel(int p, int n) {

    int per_thread = p / n;
    std::vector<std::thread> vec(per_thread);
    double all_points_inside = 0;

    for (auto t = 0; t < n; ++t) {

        vec[t] = std::thread(Ppi, (double)t / (double)n,
            (double)t + 1 / (double)n,
            per_thread, std::ref(all_points_inside));

    }

    for (auto i = 0; i < n; ++i) {
        vec[i].join();
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