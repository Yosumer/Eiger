#ifndef __BSP_PID_H
#define __BSP_PID_H
struct  PID
{
    /*  set  kp  ki  kd  */
    void  (*  SetCtrlPrm)(struct  PID  *self,  float  kp,  float  ki,  float  kd);
    /*  set  integral  range  */
    void  (*  SetIntegralPrm)(struct  PID  *self,  float  integral_up,  float  integral_low);
    /*  Controller  interface  */
    float  (*  P)(struct  PID  *self,  float  err);
    float  (*  PI)(struct  PID  *self,  float  err);
    float  (*  PID)(struct  PID  *self,  float  err);

    /*  in  value  */
    float  err;
    
    /*  out  value  */
    float  out;
    
    /*  private  */
    struct  
    {
        float  kp;
        float  ki;
        float  kd;
        
        float  integral_up;
        float  integral_low;
        float  sum;
        float  pre_err;     // Error[n-1]
        float  last_err;    // Error[n-2]
    }pri;
};

//  Object  construction
void  PID_Constructor(struct  PID  *self);

void  PID_Constructor_Inc(struct PID *self);    //增量式PID
#endif