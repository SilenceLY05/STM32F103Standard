/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-指南者 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "bsp_systick.h"
#include "bsp_usart.h"
#include "bsp_led.h"
#include "bsp_i2c.h"
#include "bsp_EXTI.h"

//设置是否使用LCD进行显示，不需要的话把这个宏注释掉即可
#define USE_LCD_DISPLAY

#ifdef USE_LCD_DISPLAY
 #include "./lcd/bsp_ili9341_lcd.h"
#endif


#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "invensense.h"
#include "invensense_adv.h"
#include "eMPL_outputs.h"
#include "mltypes.h"
#include "mpu.h"
#include "log.h"
#include "packet.h"

#define TASK_ENABLE  0
extern unsigned int Task_Delay[NumOfTask];

/* Private typedef -----------------------------------------------------------*/
/* Data read from MPL. */
#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)
#define PRINT_COMPASS   (0x08)
#define PRINT_EULER     (0x10)
#define PRINT_ROT_MAT   (0x20)
#define PRINT_HEADING   (0x40)
#define PRINT_PEDO      (0x80)
#define PRINT_LINEAR_ACCEL (0x100)
#define PRINT_GRAVITY_VECTOR (0x200)

volatile uint32_t hal_timestamp = 0;
#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)
#define COMPASS_ON      (0x04)

#define MOTION          (0)
#define NO_MOTION       (1)

/* Starting sampling rate. */
#define DEFAULT_MPU_HZ  (20)

#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)

#define PEDO_READ_MS    (1000)
#define TEMP_READ_MS    (500)
#define COMPASS_READ_MS (100)
struct rx_s {
    unsigned char header[3];
    unsigned char cmd;
};
struct hal_s {
    unsigned char lp_accel_mode;
    unsigned char sensors;
    unsigned char dmp_on;
    unsigned char wait_for_tap;
    volatile unsigned char new_gyro;
    unsigned char motion_int_mode;
    unsigned long no_dmp_hz;
    unsigned long next_pedo_ms;
    unsigned long next_temp_ms;
    unsigned long next_compass_ms;
    unsigned int report;
    unsigned short dmp_features;
    struct rx_s rx;
};
static struct hal_s hal = {0};

/* USB RX二进制信号量。实际上，它只是一个flag。没有包含在struct中，因为它在其他地方声明为extern。*/
volatile unsigned char rx_new;

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";

/* 特定于平台的信息. Kinda like a boardfile. */
struct platform_data_s {
    signed char orientation[9];
};

/* 传感器可以以任何方向安装在电路板上。下面的安装矩阵告诉MPL如何旋转来自驱动程序的原始数据。
待办事项:以下矩阵参考Invensense内部测试板上的配置。如果需要，请修改矩阵以匹配您的特定设置的芯片到身体矩阵。 */
static struct platform_data_s gyro_pdata = {
    .orientation = { 1, 0, 0,
                     0, 1, 0,
                     0, 0, 1}
};

#if defined MPU9150 || defined MPU9250
static struct platform_data_s compass_pdata = {
    .orientation = { 0, 1, 0,
                     1, 0, 0,
                     0, 0, -1}
};
#define COMPASS_ENABLED 1
#elif defined AK8975_SECONDARY
static struct platform_data_s compass_pdata = {
    .orientation = {-1, 0, 0,
                     0, 1, 0,
                     0, 0,-1}
};
#define COMPASS_ENABLED 1
#elif defined AK8963_SECONDARY
static struct platform_data_s compass_pdata = {
    .orientation = {-1, 0, 0,
                     0,-1, 0,
                     0, 0, 1}
};
#define COMPASS_ENABLED 1
#endif


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* ---------------------------------------------------------------------------*/

/* 控制串口发送1个字符 c：要发送的字符 */
void usart1_send_char(uint8_t c)
{
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);  //循环发送，直到发送完毕
	USART_SendData(DEBUG_USARTx,c);
}

/* 按“匿名四轴”上位机软件的协议发送数据 c:要发送的字符 fun:功能字，0xA0~0xAF data：数据缓存区，最多28字节 len:Data区有效数据个数 */
void usart1_niming_report(uint8_t fun,uint8_t *data,uint8_t len)
{
	uint8_t send_buf[32];
	uint8_t i;
	if(len>28)return;                                          //最多28字节数据
	send_buf[len+3] = 0;                                       //校验数置零
	send_buf[0] = 0x88;                                        //帧头
	send_buf[1] = fun;                                         //功能字
	send_buf[2] = len;                                         //数据长度
	for(i=0;i<len;i++)send_buf[3+i] = data[i];                 //复制数据
	for(i=0;i<len+3;i++)send_buf[len+3] = send_buf[i];         //计算校验和
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);         //发送数据到串口1 
}

/* 发送加速度传感器数据和陀螺仪原始数据 aacx,aacy,aacz:x,y,z三个方向上面的加速度值 gyrox,gyroy,gyroz:x,y,z三个方向上的陀螺仪值 */
void mpu6050_send_data(short accx,short accy,short accz,short gyrox,short gyroy,short gyroz)
{
	uint8_t tbuf[12];
	tbuf[0] = (accx>>8)&0xFF;
	tbuf[1] = accx&0xFF;
	tbuf[2] = (accy>>8)&0xFF;
	tbuf[3] = accy&0xFF;
	tbuf[4] = (accz>>8)&0xFF;
	tbuf[5] = accz&0xFF;
	tbuf[6] = (gyrox>>8)&0xFF;
	tbuf[7] = gyrox&0xFF;
	tbuf[8] = (gyroy>>8)&0xFF;
	tbuf[9] = gyroy&0xFF;
	tbuf[10] = (gyroz>>8)&0xFF;
	tbuf[11] = gyroz&0xFF;
	usart1_niming_report(0xA1,tbuf,12);//自定义帧，0xA1
}

/* 发送姿态数据给上位机 aacx,aacy,aacz:x,y,z三个方向上面的加速度值 gyrox,gyroy,gyroz:x,y,z三个方向上的陀螺仪值
	 roll：横滚角 单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
	 pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
	 yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度 */
void usart1_report_imu(short accx,short accy,short accz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	uint8_t tbuf[28];
	uint8_t i;
	for(i=0;i<28;i++)tbuf[i] = 0;//清0
	tbuf[0] = (accx>>8)&0xFF;
	tbuf[1] = accx&0xFF;
	tbuf[2] = (accy>>8)&0xFF;
	tbuf[3] = accy&0xFF;
	tbuf[4] = (accz>>8)&0xFF;
	tbuf[5] = accz&0xFF;
	tbuf[6] = (gyrox>>8)&0xFF;
	tbuf[7] = gyrox&0xFF;
	tbuf[8] = (gyroy>>8)&0xFF;
	tbuf[9] = gyroy&0xFF;
	tbuf[10] = (gyroz>>8)&0xFF;
	tbuf[11] = gyroz&0xFF;
	tbuf[18] = (roll>>8)&0xFF;
	tbuf[19] = roll&0xFF;
	tbuf[20] = (pitch>>8)&0xFF;
	tbuf[21] = pitch&0xFF;
	tbuf[22] = (yaw>>8)&0xFF;
	tbuf[23] = yaw&0xFF;
	usart1_niming_report(0xAF,tbuf,28);//飞控显示帧，0xAF
}

extern struct inv_sensor_cal_t sensors;


/* 从MPL获取数据。
	 待办事项:向inv_get_sensor_type_xxx api添加返回值，以区分新数据和旧数据。 */
static void read_from_mpl(void)
{
    long msg, data[9];
    int8_t accuracy;
    unsigned long timestamp;
    float float_data[3] = {0};

		MPU_DEBUG_FUNC();
    if (inv_get_sensor_type_quat(data, &accuracy, (inv_time_t*)&timestamp)) {
       /* 向PC发送一个四元数报文。由于Python测试应用程序使用它来直观地表示3D四元数，因此每次MPL有新数据时都会发送它 */
        eMPL_send_quat(data);

        /* 可以使用USB命令发送或抑制特定的数据包。 */
        if (hal.report & PRINT_QUAT)
            eMPL_send_data(PACKET_DATA_QUAT, data);
    }

    if (hal.report & PRINT_ACCEL) {
        if (inv_get_sensor_type_accel(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_ACCEL, data);
    }
    if (hal.report & PRINT_GYRO) {
        if (inv_get_sensor_type_gyro(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_GYRO, data);
    }
#ifdef COMPASS_ENABLED
    if (hal.report & PRINT_COMPASS) {
        if (inv_get_sensor_type_compass(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_COMPASS, data);
    }
#endif
    if (hal.report & PRINT_EULER) {
        if (inv_get_sensor_type_euler(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_EULER, data);
    }
		
/************************** 使用液晶显示屏显示数据 ******************************/
#ifdef USE_LCD_DISPLAY
		
		if(1)
		{
			char cStr[70];
			unsigned long timestamp,step_count,walk_time;
			
			/* 获取欧拉角 */
			if(inv_get_sensor_type_euler(data,&accuracy,(inv_time_t*)&timestamp))
			{
				sprintf(cStr,"Pitch: %.4f ",data[0]*1.0/(1<<16));   //inv_get_sensor_type_euler读出的数据是Q16格式，所以左移16位
				ILI9341_DispStringLine_EN(LINE(5),cStr);
				
				sprintf(cStr,"Roll: %.4f ",data[1]*1.0/(1<<16));   //inv_get_sensor_type_euler读出的数据是Q16格式，所以左移16位
				ILI9341_DispStringLine_EN(LINE(7),cStr);
				
				sprintf(cStr,"Yaw: %.4f ",data[2]*1.0/(1<<16));   //inv_get_sensor_type_euler读出的数据是Q16格式，所以左移16位
				ILI9341_DispStringLine_EN(LINE(9),cStr);
				
				/* 温度数据 */
				mpu_get_temperature(data,(inv_time_t*)&timestamp);
				sprintf(cStr,"Temperature: %.2f ",data[0]*1.0/(1<<16));   //inv_get_sensor_type_euler读出的数据是Q16格式，所以左移16位
				ILI9341_DispStringLine_EN(LINE(11),cStr);
			}
			
			/* 获取步数 */
			get_tick_count(&timestamp);
			if(timestamp>hal.next_pedo_ms)
			{
				hal.next_pedo_ms = timestamp + PEDO_READ_MS;
				dmp_get_pedometer_step_count(&step_count);
				dmp_get_pedometer_walk_time(&walk_time);
				
				sprintf(cStr,"Walked steps: %ld steps over  %ld milliseconds ",step_count,walk_time);   //inv_get_sensor_type_euler读出的数据是Q16格式，所以左移16位
				ILI9341_DispStringLine_EN(LINE(13),cStr);
			}
		}
		
#endif
    if (hal.report & PRINT_ROT_MAT) {
        if (inv_get_sensor_type_rot_mat(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_ROT, data);
    }
    if (hal.report & PRINT_HEADING) {
        if (inv_get_sensor_type_heading(data, &accuracy,
            (inv_time_t*)&timestamp))
            eMPL_send_data(PACKET_DATA_HEADING, data);
    }
    if (hal.report & PRINT_LINEAR_ACCEL) {
        if (inv_get_sensor_type_linear_acceleration(float_data, &accuracy, (inv_time_t*)&timestamp)) {
        	MPL_LOGI("Linear Accel: %7.5f %7.5f %7.5f\r\n",
        			float_data[0], float_data[1], float_data[2]);                                        
         }
    }
    if (hal.report & PRINT_GRAVITY_VECTOR) {
            if (inv_get_sensor_type_gravity(float_data, &accuracy,
                (inv_time_t*)&timestamp))
            	MPL_LOGI("Gravity Vector: %7.5f %7.5f %7.5f\r\n",
            			float_data[0], float_data[1], float_data[2]);
    }
    if (hal.report & PRINT_PEDO) {
        unsigned long timestamp;
        get_tick_count(&timestamp);
        if (timestamp > hal.next_pedo_ms) {
            hal.next_pedo_ms = timestamp + PEDO_READ_MS;
            unsigned long step_count, walk_time;
            dmp_get_pedometer_step_count(&step_count);
            dmp_get_pedometer_walk_time(&walk_time);
            MPL_LOGI("Walked %ld steps over %ld milliseconds..\n", step_count,
            walk_time);
        }
    }

    /* 每当MPL检测到运动状态的变化时，应用程序就会收到通知。在本例中，我们使用LED来表示当前的运动状态。*/
    msg = inv_get_message_level_0(INV_MSG_MOTION_EVENT |
            INV_MSG_NO_MOTION_EVENT);
    if (msg) {
        if (msg & INV_MSG_MOTION_EVENT) {
            MPL_LOGI("Motion!\n");
        } else if (msg & INV_MSG_NO_MOTION_EVENT) {
            MPL_LOGI("No motion!\n");
        }
    }
}

#ifdef COMPASS_ENABLED
void send_status_compass() {
	long data[3] = { 0 };
	int8_t accuracy = { 0 };
	unsigned long timestamp;
	inv_get_compass_set(data, &accuracy, (inv_time_t*) &timestamp);
	MPL_LOGI("Compass: %7.4f %7.4f %7.4f ",
			data[0]/65536.f, data[1]/65536.f, data[2]/65536.f);
	MPL_LOGI("Accuracy= %d\r\n", accuracy);

}
#endif

/* 处理传感器开/关组合 */
static void setup_gyro(void)
{
    unsigned char mask = 0, lp_accel_was_on = 0;
	
		MPU_DEBUG_FUNC();
    if (hal.sensors & ACCEL_ON)
        mask |= INV_XYZ_ACCEL;
    if (hal.sensors & GYRO_ON) {
        mask |= INV_XYZ_GYRO;
        lp_accel_was_on |= hal.lp_accel_mode;
    }
#ifdef COMPASS_ENABLED
    if (hal.sensors & COMPASS_ON) {
        mask |= INV_XYZ_COMPASS;
        lp_accel_was_on |= hal.lp_accel_mode;
    }
#endif
    /* 如果您需要功率转换，则应该在传感器的掩码仍然启用的情况下调用此函数。驱动程序关闭任何排除在这个掩码之外的传感器。 */
    mpu_set_sensors(mask);
    mpu_configure_fifo(mask);
    if (lp_accel_was_on) {
        unsigned short rate;
        hal.lp_accel_mode = 0;
        /* 切换出LP加速，通知MPL新的加速采样率. */
        mpu_get_sample_rate(&rate);
        inv_set_accel_sample_rate(1000000L / rate);
    }
}

static void tap_cb(unsigned char direction, unsigned char count)
{
	MPU_DEBUG_FUNC();
    switch (direction) {
    case TAP_X_UP:
        MPL_LOGI("Tap X+ ");
        break;
    case TAP_X_DOWN:
        MPL_LOGI("Tap X- ");
        break;
    case TAP_Y_UP:
        MPL_LOGI("Tap Y+ ");
        break;
    case TAP_Y_DOWN:
        MPL_LOGI("Tap Y- ");
        break;
    case TAP_Z_UP:
        MPL_LOGI("Tap Z+ ");
        break;
    case TAP_Z_DOWN:
        MPL_LOGI("Tap Z- ");
        break;
    default:
        return;
    }
    MPL_LOGI("x%d\n", count);
    return;
}

static void android_orient_cb(unsigned char orientation)
{
	MPU_DEBUG_FUNC();
	switch (orientation) {
	case ANDROID_ORIENT_PORTRAIT:
        MPL_LOGI("Portrait\n");
        break;
	case ANDROID_ORIENT_LANDSCAPE:
        MPL_LOGI("Landscape\n");
        break;
	case ANDROID_ORIENT_REVERSE_PORTRAIT:
        MPL_LOGI("Reverse Portrait\n");
        break;
	case ANDROID_ORIENT_REVERSE_LANDSCAPE:
        MPL_LOGI("Reverse Landscape\n");
        break;
	default:
		return;
	}
}


static inline void run_self_test(void)
{
    int result;
    long gyro[3], accel[3];
		MPU_DEBUG_FUNC();
#if defined (MPU6500) || defined (MPU9250)
    result = mpu_run_6500_self_test(gyro, accel, 0);
#elif defined (MPU6050) || defined (MPU9150)
    result = mpu_run_self_test(gyro, accel);
#endif
    if (result == 0x7) {
	MPL_LOGI("Passed!\n");
        MPL_LOGI("accel: %7.4f %7.4f %7.4f\n",
                    accel[0]/65536.f,
                    accel[1]/65536.f,
                    accel[2]/65536.f);
        MPL_LOGI("gyro: %7.4f %7.4f %7.4f\n",
                    gyro[0]/65536.f,
                    gyro[1]/65536.f,
                    gyro[2]/65536.f);
        /* 测试通过。我们可以相信这里的陀螺数据，所以现在我们需要更新校准数据 */

#ifdef USE_CAL_HW_REGISTERS
        /* 这部分代码使用MPUxxxx设备中的HW偏移寄存器，而不是将调用数据推送到MPL软件库 */
        unsigned char i = 0;

        for(i = 0; i<3; i++) {
        	gyro[i] = (long)(gyro[i] * 32.8f); //convert to +-1000dps
        	accel[i] *= 2048.f; //convert to +-16G
        	accel[i] = accel[i] >> 16;
        	gyro[i] = (long)(gyro[i] >> 16);
        }

        mpu_set_gyro_bias_reg(gyro);

#if defined (MPU6500) || defined (MPU9250)
        mpu_set_accel_bias_6500_reg(accel);
#elif defined (MPU6050) || defined (MPU9150)
        mpu_set_accel_bias_6050_reg(accel);
#endif
#else
        /* 将校准数据推送到MPL库。
					 MPL期望硬件单元<< 16的偏差，但自检返回
					 偏差在g << 16。*/
    	unsigned short accel_sens;
    	float gyro_sens;

		mpu_get_accel_sens(&accel_sens);
		accel[0] *= accel_sens;
		accel[1] *= accel_sens;
		accel[2] *= accel_sens;
		inv_set_accel_bias(accel, 3);
		mpu_get_gyro_sens(&gyro_sens);
		gyro[0] = (long) (gyro[0] * gyro_sens);
		gyro[1] = (long) (gyro[1] * gyro_sens);
		gyro[2] = (long) (gyro[2] * gyro_sens);
		inv_set_gyro_bias(gyro, 3);
#endif
    }
    else {
            if (!(result & 0x1))
                MPL_LOGE("Gyro failed.\n");
            if (!(result & 0x2))
                MPL_LOGE("Accel failed.\n");
            if (!(result & 0x4))
                MPL_LOGE("Compass failed.\n");
     }

}

static void handle_input(void)
{
  
    char c = USART_ReceiveData(USART2);
		MPU_DEBUG_FUNC();
    switch (c) {
    /* 这些命令关闭单个传感器. */
    case '8':
        hal.sensors ^= ACCEL_ON;
        setup_gyro();
        if (!(hal.sensors & ACCEL_ON))
            inv_accel_was_turned_off();
        break;
    case '9':
        hal.sensors ^= GYRO_ON;
        setup_gyro();
        if (!(hal.sensors & GYRO_ON))
            inv_gyro_was_turned_off();
        break;
#ifdef COMPASS_ENABLED
    case '0':
        hal.sensors ^= COMPASS_ON;
        setup_gyro();
        if (!(hal.sensors & COMPASS_ON))
            inv_compass_was_turned_off();
        break;
#endif
    /* 这些命令将单个传感器数据或融合数据发送到PC. */
    case 'a':
        hal.report ^= PRINT_ACCEL;
        break;
    case 'g':
        hal.report ^= PRINT_GYRO;
        break;
#ifdef COMPASS_ENABLED
    case 'c':
        hal.report ^= PRINT_COMPASS;
        break;
#endif
    case 'e':
        hal.report ^= PRINT_EULER;
        break;
    case 'r':
        hal.report ^= PRINT_ROT_MAT;
        break;
    case 'q':
        hal.report ^= PRINT_QUAT;
        break;
    case 'h':
        hal.report ^= PRINT_HEADING;
        break;
    case 'i':
        hal.report ^= PRINT_LINEAR_ACCEL;
        break;
    case 'o':
        hal.report ^= PRINT_GRAVITY_VECTOR;
        break;
#ifdef COMPASS_ENABLED
	case 'w':
		send_status_compass();
		break;
#endif
    /* 该命令打印出每个陀螺仪寄存器的值，用于调试。
			 如果禁用日志记录，则此功能不起作用。*/
    case 'd':
        mpu_reg_dump();
        break;
    /* 测试低功率加速模式. */
    case 'p':
        if (hal.dmp_on)
            /* LP加速与DMP不兼容. */
            break;
        mpu_lp_accel_mode(20);
        /* 当LP加速模式被启用时，驱动程序自动为锁存中断配置硬件。然而，MCU有时会忽略上升/下降沿和半边。
				   从未设置New_gyro标志。为了避免被锁定在这种状态，我们重写了驱动程序的配置，并坚持使用未锁存的中断模式。
         * TODO: MCU支持电平触发中断。
         */
        mpu_set_int_latched(0);
        hal.sensors &= ~(GYRO_ON|COMPASS_ON);
        hal.sensors |= ACCEL_ON;
        hal.lp_accel_mode = 1;
        inv_gyro_was_turned_off();
        inv_compass_was_turned_off();
        break;
    /* 硬件自检可以在没有任何与MPL交互的情况下运行，因为它完全定位在陀螺仪驱动程序中。
			 假定日志记录已启用;否则，可以在这里使用一对led来显示测试结果。*/
    case 't':
        run_self_test();
        /* 让MPL知道邻近被破坏. */
        inv_accel_was_turned_off();
        inv_gyro_was_turned_off();
        inv_compass_was_turned_off();
        break;
    /* 根据您的应用，可能需要更快或更慢的传感器数据速率。这些命令可以加快或减慢传感器数据推送到MPL的速度。
			 在本例中，罗盘速率永远不会改变。*/
    case '1':
        if (hal.dmp_on) {
            dmp_set_fifo_rate(10);
            inv_set_quat_sample_rate(100000L);
        } else
            mpu_set_sample_rate(10);
        inv_set_gyro_sample_rate(100000L);
        inv_set_accel_sample_rate(100000L);
        break;
    case '2':
        if (hal.dmp_on) {
            dmp_set_fifo_rate(20);
            inv_set_quat_sample_rate(50000L);
        } else
            mpu_set_sample_rate(20);
        inv_set_gyro_sample_rate(50000L);
        inv_set_accel_sample_rate(50000L);
        break;
    case '3':
        if (hal.dmp_on) {
            dmp_set_fifo_rate(40);
            inv_set_quat_sample_rate(25000L);
        } else
            mpu_set_sample_rate(40);
        inv_set_gyro_sample_rate(25000L);
        inv_set_accel_sample_rate(25000L);
        break;
    case '4':
        if (hal.dmp_on) {
            dmp_set_fifo_rate(50);
            inv_set_quat_sample_rate(20000L);
        } else
            mpu_set_sample_rate(50);
        inv_set_gyro_sample_rate(20000L);
        inv_set_accel_sample_rate(20000L);
        break;
    case '5':
        if (hal.dmp_on) {
            dmp_set_fifo_rate(100);
            inv_set_quat_sample_rate(10000L);
        } else
            mpu_set_sample_rate(100);
        inv_set_gyro_sample_rate(10000L);
        inv_set_accel_sample_rate(10000L);
        break;
	case ',':
        /* 将硬件设置为仅在手势事件时中断。此功能有助于保持MCU休眠状态，直到DMP检测到轻击或定向事件。*/
        dmp_set_interrupt_mode(DMP_INT_GESTURE);
        break;
    case '.':
        /* 设置硬件周期性中断. */
        dmp_set_interrupt_mode(DMP_INT_CONTINUOUS);
        break;
    case '6':
        /* 切换计步器显示. */
        hal.report ^= PRINT_PEDO;
        break;
    case '7':
        /* 重置计步器。 */
        dmp_set_pedometer_step_count(0);
        dmp_set_pedometer_walk_time(0);
        break;
    case 'f':
        if (hal.lp_accel_mode)
            /* LP加速与DMP不兼容. */
            return;
        /* Toggle DMP. */
        if (hal.dmp_on) {
            unsigned short dmp_rate;
            unsigned char mask = 0;
            hal.dmp_on = 0;
            mpu_set_dmp_state(0);
            /* Restore FIFO settings. */
            if (hal.sensors & ACCEL_ON)
                mask |= INV_XYZ_ACCEL;
            if (hal.sensors & GYRO_ON)
                mask |= INV_XYZ_GYRO;
            if (hal.sensors & COMPASS_ON)
                mask |= INV_XYZ_COMPASS;
            mpu_configure_fifo(mask);
            /* 当使用DMP时，硬件采样率固定为200Hz, DMP配置为使用dmp_set_fifo_rate函数对FIFO输出进行下采样。
							 然而，当DMP关闭时，采样率保持在200Hz。这可以在inv_mpu_dmp_motion_driver.c中处理，
							 但它需要知道存在inv_mpu_dmp_motion_driver.c。为了避免这种情况，我们将把额外的逻辑放在应用层中。
             */
            dmp_get_fifo_rate(&dmp_rate);
            mpu_set_sample_rate(dmp_rate);
            inv_quaternion_sensor_was_turned_off();
            MPL_LOGI("DMP disabled.\n");
        } else {
            unsigned short sample_rate;
            hal.dmp_on = 1;
            /* Preserve current FIFO rate. */
            mpu_get_sample_rate(&sample_rate);
            dmp_set_fifo_rate(sample_rate);
            inv_set_quat_sample_rate(1000000L / sample_rate);
            mpu_set_dmp_state(1);
            MPL_LOGI("DMP enabled.\n");
        }
        break;
    case 'm':
        /* Test the motion interrupt hardware feature. */
	#ifndef MPU6050 // not enabled for 6050 product
	hal.motion_int_mode = 1;
	#endif 
        break;

    case 'v':
        /* Toggle LP quaternion.
         * The DMP features can be enabled/disabled at runtime. Use this same
         * approach for other features.
         */
        hal.dmp_features ^= DMP_FEATURE_6X_LP_QUAT;
        dmp_enable_feature(hal.dmp_features);
        if (!(hal.dmp_features & DMP_FEATURE_6X_LP_QUAT)) {
            inv_quaternion_sensor_was_turned_off();
            MPL_LOGI("LP quaternion disabled.\n");
        } else
            MPL_LOGI("LP quaternion enabled.\n");
        break;
    default:
        break;
    }
    hal.rx.cmd = 0;
}

/* 每次有新的陀螺数据可用时，在ISR上下文中调用此函数。在这个例子中，它设置了一个保护FIFO读函数的标志。*/
void gyro_data_ready_cb(void)
{
    hal.new_gyro = 1;
}
/*******************************************************************************/

/**
  * @brief main entry point.
  * @par Parameters None
  * @retval void None
  * @par Required preconditions: None
  */
                                  
int main(void)
{ 
  
  inv_error_t result;
    unsigned char accel_fsr,  new_temp = 0;
    unsigned short gyro_rate, gyro_fsr;
    unsigned long timestamp;
    struct int_param_s int_param;
	
#ifdef USE_LCD_DISPLAY
	
	ILI9341_Init();
	ILI9341_GramScan(6);
	LCD_SetFont(&Font8x16);
	LCD_SetColors(RED,BLACK);
#endif
	SysTick_Init();
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	LED_GPIO_Config();
	LED_BLUE;
	
	USART_Config();
	
	EXTI_Pxy_Config();
	I2C_Bus_Init();
	
#ifdef USE_LCD_DISPLAY
	ILI9341_Clear(0,0,240,360);
	ILI9341_DispStringLine_EN(LINE(0),"This is MPU6050 demo");
#endif
	printf("mpu6050 test start");
	
	result = mpu_init(&int_param);
	if(result)
	{
		LED_RED;
		MPL_LOGE("Could not initialize gyro.result = %d\n",result);
		
		#ifdef USE_LCD_DISPLAY
		LCD_SetColors(BLUE,BLACK);
		
		ILI9341_DispStringLine_EN(LINE(2),"No MPU6050 detected! Please check the hardware connnection");
		#endif
		
	}
	else
	{
		LED_GREEN;
		#ifdef USE_LCD_DISPLAY
		ILI9341_DispStringLine_EN(LINE(2),"MPU6050 detected!");
		#endif
	}
	
	/* 如果您不使用MPU9150并且不使用DMP特性，则此函数将把所有从服务器放在主总线上。
		 mpu_set_bypass (1); */

	  result = inv_init_mpl();
	  if (result) {
		  MPL_LOGE("Could not initialize MPL.\n");
	  }
	  
	  /* Compute 6-axis and 9-axis quaternions. */
    inv_enable_quaternion();
    inv_enable_9x_sensor_fusion();
    /* MPL期望罗盘数据以恒定的速率(与传递给inv_set_compass_sample_rate的速率相匹配)。
		   如果这对您的应用程序来说是一个问题，那么调用这个函数，MPL将依赖于传递给inv_build_compass的时间戳。
       inv_9x_fusion_use_timestamps(1);  */

    /* This function has been deprecated.
     * inv_enable_no_gyro_fusion();
     */

    /* Update gyro biases when not in motion.
     * WARNING: These algorithms are mutually exclusive.
     */
    inv_enable_fast_nomot();
    /* inv_enable_motion_no_motion(); */
    /* inv_set_no_motion_time(1000); */

    /* Update gyro biases when temperature changes. */
    inv_enable_gyro_tc();

    /* 该算法在运动时更新加速度偏差。在运行自检时可以进行更精确的偏差测量(参见handle_input中的case 't')，
		   但是如果不能在应用程序中执行自检，则可以启用此算法。
     *
     * inv_enable_in_use_auto_calibration();
     */

#ifdef COMPASS_ENABLED
    /* Compass calibration algorithms. */
    inv_enable_vector_compass_cal();
    inv_enable_magnetic_disturbance();
#endif
    /* If you need to estimate your heading before the compass is calibrated,
     * enable this algorithm. It becomes useless after a good figure-eight is
     * detected, so we'll just leave it out to save memory.
     * inv_enable_heading_from_gyro();
     */

    /* Allows use of the MPL APIs in read_from_mpl. */
    inv_enable_eMPL_outputs();

  result = inv_start_mpl();
  if (result == INV_ERROR_NOT_AUTHORIZED) {
      while (1) {
          MPL_LOGE("Not authorized.\n");
      }
  }
  if (result) {
      MPL_LOGE("Could not start the MPL.\n");
  }

    /* Get/set hardware configuration. Start gyro. */
    /* Wake up all sensors. */
#ifdef COMPASS_ENABLED
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
#else
    mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
#endif
    /* Push both gyro and accel data into the FIFO. */
    mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    mpu_set_sample_rate(DEFAULT_MPU_HZ);
#ifdef COMPASS_ENABLED
    /* The compass sampling rate can be less than the gyro/accel sampling rate.
     * Use this function for proper power management.
     */
    mpu_set_compass_sample_rate(1000 / COMPASS_READ_MS);
#endif
    /* Read back configuration in case it was set improperly. */
    mpu_get_sample_rate(&gyro_rate);
    mpu_get_gyro_fsr(&gyro_fsr);
    mpu_get_accel_fsr(&accel_fsr);
#ifdef COMPASS_ENABLED
    mpu_get_compass_fsr(&compass_fsr);
#endif
    /* Sync driver configuration with MPL. */
    /* Sample rate expected in microseconds. */
    inv_set_gyro_sample_rate(1000000L / gyro_rate);
    inv_set_accel_sample_rate(1000000L / gyro_rate);
#ifdef COMPASS_ENABLED
    /* The compass rate is independent of the gyro and accel rates. As long as
     * inv_set_compass_sample_rate is called with the correct value, the 9-axis
     * fusion algorithm's compass correction gain will work properly.
     */
    inv_set_compass_sample_rate(COMPASS_READ_MS * 1000L);
#endif
    /* Set chip-to-body orientation matrix.
     * Set hardware units to dps/g's/degrees scaling factor.
     */
    inv_set_gyro_orientation_and_scale(
            inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
            (long)gyro_fsr<<15);
    inv_set_accel_orientation_and_scale(
            inv_orientation_matrix_to_scalar(gyro_pdata.orientation),
            (long)accel_fsr<<15);
#ifdef COMPASS_ENABLED
    inv_set_compass_orientation_and_scale(
            inv_orientation_matrix_to_scalar(compass_pdata.orientation),
            (long)compass_fsr<<15);
#endif
    /* Initialize HAL state variables. */
#ifdef COMPASS_ENABLED
    hal.sensors = ACCEL_ON | GYRO_ON | COMPASS_ON;
#else
    hal.sensors = ACCEL_ON | GYRO_ON;
#endif
    hal.dmp_on = 0;
    hal.report = 0;
    hal.rx.cmd = 0;
    hal.next_pedo_ms = 0;
    hal.next_compass_ms = 0;
    hal.next_temp_ms = 0;

  /* Compass reads are handled by scheduler. */
  get_tick_count(&timestamp);

    /* To initialize the DMP:
     * 1. Call dmp_load_motion_driver_firmware(). This pushes the DMP image in
     *    inv_mpu_dmp_motion_driver.h into the MPU memory.
     * 2. Push the gyro and accel orientation matrix to the DMP.
     * 3. Register gesture callbacks. Don't worry, these callbacks won't be
     *    executed unless the corresponding feature is enabled.
     * 4. Call dmp_enable_feature(mask) to enable different features.
     * 5. Call dmp_set_fifo_rate(freq) to select a DMP output rate.
     * 6. Call any feature-specific control functions.
     *
     * To enable the DMP, just call mpu_set_dmp_state(1). This function can
     * be called repeatedly to enable and disable the DMP at runtime.
     *
     * The following is a short summary of the features supported in the DMP
     * image provided in inv_mpu_dmp_motion_driver.c:
     * DMP_FEATURE_LP_QUAT: Generate a gyro-only quaternion on the DMP at
     * 200Hz. Integrating the gyro data at higher rates reduces numerical
     * errors (compared to integration on the MCU at a lower sampling rate).
     * DMP_FEATURE_6X_LP_QUAT: Generate a gyro/accel quaternion on the DMP at
     * 200Hz. Cannot be used in combination with DMP_FEATURE_LP_QUAT.
     * DMP_FEATURE_TAP: Detect taps along the X, Y, and Z axes.
     * DMP_FEATURE_ANDROID_ORIENT: Google's screen rotation algorithm. Triggers
     * an event at the four orientations where the screen should rotate.
     * DMP_FEATURE_GYRO_CAL: Calibrates the gyro data after eight seconds of
     * no motion.
     * DMP_FEATURE_SEND_RAW_ACCEL: Add raw accelerometer data to the FIFO.
     * DMP_FEATURE_SEND_RAW_GYRO: Add raw gyro data to the FIFO.
     * DMP_FEATURE_SEND_CAL_GYRO: Add calibrated gyro data to the FIFO. Cannot
     * be used in combination with DMP_FEATURE_SEND_RAW_GYRO.
     */
    dmp_load_motion_driver_firmware();
    dmp_set_orientation(
        inv_orientation_matrix_to_scalar(gyro_pdata.orientation));
    dmp_register_tap_cb(tap_cb);
    dmp_register_android_orient_cb(android_orient_cb);
    /*
     * Known Bug -
     * DMP when enabled will sample sensor data at 200Hz and output to FIFO at the rate
     * specified in the dmp_set_fifo_rate API. The DMP will then sent an interrupt once
     * a sample has been put into the FIFO. Therefore if the dmp_set_fifo_rate is at 25Hz
     * there will be a 25Hz interrupt from the MPU device.
     *
     * There is a known issue in which if you do not enable DMP_FEATURE_TAP
     * then the interrupts will be at 200Hz even if fifo rate
     * is set at a different rate. To avoid this issue include the DMP_FEATURE_TAP
     *
     * DMP sensor fusion works only with gyro at +-2000dps and accel +-2G
     */
    hal.dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
        DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
        DMP_FEATURE_GYRO_CAL;
    dmp_enable_feature(hal.dmp_features);
    dmp_set_fifo_rate(DEFAULT_MPU_HZ);
    mpu_set_dmp_state(1);
    hal.dmp_on = 1;

  while(1){
    
    unsigned long sensor_timestamp;
    int new_data = 0;
    if (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE)) {
        /* A byte has been received via USART. See handle_input for a list of
         * valid commands.
         */
        USART_ClearFlag(DEBUG_USARTx, USART_FLAG_RXNE);
        handle_input();
    }
    get_tick_count(&timestamp);

#ifdef COMPASS_ENABLED
        /* We're not using a data ready interrupt for the compass, so we'll
         * make our compass reads timer-based instead.
         */
        if ((timestamp > hal.next_compass_ms) && !hal.lp_accel_mode &&
            hal.new_gyro && (hal.sensors & COMPASS_ON)) {
            hal.next_compass_ms = timestamp + COMPASS_READ_MS;
            new_compass = 1;
        }
#endif
        /* Temperature data doesn't need to be read with every gyro sample.
         * Let's make them timer-based like the compass reads.
         */
        if (timestamp > hal.next_temp_ms) {
            hal.next_temp_ms = timestamp + TEMP_READ_MS;
            new_temp = 1;
        }

    if (hal.motion_int_mode) {
        /* Enable motion interrupt. */
        mpu_lp_motion_interrupt(500, 1, 5);
        /* Notify the MPL that contiguity was broken. */
        inv_accel_was_turned_off();
        inv_gyro_was_turned_off();
        inv_compass_was_turned_off();
        inv_quaternion_sensor_was_turned_off();
        /* Wait for the MPU interrupt. */
        while (!hal.new_gyro) {}
        /* Restore the previous sensor configuration. */
        mpu_lp_motion_interrupt(0, 0, 0);
        hal.motion_int_mode = 0;
    }

    if (!hal.sensors || !hal.new_gyro) {
        continue;
    }    

        if (hal.new_gyro && hal.lp_accel_mode) {
            short accel_short[3];
            long accel[3];
            mpu_get_accel_reg(accel_short, &sensor_timestamp);
            accel[0] = (long)accel_short[0];
            accel[1] = (long)accel_short[1];
            accel[2] = (long)accel_short[2];
            inv_build_accel(accel, 0, sensor_timestamp);
            new_data = 1;
            hal.new_gyro = 0;
        } else if (hal.new_gyro && hal.dmp_on) {
            short gyro[3], accel_short[3], sensors;
            unsigned char more;
            long accel[3], quat[4], temperature;
            /* This function gets new data from the FIFO when the DMP is in
             * use. The FIFO can contain any combination of gyro, accel,
             * quaternion, and gesture data. The sensors parameter tells the
             * caller which data fields were actually populated with new data.
             * For example, if sensors == (INV_XYZ_GYRO | INV_WXYZ_QUAT), then
             * the FIFO isn't being filled with accel data.
             * The driver parses the gesture data to determine if a gesture
             * event has occurred; on an event, the application will be notified
             * via a callback (assuming that a callback function was properly
             * registered). The more parameter is non-zero if there are
             * leftover packets in the FIFO.
             */
            dmp_read_fifo(gyro, accel_short, quat, &sensor_timestamp, &sensors, &more);
            if (!more)
                hal.new_gyro = 0;
            if (sensors & INV_XYZ_GYRO) {
                /* Push the new data to the MPL. */
                inv_build_gyro(gyro, sensor_timestamp);
                new_data = 1;
                if (new_temp) {
                    new_temp = 0;
                    /* Temperature only used for gyro temp comp. */
                    mpu_get_temperature(&temperature, &sensor_timestamp);
                    inv_build_temp(temperature, sensor_timestamp);
                }
            }
            if (sensors & INV_XYZ_ACCEL) {
                accel[0] = (long)accel_short[0];
                accel[1] = (long)accel_short[1];
                accel[2] = (long)accel_short[2];
                inv_build_accel(accel, 0, sensor_timestamp);
                new_data = 1;
            }
            if (sensors & INV_WXYZ_QUAT) {
                inv_build_quat(quat, 0, sensor_timestamp);
                new_data = 1;
            }
        } else if (hal.new_gyro) {
            short gyro[3], accel_short[3];
            unsigned char sensors, more;
            long accel[3], temperature;
            /* This function gets new data from the FIFO. The FIFO can contain
             * gyro, accel, both, or neither. The sensors parameter tells the
             * caller which data fields were actually populated with new data.
             * For example, if sensors == INV_XYZ_GYRO, then the FIFO isn't
             * being filled with accel data. The more parameter is non-zero if
             * there are leftover packets in the FIFO. The HAL can use this
             * information to increase the frequency at which this function is
             * called.
             */
            hal.new_gyro = 0;
            mpu_read_fifo(gyro, accel_short, &sensor_timestamp,
                &sensors, &more);
            if (more)
                hal.new_gyro = 1;
            if (sensors & INV_XYZ_GYRO) {
                /* Push the new data to the MPL. */
                inv_build_gyro(gyro, sensor_timestamp);
                new_data = 1;
                if (new_temp) {
                    new_temp = 0;
                    /* Temperature only used for gyro temp comp. */
                    mpu_get_temperature(&temperature, &sensor_timestamp);
                    inv_build_temp(temperature, sensor_timestamp);
                }
            }
            if (sensors & INV_XYZ_ACCEL) {
                accel[0] = (long)accel_short[0];
                accel[1] = (long)accel_short[1];
                accel[2] = (long)accel_short[2];
                inv_build_accel(accel, 0, sensor_timestamp);
                new_data = 1;
            }
        }
#ifdef COMPASS_ENABLED
        if (new_compass) {
            short compass_short[3];
            long compass[3];
            new_compass = 0;
            /* For any MPU device with an AKM on the auxiliary I2C bus, the raw
             * magnetometer registers are copied to special gyro registers.
             */
            if (!mpu_get_compass_reg(compass_short, &sensor_timestamp)) {
                compass[0] = (long)compass_short[0];
                compass[1] = (long)compass_short[1];
                compass[2] = (long)compass_short[2];
                /* NOTE: If using a third-party compass calibration library,
                 * pass in the compass data in uT * 2^16 and set the second
                 * parameter to INV_CALIBRATED | acc, where acc is the
                 * accuracy from 0 to 3.
                 */
                inv_build_compass(compass, 0, sensor_timestamp);
            }
            new_data = 1;
        }
#endif
        if (new_data) {
            inv_execute_on_data();
            /* This function reads bias-compensated sensor data and sensor
             * fusion outputs from the MPL. The outputs are formatted as seen
             * in eMPL_outputs.c. This function only needs to be called at the
             * rate requested by the host.
             */
            read_from_mpl();
        }
    }
}


/*********************************************END OF FILE**********************/
