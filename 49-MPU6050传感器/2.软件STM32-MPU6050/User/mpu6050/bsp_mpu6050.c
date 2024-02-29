#include "bsp_mpu6050.h"
#include "bsp_i2c.h"
#include "bsp_usart.h"

/* 写数据到MPU6050寄存器 */
void MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_data)
{
	i2c_Start();
	i2c_SendByte(MPU6050_SLAVE_ADDRESS);
	i2c_WaitAck();
	i2c_SendByte(reg_add);
	i2c_WaitAck();
	i2c_SendByte(reg_data);
	i2c_WaitAck();
	i2c_Stop();
}

/* 从MPU6050寄存器读取数据 */
void MPU6050_ReadData(uint8_t reg_add,unsigned char* Read,uint8_t num)
{
	unsigned char i;
	
	i2c_Start();
	i2c_SendByte(MPU6050_SLAVE_ADDRESS);
	i2c_WaitAck();
	i2c_SendByte(reg_add);
	i2c_WaitAck();
	
	i2c_Start();
	i2c_SendByte(MPU6050_SLAVE_ADDRESS+1);
	i2c_WaitAck();
	
	for( i = 0; i <num-1 ;i++)
	{
		*Read = i2c_ReadByte(1);
		Read++;
	}
	*Read = i2c_ReadByte(0);
	i2c_Stop();
}

/* 初始化MPU6050芯片 */
void MPU6050_Init(void)
{
	int i=0 ,j=0;
	//在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
	for( i=0 ; i < 1000;i++)
	{
		for(j=0;j<1000;j++)
		{
			;
		}
	}
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1,0x00);     //解除休眠状态
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV,0x07);     //陀螺仪采样率
	MPU6050_WriteReg(MPU6050_RA_CONFIG,0x06);
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG,0x00);   //配置加速度传感器工作在2G模式,不自检
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG,0x18);    //陀螺仪自检及测量范围，典型值:0x18(不自检，2000deg/s)
}

/* 读取MPU6050的ID 正常返回1，异常返回0 */
uint8_t MPU6050ReadID(void)
{
	unsigned char Re = 0;
		MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1);   // 读器件地址
	
	if(Re != 0x68)
	{
		printf("MPU6050 dectected error !\r\n 检测不到MPU6050模块，请检查模块与开发板的接线");
		return 0;
	}
	else
	{
		printf("MPU6050 ID = %d \r\n",Re);
		return 1;
	}
}

/* 读取MPU6050的加速度数据 */
void MPU6050ReadAcc(short *accData)
{
	uint8_t buf[6];
	MPU6050_ReadData(MPU6050_ACC_OUT,buf,6);
	accData[0] = (buf[0] << 8) | buf[1];
	accData[1] = (buf[2] << 8) | buf[3];
	accData[2] = (buf[4] << 8) | buf[5];
}

/* 读取MPU6050的角加速度数据 */
void MPU6050ReadGyro(short *gyroData)
{
	uint8_t buf[6];
	MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
	gyroData[0] = (buf[0] << 8) | buf[1];
	gyroData[1] = (buf[2] << 8) | buf[3];
	gyroData[2] = (buf[4] << 8) | buf[5];
}

/* 读取MPU6050的原始温度数据 */
void MPU6050ReadTemp(short *tempData)
{
	uint8_t buf[2];
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);
	*tempData = (buf[0]<<8) | buf[1];
}

/* 读取MPU6050的温度数据，转化为摄氏度 */
void MPU6050_ReturnTemp(float *Temperature)
{
	short temp3;
	uint8_t buf[2];
	
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);
	temp3 = (buf[0]<<8) | buf[1]; 
	*Temperature = ((double) temp3/340.0)+36.53;
}

 