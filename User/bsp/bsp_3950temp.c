/*------------------------------------------------------------------------------
 * @file    BSP_3950TEMP.C
 * @author  FIRMWARE TEAM 
 * @date    2024/07/03 10:15:22
 * @brief   THIS FILE PROVIDES ALL THE FIRMWARE FUNCTIONS.
 * -----------------------------------------------------------------------------
 * @attention 
 
 * THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE 
 * CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE 
 * TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH 
 * HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN 
 * CONNECTION WITH PRODUCTS MADE BY CUSTOMERS. 
 * 
 * <H2><CENTER>&COPY; COPYRIGHT MINDMOTION </CENTER></H2> 
------------------------------------------------------------------------------*/

/* Define to prevent recursive inclusion  ------------------------------------*/


/* Files includes  -----------------------------------------------------------*/
#include "bsp_3950temp.h"



/*定义一个结构体：温度，电阻阻值*/
typedef struct{
   int32_t    temperature;//单位：°C
   float     resistance;//单位：Ω
}NTC_10KTYPE;

/*定义一个结构体类型数组：(温度-阻值)表 温度按降序填*/
const NTC_10KTYPE  ntc_10k_table[] = {
125,332.07, 	
124,340.96, 	
123,350.12,  	
122,359.57, 	
121,369.32, 	
120,379.37, 		
119,389.77, 
118,400.5,
117,411.58,
116,423.02,	
115,434.84, 	
114,447.04, 	
113,459.65,
112,472.68,
111,486.14,
110,500.05,
109,514.38,
108,529.2,
107,544.5,
106,560.32,
105,576.67,
104,593.58,
103,611.06,
102,629.13,
101,647.83,
100,667.17,
99,687.22,
98,707.99,
97,729.49,
96,751.76,
95,774.82,
94,798.71,
93,823.47,
92,849.12,
91,875.7,
90,903.26,
89,931.76,
88,961.31,
87,991.94,
86,1023.7,
85,1056.6,
84,1090.8,
83,1126.2,
82,1163,
81,1201.2,
80,1240.8,
79,1282.1,
78,1325,
77,1369.5,
76,1415.9,
75,1464,
74,1514.1,
73,1566.1,
72,1620.3,
71,1676.6,
70,1735.2,
69,1796.2,
68,1859.5,		
67,1925.5, 		
66,1994.2,	
65,2065.7, 	
64,2140.1, 	
63,2217.7, 	
62,2298.5, 	
61,2382.7, 	
60,2470.4, 	
59,2562.1, 	
58,2657.6, 	
57,2757.3, 	
56,2861.3, 	
55,2969.8, 	
54,3083,  	
53,3201.2, 	
52,3324.7, 	
51,3453.6, 	
50,3588.2, 	
49,3728.8, 	
48,3875.7, 	
47,4029.2, 	
46,4189.7, 	
45,4357.4, 	
44,4532.9, 		
43,4716.4, 		
42,4908.3, 	
41,5109.2, 	
40,5319.4, 	
39,5539.4, 	
38,5769.8,
37,6011.1,
36,6263.9,	
35,6528.7, 	
34,6806.2, 	
33,7097,	
32,7402,
31,7721.8, 	
30,8057.2, 	
29,8409.1, 	
28,8778.3, 	
27,9165.9, 	
26,9572.8, 	
25,10000,	
24,10449, 	
23,10920, 	
22,11415, 	
21,11936,  	
20,12483, 	
19,13058, 	
18,13662, 	
17,14297, 	
16,14965, 	
15,15667,	
14,16405, 	
13,17181, 	
12,17997, 	
11,18856, 	
10,19760, 	
9,20716, 	
8,21724,  	
7,22788, 	
6,23910,	
5,25095, 	
4,26346, 	
3,27669, 	
2,29066, 	
1,30544, 	
0,32108, 	
-1,33738, 	
-2,35456, 		
-3,37267, 	
-4,39175, 	
-5,41185, 	 
-6,43303, 	
-7,45535,	
-8,47885, 	
-9,50360, 		
-10,52966,
-11,55736,
-12,58659,
-13,61745,
-14,65003,
-15,68443,
-16,72075,
-17,75910,
-18,79961,
-19,84239, 		
-20,88758, 	
-21,93644,
-22,98839, 
-23,104370,
-24,110250, 
-25,116530, 	
-26,123210,
-27,130350,
-28,137960,
-29,146100,
-30,154800,
-31,164000, 	
-32,173810, 	
-33,184290, 	
-34,195490,
-35,207460, 	
-36,220260, 	
-37,233950, 	
-38,248610,	
-39,264320,	
-40,281160, 	
};

const uint16_t _size = sizeof(ntc_10k_table)/sizeof(ntc_10k_table[0]);
// 根据电阻查表
static float _bsp_3950checkTable(float _res)
{
    for(int16_t index=0;index<_size;index++)
    {
      	if(ntc_10k_table[index].resistance >= _res)/*查到该温度表索引为 index*/
      	{
         	float min_resistance = (float)ntc_10k_table[(index>0) ? index-1 : 0].resistance;/*得出下区间电阻值*/
         
         	float max_resistance = (float)ntc_10k_table[index].resistance;/*得出上区间电阻值*/
         
         	float d1 = (float)(max_resistance - min_resistance) / 10;/*将该区间的阻值分为10等分*/
         
         	float d2 = (float)(((_res - min_resistance) / d1) *(1/ 10));/*得出n份*0.1°C*/
        
         	/*以上四步是求小数点温度*/
         	return (float)ntc_10k_table[index].temperature + d2;
      	}
    }
}
/*----------------------------------------------------------------------------*/
void  bsp_3950temp_Init(bsp_3950temp_t *_this,float (*_get_res)())
{
    _this->get_res = _get_res;
    _this->_temp = (_bsp_3950checkTable(_this->get_res()));
}
float bsp_3950temp_Read(bsp_3950temp_t *_this)
{
    _this->_temp = (_bsp_3950checkTable(_this->get_res()));
    return _this->_temp;
}

