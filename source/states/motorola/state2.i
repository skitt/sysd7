# 1 "state2.c"
 






































# 1 "/home/sysd7/source/include/motorola.video.h" 1



 



 











int RLEencode(unsigned char *, int, unsigned char *);



# 40 "state2.c" 2

# 1 "/home/sysd7/source/include/serial.h" 1



 










 
typedef struct
{
  unsigned char datatype;
  unsigned int length;
  unsigned char * data;
} packet;

 

typedef void (* packethandler) (packet *);

 
int serialinit (void);

 
int serialclose (void);

 
 
 

int senddata (packet *);

 
 

 
int registerhandler (unsigned char, packethandler);

 


 
int unregisterhandler (unsigned char);

 
int ping (void);


# 41 "state2.c" 2

# 1 "/usr/lib/gcc-lib/m68k-68board-blob/include/m68kboard.h" 1 3

struct mc68230fm {
		unsigned char	fill0;
		unsigned char	fill1;
		unsigned char	fill2;
	unsigned char	gcr;
		unsigned char	fill4;
		unsigned char	fill5;
		unsigned char	fill6;
	unsigned char	srr;
		unsigned char	fill8;
		unsigned char	fill9;
		unsigned char	filla;
	unsigned char	ddra;
		unsigned char	fillc;
		unsigned char	filld;
		unsigned char	fille;
	unsigned char	ddrb;
		unsigned char	fill10;
		unsigned char	fill11;
		unsigned char	fill12;
	unsigned char	ddrc;
		unsigned char	fill14;
		unsigned char	fill15;
		unsigned char	fill16;
	unsigned char	pivr;
		unsigned char	fill18;
		unsigned char	fill19;
		unsigned char	fill1a;
	unsigned char	cra;
		unsigned char	fill1c;
		unsigned char	fill1d;
		unsigned char	fill1e;
	unsigned char	crb;
		unsigned char	fill20;
		unsigned char	fill21;
		unsigned char	fill22;
	unsigned char	dra;
		unsigned char	fill24;
		unsigned char	fill25;
		unsigned char	fill26;
	unsigned char	drb;
		unsigned char	fill28;
		unsigned char	fill29;
		unsigned char	fill2a;
	unsigned char	ara;
		unsigned char	fill2c;
		unsigned char	fill2d;
		unsigned char	fill2e;
	unsigned char	arb;
		unsigned char	fill30;
		unsigned char	fill31;
		unsigned char	fill32;
	unsigned char	drc;
		unsigned char	fill34;
		unsigned char	fill35;
		unsigned char	fill36;
	unsigned char	sr;
		unsigned char	fill38;
		unsigned char	fill39;
		unsigned char	fill3a;
	unsigned char	nc1;
		unsigned char	fill3c;
		unsigned char	fill3d;
		unsigned char	fill3e;
	unsigned char	nc2;
		unsigned char	fill40;
		unsigned char	fill41;
		unsigned char	fill42;
	unsigned char	tcr;
		unsigned char	fill44;
		unsigned char	fill45;
		unsigned char	fill46;
	unsigned char	tivr;
		unsigned char	fill48;
		unsigned char	fill49;
		unsigned char	fill4a;
	unsigned char	nc3;
		unsigned char	fill4c;
		unsigned char	fill4d;
		unsigned char	fill4e;
	unsigned char	cph;
		unsigned char	fill50;
		unsigned char	fill51;
		unsigned char	fill52;
	unsigned char	cpm;
		unsigned char	fill54;
		unsigned char	fill55;
		unsigned char	fill56;
	unsigned char	cpl;
		unsigned char	fill58;
		unsigned char	fill59;
		unsigned char	fill5a;
	unsigned char	nc4;
		unsigned char	fill5c;
		unsigned char	fill5d;
		unsigned char	fill5e;
	unsigned char	crh;
		unsigned char	fill60;
		unsigned char	fill61;
		unsigned char	fill62;
	unsigned char	crm;
		unsigned char	fill64;
		unsigned char	fill65;
		unsigned char	fill66;
	unsigned char	crl;
		unsigned char	fill68;
		unsigned char	fill69;
		unsigned char	fill6a;
	unsigned char	tsr;
};

struct mc68681rfm {
	unsigned char		fill0;
	unsigned char		fill1;
	unsigned char		fill2;
	unsigned char	mr1a;

	unsigned char		fill4;
	unsigned char		fill5;
	unsigned char		fill6;
	unsigned char	sra;
	unsigned char		fill8;
	unsigned char		fill9;
	unsigned char		fillA;
	unsigned char	ignore;
	unsigned char		fillC;
	unsigned char		fillD;
	unsigned char		fillE;
	unsigned char	rba;
	unsigned char		fill10;
	unsigned char		fill11;
	unsigned char		fill12;
	unsigned char	ipcr;
	unsigned char		fill14;
	unsigned char		fill15;
	unsigned char		fill16;
	unsigned char	isr;
	unsigned char		fill18;
	unsigned char		fill19;
	unsigned char		fill1A;
	unsigned char	cur;
	unsigned char		fill1C;
	unsigned char		fill1D;
	unsigned char		fill1E;
	unsigned char	clr;
	unsigned char		fill20;
	unsigned char		fill21;
	unsigned char		fill22;
	unsigned char	mr1b;

	unsigned char		fill24;
	unsigned char		fill25;
	unsigned char		fill26;
	unsigned char	srb;
	unsigned char		fill28;
	unsigned char		fill29;
	unsigned char		fill2A;
	unsigned char	ignore2;
	unsigned char		fill2C;
	unsigned char		fill2D;
	unsigned char		fill2E;
	unsigned char	rbb;
	unsigned char		fill30;
	unsigned char		fill31;
	unsigned char		fill32;
	unsigned char	ivr;
	unsigned char		fill34;
	unsigned char		fill35;
	unsigned char		fill36;
	unsigned char	ip;
	unsigned char		fill38;
	unsigned char		fill39;
	unsigned char		fill3A;
	unsigned char	start;
	unsigned char		fill3C;
	unsigned char		fill3D;
	unsigned char		fill3E;
	unsigned char	stop;
};

struct mc68681wfm {
	unsigned char		fill00;
	unsigned char		fill01;
	unsigned char		fill02;
	unsigned char	mr1a;
	unsigned char		fill04;
	unsigned char		fill05;
	unsigned char		fill06;
	unsigned char	csra;
	unsigned char		fill08;
	unsigned char		fill09;
	unsigned char		fill0A;
	unsigned char	cra;
	unsigned char		fill0C;
	unsigned char		fill0D;
	unsigned char		fill0E;
	unsigned char	tba;
	unsigned char		fill10;
	unsigned char		fill11;
	unsigned char		fill12;
	unsigned char	acr;
	unsigned char		fill14;
	unsigned char		fill15;
	unsigned char		fill16;
	unsigned char	imr;
	unsigned char		fill18;
	unsigned char		fill19;
	unsigned char		fill1A;
	unsigned char	ctur;
	unsigned char		fill1C;
	unsigned char		fill1D;
	unsigned char		fill1E;
	unsigned char	ctlr;
	unsigned char		fill20;
	unsigned char		fill21;
	unsigned char		fill22;
	unsigned char	mr1b;
	unsigned char		fill24;
	unsigned char		fill25;
	unsigned char		fill26;
	unsigned char	csrb;
	unsigned char		fill28;
	unsigned char		fill29;
	unsigned char		fill2A;
	unsigned char	crb;
	unsigned char		fill2C;
	unsigned char		fill2D;
	unsigned char		fill2E;
	unsigned char	tbb;
	unsigned char		fill30;
	unsigned char		fill31;
	unsigned char		fill32;
	unsigned char	ivr;
	unsigned char		fill34;
	unsigned char		fill35;
	unsigned char		fill36;
	unsigned char	opcr;
	unsigned char		fill38;
	unsigned char		fill39;
	unsigned char		fill3A;
	unsigned char	opr_set;
	unsigned char		fill3C;
	unsigned char		fill3D;
	unsigned char		fill3E;
	unsigned char	opr_reset;

};










# 42 "state2.c" 2

# 1 "/usr/lib/gcc-lib/m68k-68board-blob/include/quickcam.h" 1 3
 





extern int QC_init();
extern int QC_setexposure(unsigned char value);
extern int QC_setcontrast(unsigned char value);
extern int QC_grabframe(unsigned char *buffer);
extern int QC_grabdata(unsigned char *buffer,int howmuch);

# 43 "state2.c" 2

# 1 "/home/sysd7/source/include/support.h" 1



 




 

 


void puts (char *);








 

 


void putn (unsigned long, int, int);


# 44 "state2.c" 2

# 1 "/home/sysd7/source/include/data.h" 1



 




typedef enum
{
  DEFAULT,
  PING,          
  PING_ACK,      
  BUZZER,        
  BUZZER_ACK,    
  CONTROL,       
  DATABASE,      
  AUDIO,         
  VIDEO          
} datatype_t;


# 45 "state2.c" 2

# 1 "/home/sysd7/source/include/control.h" 1
 












 









typedef enum
{
   
  RESET_MOTOROLA,
  RESTART_VIDEO,
} controltag_t;
# 46 "state2.c" 2






 
unsigned char in[2][19200 ];
unsigned char diff[19200 ];
unsigned char compressed[19200 +1];

 
int goToInit=0;

 
int retransmit;

void control_handler(packet p)
{
  if (p.data[0]==RESET_MOTOROLA)
    goToInit=1;
}

void main()
{
  packet pkt;
  int errno, current=0, last=1, csz, x, rt, *a, *b, *c;
  retransmit=1;  
  if((errno=QC_init())== 0 )
    {
       
      registerhandler(CONTROL,control_handler);

      while((!checkring())&&(!checkkeypad())&&(!goToInit))
	{
	  rt=retransmit;  


	  retransmit=0;
	   
	  if(rt==1)
	    {    
	       

	      for(x=0;x< 19200 ;x++)
		{
		  in[last][x]=0;
		}
	    }
	   
	  if ((errno = QC_grabframe (in[current])) == 0 )
	    {
	       
	      for(x=0;x< 19200 ;x++)
		{
		   if (in[current][x] != 0)
		    in[current][x]=16-in[current][x];

		   if(in[current][x-1]==in[current][x+1])
		     in[current][x]=in[current][x-1];

		}
	       




	      a=(int *) in[current];
	      b=(int *) in[last];
	      c=(int *) diff;
	      for(x=0;x<(19200 /4);x++)
		c[x] = a[x] ^ b[x];

	       


	      csz=RLEencode(diff,19200 ,compressed+1);
	      
	       

	      compressed[0]=rt;
	      
	       

	      pkt.datatype=VIDEO;
	      pkt.length=csz/8+2;
	      pkt.data=compressed;

	       

	      if( senddata(pkt) != 0)
		retransmit=1;
	      
	       
	      current=1-current;
	      last=1-last;
	    }
	  else
	    {
	       
	      switch (errno)
		{
		case -1 :
		  puts ("Framegrab: Bad response from camera\n\r");
		  break;
		case -2 :
		  puts ("Framegrab: Camera timed out\n\r");
		  break;
		}
	    }
	}
       
      unregisterhandler(CONTROL);
    }
  else
    {
       
      switch (errno)
	{
	case -1 :
	  puts ("Init: Bad response from camera\n\r");
	  break;
	case -2 :
	  puts ("Init: Camera timed out\n\r");
	  break;
	}
    }
}
