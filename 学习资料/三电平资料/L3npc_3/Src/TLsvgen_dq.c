/*=====================================================================================
 File name:        SVGEN_DQ.C  (IQ version)                  
                    
 Originator:	Digital Control Systems Group
			Texas Instruments

 Description:  Space-vector PWM generation based on d-q components                    

=====================================================================================
 History:
-------------------------------------------------------------------------------------
 05-15-2002	Release	Rev 1.0
-------------------------------------------------------------------------------------*/

#include "l3_global.h"

void TLsvgendq_calc(TLSVGENDQ *v)
{	

	_iq Va,Vb,Vc,temp1;
	_iq sector 	= 0; 
	_iq region 	= 0; 
	_iq theta  	= 0;
	_iq sin_ang	= 0;
	_iq cos_ang	= 0;
	_iq Valf	= 0;
	_iq Vbet	= 0;
	_iq temp0	= 0;
	_iq X1		= 0;
	_iq	X2		= 0;
	_iq Tx		= 0;
	_iq	Ty		= 0;
	_iq	Tmin	= 0;
	_iq Tmid	= 0;
	_iq	Tmax	= 0;
#if L3_SV_MODE == L3_SV_SELECTSR
	_iq NewSR	= 0;
//	_iq Km_sv = 0;
#elif L3_SV_MODE == L3_SV_SMALLVEC
	_iq Ksmall_sv = 0;
#endif																
/* 1/sqrt(3) = 0.57735026918963 */
/* 	 sqrt(3) = 1.73205080756888 */

    Va = v->Ubeta;
    Vb = _IQmpy(_IQ(-0.5),v->Ubeta) + _IQmpy(_IQ(0.8660254),v->Ualfa);  
    Vc = _IQmpy(_IQ(-0.5),v->Ubeta) - _IQmpy(_IQ(0.8660254),v->Ualfa);  
   // v->Vaa = Vb;
  //  v->Vbb = Vc;

  //  Va = v->Ubeta>>1;
   // Vb = _IQmpy(_IQ(-0.5),(v->Ubeta>>1)) + _IQmpy(_IQ(0.8660254),(v->Ualfa>>1));  
   // Vc = _IQmpy(_IQ(-0.5),(v->Ubeta>>1)) - _IQmpy(_IQ(0.8660254),(v->Ualfa>>1));  

//calc sector
    if (Va>_IQ(0))
       sector = 1; 
    if (Vb>_IQ(0))
       sector = sector + 2;
    if (Vc>_IQ(0))   
       sector = sector + 4;
     v->sec = sector; 

	switch(sector)
	{
		case 1: sector = 2;
			break;
		
		case 2: sector = 6;
			break;
		
		case 3: sector = 1;
			break;
		
		case 4: sector = 4;
			break;
		
		case 5: sector = 3;
			break;
		
		case 6: sector = 5;
			break;
		
	}

	v->sec = sector;

    Va = v->Ubeta;                                                  
    Vb = _IQmpy(_IQ(0.5),v->Ubeta) + _IQmpy(_IQ(0.8660254),v->Ualfa);   
    Vc = _IQmpy(_IQ(0.5),v->Ubeta) - _IQmpy(_IQ(0.8660254),v->Ualfa);   
//calc region   先将vref转换到第一个扇区 
    theta = (sector-1)*_IQ(0.166666667);
    sin_ang = _IQsinPU(theta);
    cos_ang = _IQcosPU(theta);
    Valf = _IQmpy(v->Ualfa,cos_ang) + _IQmpy(v->Ubeta,sin_ang);
    Vbet = _IQmpy(v->Ubeta,cos_ang) - _IQmpy(v->Ualfa,sin_ang);
   
   temp0 =  _IQmpy(Vbet,_IQ(0.57735026918963));
   temp1 =  _IQmpy(Vbet,_IQ(1.73205080756888));
   
   if((Valf+temp0)<=_IQ(0.57735026918963)) // <=Ed/3  Ed/sqrt(3)=1
   	{
   		if((Valf-temp1)<0)
    		region = 1;
   		else
   			region = 2;
   	}
   	else if(Vbet>_IQ(0.5))						// sqrt(3)*Ed/6 -> 0.5
   			region = 5;
		else if((Valf-temp0)<=_IQ(0.57735026918963))
		{
				if((Valf-temp1)<0)
					region = 3;
				else
					region = 4;
									
		}
		else
			region = 6;

		v->Vaa = region;
 
//calc Ta,Tb,Tc
 		X1 = _IQmpy(_IQ(1.73205080756888),v->Ualfa)-v->Ubeta; //电压以Ed/sqrt(3)标幺
 		X2 = _IQmpy(_IQ(2),v->Ubeta);
 		
 		switch (sector)
 		{
 			case 1:
 				{
 					switch (region)
 					{
 						case 1:
 							Tx = _IQ(1)-X1-X2;
 							Ty = X1;
 							break;
 						case 2:
 							Tx = X2;
 							Ty = _IQ(1)-X1-X2;
 							break;
 						case 3:
 							Tx = X1+X2-_IQ(1);
 							Ty = _IQ(1)-X2;
 							break;
 						case 4:
 							Tx = _IQ(1)-X1;
 							Ty = X1+X2-_IQ(1);
 							break;
 						case 5:
 							Tx = X1;
 							Ty = X2-_IQ(1);
 							break;
 						case 6:
 							Tx = X1-_IQ(1);
 							Ty = X2;
 							break;
 						}
						break;
 					}
 			case 2:
 				{
 					switch (region)
 					{
 						case 1:
 							Tx = X1+X2;
 							Ty = _IQ(1)-X2;
 							break;
 						case 2:
 							Tx = _IQ(1)-X2;
 							Ty = -1*X1;
 							break;
 						case 3:
 							Tx = _IQ(1)+X1;
 							Ty = X2-_IQ(1);
 							break;
 						case 4:
 							Tx = X2-_IQ(1);
 							Ty = _IQ(1)-X1-X2;
 							break;
 						case 5:
 							Tx = -1*X1-_IQ(1);
 							Ty = X1+X2;
 							break;
 						case 6:
 							Tx = -1*X1;
 							Ty = X1+X2-_IQ(1);
 							break;
 						}
						break;
 					} 					
 					
 			case 3:
 				{
 					switch (region)
 					{
 						case 1:
 							Tx = _IQ(1)+X1;
 							Ty = X2;
 							break;
 						case 2:
 							Tx = -1*X1-X2;
 							Ty = _IQ(1)+X1;
 							break;
 						case 3:
 							Tx = _IQ(-1)-X1;
 							Ty = _IQ(1)+X1+X2;
 							break;
 						case 4:
 							Tx = _IQ(1)-X2;
 							Ty = _IQ(-1)-X1;
 							break;
 						case 5:
 							Tx = X2;
 							Ty = _IQ(-1)-X1-X2;
 							break;
 						case 6:
 							Tx = _IQ(-1)+X2;
 							Ty = -1*X1-X2;
 							break;
 						}
						break;
 					}

 			case 4:
 				{
 					switch (region)
 					{
 						case 1:
 							Tx = -1*X1;
 							Ty = _IQ(1)+X1+X2;
 							break;
 						case 2:
 							Tx = _IQ(1)+X1+X2;
 							Ty = -1*X2;
 							break;
 						case 3:
 							Tx = _IQ(1)+X2;
 							Ty = _IQ(-1)-X1-X2;
 							break;
 						case 4:
 							Tx = _IQ(-1)-X1-X2;
 							Ty = _IQ(1)+X1;
 							break;
 						case 5:
 							Tx = _IQ(-1)-X2;
 							Ty = -1*X1;
 							break;
 						case 6:
 							Tx = -1*X2;
 							Ty = _IQ(-1)-X1;
 							break;
 						}
						break;
 					}

 			case 5:
 				{
 					switch (region)
 					{
 						case 1:
 							Tx = _IQ(1)+X2;
 							Ty = -1*X1-X2;
 							break;
 						case 2:
 							Tx = X1;
 							Ty = _IQ(1)+X2;
 							break;
 						case 3:
 							Tx = _IQ(-1)-X2;
 							Ty = _IQ(1)-X1;
 							break;
 						case 4:
 							Tx = _IQ(1)+X1+X2;
 							Ty = _IQ(-1)-X2;
 							break;
 						case 5:
 							Tx = -1*X1-X2;
 							Ty = _IQ(-1)+X1;
 							break;
 						case 6:
 							Tx = _IQ(-1)-X1-X2;
 							Ty = X1;
 							break;
 						}
						break;
 					}
 					
 			case 6:
 				{
 					switch (region)
 					{
 						case 1:
 							Tx = -1*X2;
 							Ty = _IQ(1)-X1;
 							break;
 						case 2:
 							Tx = _IQ(1)-X1;
 							Ty = X1+X2;
 							break;
 						case 3:
 							Tx = _IQ(1)-X1-X2;
 							Ty = _IQ(-1)+X1;
 							break;
 						case 4:
 							Tx = _IQ(-1)+X1;
 							Ty = _IQ(1)+X2;
 							break;
 						case 5:
 							Tx = _IQ(-1)+X1+X2;
 							Ty = -1*X2;
 							break;
 						case 6:
 							Tx = X1+X2;
 							Ty = _IQ(-1)-X2;
 							break;
 						}
						break;
 					} 					 					 					 					
		} 	
		
		Tmin = _IQmpy((_IQ(1)-Tx-Ty),_IQ(0.5));
		Tmid = Tmin+Tx;
		Tmax = Tmid+Ty;

	if(l3c.flag.bit.NpcOn==KEY_ON)				// 由按键置位或清除该bit.NpcOn
	{
		#if L3_SV_MODE == L3_SV_SELECTSR
				NewSR = 6*(sector-1)+region;
				v->SR = NewSR;
			//	l3c.qUnpe = _IQ(10);
		if(l3c.qUnpe>_IQ(NPC_DEAD_q))			// 中点需上移, Kr取>0
			npc.Km_sv = _IQ(NPC_KM_SV);
		#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe>_IQ(NPC_DEAD_IN_q))	// 双滞环
			npc.Km_sv = _IQ(NPC_KM_SV_q);
		#endif
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_q))	// 中点需下移, Kr取>0
			npc.Km_sv = _IQ(NPC_KM_SV);
	 	#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_IN_q))
			npc.Km_sv = _IQ(NPC_KM_SV_q);
		#endif
				
			//	Km_sv = NPC_KM_SV;				
				if(l3c.qUnpe>_IQ(NPC_DEAD_IN_q))		//中点电压需要增加
				{
					if((NewSR==2)||(NewSR==5)||(NewSR==7)||(NewSR==12)||(NewSR==14)||(NewSR==17)||(NewSR==19)||(NewSR==24)||(NewSR==26)||(NewSR==29)||(NewSR==31)||(NewSR==36))//最小
						{
						   //	Tmin = Tmin+ _IQmpy(npc.Km_sv,(Tmid-Tmin));
							Tmax = Tmax +  _IQmpy(npc.Km_sv,(_IQ(1)-Tmax));
							Tmid = Tmid;
							Tmin = Tmin;
						}	
					else if ((NewSR==1)||(NewSR==6)||(NewSR==8)||(NewSR==11)||(NewSR==13)||(NewSR==18)||(NewSR==20)||(NewSR==23)||(NewSR==25)||(NewSR==30)||(NewSR==32)||(NewSR==35))//最
						{
							//Tmax = Tmax+ _IQmpy(npc.Km_sv,(_IQ(1)-Tmax));
							Tmin = Tmin + _IQmpy(npc.Km_sv,(Tmid-Tmin));
							Tmid = Tmid;
							Tmax = Tmax;
						}	
				}
				else if (l3c.qUnpe<_IQ(-NPC_DEAD_IN_q))
				{
					if((NewSR==2)||(NewSR==5)||(NewSR==7)||(NewSR==12)||(NewSR==14)||(NewSR==17)||(NewSR==19)||(NewSR==24)||(NewSR==26)||(NewSR==29)||(NewSR==31)||(NewSR==36))//最小
						{
							//Tmin = Tmin- _IQmpy(npc.Km_sv,Tmin);
							Tmax = Tmax - _IQmpy(npc.Km_sv,(Tmax-Tmid));
							
							Tmid = Tmid;
							Tmin = Tmin;
						}						
					else if ((NewSR==1)||(NewSR==6)||(NewSR==8)||(NewSR==11)||(NewSR==13)||(NewSR==18)||(NewSR==20)||(NewSR==23)||(NewSR==25)||(NewSR==30)||(NewSR==32)||(NewSR==35))//最
						{
							//Tmax = Tmax- _IQmpy(npc.Km_sv,(Tmax-Tmid));
							
							Tmin = Tmin - _IQmpy(npc.Km_sv,Tmin);
							Tmid = Tmid;
							Tmax = Tmax;
						}						
				}
		#elif L3_SV_MODE == L3_SV_SMALLVEC
		
		if(l3c.qUnpe>_IQ(NPC_DEAD_q))			// 中点需上移, Kr取>0
			Ksmall_sv = _IQ(0.5)+_IQmpy(_IQ(NPC_Ksmall_SV),_IQ(0.5));
		#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe>_IQ(NPC_DEAD_IN_q))	// 双滞环
			Ksmall_sv = _IQ(0.5)+_IQmpy(_IQ(NPC_Ksmall_SV_q),_IQ(0.5));
		#endif
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_q))	// 中点需下移, Kr取>0
			Ksmall_sv = _IQ(0.5)-_IQmpy(_IQ(NPC_Ksmall_SV),_IQ(0.5));
	 	#if	NPC_HYS_MODE==NPC_HYS_DOUBLE
		else if (l3c.qUnpe<_IQ(-NPC_DEAD_IN_q))
			Ksmall_sv = _IQ(0.5)-_IQmpy(_IQ(NPC_Ksmall_SV_q),_IQ(0.5));
		#endif		
		
				
				if(l3c.qUnpe>_IQ(NPC_DEAD_q))
				{
				  //Ksmall_sv = _IQ(0.5)+_IQmpy(_IQ(NPC_Ksmall_SV),_IQ(0.5));
					Tmin = _IQmpy((_IQ(1)-Tx-Ty),Ksmall_sv);
					Tmid = Tmin+Tx;
					Tmax = Tmid+Ty;
				}
				else if (l3c.qUnpe<_IQ(-NPC_DEAD_q))
				{
				  //Ksmall_sv = _IQ(0.5)-_IQmpy(_IQ(NPC_Ksmall_SV),_IQ(0.5));
					Tmin = _IQmpy((_IQ(1)-Tx-Ty),Ksmall_sv);
					Tmid = Tmin+Tx;
					Tmax = Tmid+Ty;
				
				}
		#elif
				npc.Km_sv = _IQ(0);	
				Ksmall_sv = 0;	
		#endif
	}
				
		 		switch (sector)
 		{
 			case 1:
 				{
 					switch (region)
 					{
 						case 1:
 							v->Ta = Tmid;
 							v->Tb = Tmax;
 							v->Tc = Tmin;
 							break;
 						case 2:
 							v->Ta = Tmax;
 							v->Tb = Tmin;
 							v->Tc = Tmid;
 							break;
 						case 3:
 							v->Ta = Tmin;
 							v->Tb = Tmax;
 							v->Tc = Tmid;
 							break;
 						case 4:
 							v->Ta = Tmid;
 							v->Tb = Tmin;
 							v->Tc = Tmax;
 							break;
 						case 5:
 							v->Ta = Tmin;
 							v->Tb = Tmid;
 							v->Tc = Tmax;
 							break;
 						case 6:
 							v->Ta = Tmin;
 							v->Tb = Tmid;
 							v->Tc = Tmax;
 							break;
 						}
						break;
 					}
 			case 2:
 				{
 					switch (region)
 					{
 						case 1:
 							v->Ta = Tmin;
 							v->Tb = Tmax;
 							v->Tc = Tmid;
 							break;
 						case 2:
 							v->Ta = Tmax;
 							v->Tb = Tmid;
 							v->Tc = Tmin;
 							break;
 						case 3:
 							v->Ta = Tmin;
 							v->Tb = Tmid;
 							v->Tc = Tmax;
 							break;
 						case 4:
 							v->Ta = Tmax;
 							v->Tb = Tmin;
 							v->Tc = Tmid;
 							break;
 						case 5:
 							v->Ta = Tmid;
 							v->Tb = Tmin;
 							v->Tc = Tmax;
 							break;
 						case 6:
 							v->Ta = Tmid;
 							v->Tb = Tmin;
 							v->Tc = Tmax;
 							break;
 						}
						break;
 					} 					
 					
 			case 3:
 				{
 					switch (region)
 					{
 						case 1:
 							v->Ta = Tmin;
 							v->Tb = Tmid;
 							v->Tc = Tmax;
 							break;
 						case 2:
 							v->Ta = Tmid;
 							v->Tb = Tmax;
 							v->Tc = Tmin;
 							break;
 						case 3:
 							v->Ta = Tmid;
 							v->Tb = Tmin;
 							v->Tc = Tmax;
 							break;
 						case 4:
 							v->Ta = Tmax;
 							v->Tb = Tmid;
 							v->Tc = Tmin;
 							break;
 						case 5:
 							v->Ta = Tmax;
 							v->Tb = Tmin;
 							v->Tc = Tmid;
 							break;
 						case 6:
 							v->Ta = Tmax;
 							v->Tb = Tmin;
 							v->Tc = Tmid;
 							break;
 						}
						break;
 					}

 			case 4:
 				{
 					switch (region)
 					{
 						case 1:
 							v->Ta = Tmid;
 							v->Tb = Tmin;
 							v->Tc = Tmax;
 							break;
 						case 2:
 							v->Ta = Tmin;
 							v->Tb = Tmax;
 							v->Tc = Tmid;
 							break;
 						case 3:
 							v->Ta = Tmax;
 							v->Tb = Tmin;
 							v->Tc = Tmid;
 							break;
 						case 4:
 							v->Ta = Tmid;
 							v->Tb = Tmax;
 							v->Tc = Tmin;
 							break;
 						case 5:
 							v->Ta = Tmax;
 							v->Tb = Tmid;
 							v->Tc = Tmin;
 							break;
 						case 6:
 							v->Ta = Tmax;
 							v->Tb = Tmid;
 							v->Tc = Tmin;
 							break;
 						}
						break;
 					}

 			case 5:
 				{
 					switch (region)
 					{
 						case 1:
 							v->Ta = Tmax;
 							v->Tb = Tmin;
 							v->Tc = Tmid;
 							break;
 						case 2:
 							v->Ta = Tmin;
 							v->Tb = Tmid;
 							v->Tc = Tmax;
 							break;
 						case 3:
 							v->Ta = Tmax;
 							v->Tb = Tmid;
 							v->Tc = Tmin;
 							break;
 						case 4:
 							v->Ta = Tmin;
 							v->Tb = Tmax;
 							v->Tc = Tmid;
 							break;
 						case 5:
 							v->Ta = Tmid;
 							v->Tb = Tmax;
 							v->Tc = Tmin;
 							break;
 						case 6:
 							v->Ta = Tmid;
 							v->Tb = Tmax;
 							v->Tc = Tmin;
 							break;
 						}
						break;
 					}
 					
 			case 6:
 				{
 					switch (region)
 					{
 						case 1:
 							v->Ta = Tmax;
 							v->Tb = Tmid;
 							v->Tc = Tmin;
 							break;
 						case 2:
 							v->Ta = Tmid;
 							v->Tb = Tmin;
 							v->Tc = Tmax;
 							break;
 						case 3:
 							v->Ta = Tmid;
 							v->Tb = Tmax;
 							v->Tc = Tmin;
 							break;
 						case 4:
 							v->Ta = Tmin;
 							v->Tb = Tmid;
 							v->Tc = Tmax;
 							break;
 						case 5:
 							v->Ta = Tmin;
 							v->Tb = Tmax;
 							v->Tc = Tmid;
 							break;
 						case 6:
 							v->Ta = Tmin;
 							v->Tb = Tmax;
 							v->Tc = Tmid;
 							break;
 						}
						break;
 					} 					 					 					 					
		} 	
    
    
    
    
    
    
    
    
    if(sector==0)
    {
       v->Ta = _IQ(0.5);
       v->Tb = _IQ(0.5);
       v->Tc = _IQ(0.5);
    }    
   
 
//       v->Vaa = t1;
//       v->Vbb = t2;
  
  
 /*	_iq Va,Vb,Vc,t1,t2,t_temp1,t_temp2;
	_iq Sector = 0;  // Sector is treated as Q0 - independently with global Q
	Sector = 0;//???????																
// Inverse clarke transformation
    Va = v->Ubeta;
    Vb = _IQmpy(_IQ(-0.5),v->Ubeta) + _IQmpy(_IQ(0.8660254),v->Ualfa);  // 0.8660254 = sqrt(3)/2 
    Vc = _IQmpy(_IQ(-0.5),v->Ubeta) - _IQmpy(_IQ(0.8660254),v->Ualfa);  // 0.8660254 = sqrt(3)/2

// 60 degree Sector determination
    if (Va>_IQ(0))
       Sector = 1; 
    if (Vb>_IQ(0))
       Sector = Sector + 2;
    if (Vc>_IQ(0))   
       Sector = Sector + 4;
       
// X,Y,Z (Va,Vb,Vc) calculations
    Va = v->Ubeta;                                                       // X = Va 
    Vb = _IQmpy(_IQ(0.5),v->Ubeta) + _IQmpy(_IQ(0.8660254),v->Ualfa);   // Y = Vb 
    Vc = _IQmpy(_IQ(0.5),v->Ubeta) - _IQmpy(_IQ(0.8660254),v->Ualfa);   // Z = Vc
    
   if (Sector==0)  // Sector 0: this is special case for (Ualpha,Ubeta) = (0,0)
   {
      v->Ta = _IQ(0.5);
       v->Tb = _IQ(0.5);
     v->Tc = _IQ(0.5);
    }
    else if (Sector==1)  // Sector 1: t1=Z and t2=Y (abc ---> Tb,Ta,Tc)
    {
       t1 = Vc;
       t2 = Vb;
	   if((t1+t2)>_IQ(0.99))
	   {
	   
	   t1=_IQdiv(t1,(t1+t2));
	   t2=_IQ(0.99)-t1;//_IQdiv(t2,(t1+t2));

	   v->Tb = _IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // tbon = (1-t1-t2)/2
       v->Ta = t1+v->Tb;                             // taon = tbon+t1
       v->Tc = t2+v->Ta;

	   }
	   else
	   {
	   
       v->Tb = _IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // tbon = (1-t1-t2)/2
       v->Ta = t1+v->Tb;                             // taon = tbon+t1
       v->Tc = t2+v->Ta;                             // tcon = taon+t2
       }
    }
    else if (Sector==2)  // Sector 2: t1=Y and t2=-X (abc ---> Ta,Tc,Tb)
    {
       t1 = Vb;
       t2 = -Va;

	  
	   if((t1+t2)>_IQ(0.99))
	   {
	   
	   t1=_IQdiv(t1,(t1+t2));
	   t2=_IQ(0.99)-t1;//_IQdiv(t2,(t1+t2));

	   v->Ta = _IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // tbon = (1-t1-t2)/2
       v->Tc = t1+v->Ta;                             // taon = tbon+t1
       v->Tb = t2+v->Tc;
	   }
	   else
	   {
	   
       v->Ta = _IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // taon = (1-t1-t2)/2
       v->Tc = t1+v->Ta;                             // tcon = taon+t1
       v->Tb = t2+v->Tc;
	   }
                                    // tbon = tcon+t2
    }      
    else if (Sector==3)  // Sector 3: t1=-Z and t2=X (abc ---> Ta,Tb,Tc)
    {
       t1 = -Vc;
       t2 = Va;

	   if((t1+t2)>_IQ(0.99))
	   {
	   
	   t1=_IQdiv(t1,(t1+t2));
	   t2=_IQ(0.99)-t1;//_IQdiv(t2,(t1+t2));
       v->Ta = _IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // tbon = (1-t1-t2)/2
       v->Tb = t1+v->Ta;                             // taon = tbon+t1
       v->Tc = t2+v->Tb;
	   }
	   else
	   {
	   
       v->Ta = _IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // taon = (1-t1-t2)/2
       v->Tb = t1+v->Ta;                             // tbon = taon+t1
       v->Tc = t2+v->Tb;
	   }
                                    // tcon = tbon+t2
    }   
    else if (Sector==4)  // Sector 4: t1=-X and t2=Z (abc ---> Tc,Tb,Ta)
    {
       t1 = -Va;
       t2 = Vc;

	   if((t1+t2)>_IQ(0.99))
	   {
	   
	   t1=_IQdiv(t1,(t1+t2));
	   t2=_IQ(0.99)-t1;//_IQdiv(t2,(t1+t2));

       v->Tc = _IQrsmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // tbon = (1-t1-t2)/2
       v->Tb = t1+v->Tc;                             // taon = tbon+t1
       v->Ta = t2+v->Tb;
	   }
	   else 
	   {
	   
       v->Tc = _IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // tcon = (1-t1-t2)/2
       v->Tb = t1+v->Tc;                             // tbon = tcon+t1
       v->Ta = t2+v->Tb;
       }  
                                  // taon = tbon+t2
    }   
    else if (Sector==5)  // Sector 5: t1=X and t2=-Y (abc ---> Tb,Tc,Ta)
    {
       t1 = Va;
       t2 = -Vb;

	   if((t1+t2)>_IQ(0.99))
	   {
	   
	   t1=_IQdiv(t1,(t1+t2));
	   t2=_IQdiv(t2,(t1+t2));
       v->Tb =_IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // tbon = (1-t1-t2)/2
       v->Tc = t1+v->Tb;                             // taon = tbon+t1
       v->Ta = t2+v->Tc;
	   }
	   else
	   {
	   
       v->Tb = _IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // tbon = (1-t1-t2)/2
       v->Tc = t1+v->Tb;                             // tcon = tbon+t1
       v->Ta = t2+v->Tc; 
	   }
                                   // taon = tcon+t2
    }   
    else if(Sector==6)  // Sector 6: t1=-Y and t2=-Z (abc ---> Tc,Ta,Tb)
    {
       t1 = -Vb;
       t2 = -Vc;

	   if((t1+t2)>_IQ(0.99))
	   {
	   
	   t1=_IQdiv(t1,(t1+t2));
	   t2=_IQ(0.99)-t1;//_IQdiv(t2,(t1+t2));
       v->Tc = _IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // tbon = (1-t1-t2)/2
       v->Ta = t1+v->Tc;                             // taon = tbon+t1
       v->Tb = t2+v->Ta;
	   }
	   else
	   {
	   
	   
       v->Tc = _IQmpy(_IQ(0.5),(_IQ(1)-t1-t2));      // tcon = (1-t1-t2)/2
       v->Ta =t1+v->Tc;                             // taon = tcon+t1
       v->Tb =t2+v->Ta;
	   }
                                   // tbon = taon+t2 
   }  */
    if(v->Ta>=_IQ(1))
	   v->Ta=_IQ(0.99999);//16777215;//
    if(v->Tb>=_IQ(1))
	   v->Tb=_IQ(0.99999);//16777215;//_IQ(0.9998);
    if(v->Tc>=_IQ(1))
	   v->Tc=_IQ(0.99999);//16777215;//_IQ(0.9998);
  if(v->Ta<=_IQ(0))
	   v->Ta=_IQ(0);
    if(v->Tb<=_IQ(0))
	   v->Tb=_IQ(0);
    if(v->Tc<=_IQ(0))
	   v->Tc=_IQ(0);
//	v->Ta = _IQmpy(v->Ta,_IQ(2))-_IQ(1);
//	v->Tb = _IQmpy(v->Tb,_IQ(2))-_IQ(1);   
//	v->Tc = _IQmpy(v->Tc,_IQ(2))-_IQ(1);
}


