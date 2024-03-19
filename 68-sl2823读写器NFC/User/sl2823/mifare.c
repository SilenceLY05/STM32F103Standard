#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include "mifare.h"


/**
 ****************************************************************
 * @brief pcd_auth_state()
 *
 * ���ܣ��ô����FIFO�е���Կ�Ϳ��ϵ���Կ������֤
 *
 * @param: auth_mode=��֤��ʽ,0x60:��֤A��Կ,0x61:��֤B��Կ
 * @param: block=Ҫ��֤�ľ��Կ��
 * @param: psnr=���к��׵�ַ,��UID
 * @return: status ֵΪMI_OK:�ɹ�
 *
 ****************************************************************
 */
int8_t pcd_auth_state(uint8_t auth_mode, uint8_t block, uint8_t *psnr, uint8_t *pkey)
{
    int8_t status;
    uint8_t i;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
    printf("AUTH:\n");
#endif
    write_reg(BitFramingReg,0x00);  // // Tx last bits = 0, rx align = 0
//  clear_bit_mask(RxModeReg, BIT7); //��ʹ�ܽ���crc�� added by xlwang,20180404

    pcd_set_tmo(4);

    mf_com_data.mf_command = PCD_AUTHENT;
    mf_com_data.mf_length = 12;
    mf_com_data.mf_data[0] = auth_mode;
    mf_com_data.mf_data[1] = block;
    for(i = 0; i < 6; i++)
    {
        mf_com_data.mf_data[2+i] = pkey[i];
    }
    memcpy(&mf_com_data.mf_data[8], psnr, 4);

    status = pcd_com_transceive(pi);

    if(MI_OK == status)
    {
        if(read_reg(Status2Reg) & BIT3)  //MFCrypto1On
        {
            status = MI_OK;
        }
        else
        {
            status = MI_AUTHERR;
        }
    }

    return status;

}



/**
 ****************************************************************
 * @brief pcd_read()
 *
 * ���ܣ���mifare_one����һ��(block)����(16�ֽ�)
 *
 * @param: addr = Ҫ���ľ��Կ��
 * @param: preaddata = ��Ŷ��������ݻ��������׵�ַ
 * @return: status ֵΪMI_OK:�ɹ�
 * @retval: preaddata  ����������
 *
 ****************************************************************
 */
int8_t pcd_read(uint8_t addr,uint8_t *preaddata)
{
    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
    printf("READ:\n");
#endif

    write_reg(BitFramingReg,0x00);  // // Tx last bits = 0, rx align = 0
    set_bit_mask(TxModeReg, BIT7); //ʹ�ܷ���crc
    set_bit_mask(RxModeReg, BIT7); //ʹ�ܽ���crc
    pcd_set_tmo(4);

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 2;
    mf_com_data.mf_data[0] = PICC_READ;
    mf_com_data.mf_data[1] = addr;

    status = pcd_com_transceive(pi);
    if(status == MI_OK)
    {
        if(mf_com_data.mf_length != 0x80)
        {
            status = MI_BITCOUNTERR;
        }
        else
        {
            memcpy(preaddata, &mf_com_data.mf_data[0], 16);
        }
    }

    return status;
}


/**
 ****************************************************************
 * @brief pcd_write()
 *
 * ���ܣ�д���ݵ����ϵ�һ��
 *
 * @param: addr = Ҫд�ľ��Կ��
 * @param: pwritedata = ���д�����ݻ��������׵�ַ
 * @return: status ֵΪMI_OK:�ɹ�
 *
 ****************************************************************
 */
int8_t pcd_write(uint8_t addr,uint8_t *pwritedata)
{
    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
    printf("WRITE:\n");
#endif

    write_reg(BitFramingReg,0x00);  // // Tx last bits = 0, rx align = 0
    set_bit_mask(TxModeReg, BIT7); //ʹ�ܷ���crc
    clear_bit_mask(RxModeReg, BIT7); //��ʹ�ܽ���crc

    pcd_set_tmo(5);

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 2;
    mf_com_data.mf_data[0] = PICC_WRITE;
    mf_com_data.mf_data[1] = addr;

    status = pcd_com_transceive(pi);
//   ret= read_reg(BitFramingReg);  // // Tx last bits = 0, rx align = 0
//   ret= read_reg(ControlReg);  // // Tx last bits = 0, rx align = 0
    if(status != MI_NOTAGERR)
    {
        if(mf_com_data.mf_length != 4)
        {
            status=MI_BITCOUNTERR;
        }
        else
        {
            mf_com_data.mf_data[0] &= 0x0F;
            switch(mf_com_data.mf_data[0])
            {
                case 0x00:
                    status = MI_NOTAUTHERR;
                    break;
                case 0x0A:
                    status = MI_OK;
                    break;
                default:
                    status = MI_CODEERR;
                    break;
            }
        }
    }
    if(status == MI_OK)
    {
        pcd_set_tmo(5);

        mf_com_data.mf_command = PCD_TRANSCEIVE;
        mf_com_data.mf_length  = 16;
        memcpy(&mf_com_data.mf_data[0], pwritedata, 16);

        status = pcd_com_transceive(pi);
        if(status != MI_NOTAGERR)
        {
            mf_com_data.mf_data[0] &= 0x0F;
            switch(mf_com_data.mf_data[0])
            {
                case 0x00:
                    status = MI_WRITEERR;
                    break;
                case 0x0A:
                    status = MI_OK;
                    break;
                default:
                    status = MI_CODEERR;
                    break;
            }
        }
        pcd_set_tmo(4);
    }
    return status;
}

/**
 ****************************************************************
 * @brief pcd_write_ultralight()
 *
 * ���ܣ�д���ݵ����ϵ�һ��
 *
 * @param: addr = Ҫд�ľ��Կ��
 * @param: pwritedata = ���д�����ݻ��������׵�ַ
 * @return: status ֵΪMI_OK:�ɹ�
 *
 ****************************************************************
 */
int8_t pcd_write_ultralight(uint8_t addr,uint8_t *pwritedata)
{
    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
    printf("WRITE_UL:\n");
#endif

    write_reg(BitFramingReg,0x00);  // // Tx last bits = 0, rx align = 0
    set_bit_mask(TxModeReg, BIT7); //ʹ�ܷ���crc
    clear_bit_mask(RxModeReg, BIT7); //��ʹ�ܽ���crc
    pcd_set_tmo(5);

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 6; //a2h ADR D0 D1 D2 D3
    mf_com_data.mf_data[0] = PICC_WRITE_ULTRALIGHT;
    mf_com_data.mf_data[1] = addr;
    memcpy(&mf_com_data.mf_data[2], pwritedata, 4); // 4 �ֽ�����

    status = pcd_com_transceive(pi);

    if(status != MI_NOTAGERR)
    {
        mf_com_data.mf_data[0] &= 0x0F;
        switch(mf_com_data.mf_data[0])
        {
            case 0x00:
                status = MI_WRITEERR;
                break;
            case 0x0A:
                status = MI_OK;
                break;
            default:
                status = MI_CODEERR;
                break;
        }
    }
    pcd_set_tmo(4);

    return status;
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�
//               0xC1 = ��ֵ
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char pcd_value(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{

    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

    write_reg(BitFramingReg,0x00);  // // Tx last bits = 0, rx align = 0
    set_bit_mask(TxModeReg, BIT7); //ʹ�ܷ���crc
    clear_bit_mask(RxModeReg, BIT7); //��ʹ�ܽ���crc

    pcd_set_tmo(5);

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 2;
    mf_com_data.mf_data[0] = dd_mode;
    mf_com_data.mf_data[1] = addr;

    status = pcd_com_transceive(pi);
    if(status != MI_NOTAGERR)
    {
        if(mf_com_data.mf_length != 4)
        {
            status=MI_BITCOUNTERR;
        }
        else
        {
            mf_com_data.mf_data[0] &= 0x0F;
            switch(mf_com_data.mf_data[0])
            {
                case 0x00:
                    status = MI_NOTAUTHERR;
                    break;
                case 0x0A:
                    status = MI_OK;
                    break;
                default:
                    status = MI_CODEERR;
                    break;
            }
        }
    }
    if(status == MI_OK)
    {
        pcd_set_tmo(5);

        mf_com_data.mf_command = PCD_TRANSCEIVE;
        mf_com_data.mf_length  = 4;
        memcpy(&mf_com_data.mf_data[0], pValue, 4);
        status = pcd_com_transceive(pi);
        if(status == MI_NOTAGERR || status == MI_OK)
        {
            status = MI_OK;
        }

    }

    if(status == MI_OK)
    {
        mf_com_data.mf_command = PCD_TRANSCEIVE;
        mf_com_data.mf_length  = 2;
        mf_com_data.mf_data[0] = PICC_TRANSFER;
        mf_com_data.mf_data[1] = addr;
        pcd_set_tmo(5);
        status = pcd_com_transceive(pi);
        if(status != MI_NOTAGERR)
        {
            if(mf_com_data.mf_length != 4)
            {
                status=MI_BITCOUNTERR;
            }
            else
            {
                mf_com_data.mf_data[0] &= 0x0F;
                switch(mf_com_data.mf_data[0])
                {
                    case 0x00:
                        status = MI_NOTAUTHERR;
                        break;
                    case 0x0A:
                        status = MI_OK;
                        break;
                    default:
                        status = MI_CODEERR;
                        break;
                }
            }
        }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����Ǯ��
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char pcd_bak_value(unsigned char sourceaddr, unsigned char goaladdr)
{
    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

    write_reg(BitFramingReg,0x00);  // // Tx last bits = 0, rx align = 0
    set_bit_mask(TxModeReg, BIT7); //ʹ�ܷ���crc
    clear_bit_mask(RxModeReg, BIT7); //��ʹ�ܽ���crc

    pcd_set_tmo(5);

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 2;
    mf_com_data.mf_data[0] = PICC_RESTORE;
    mf_com_data.mf_data[1] = sourceaddr;

    status = pcd_com_transceive(pi);
    if(status != MI_NOTAGERR)
    {
        if(mf_com_data.mf_length != 4)
        {
            status=MI_BITCOUNTERR;
        }
        else
        {
            mf_com_data.mf_data[0] &= 0x0F;
            switch(mf_com_data.mf_data[0])
            {
                case 0x00:
                    status = MI_NOTAUTHERR;
                    break;
                case 0x0A:
                    status = MI_OK;
                    break;
                default:
                    status = MI_CODEERR;
                    break;
            }
        }
    }
    if(status == MI_OK)
    {
        pcd_set_tmo(5);

        mf_com_data.mf_command = PCD_TRANSCEIVE;
        mf_com_data.mf_length  = 4;
        memset(&mf_com_data.mf_data[0], 0, 4);

        status = pcd_com_transceive(pi);
        if(status != MI_NOTAGERR)
        {
            mf_com_data.mf_data[0] &= 0x0F;
            switch(mf_com_data.mf_data[0])
            {
                case 0x00:
                    status = MI_WRITEERR;
                    break;
                case 0x0A:
                    status = MI_OK;
                    break;
                default:
                    status = MI_CODEERR;
                    break;
            }
        }

    }
    if(status == MI_OK)
    {
        pcd_set_tmo(4);
        mf_com_data.mf_command = PCD_TRANSCEIVE;
        mf_com_data.mf_length  = 2;
        mf_com_data.mf_data[0] = PICC_TRANSFER;
        mf_com_data.mf_data[1] = goaladdr;

        status = pcd_com_transceive(pi);
        if(status != MI_NOTAGERR)
        {
            if(mf_com_data.mf_length != 4)
            {
                status=MI_BITCOUNTERR;
            }
            else
            {
                mf_com_data.mf_data[0] &= 0x0F;
                switch(mf_com_data.mf_data[0])
                {
                    case 0x00:
                        status = MI_NOTAUTHERR;
                        break;
                    case 0x0A:
                        status = MI_OK;
                        break;
                    default:
                        status = MI_CODEERR;
                        break;
                }
            }
        }
    }
    return status;
}

char InitBlock(unsigned char addr,unsigned char *value)
{
    unsigned char tmp[16],i;
    int8_t status;
    for(i=0; i<4; i++)
    {
        tmp[i]=value[i];
        tmp[i+4]=255-value[i];
        tmp[i+8]=value[i];
    }
    tmp[12]=addr;
    tmp[13]=255-addr;
    tmp[14]=tmp[12];
    tmp[15]=tmp[13];
    status=pcd_write(addr,tmp);
    return status;
}



