#include "bsp_prsurSen.h"

#include "includes.h"
extern Data_Block  ramdata; 




uint16_t readAD(PressureChannel channel)
{
    switch (channel)
    {
//        case INFUSION_B1:
//            return ramdata.ADCTables.ch.infusion_b1;
//            break;
//        case INFUSION_FSP:
//            return ramdata.ADCTables.ch.infusion_fsp;
//            break;
//        case INFUSION_A1:
//            return ramdata.ADCTables.ch.infusion_a1;
//            break;
//        case INFUSION_A2:
//            return ramdata.ADCTables.ch.infusion_a2;
//            break;
//        case INFUSION_BUS:
//            return ramdata.ADCTables.ch.infusion_bus;
//            break;
//        case INFUSION_B2:
//            return ramdata.ADCTables.ch.infusion_b2;
//            break;
//        case SUCTION_VACCUM:
//            return ramdata.ADCTables.ch.suction_vacum;
//            break;
//        case SUCTION_REFLUX:
//            return ramdata.ADCTables.ch.suction_refluex;
//            break;
        default:
            break;
    }
}

PressureSensor_t SensorTable[SENSOR_MAX]; 

 CalibrationPara CaliTable[SENSOR_MAX] = 
 {
    {
        .A = 0.007485,
        .B = -64.923,
        .C = 0,
    },  //INFUSION_B1
    {
        .A = 0.007388,
        .B = -63.1428,
        .C = 0,
    },  //INFUSION_FSP
    {
        .A = 0.0075,
        .B = -65.224,
        .C = 0,
    },  //INFUSION_A1

 };

void bsp_prsurSen_Init(CalibrationPara * caliTable)
{
    for(int i=0;i<SENSOR_MAX;i++)
    {
        SensorTable[i].channel = i;
        bsp_prsureSen_Init(&SensorTable[i],readAD);
        if(caliTable == NULL)   //使用默认参数进行校准
        {
            // 设置测量拟合参数
            SensorTable[i].calib = CaliTable[i]; //斜率为1
        }
        else    //使用ram中的参数进行校准
        {
            SensorTable[i].calib = *caliTable;
            caliTable++;
        }
        bsp_prsureSen_SetCalib(&SensorTable[i],SensorTable[i].calib);
    }
}


// 计算相关度（皮尔逊相关系数）
float correlation(float x[], float y[], int n)
{
    float sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0, sum_y2 = 0.0;

    // 计算各项累加和
    for (int i = 0; i < n; i++)
    {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sum_x2 += x[i] * x[i];
        sum_y2 += y[i] * y[i];
    }

    float mean_x = sum_x / n; // x 的均值
    float mean_y = sum_y / n; // y 的均值

    // 计算相关系数
    float numerator = (n * sum_xy) - (sum_x * sum_y);
    float denominator = sqrt((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));

    if (denominator == 0) {
        return 0; // 避免除以零错误
    }
    return numerator / denominator;
}

// 线性拟合函数
float bsp_prsurSen_LinearFit(float x[],float y[],int n,float *A,float *B)
{
    float sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0;

    // 计算各项累加和
    for (int i = 0; i < n; i++)
    {
        sum_x += x[i];
        sum_y += y[i];
        sum_xy += x[i] * y[i];
        sum_x2 += x[i] * x[i];
    }

    // 计算斜率和截距
    *A = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x);
    *B = (sum_y - (*A) * sum_x) / n;

    // 计算线性拟合模型的相关度
    float fitted_y[n];
    for (int i = 0; i < n; i++)
    {
        fitted_y[i] = (*A) * x[i] + (*B);
    }
    float correlation_value = correlation(y, fitted_y, n);

    // 输出结果
    rt_kprintf(" LinearFit correlation:%.5f\n", correlation_value);

    return correlation_value;
}



