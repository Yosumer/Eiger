#include "bsp_pid.h"

/*  使用例子
	// 声明pid类
	struct PID pid;
	// 构造pid
    PID_Constructor(&pid);
    // 设置相关变量
    pid.SetCtrlPrm(&pid, 1.3, 0.005, 0.1);
    pid.SetIntegralPrm(&pid,10000,-10000);
	// 调用
	out = pid.PID(&pid,err);

*/
// 设置控制参数
static void _SetCtrPrm(struct PID *self, float kp, float ki, float kd)
{
    self->pri.kp = kp;
    self->pri.ki = ki;
    self->pri.kd = kd;
}

// 设置积分限幅
static void _SetIntegralPrm(struct PID *self, float integral_up, float integral_low)
{
    self->pri.integral_up = integral_up;
    self->pri.integral_low = integral_low;
}

// 比例控制器
static float _P(struct PID *self, float err)
{
    self->err = err;
    self->out = self->err * self->pri.kp;

    return self->out;
}

// 比例积分控制器
static float _PI(struct PID *self, float err)
{
    self->err = err;
    // kp*err+ki*err_sum
    self->out = self->pri.kp * self->err + self->pri.ki * self->pri.sum;

    // 误差积分
    self->pri.sum += self->err;

    // 积分限幅
    if (self->pri.sum > self->pri.integral_up) self->pri.sum = self->pri.integral_up;
    if (self->pri.sum < self->pri.integral_low) self->pri.sum = self->pri.integral_low;

    return self->out;
}

// PID控制器（位置式PID）
static float _PID(struct PID *self, float err)
{
    self->err = err;
    
    // kp*err+ki*err_sum + kd*(err - last_err)
    self->out = self->pri.kp * self->err + 
                self->pri.ki * self->pri.sum + 
                self->pri.kd*(self->err - self->pri.pre_err);

    // 误差积分
    self->pri.sum += self->err;

    // 积分限幅
    if (self->pri.sum > self->pri.integral_up) self->pri.sum = self->pri.integral_up;
    if (self->pri.sum < self->pri.integral_low) self->pri.sum = self->pri.integral_low;

    // 记录上次误差
    self->pri.pre_err = self->err;
    return self->out;
}


static float _IncPID(struct PID *self, float err)
{
    // 增量式PID
    self->err = err;
    // 增量式PID算法
    float increment_val = self->pri.kp * (self->err - self->pri.pre_err)+ 
                 self->pri.ki * self->err + 
                 self->pri.kd * (self->err - 2*self->pri.pre_err+self->pri.last_err);

    self->out += increment_val;
    self->pri.last_err = self->pri.pre_err;
    self->pri.pre_err = self->err;

    if (self->out > self->pri.integral_up) self->out = self->pri.integral_up;
    if (self->out < self->pri.integral_low) self->out = self->pri.integral_low;
    return self->out;
}

// 构造函数将接口绑定
void PID_Constructor(struct PID *self)
{
    self->SetCtrlPrm = _SetCtrPrm;
    self->SetIntegralPrm = _SetIntegralPrm;
    self->P = _P;
    self->PI = _PI;
    self->PID = _PID;
}

void PID_Constructor_Inc(struct PID *self)
{
    self->SetCtrlPrm = _SetCtrPrm;
    self->SetIntegralPrm = _SetIntegralPrm;
    memset(self->pri, 0, sizeof(self->pri));
    self->P = _IncPID;
    self->PI = _IncPID;
    self->PID = _IncPID;
}