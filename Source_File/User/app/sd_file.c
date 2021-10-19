
#include "sd_file.h"
#include <string.h>
#include <stdlib.h>

extern uint8_t ReceiveBuff[RECEIVEBUFF_SIZE];
extern uint8_t SendBuff[SENDBUFF_SIZE];

uint8_t write_flag=0;
char File_Receive_Name[200];  //�ļ���
void test(void);

//�ļ�ϵͳ��ر���
FRESULT res;
DIR dir;
FILINFO  fno;
FIL fil;
UINT fnumber;

uint16_t  file_num; 

CUR_FILE  Cur_File;

uint16_t   NestDepth=0;  //�ļ���Ƕ�����

uint8_t   cur_env=0;    //��ǰ�����������ļ��л����ļ��ڲ�

Touch_Button  button[BUTTON_NUM];   //��������
char File_Name[BUTTON_NUM][FILE_NAME_LENGTH];  //�ļ�������
uint8_t  IS_DIR[BUTTON_NUM];  //�Ƿ����ļ�������
char CurPath[256];   //��ǰ·��
  
BYTE file_read_buffer[5000];  //��ȡ�ļ���������

char cur_page_read_buffer[FILE_PAGE_MAX_BTYE];
uint16_t  File_Pages[MAX_NUM_PAGE];
uint8_t  Total_Txt_Pages;

int8_t num_read_page = 0; // �ļ����ݵ�ҳ��
int8_t file_num_read_page = 0; // �ļ���ҳ��
/****************************************************************/
static void Draw_File_Button(void *btn);     
static void Draw_Function_Button(void *btn); 
static void Draw_NextPage_Button(void *btn); // ������һҳ���� 
static void Draw_PrePage_Button(void *btn); // ������һҳ���� 

static void Command_Select_File(void *btn);
static void Command_Select_Function(void *btn);
static void Command_Select_NextPage(void *btn); //��һҳ����ѡ��
static void Command_Select_PrePage(void *btn); //��һҳ����ѡ��

static void File_Button_Disable(void);
//static void File_Button_Enable(void);
//static void Function_Button_Disable(void);
static void Fuction_Button_Enable(void);

static void File_Disable(void);
// ʹ��/ʧ����һҳ������ Function_Button_Dis/Enable��ʵ��
// ע����һҳʼ��ʹ��
//static void NextPage_Button_Enable(void); // ʹ����һҳ����ѡ��
//static void NextPage_Button_Disable(void); // ʧ����һҳ����ѡ��
static void File_Page_Init(void); //��ʼ���ļ�����ÿһҳҳ��


/******************************************************************/

static void Open_File(char *path);

static uint16_t  Last_Slash(char *path);   //���һ��/��λ��

/**
* @brief  Touch_Button_Init ��ʼ����ť����
* @param  ��
* @retval ��
*/
void Touch_Button_Init(void){
	uint16_t  i;
	uint16_t file_current_x=FILE_BUTTON_START_X ;
	uint16_t file_current_y=FILE_BUTTON_START_Y;
	
    uint8_t start_position = FUNCTION_BUTTON_NUM + file_num_read_page * NUM_FILE_ONE_PAGE;
    uint8_t end_position = FUNCTION_BUTTON_NUM + (file_num_read_page + 1) * NUM_FILE_ONE_PAGE;
    
	//���书������ļ���Ŀ¼�����ð�����ʵ��
	//��һ����ťΪ�̶���ť�������ļ��ĸĶ����仯
	//����Ϊ�˳��ļ���ʾ���򷵻��ϼ�Ŀ¼
    button[0].start_x = FUNCTION_BUTTON_START_X;
    button[0].start_y = FUNCTION_BUTTON_START_Y;
    button[0].end_x = FUNCTION_BUTTON_START_X + FUNCTION_BLOCK_WIDTH ;
    button[0].end_y = FUNCTION_BUTTON_END_Y;
    button[0].para = CL_GREY;
	if(NestDepth > 0){
	     button[0].en=1;   // �˳�����ʹ��
	}
	else{
		 button[0].en=0;
	}
    button[0].touch_flag = 0;  
    button[0].draw_btn = Draw_Function_Button;
    button[0].btn_command = Command_Select_Function ; 

    // 1�Ű��� �� ��һҳ
    button[1].start_x = FUNCTION_BUTTON_START_X + 2 * FUNCTION_BLOCK_WIDTH;
    button[1].start_y = FUNCTION_BUTTON_START_Y;
    button[1].end_x = FUNCTION_BUTTON_START_X + 3 * FUNCTION_BLOCK_WIDTH ;
    button[1].end_y = FUNCTION_BUTTON_END_Y;
    button[1].para = CL_GREY;
    button[1].en = 1;
    button[1].touch_flag = 0;  
    button[1].draw_btn = Draw_NextPage_Button;
    button[1].btn_command = Command_Select_NextPage ;  
    // ��������1����

    // 2�Ű��� �� ��һҳ
    button[2].start_x = FUNCTION_BUTTON_START_X + FUNCTION_BLOCK_WIDTH;
    button[2].start_y = FUNCTION_BUTTON_START_Y;
    button[2].end_x = FUNCTION_BUTTON_START_X + 2 * FUNCTION_BLOCK_WIDTH ;
    button[2].end_y = FUNCTION_BUTTON_END_Y;
    button[2].para = CL_GREY;
    button[2].en = 1;
    button[2].touch_flag = 0;  
    button[2].draw_btn = Draw_PrePage_Button;
    button[2].btn_command = Command_Select_PrePage;  
    // ��������2����
    
	//�ļ���ʾģ��
    File_Disable(); // disable �޹��ļ�
	for(i = start_position; i < end_position; i++){
		button[i].start_x = file_current_x;
        button[i].start_y = file_current_y;
        button[i].end_x = file_current_x+ FILE_BLOCK_WIDTH;
        button[i].end_y = file_current_y+FILE_BLOCK_HEIGHT;
		if (IS_DIR[i-FUNCTION_BUTTON_NUM]) { //�ж��Ƿ���Ŀ¼
			button[i].para = CL_ORANGE;
			button[i].is_dir=1;
		} else {
            button[i].para = CL_GREEN;
			button[i].is_dir=0;
		}
		button[i].en=1;
        button[i].touch_flag = 0;  
		strcpy(button[i].fname,File_Name[i-FUNCTION_BUTTON_NUM]);
        button[i].draw_btn = Draw_File_Button;
        button[i].btn_command = Command_Select_File;  
		
		//��ֹ���
		file_current_x += FILE_BLOCK_WIDTH;
		if (file_current_x >= FILE_BUTTON_END_X) {
			file_current_x =FILE_BUTTON_START_X;
			file_current_y +=FILE_BLOCK_HEIGHT;
		}
	}
	
	
}

/**
* @brief  Buttuon_Fresh  ˢ�°�ť�����ļ�ϵͳ����һ��
* @param  ��
* @retval ��
*/
void Button_Fresh(char *path){
	 uint8_t i;
    
	 find_sd_file(path);
	 LCD_SetBackColor(CL_WHITE);
	 ILI9341_Clear (0, 0, LCD_X_LENGTH, LCD_Y_LENGTH);
	
	 Touch_Button_Init();
	 
     // ˢ����ʾ
	 for (i = 0; i < FUNCTION_BUTTON_NUM + file_num; i++){
        if(button[i].en) {
            button[i].draw_btn(&button[i]);
        }   
     }
     
     
    
    
          
     Cur_File.CurrentFileName=NULL;
     if (strcmp(CurPath,path) != 0) {
         memset(CurPath, '\0', sizeof(CurPath));
         strcpy(CurPath,path);
     }
     cur_env=0;
}

/**
* @brief  Draw_File_Button  �����ʾ�ļ��İ�ť 
* @param  ��
* @retval ��
*/
static void Draw_File_Button(void *btn){
	Touch_Button *ptr = (Touch_Button *)btn;
	uint32_t  temp_color;
	 if(ptr->touch_flag==0){      //����δ��ѡ��
		 LCD_SetColors(ptr->para,CL_WHITE);
		 temp_color=ptr->para;
     ILI9341_DrawRectangle(	ptr->start_x,
							ptr->start_y,
							ptr->end_x - ptr->start_x,
							ptr->end_y - ptr->start_y,1);
	 }
	 else{    //������ѡ��
		 LCD_SetColors(CL_WHITE,CL_WHITE);
		 temp_color=CL_WHITE;
		 ILI9341_DrawRectangle(	ptr->start_x,
								ptr->start_y,
															ptr->end_x - ptr->start_x,
															ptr->end_y - ptr->start_y,1);
	 }
	 
	 //��ʾ�ļ���
	 LCD_SetColors(CL_BLACK,temp_color);
	 LCD_SetFont(&Font8x16);
	 ILI9341_DispString_EN_CH_Custom(ptr->start_x,ptr->start_y,ptr->end_x,ptr->end_y,ptr->fname);
	 
	 //���߿�
     ILI9341_DrawRectangle(	ptr->start_x,
															ptr->start_y,
															ptr->end_x - ptr->start_x,
															ptr->end_y - ptr->start_y,0);
	 
}

/**
* @brief  Draw_Function_Button  ��淵�ذ�ť 
* @param  ��
* @retval ��
*/
static void Draw_Function_Button(void *btn){
	uint32_t temp_color;
	Touch_Button *ptr = (Touch_Button *)btn;
	
	 if(ptr->touch_flag==0){      //����δ��ѡ��
		 LCD_SetColors(ptr->para,CL_WHITE);
		 temp_color=ptr->para;
        ILI9341_DrawRectangle(	ptr->start_x,
															ptr->start_y,
															ptr->end_x - ptr->start_x,
															ptr->end_y - ptr->start_y,1);
	 }
	 else{    //������ѡ��
		 LCD_SetColors(CL_GREY3	,CL_WHITE);
		 temp_color=CL_GREY3;
		 ILI9341_DrawRectangle(	ptr->start_x,
															ptr->start_y,
															ptr->end_x - ptr->start_x,
															ptr->end_y - ptr->start_y,1);
	 }
	 
	 //��ʾ������
	 LCD_SetColors(CL_RED,temp_color);
	 LCD_SetFont(&Font8x16);
	 ILI9341_DispString_EN_CH( ptr->start_x + (ptr->end_x - ptr->start_x - 16*2 )/2,                 
							   ptr->start_y + ((ptr->end_y - ptr->start_y-16)/2),	
							   "����");
	 
	 //���߿�
     ILI9341_DrawRectangle(	ptr->start_x,
							ptr->start_y,
							ptr->end_x - ptr->start_x,
							ptr->end_y - ptr->start_y,0);
}

/**
* @brief  Draw_NextPage_Button  ���ѡ����һҳ�İ�ť 
* @param  ��
* @retval ��
*/
static void Draw_NextPage_Button(void *btn) { 
    uint32_t temp_color;
	Touch_Button *ptr = (Touch_Button *)btn;
   
    if (ptr->touch_flag == 0) {      //����δ��ѡ��   
		LCD_SetColors(ptr->para,CL_WHITE);
		temp_color=ptr->para;
        ILI9341_DrawRectangle(ptr->start_x, ptr->start_y,
                              ptr->end_x - ptr->start_x, 
                              ptr->end_y - ptr->start_y, 1);

	} else {  // ��һҳ������ѡ��   
        LCD_SetColors(CL_GREY3, CL_WHITE);
		temp_color = CL_GREY3;
		ILI9341_DrawRectangle(ptr->start_x, 
							  ptr->start_y, 
							  ptr->end_x - ptr->start_x,
							  ptr->end_y - ptr->start_y, 1);
    }
     //��ʾ������
	 LCD_SetColors(CL_RED,temp_color);
	 LCD_SetFont(&Font8x16);
	 ILI9341_DispString_EN_CH(ptr->start_x + (ptr->end_x - ptr->start_x - 16*2 ) / 2,                 
							  ptr->start_y + ((ptr->end_y - ptr->start_y- 16) / 2),	
							   "��һҳ");
	 
	 //���߿�
     ILI9341_DrawRectangle(	ptr->start_x,
							ptr->start_y,
							ptr->end_x - ptr->start_x,
							ptr->end_y - ptr->start_y, 0);
    
}

/**
* @brief  Draw_PrePage_Button  ���ѡ����һҳ�İ�ť 
* @param  ��
* @retval ��
*/
static void Draw_PrePage_Button(void *btn) { 
    uint32_t temp_color;
	Touch_Button *ptr = (Touch_Button *)btn;
   
    if (ptr->touch_flag == 0) {      //����δ��ѡ��   
		LCD_SetColors(ptr->para,CL_WHITE);
		temp_color=ptr->para;
        ILI9341_DrawRectangle(ptr->start_x, ptr->start_y,
                              ptr->end_x - ptr->start_x, 
                              ptr->end_y - ptr->start_y, 1);

	} else {  // ��һҳ������ѡ��   
        LCD_SetColors(CL_GREY3, CL_WHITE);
		temp_color = CL_GREY3;
		ILI9341_DrawRectangle(ptr->start_x, 
							  ptr->start_y, 
							  ptr->end_x - ptr->start_x,
							  ptr->end_y - ptr->start_y, 1);
    }
     //��ʾ������
	 LCD_SetColors(CL_RED,temp_color);
	 LCD_SetFont(&Font8x16);
	 ILI9341_DispString_EN_CH(ptr->start_x + (ptr->end_x - ptr->start_x - 16*2 ) / 2,                 
							  ptr->start_y + ((ptr->end_y - ptr->start_y- 16) / 2),	
							   "��һҳ");
	 
	 //���߿�
     ILI9341_DrawRectangle(	ptr->start_x,
							ptr->start_y,
							ptr->end_x - ptr->start_x,
							ptr->end_y - ptr->start_y, 0);
    
}

/**
* @brief  Command_Select_File   �л���ǰѡ�е��ļ�
* @param   ��
* @retval ��
*/
static void Command_Select_File(void *btn){
	Touch_Button *ptr = (Touch_Button *)btn;
	Cur_File.CurrentFileName=ptr->fname;
	Cur_File.is_dir=ptr->is_dir;
}

/**
* @brief  Command_Select_Function   ѡ�з��ؼ�
* @param   ��
* @retval ��
*/
static void Command_Select_Function(void *btn){
	uint16_t index;
	char temp_path[256];
	memset(temp_path,'\0',sizeof(temp_path));
	
	index=Last_Slash(CurPath);
	strncpy(temp_path,CurPath,index);
	NestDepth--;
	Button_Fresh(temp_path);
	cur_env=0;
    num_read_page = 0;
}

/**
* @brief  Command_Select_NextPage   ѡ����һҳ���ܼ�
* @param   ��
* @retval ��
*/

static void Command_Select_NextPage(void *btn) {
	  uint16_t num=0,i;
    if (cur_env == 1) { //�ļ��ڲ�
        LCD_SetBackColor(CL_WHITE);
        LCD_SetTextColor(BLACK);
        ILI9341_Clear (0,0, LCD_X_LENGTH, LCD_Y_LENGTH);
        
        // ����
        
       num_read_page = (num_read_page + 1) % (Total_Txt_Pages); 
        //ҳ����һ������Ѿ��������һҳ����ô��һҳ���ǵ�һҳ
        for(i =0;i<num_read_page;i++){
					num+=File_Pages[i];
				}
				memset(cur_page_read_buffer,'\0',sizeof(cur_page_read_buffer));
        strncpy((char*)cur_page_read_buffer, 
                (char*)file_read_buffer + num, 
                File_Pages[num_read_page]);
                
		ILI9341_DispString_EN_CH_Custom(0, 0, LCD_X_LENGTH,
                                        LCD_Y_LENGTH - FUNCTION_BLOCK_HEIGHT,
                                        (char *)cur_page_read_buffer);
		// ������ݵ���ʾ��	
		
		button[0].draw_btn(&button[0]);
        button[1].draw_btn(&button[1]); //������һҳ���� 
        button[2].draw_btn(&button[2]); //������һҳ����
    } else { //�ļ��ⲿ
        if (++file_num_read_page >= FILE_MAX_NUM_PAGE) {
            file_num_read_page--;
            return;
        } 
        Fuction_Button_Enable();
		Button_Fresh(CurPath);
    }
}

/**
* @brief  Command_Select_PrePage   ѡ����һҳ���ܼ�
* @param   ��
* @retval ��
*/

static void Command_Select_PrePage(void *btn) { 
     uint16_t num=0,i;
     if (cur_env == 1) { //�ļ��ڲ�
         
         LCD_SetBackColor(CL_WHITE);
         LCD_SetTextColor(BLACK);
         ILI9341_Clear (0,0, LCD_X_LENGTH, LCD_Y_LENGTH); 
         // ����
         
          if (num_read_page > 0 ) {
            num_read_page = num_read_page - 1;
         }
          for(i =0;i<num_read_page;i++){
					num+=File_Pages[i];
				}
         
         //ҳ����һ
        memset(cur_page_read_buffer,'\0',sizeof(cur_page_read_buffer));
         strncpy((char*)cur_page_read_buffer, 
                 (char*)file_read_buffer +num, 
                 File_Pages[num_read_page]);
		 // ��ȡ��Ҫ��ʾҳ������
                
		 ILI9341_DispString_EN_CH_Custom(0, 0, LCD_X_LENGTH,
                                         LCD_Y_LENGTH - FUNCTION_BLOCK_HEIGHT,
                                         (char *)cur_page_read_buffer);
		 // ������ݵ���ʾ��	
		
		 button[0].draw_btn(&button[0]);
         button[1].draw_btn(&button[1]); //������һҳ����    
         button[2].draw_btn(&button[2]); //������һҳ���� 
     } else { //�ļ��ⲿ
        if (--file_num_read_page < 0) {
            file_num_read_page++;
            return;
        }
        Fuction_Button_Enable();
		Button_Fresh(CurPath);
     }
}

/**
* @brief  Touch_Button_Down ����������ʱ���õĺ������ɴ���������
* @param  x ����λ�õ�x����
* @param  y ����λ�õ�y����
* @retval ��
*/
void Touch_Button_Down(uint16_t x,uint16_t y)
{
  uint8_t i;
  for(i=0;i<file_num+FUNCTION_BUTTON_NUM;i++)
  {  
	if(button[i].en==0)  //�����Ƿ�ʹ��
			continue;
    /* �������˰�ť */
    if(x<=button[i].end_x && y<=button[i].end_y && y>=button[i].start_y && x>=button[i].start_x)
    {
      if(button[i].touch_flag == 0)     /*ԭ����״̬Ϊû�а��£������״̬*/
      {
      button[i].touch_flag = 1;         /* ��¼���±�־ */
      
      button[i].draw_btn(&button[i]);  /*�ػ水ť*/
      }        
      
    }
    else if(button[i].touch_flag == 1) /* �����Ƴ��˰����ķ�Χ��֮ǰ�а��°�ť */
    {
      button[i].touch_flag = 0;         /* ������±�־���ж�Ϊ�����*/
      
      button[i].draw_btn(&button[i]);   /*�ػ水ť*/
			
	  Cur_File.CurrentFileName=NULL;  //��ǰδѡ���ļ�
			
    }

  }

}

/**
* @brief  Touch_Button_Up �������ͷ�ʱ���õĺ������ɴ���������
* @param  x ��������ͷ�ʱ��x����
* @param  y ��������ͷ�ʱ��y����
* @retval ��
*/
void Touch_Button_Up(uint16_t x,uint16_t y)
{
   uint8_t i; 
   for(i=0;i<file_num+FUNCTION_BUTTON_NUM;i++)
   {
	  if(button[i].en==0)   //�����Ƿ�ʹ��
			 continue;
     /* �����ڰ�ť�����ͷ� */
      if((x<button[i].end_x && x>button[i].start_x && y<button[i].end_y && y>button[i].start_y))
      {        
			if(i==0 || i == 1 || i == 2) {
              button[i].touch_flag = 0;       /*�ͷŴ�����־*/
        
              button[i].draw_btn(&button[i]); /*�ػ水ť*/        
			}
      
        button[i].btn_command(&button[i]);  /*ִ�а����Ĺ�������*/
        
        break;
      }
    }  

}


/**
* @brief  find_sd_file Ϊ����sd�������ļ�
* @param  ��
* @retval ��
*/
void find_sd_file(char *path){
	char lf[FILE_NAME_LENGTH];  //���ļ�������
	memset(IS_DIR,0,sizeof(IS_DIR));//��ʼ���Ƿ���Ŀ¼��
    File_Name_Init();  //��ʼ���ļ�����
	res=f_opendir(&dir,path);
	file_num=0;	  
	if(res==FR_NO_PATH){
		printf("no path");
		
	}
	else if(res!=FR_OK){
		printf("open failed");
	}
	else{
		while(1){
            memset(lf,'\0',sizeof(lf));
			fno.lfname=lf;
			fno.lfsize=sizeof(lf);
			res=f_readdir(&dir,&fno);
			if(res!=FR_OK || !fno.fname[0]){  //Ŀ¼������
				break;
			}
			else if(fno.fattrib & AM_HID){  //�����ļ�
				
			}
			else if(fno.fattrib & AM_DIR){  //�ļ���
				 //printf("dir name is %s\n",fno.fname);
				 strcpy(File_Name[file_num],fno.fname);
                 IS_DIR[file_num]=1;
				 file_num++;
			}
			else if (fno.fattrib & AM_SYS){  //ϵͳ�ļ�
				
			}
			else{  //�����ļ�������ֻ�����洢
				
				if(!fno.lfname[0]){   //���ļ���
					if(fno.fname[0]){
						//printf("file name is %s\n",fno.fname);
						strcpy(File_Name[file_num],fno.fname);
				    file_num++;
					}
				}
				else{  //���ļ���
					  //printf("file name is %s\n",fno.lfname);
						strcpy(File_Name[file_num],fno.lfname);
				    file_num++;
				}
			}
 
		}
	}
	f_closedir(&dir);
	 
}


/**
* @brief   ��ʼ������
* @param  ��
* @retval ��
*/
void File_Name_Init(void){
	uint16_t i;
	for(i=0;i<BUTTON_NUM;i++){
		memset((char *)File_Name[i], '\0', sizeof(File_Name[i]));
	}
}


/**
* @brief  ��ⰴ��״̬����ִ����Ӧ����
* @param  ·��
* @retval ��
*/
void  KeyTrace(void){
	//key1���´����ļ���ʼ
	if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON )  
		{
			LED1_ON;  
			USARTx_SendFile();
			LED1_OFF;
		} 
		
   //key2���´��ļ���Ŀ¼
		if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
		{
			LED2_ON;
			Open_File(CurPath);
			LED2_OFF;
		}		
}

/**
* @brief  ����ʾ������ʾ�ļ�����
* @param  ��
* @retval ��
*/
static void Open_File(char *path){
	
	char file_path[256];
	if(Cur_File.CurrentFileName==NULL){   //û��ѡ���ļ�
		return;
	}
	LCD_SetBackColor(CL_WHITE);
	ILI9341_Clear (0,0, LCD_X_LENGTH, LCD_Y_LENGTH);
	
	//����·��Ϊxx/xx/../��ǰ�ļ���
	memset(file_path,'\0',sizeof(file_path));
	strcat(file_path,path);
	strcat(file_path,"/");
	strcat(file_path,Cur_File.CurrentFileName);
	
	memset(CurPath,'\0',sizeof(CurPath));
	strcpy(CurPath,file_path);
	NestDepth++;
	if(Cur_File.is_dir==0) {  //ѡ�е����ļ�
		memset(file_read_buffer,'\0',sizeof(file_read_buffer));
		File_Button_Disable();
	    Fuction_Button_Enable();
		res=f_open(&fil,file_path,FA_OPEN_EXISTING | FA_READ);
		if(res==FR_OK){
			res=f_read(&fil,file_read_buffer,sizeof(file_read_buffer),&fnumber);
			
      File_Page_Init();
            //����ȡ�����ļ����ݵ�ǰFILE_PAGE_MAX_BTYE���ֽڸ��Ƹ���ǰҳ�滺���ַ���
			memset(cur_page_read_buffer,'\0',sizeof(cur_page_read_buffer));
      strncpy((char*)cur_page_read_buffer, 
                    (char*)file_read_buffer , File_Pages[0]
                     );
										 
			if(res==FR_OK){ 
				ILI9341_DispString_EN_CH_Custom(0, 0, LCD_X_LENGTH,
                                                LCD_Y_LENGTH - FUNCTION_BLOCK_HEIGHT,
                                                (char *)cur_page_read_buffer);
			}
		}
		f_close(&fil);
		button[0].draw_btn(&button[0]);
        button[1].draw_btn(&button[1]); //������һҳ����
        button[2].draw_btn(&button[2]); //������һҳ����
		cur_env=1;
	}
	else{   //ѡ�е����ļ���
		 Fuction_Button_Enable();
		 Button_Fresh(file_path);
	}

}

/**
  * @brief  USARTx �����ļ���ʶ���ļ���������sd��
  * @param  ��
  * @retval ��
  */

void  USARTx_ReceiveFile(void){
    uint16_t  total_num;
    uint16_t index;
	char temp_data[RECEIVEBUFF_SIZE];
	 
	memset((char *)temp_data,'\0',sizeof(temp_data));
	strncpy((char *)temp_data,(char *)ReceiveBuff,strlen("txt_data_sta"));
	if(strcmp("txt_data_sta",temp_data)==0){   //ʶ�����ļ����俪ʼ
		LED1_ON;
		//printf("detect file\n");
		write_flag=1;
		memset((char *)File_Receive_Name,'\0',sizeof(File_Receive_Name));
		
		if(cur_env==1){  //��ǰλ���ļ��ڲ�
			index=Last_Slash(CurPath);
			strncpy(File_Receive_Name,CurPath,index);
			NestDepth--;
		}
		else{  //��ǰ���ļ����ڲ�
			strcpy(File_Receive_Name,CurPath);
		}
		memset(CurPath,'\0',sizeof(CurPath));
		strcpy(CurPath,File_Receive_Name);
		strcat(File_Receive_Name,"/");
		strcat(File_Receive_Name,(char *)ReceiveBuff+strlen("txt_data_sta"));
//		memset((char *)temp_data,'\0',sizeof(temp_data));
		
		res=f_open(&fil,(TCHAR *)File_Receive_Name,FA_CREATE_ALWAYS|FA_WRITE);
		if(res==FR_OK){
			//printf("file create success,file name is %s\n",(char *)File_Receive_Name);
		}
    f_close(&fil);
	}
	else if(strcmp("txt_data_end",temp_data)==0&&write_flag==1){ //ʶ�����ļ��������
		write_flag=0;
		Button_Fresh(CurPath);
		LED1_OFF;
		
	}
	else if(write_flag==1){  //ʶ�����ļ�����
		 
		res=f_open(&fil,File_Receive_Name,FA_OPEN_EXISTING|FA_WRITE);
       
        
    if(strlen((char *)ReceiveBuff)< sizeof(ReceiveBuff)){
			total_num=strlen((char *)ReceiveBuff);
		}
		else{
			total_num=sizeof(ReceiveBuff);
		}
        
        
        
		 f_lseek(&fil,f_size(&fil));  //���ļ�ָ���ƶ����ļ�ĩβ
		res=f_write(&fil,ReceiveBuff,total_num,&fnumber);

		f_close(&fil);
	}
	else{
		//printf("no meaning data");
	}

  memset(ReceiveBuff,'\0',sizeof(ReceiveBuff));
}
 
/**
  * @brief  USARTx �����ļ���ʶ���ļ���������sd��
  * @param  ��
  * @retval ��
  */
void  USARTx_SendFile(void){
	char file_path[256];
	if(Cur_File.CurrentFileName==NULL)
		return;
	memset(SendBuff,'\0',sizeof(SendBuff));
	strcpy((char *)SendBuff,"txt_data_sta");
	strcat((char *)SendBuff,Cur_File.CurrentFileName);
	//printf("%s\n",SendBuff);
	Usart_DMA_TX_Wait();
	
	//�ڶ��η��ͣ������ļ�����
	memset(file_path,'\0',sizeof(file_path));
	strcpy(file_path,CurPath);
	if(cur_env==0){  //��ǰ������ѡ�ļ��ڲ�
		strcat(file_path,"/");
		strcat(file_path,Cur_File.CurrentFileName);
	}
	res=f_open(&fil,file_path, FA_OPEN_EXISTING|FA_READ);
  if(res==FR_OK){
		while(1){
		    memset((char *)SendBuff,'\0',sizeof(SendBuff));
			res=f_read(&fil,(BYTE *)SendBuff,sizeof(SendBuff),&fnumber);
			Usart_DMA_TX_Wait();
			if(fnumber < sizeof(SendBuff)){  //�ļ���ȡ����
				break;
			}
		}
	}
  f_close(&fil);
	
	//�����η��ͣ������ļ����ͽ���
	memset((char *)SendBuff,'\0',sizeof(SendBuff));
	strcpy((char *)SendBuff,"txt_data_end");
	Usart_DMA_TX_Wait(); 
}



/**
* @brief  �ļ���ť����
* @param  ��
* @retval ��
*/
static void File_Button_Disable(void){
	uint16_t i;
	for(i=FUNCTION_BUTTON_NUM;i<file_num+FUNCTION_BUTTON_NUM;i++){
		button[i].en=0;
	}
}

/**
* @brief  �ļ���ťʹ��
* @param  ��
* @retval ��
*/
//static void File_Button_Enable(void){
//	uint16_t i;
//	for(i=FUNCTION_BUTTON_NUM;i<file_num+FUNCTION_BUTTON_NUM;i++){
//		button[i].en=1;
//	}
//}

/**
* @brief  ���ܰ�������
* @param  ��
* @retval ��
*/
//static void Function_Button_Disable(void){
//	uint16_t i;
//	for(i=0;i<FUNCTION_BUTTON_NUM;i++){
//        if (i == 1) { // 1�Ű�������һҳ��ʼ��ʹ��
//            continue;
//        }
//		button[i].en=0;
//	}
//}

/**
* @brief  ���ܰ���ʹ��
* @param  ��
* @retval ��
*/
static void Fuction_Button_Enable(void){
	uint16_t i;
	for(i=0;i<FUNCTION_BUTTON_NUM;i++){
		button[i].en=1;
	}
}


/**
* @brief  ���һ��/��λ��
* @param  ��
* @retval ��
*/
static uint16_t  Last_Slash(char *path){
	uint16_t i;
	uint16_t  index=0;
	
	for(i=0;i<strlen(path);i++){
		if(path[i]=='/'){
			index=i;
		}
	}
	return index;
}


static void File_Disable(void) {
     uint8_t i;
     uint8_t start_position = FUNCTION_BUTTON_NUM + file_num_read_page * NUM_FILE_ONE_PAGE;
     uint8_t end_position = FUNCTION_BUTTON_NUM + (file_num_read_page + 1) * NUM_FILE_ONE_PAGE;
     for (i = FUNCTION_BUTTON_NUM; i < BUTTON_NUM; i++) {
        if (i < start_position || i >= end_position) {
            button[i].en = 0;
        }
     }
}

/**
* @brief  ��ʼ���ļ�����ÿһҳ��ҳ������
* @param  ��
* @retval ��
*/
static void File_Page_Init(void){
	uint16_t count=0,cur_page=0;
	char *pStr=(char *)file_read_buffer;
	Total_Txt_Pages=0;
	num_read_page=0;
	 while(*pStr!='\0'){
		 if(*pStr<=126){
			 count++;
			 pStr++;
			 if(count==FILE_PAGE_MAX_BTYE-1){
				 File_Pages[cur_page]=count;
				 count=0;
				 cur_page++;
				 Total_Txt_Pages++;
				  
			 }
			  
		 }
		 else{
			 count+=2;
			 pStr+=2;
			 if(count>=FILE_PAGE_MAX_BTYE-1){
				 File_Pages[cur_page]=count;
				 count=0;
				 cur_page++;
				 Total_Txt_Pages++;
				  
			 }
			  
		 }
		 if(cur_page>=MAX_NUM_PAGE){
			 Total_Txt_Pages--;
			 return;
		 }
	 }
	 if(count!=0){
		 File_Pages[cur_page]=count;
		 Total_Txt_Pages++;
	 }
//	 for(count=0;count<Total_Txt_Pages;count++){
//		 printf("%d\n",File_Pages[count]);
//	 }
}

