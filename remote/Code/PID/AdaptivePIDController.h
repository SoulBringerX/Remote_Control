// AdaptivePIDController.h
#ifndef ADAPTIVEPIDCONTROLLER_H
#define ADAPTIVEPIDCONTROLLER_H

#include <cmath>
#include <algorithm>

class AdaptivePIDController {
public:
    AdaptivePIDController(double Kp, double Ki, double Kd, double alpha,
                         double Kp_min = 0.1, double Ki_min = 0.05,
                         double integral_max = 100.0)
        : Kp(Kp), Ki(Ki), Kd(Kd), alpha(alpha),
          Kp_min(Kp_min), Ki_min(Ki_min),
          integral_max(integral_max){}

    double compute(double error, double dt);
    void adjustParameters(double error);
    void reset();

private:
    double Kp, Ki, Kd;
    double alpha;
    double Kp_min, Ki_min;
    double integral_max;

    double prev_error = 0;
    double integral = 0;
    double prev_derivative = 0;
};

#endif // ADAPTIVEPIDCONTROLLER_H
