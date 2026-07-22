/*
*********************************************************************************************************
*
*	模块名称 : 指令通用模块
*	文件名称 : app_instruction_common.c
*	版    本 : V1.0
*	说    明 : .
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2025-04-08  Zhang  正式发布
*
*
*	Copyright (C), 2022-2032, 思埃然电子 Sierra Medical
*
*********************************************************************************************************
*/
/* 包含头文件 ----------------------------------------------------------------*/
#include "includes.h"
#include "math.h"

/* 私有类型定义 --------------------------------------------------------------*/



/* 私有宏定义 ----------------------------------------------------------------*/
#define FAL_BARCODE_USE  0U


/* 私有变量 ------------------------------------------------------------------*/


/* 私有函数原形 --------------------------------------------------------------*/

/* 函数体 --------------------------------------------------------------------*/


// 计算相关度（皮尔逊相关系数）
float m_correlation(float x[], float y[], int n)
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


/*
*********************************************************************************************************
*	函 数 名: Calib_linearFitting
*	功能说明: 一阶线性拟合函数
*	形    参: _usCMD
*	返 回 值: 无
*********************************************************************************************************
*/
float Calib_linearFitting(uint32_t n, float *x, float *y, CalibrationPara *p)
{
    float sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;

    for (uint32_t i = 0; i < n; i++) {
        sumX += x[i];
        sumY += y[i];
        sumXY += x[i] * y[i];
        sumXX += x[i] * x[i];
    }

    float denominator = (n * sumXX) - (sumX * sumX);
    p->A = ((n * sumXY) - (sumX * sumY)) / denominator; 	// 斜率a
    p->B = (sumY - (p->A * sumX)) / n;                   	// 截距b

    // 计算线性拟合模型的相关度
    float fitted_y[n];

    for (int i = 0; i < n; i++)
    {
        fitted_y[i] = (p->A) * x[i] + (p->B);
    }

    float correlation_value = m_correlation(y, fitted_y, n);
    return correlation_value;
}

/*
*********************************************************************************************************
*	函 数 名: polyfit
*	功能说明: 多项式拟合函数
*	形    参:
*	返 回 值: 无
*********************************************************************************************************
*/
int polyfit(const float* const dependentValues,
            const float* const independentValues,
            unsigned int		countOfElements,
            unsigned int        order,
            float*      		coefficients)
{
    // Declarations...
    // ----------------------------------
    enum {maxOrder = 5};

    float B[maxOrder + 1] = {0.0f};
    float P[((maxOrder + 1) * 2) +1] = {0.0f};
    float A[(maxOrder + 1) * 2 * (maxOrder + 1)] = {0.0f};

    float x, y, powx;

    unsigned int ii, jj, kk;

    // Verify initial conditions....
    // ----------------------------------

    // This method requires that the countOfElements >
    // (order+1)
    if (countOfElements <= order)
        return -1;

    // This method has imposed an arbitrary bound of
    // order <= maxOrder.  Increase maxOrder if necessary.
    if (order > maxOrder)
        return -1;

    // Begin Code...
    // ----------------------------------

    // Identify the column vector
    for (ii = 0; ii < countOfElements; ii++)
    {
        x    = dependentValues[ii];
        y    = independentValues[ii];
        powx = 1;

        for (jj = 0; jj < (order + 1); jj++)
        {
            B[jj] = B[jj] + (y * powx);
            powx  = powx * x;
        }
    }

    // Initialize the PowX array
    P[0] = countOfElements;

    // Compute the sum of the Powers of X
    for (ii = 0; ii < countOfElements; ii++)
    {
        x    = dependentValues[ii];
        powx = dependentValues[ii];

        for (jj = 1; jj < ((2 * (order + 1)) + 1); jj++)
        {
            P[jj] = P[jj] + powx;
            powx  = powx * x;
        }
    }

    // Initialize the reduction matrix
    //
    for (ii = 0; ii < (order + 1); ii++)
    {
        for (jj = 0; jj < (order + 1); jj++)
        {
            A[(ii * (2 * (order + 1))) + jj] = P[ii + jj];
        }

        A[(ii * (2 * (order + 1))) + (ii + (order + 1))] = 1;
    }

    // Move the Identity matrix portion of the redux matrix
    // to the left side (find the inverse of the left side
    // of the redux matrix
    for (ii = 0; ii < (order + 1); ii++)
    {
        x = A[(ii * (2 * (order + 1))) + ii];

        if (x != 0)
        {
            for (kk = 0; kk < (2 * (order + 1)); kk++)
            {
                A[(ii * (2 * (order + 1))) + kk] =
                    A[(ii * (2 * (order + 1))) + kk] / x;
            }

            for (jj = 0; jj < (order + 1); jj++)
            {
                if ((jj - ii) != 0)
                {
                    y = A[(jj * (2 * (order + 1))) + ii];

                    for (kk = 0; kk < (2 * (order + 1)); kk++)
                    {
                        A[(jj * (2 * (order + 1))) + kk] =
                            A[(jj * (2 * (order + 1))) + kk] -
                            y * A[(ii * (2 * (order + 1))) + kk];
                    }
                }
            }
        }
        else
        {
            // Cannot work with singular matrices
            return -1;
        }
    }

    // Calculate and Identify the coefficients
    for (ii = 0; ii < (order + 1); ii++)
    {
        for (jj = 0; jj < (order + 1); jj++)
        {
            x = 0;

            for (kk = 0; kk < (order + 1); kk++)
            {
                x = x + (A[(ii * (2 * (order + 1))) + (kk + (order + 1))] *
                         B[kk]);
            }

            coefficients[ii] = x;
        }
    }

    return 0;
}

// 计算拟合误差
float calculate_error(float x[], float y[], int n, float a, float b, float c) {
    float sum = 0.0;

    for (int i = 0; i < n; i++) {
        sum += pow(y[i] - (a * x[i] * x[i] + b * x[i] + c), 2);
    }

    return sqrt(sum / n);
}

/*
*********************************************************************************************************
*	函 数 名: calculate_Rsquard
*	功能说明: 计算拟合优度
*	形    参:
*	返 回 值: 无
*********************************************************************************************************
*/
float calculate_Rsquard(float x[], float y[], int n, float a, float b, float c) {
    float ssr = 0.0f;
    float sse = 0.0f;
    float sst = 0.0f;
    float ymean = 0.0f;

    for (int i = 0; i < n; i++) {
        ymean += y[i];
    }

    ymean = ymean / n;

    for (int i = 0; i < n; i++) {
        ssr += pow(x[i] - (a * x[i] * x[i] + b * x[i] + c), 2);
        sse += pow(y[i] - (a * x[i] * x[i] + b * x[i] + c), 2);
    }

    sst = ssr + sse;
    return (1.0f - fabs(sse / sst));
}



/*
*********************************************************************************************************
*	函 数 名: Flash_param_init
*	功能说明:
*	形    参: _usCMD
*	返 回 值: 无
*********************************************************************************************************
*/
void Flash_DiaParam_init(void)
{
    uint8_t *pdt;

    /*  dev 获取硬件二维码信息 */ 
    pdt = Flash_Task_Read("dev");
    Flash_DeviceInfo  *pDev = (Flash_DeviceInfo  *)pdt;

    if(NULL != pDev)
    {
        memcpy(&ramdata.serial, pdt, sizeof(barcode_t));
    }

    /* sensor 读取电凝校准信息 */ 
    pdt = Flash_Task_Read("sensor");
    struct Flash_Sensor  *pSensor = (struct Flash_Sensor  *)pdt;

    if(0x55AA == pSensor->_unuse )
    {
        memcpy(_dia_instance._cal, pdt, sizeof(struct Flash_Sensor) - 4);

        updata_time1_freq(_dia_instance._cal->_diaFreq);
    }
	else
	{
		bsp_dia_default();
	}


}

void Flash_PhacoParam_init(void)
{
    uint8_t *pdt;

    /* sensor 读取超乳校准信息 */ 
    pdt = Flash_Task_Read("user");
    struct Flash_User  *pUser = (struct Flash_User  *)pdt;

    if(0x55AA == pUser->_unuse)
    {
        memcpy(_phaco.cal_param_t._lcali_tbl, pdt, sizeof(struct Flash_User) - 4);
        _phaco.cal_param_t._flash_flag = 1;
    }
    else
    {
		phaco_set_default();
    }

}


