# MS51_CAP
 MS51_CAP

1. Initial Input capture pin P1.2 (PWM0_CH0 , IC0) , to meausre input signal frequency and duty

2. under IC0 , use CAP0 to measure input signal RISINGEDGE , use CAP1 to measure input signal FALLINGEDGE 

3. check _debug_log_CAPTURE_ , for freq and duty calculation

CAP0 will get width0

CAP1 will get width1

input signal frequency will be : system clock / (width0 + width1 ) 

input signal duty will be : duty resolution * ( width1 / (width0 + width1 ) ) , with resolution set to 10000 

4. Initial PWM x 1 (P1.5 , PWM0_CH5) , with freq : 16K to simulate input signal

5. connect P1.5 (PWM) , to P12. (IC0)

6. check PWM0_CHx_Init function under task_10ms 

PWM0_CHx_Init(PWM_FREQ , 850) , will output 16K freq , 8.5 percent duty

PWM0_CHx_Init(PWM_FREQ , 7510) , will output 16K freq , 75.1 percent duty

below is screen capture , with terminal log message , and PWM channel scope measurement

8.5 percent duty

![image](https://github.com/released/MS51_CAP/blob/main/capture01.jpg)

75.1 percent duty

![image](https://github.com/released/MS51_CAP/blob/main/capture02.jpg)

