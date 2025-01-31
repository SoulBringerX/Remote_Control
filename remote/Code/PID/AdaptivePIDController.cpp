#include "AdaptivePIDController.h"

// 计算PID输出
double AdaptivePIDController::compute(double error, double dt) {
    // 更新积分项，加入抗积分饱和限制
    integral += error * dt;
    integral = std::clamp(integral, -integral_max, integral_max); // 限制积分项，防止积分饱和

    // 计算微分项
    double derivative = (error - prev_error) / dt;
    prev_error = error;

    // 使用PID公式计算输出
    double output = Kp * error + Ki * integral + Kd * derivative;

    // 非线性映射：使用tanh函数进行输出平滑处理
    return alpha * std::tanh(output);
}

// 动态调整PID参数
void AdaptivePIDController::adjustParameters(double error) {
    if (std::abs(error) > 0.1) {
        // 如果误差较大，增加比例项Kp，减少积分项Ki，避免系统过度超调
        Kp = std::max(Kp * 1.2, Kp_min); // 比例项增加，设定最小值限制
        Ki = std::max(Ki * 0.8, Ki_min); // 积分项减少，设定最小值限制
    } else {
        // 如果误差较小，减小比例项Kp，增加积分项Ki，帮助消除稳态误差
        Kp *= 0.8; // 比例项减少
        Ki *= 1.2; // 积分项增加
    }
}

// 重置PID控制器状态
void AdaptivePIDController::reset() {
    prev_error = 0;   // 重置上次误差
    integral = 0;     // 重置积分项
    prev_derivative = 0; // 重置上次微分项（此变量在当前实现中未使用，但可以扩展）
}
