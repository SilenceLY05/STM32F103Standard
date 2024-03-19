#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "sl2x23.h"
#include "iso14443b.h"


extern uint8_t  g_fwi;//frame waiting time integer

uint8_t  g_fwi = 4;//frame waiting time integer


//////////////////////////////////////////////////////////////////////
// ����ԭ��:    char pcd_request_b(u8 req_code, u8 AFI, u8 N, u8 *ATQB)
// ��������:    B�Ϳ�����
// ��ڲ���:    req_code                // ������� ISO14443_3B_REQIDL 0x00 -- ���еĿ�
//                                      //          ISO14443_3B_REQALL 0x08 -- ���еĿ�
//              AFI                     // Ӧ�ñ�ʶ����0x00��ȫѡ
//              N                       // ʱ϶����,ȡֵ��Χ0--4��
// ���ڲ���:    *ATQB                   // ����Ӧ��11�ֽ�
// �� �� ֵ:    STATUS_SUCCESS -- �ɹ�������ֵ -- ʧ�ܡ�
// ˵    ��:    -
//////////////////////////////////////////////////////////////////////
int8_t pcd_request_b(uint8_t req_code, uint8_t AFI, uint8_t N, uint8_t *ATQB)
{
    int8_t  status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi=&mf_com_data;

#if (NFC_DEBUG)
    printf("REQB/WUPB:\n");
#endif

    pcd_set_tmo(5);

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length = 3;
    mf_com_data.mf_data[0] = ISO14443B_ANTICOLLISION;              // APf code
    mf_com_data.mf_data[1] = AFI;                //
    mf_com_data.mf_data[2] = (req_code & 0x08) | (N&0x07);  // PARAM

    status = pcd_com_transceive(pi);

    if(status  !=MI_OK && status !=MI_NOTAGERR)
    {
        status = MI_COLLERR;
    }
    if(status == MI_OK && mf_com_data.mf_length != 96)
    {
        status = MI_COM_ERR;
    }
    if(status == MI_OK)
    {
        memcpy(ATQB, &mf_com_data.mf_data[0], 16);
        pcd_set_tmo(ATQB[11]>>4); // set FWT
        g_fwi = (ATQB[11]>>4);
    }
#if (NFC_DEBUG)
    printf(" sta=%bd\n", status);
#endif
    return status;
}

//////////////////////////////////////////////////////////////////////
//SLOT-MARKER
//////////////////////////////////////////////////////////////////////
int8_t pcd_slot_marker(uint8_t N, uint8_t *ATQB)
{
    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
    printf("SLOT:\n");
#endif
    pcd_set_tmo(5);

    if(!N || N>15)
    {
        status = MI_WRONG_PARAMETER_VALUE;
    }
    else
    {
        mf_com_data.mf_command = PCD_TRANSCEIVE;
        mf_com_data.mf_length = 1;
        mf_com_data.mf_data[0] = 0x05 |(N << 4); // APn code

        status = pcd_com_transceive(pi);

        if(status != MI_OK && status != MI_NOTAGERR)
        {
            status = MI_COLLERR;
        }
        if(status == MI_OK && mf_com_data.mf_length != 96)
        {
            status = MI_COM_ERR;
        }
        if(status == MI_OK)
        {
            memcpy(ATQB, &mf_com_data.mf_data[0], 16);
            pcd_set_tmo(ATQB[11]>>4); // set FWT
            g_fwi = ATQB[11]>>4;
        }
    }

    return status;
}




//////////////////////////////////////////////////////////////////////
//ATTRIB
// ����ԭ��:    INchar pcd_attrib(u8 *PUPI, u8 pro_type, u8 CID, u8 *answer)
//
// ��������:    ѡ��PICC
// ��ڲ���:    u8 *PUPI                    // 4�ֽ�PICC��ʶ��
//              u8 dsi_dri                  // PCD<-->PICC ����ѡ��
//              u8 pro_type                 // ֧�ֵ�Э�飬�������Ӧ�е�ProtocolTypeָ��
// �� �� ֵ:    MI_OK -- �ɹ�������ֵ -- ʧ�ܡ�
// ˵    ��:    -
//////////////////////////////////////////////////////////////////////
int8_t pcd_attri_b(uint8_t *PUPI, uint8_t dsi_dri, uint8_t pro_type, uint8_t CID, uint8_t *answer)
{
    int8_t  status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;
    pro_type = pro_type;


#if (NFC_DEBUG)
    printf("ATTRIB:\n");
#endif

    pcd_set_tmo(g_fwi);

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 9;
    mf_com_data.mf_data[0] = ISO14443B_ATTRIB;
    memcpy(&mf_com_data.mf_data[1], PUPI, 4);
    //mf_com_data.mf_data[1] = 0x00;
    //mf_com_data.mf_data[2] = 0x00;
    //mf_com_data.mf_data[3] = 0x00;
    //mf_com_data.mf_data[4] = 0x00;
    mf_com_data.mf_data[5] = 0x00;          // EOF/SOF required, default TR0/TR1
    //EOFTEST
    //mf_com_data.mf_data[5] = 0x04; //XU //SOF not,EOF yes
    //mf_com_data.mf_data[5] = 0x08; //XU //SOF yes,EOF no
    //mf_com_data.mf_data[5] = 0x0C; //XU //SOF no,EOF no

    mf_com_data.mf_data[6] = ((dsi_dri << 4) | FSDI); //FSDI; // Max frame 64
    mf_com_data.mf_data[7] = 0x01; //pro_type & 0x0f;  //;  ISO/IEC 14443-4 compliant?;
    mf_com_data.mf_data[8] = (CID & 0x0f); //           // CID ,0 - 14,����֧��CID��������Ϊ0000

    status  = pcd_com_transceive(pi);

    if(status == MI_OK)
    {
        *answer = mf_com_data.mf_data[0];
    }

#if (NFC_DEBUG)
    printf(" sta=%bd\n", status);
#endif

    return status;
}
//////////////////////////////////////////////////////////////////////
//REQUEST B
//////////////////////////////////////////////////////////////////////
int8_t get_idcard_num(uint8_t *pid)
{
    int8_t  status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
    printf("ID_NUM:\n");
#endif
    //pcd_set_tmo(5);

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  =5;
    mf_com_data.mf_data[0] =0x00; //ISO14443B_ANTICOLLISION;               // APf code
    mf_com_data.mf_data[1] =0x36;// AFI;                //
    mf_com_data.mf_data[2] =0x00; //((req_code<<3)&0x08) | (N&0x07);  // PARAM
    mf_com_data.mf_data[3] =0x00;
    mf_com_data.mf_data[4] =0x08;

    status = pcd_com_transceive(pi);
    if(status == MI_OK)
    {
        memcpy(pid, &mf_com_data.mf_data[0], 10);
        //pcd_set_tmo(ATQB[11]>>4); // set FWT
    }
    return status;
}

//////////////////////////////////////////////////////////////////////
// ����ԭ��:    char pcd_halt_b(u8 *PUPI)
// ��������:    ����
// ��ڲ���:    INT8U *pPUPI                    // 4�ֽ�PICC��ʶ��
// ���ڲ���:    -
// �� �� ֵ:    MI_OK -- �ɹ�������ֵ -- ʧ�ܡ�//////////////////////////////////////////////////////////////////////
int8_t pcd_halt_b(uint8_t *PUPI)
{
    int8_t  status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
    printf("HALTB:\n");
#endif
    pcd_set_tmo(g_fwi);
    // disable, ISO/IEC3390 enable
    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 5;
    mf_com_data.mf_data[0] = ISO14443B_ATTRIB;
    memcpy(&mf_com_data.mf_data[1], PUPI, 4);

    status = pcd_com_transceive(pi);
#if (NFC_DEBUG)
    printf(" sta=%bd\n", status);
#endif

    return status;
}


/**
 ****************************************************************
 * @brief select_sr()
 *
 * ����ײ����
 * @param:
 * @param:
 * @return: status ֵΪMI_OK:�ɹ�
 * @retval: chip_id  �õ���SR��Ƭ��chip_id
 ****************************************************************
 */
int8_t select_sr(uint8_t *chip_id)
{
    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
    printf("SELECT_SR:\n");
#endif
    pcd_set_tmo(5);
    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 2;
    mf_com_data.mf_data[0] = 0x06;             //initiate card
    mf_com_data.mf_data[1] = 0;

    status = pcd_com_transceive(pi);

    if(status!=MI_OK && status!=MI_NOTAGERR)
    {
        status = MI_COLLERR;      // collision occurs
    }

    if(mf_com_data.mf_length != 8)
    {
        status = MI_COM_ERR;
    }

    if(status == MI_OK)
    {
        pcd_set_tmo(5);
        mf_com_data.mf_command = PCD_TRANSCEIVE;
        mf_com_data.mf_length  = 2;
        mf_com_data.mf_data[1] = mf_com_data.mf_data[0];
        mf_com_data.mf_data[0] = 0x0E;                 // Slect card

        status = pcd_com_transceive(pi);

        if(status!=MI_OK && status!=MI_NOTAGERR)   // collision occurs
        {
            status = MI_COLLERR;      // collision occurs
        }
        if(mf_com_data.mf_length != 8)
        {
            status = MI_COM_ERR;
        }
        if(status == MI_OK)
        {
            *chip_id = mf_com_data.mf_data[0];
        }
    }
#if (NFC_DEBUG)
    printf(" sta=%bd\n", status);
#endif
    return status;
}

//////////////////////////////////////////////////////////////////////
//SR176������
//////////////////////////////////////////////////////////////////////
int8_t read_sr176(uint8_t addr, uint8_t *readdata)
{
    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
    printf("read_sr176()->\n");
#endif
    pcd_set_tmo(5);
    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 2;
    mf_com_data.mf_data[0] = 0x08;
    mf_com_data.mf_data[1] = addr;

    status = pcd_com_transceive(pi);

    if((status==MI_OK) && (mf_com_data.mf_length!=16))
    {
        status = MI_BITCOUNTERR;
    }
    if(status == MI_OK)
    {
        *readdata     = mf_com_data.mf_data[0];
        *(readdata+1) = mf_com_data.mf_data[1];
    }
#if (NFC_DEBUG)
    printf(" sta=%bd\n", status);
#endif

    return status;
}
//////////////////////////////////////////////////////////////////////
//SR176��д��
//////////////////////////////////////////////////////////////////////
int8_t write_sr176(uint8_t addr, uint8_t *writedata)
{
    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;
#if (NFC_DEBUG)
    printf("write_sr176()->\n");
#endif
    pcd_set_tmo(5);
    mf_com_data.mf_command = PCD_TRANSMIT;
    mf_com_data.mf_length  = 4;
    mf_com_data.mf_data[0] = 9;
    mf_com_data.mf_data[1] = addr;
    mf_com_data.mf_data[2] = *writedata;
    mf_com_data.mf_data[3] = *(writedata+1);
    status = pcd_com_transceive(pi);
#if (NFC_DEBUG)
    printf(" sta=%bd\n", status);
#endif

    return status;
}


//////////////////////////////////////////////////////////////////////
//SR176��������
//////////////////////////////////////////////////////////////////////
int8_t protect_sr176(uint8_t lockreg)
{
    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
    printf("protect_sr176()->\n");
#endif

    pcd_set_tmo(5);
    mf_com_data.mf_command = PCD_TRANSMIT;
    mf_com_data.mf_length  = 4;
    mf_com_data.mf_data[0] = 0x09;
    mf_com_data.mf_data[1] = 0x0F;
    mf_com_data.mf_data[2] = 0;
    mf_com_data.mf_data[3] = lockreg;
    status = pcd_com_transceive(pi);

#if (NFC_DEBUG)
    printf(" sta=%bd\n", status);
#endif

    return status;
}

//////////////////////////////////////////////////////////////////////
//COMPLETION ST
//////////////////////////////////////////////////////////////////////
int8_t completion_sr(void)
{
    int8_t status;
    transceive_buffer  mf_com_data;
    transceive_buffer  *pi;
    pi = &mf_com_data;
#if (NFC_DEBUG)
    printf("completion_sr()->\n");
#endif
    pcd_set_tmo(5);
    mf_com_data.mf_command = PCD_TRANSMIT;
    mf_com_data.mf_length  = 1;
    mf_com_data.mf_data[0] = 0x0F;
    status = pcd_com_transceive(pi);
#if (NFC_DEBUG)
    printf(" sta=%bd\n", status);
#endif
    return status;
}


