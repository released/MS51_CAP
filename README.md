# MS51_CAP
 MS51_CAP


update @ 2021/06/28

1. modify to support low speed freq (ex 10Hz ~ 1000Hz)

	- with origrinal setting (TIMER2 = sys clock , 24MHz) , will cause over flow if freq too slow

	- change TIMER 2 clock div to 64 , and clear TIMER2 flag with TIMER2 interrupt (optional)
	
	- add define (ENABLE_CAP_IRQ , ENABLE_CAP_POLLING) , to enable capture function by interrupt or polling
	
		- need to set capture function to high priority , with timer interrupt enable
	
	- move width variable from local to global
	
	- set TIMER 2 clock with define

	- chage PWM clock div 64 , to support out PWM freq to 10Hz , for test

frequency calculation

![image](https://github.com/released/MS51_CAP/blob/main/freq_calculation.jpg)



below is test capture screen

10Hz

![image](https://github.com/released/MS51_CAP/blob/main/capture03_10Hz.jpg)


1000Hz

![image](https://github.com/released/MS51_CAP/blob/main/capture04_1000Hz.jpg)


====================================

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

