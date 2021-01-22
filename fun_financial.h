

/*******************************************************************************
Interest Rates
*******************************************************************************/

EQUATION("Policy_Targets")
USE_SAVED
v[0]=V("government_period");
v[13]= fmod((double) t,v[0]); 
v[1]=V("switch_moving_targets");
v[2]=V("begin_monetary_policy");
if(t>v[2]&&v[1]==1&&v[13]==0)
	{
	v[5]=v[6]=v[7]=v[8]=0;
	for(i=1;i<(10*v[0]);i++)
		{
		v[5]=v[5]+VL("Country_Annual_Inflation",i);
		v[6]=v[6]+VL("Country_Idle_Capacity",i);
		v[7]=v[7]+VL("Total_Stock_Loans_Growth",i);
		v[8]=v[8]+VL("Country_Debt_Rate_Firms",i);
		}
	v[9]=v[5]/(10*v[0]);
	v[10]=v[6]/(10*v[0]);
	v[11]=v[7]/(10*v[0]);
	v[12]=v[8]/(10*v[0]);
	WRITE("target_inflation",v[9]);
	WRITE("target_capacity",v[10]);
	WRITE("target_credit_growth",v[11]);
	WRITE("target_debt_rate",v[12]);
	}
RESULT(0)


EQUATION("Basic_Interest_Rate")
/*
Nominal Interest rate is set by the central bank following a (possible) dual mandate Taylor Rule, considering the inflation and unemployment gaps.
"switch_monetary_policy":
0-->       no monetary policy rule
1-->       single mandate (inflation) taylor rule
2-->       dual mandate (inflation and unemploymeny) taylor rule
3-->       triple mandate (inflation, unemployment and credit growth) taylor rule
4-->       triple mandate (inflation, unemployment and debt rate) taylor rule
5-->       smithin rule
6-->       smoothed smithin rule
7--> 	   pasinetti rule
8-->       smoothed pasinetti rule
9-->       kansas city rule

"interest_rate_adjustment": absolute increase

*/
	
	v[0]=V("real_interest_rate");
	v[24]=V("begin_monetary_policy");	
	
	v[1]=V("target_inflation");
	v[2]=V("target_capacity");
	v[3]=V("target_credit_growth");
	v[4]=V("target_debt_rate");
	
	v[5]=VL("Country_Annual_CPI_Inflation",1);
	v[6]=VL("Country_Idle_Capacity",1);
	v[7]=VL("Total_Stock_Loans_Growth",1);
	v[8]=VL("Country_Debt_Rate_Firms",1);
	
	v[9]=VL("Country_Avg_Productivity",1);                              //avg productivity lagged 1
	v[10]=VL("Country_Avg_Productivity",2);                         	//avg productivity lagged 2
	if(v[10]!=0)                                                //if productivity is not zero
		v[11]=(v[9]-v[10])/v[10];                               //calculate productivity growth
	else                                                        //if productivity is zero
		v[11]=0;												//use 1
		
	v[21]=VL("Country_Consumer_Price_Index",1);                             		//avg price lagged 1
	v[22]=VL("Country_Consumer_Price_Index",2);                         			//avg price lagged 2
	if(v[22]!=0)                                                //if price is not zero
		v[23]=(v[21]-v[22])/v[22];                              //calculate price growth
	else                                                        //if price is zero
		v[23]=0;												//use 1
	
	v[18]=V("interest_rate_adjustment");
	v[19]=VL("Basic_Interest_Rate", 1);
	
	v[12]=V("switch_monetary_policy");
	
	if(v[12]==0)      											//no monetary policy rule, fixed nominal interest rate set by "real_interest_rate" parameter
		v[20]=v[0];
	
	if(v[12]==1||v[12]==2||v[12]==3||v[12]==4)					//taylor rule
	{
		if(v[12]==1)											//single mandate taylor rule
		{
		v[13]=V("sensitivity_inflation");
		v[14]=0;
		v[15]=0;
		v[16]=0;
		}
		if(v[12]==2)											//dual mandate taylor rule
		{
		v[13]=V("sensitivity_inflation");
		v[14]=V("sensitivity_capacity");
		v[15]=0;
		v[16]=0;
		}
		if(v[12]==3)											//triple mandate taylor rule with credit growth target
		{
		v[13]=V("sensitivity_inflation");
		v[14]=V("sensitivity_capacity");
		v[15]=V("sensitivity_credit_growth");
		v[16]=0;
		}
		if(v[12]==4)											//triple mandate taylor rule with debt rate target
		{
		v[13]=V("sensitivity_inflation");
		v[14]=V("sensitivity_capacity");
		v[15]=0;
		v[16]=V("sensitivity_debt_rate");
		}
	
		v[17]=v[0]+v[5]+v[13]*(v[5]-v[1])+v[14]*(v[6]-v[2])+v[15]*(max(0,(v[7]-v[3])))+v[16]*(max(0,(v[8]-v[4])));
		if(abs(v[17]-v[19])>v[18])
		{
			if(v[17]>v[19])
				v[20]=v[19]+v[18];
			else if(v[17]<v[19])
				v[20]=v[19]-v[18];
			else
				v[20]=v[19];
		}
		else
			v[20]=v[17];

	}
	
	if(v[12]==5)
		{												//smithin rule
		v[20]=v[5];	
		WRITE("target_inflation", v[5]);
		}
	if(v[12]==6)												//smithin rule smoothing
	{
		v[17]=v[5];
		if(abs(v[17]-v[19])>v[18])
			{
			if(v[17]>v[19])
				v[20]=v[19]+v[18];
			else
				v[20]=v[19]-v[18];
			}
		else
			v[20]=v[17];
	WRITE("target_inflation", v[5]);
	}
	
	if(v[12]==7)		
		{										//pasinetti rule
		v[20]=v[5]+v[11];
		WRITE("target_inflation", v[5]);
		}
	if(v[12]==8)												//pasinetti rule smoothing
	{
		v[17]=v[5]+v[11];
		if(abs(v[17]-v[19])>v[18])
			{
			if(v[17]>v[19])
				v[20]=v[19]+v[18];
			else
				v[20]=v[19]-v[18];
			}
		else
			v[20]=v[17];
	WRITE("target_inflation", v[5]);
	}
	
	if(v[12]==9)
	{												//kansas city rule.
		v[20]=0;
		WRITE("target_inflation", v[5]);
	}
		
	
	if(t>v[24]&&v[24]!=-1)
		v[25]=v[20];
	else
		v[25]=v[19];
	
	v[30]=V("interest_shock_begin");          //defines when the shock happens
	v[31]=V("interest_shock_duration");       //defines how long the shock lasts
	v[32]=V("interest_shock_size");           //defines the size, in percentage, of the shock
	if(t>=v[30]&&t<v[30]+v[31])
		v[33]=v[25]*(1+v[32]);
	else
		v[33]=v[25];
		
RESULT(max(0,v[33]))


EQUATION("Interest_Rate_Deposits")
/*
Interest Rate on Bank deposits is a negative spreaded base interest rate
*/
v[0]=V("Basic_Interest_Rate");
v[1]=V("spread_deposits");
v[2]=max(0,(v[0]-v[1]));
RESULT(v[2])


/*******************************************************************************
Financial Sector Aggregates and Averages
*******************************************************************************/

EQUATION("Avg_Competitiveness_Financial_Sector")
/*
Average competitiveness, weighted by firm's market share
*/
	v[0]=0;                                           //initializes the CYCLE
	CYCLE(cur, "BANKS")                               //CYCLE trought all banks in the sector
	{
		v[1]=VS(cur, "Bank_Competitiveness");         //bank's competitiveness
		v[2]=VLS(cur, "Bank_Market_Share", 1);        //bank's market share in the last period
		v[0]=v[0]+v[1]*v[2];                          //sector average competitiveness will be a average of bank competitiveness weighted by their respective market shares
	}
RESULT(v[0])

EQUATION("Avg_Interest_Rate_Long_Term")
RESULT(WHTAVE("Bank_Interest_Rate_Long_Term", "Bank_Market_Share"))

EQUATION("Avg_Interest_Rate_Short_Term")
RESULT(WHTAVE("Bank_Interest_Rate_Short_Term", "Bank_Market_Share"))

EQUATION("Financial_Sector_Stock_Loans_Short_Term")
RESULT(SUM("Bank_Stock_Loans_Short_Term"))

EQUATION("Financial_Sector_Stock_Loans_Long_Term")
RESULT(SUM("Bank_Stock_Loans_Long_Term"))

EQUATION("Financial_Sector_Total_Stock_Loans")
RESULT(SUM("Bank_Total_Stock_Loans"))

EQUATION("Financial_Sector_Defaulted_Loans")
RESULT(SUM("Bank_Defaulted_Loans"))

EQUATION("Financial_Sector_Stock_Deposits")
RESULT(SUM("Bank_Stock_Deposits"))

EQUATION("Financial_Sector_Max_Total_Loans");
RESULT(SUM("Bank_Max_Total_Loans"))

EQUATION("Financial_Sector_Demand_Loans")
RESULT(SUM("Bank_Demand_Loans"))

EQUATION("Financial_Sector_Effective_Loans")
RESULT(SUM("Bank_Effective_Loans"))

EQUATION("Financial_Sector_Interest_Payment")
RESULT(SUM("Bank_Interest_Payment"))

EQUATION("Financial_Sector_Interest_Receivment")
RESULT(SUM("Bank_Interest_Receivment"))

EQUATION("Financial_Sector_Debt_Payment")
RESULT(SUM("Bank_Debt_Payment"))

EQUATION("Financial_Sector_Profits")
RESULT(SUM("Bank_Profits"))

EQUATION("Financial_Sector_Distributed_Profits")
RESULT(SUM("Bank_Distributed_Profits"))

EQUATION("Financial_Sector_Accumulated_Profits")
RESULT(SUM("Bank_Accumulated_Profits"))

EQUATION("Financial_Sector_Rescue")
RESULT(SUM("Bank_Rescue"))

EQUATION("Financial_Sector_Accumulated_Rescue")
RESULT(CURRENT + V("Financial_Sector_Rescue"))

EQUATION("Financial_Sector_Accumulated_Defaulted_Loans")
RESULT(CURRENT + V("Financial_Sector_Defaulted_Loans"))


/*******************************************************************************
Financial Sector Analysis
*******************************************************************************/


EQUATION("Financial_Sector_Short_Term_Rate");
/*
Share of short term loans over total loans
Analysis Variable
*/
	v[0]=V("Financial_Sector_Stock_Loans_Short_Term");
	v[1]=V("Financial_Sector_Total_Stock_Loans");
	if(v[1]!=0)
		v[2]=v[0]/v[1];
	else
		v[2]=0;
RESULT(v[2])


EQUATION("Financial_Sector_Default_Rate")
/*
Total Defaluted Loans over total stock of loans
Analysis Variable
*/
v[0]=V("Financial_Sector_Accumulated_Defaulted_Loans");
v[1]=V("Financial_Sector_Stock_Loans_Long_Term");
if(v[1]!=0)
	v[2]=v[0]/v[1];
else
	v[2]=0;
RESULT(v[2])


EQUATION("Financial_Sector_Demand_Met")
/*
Share of demand for new loans met
Analysis Variable
*/
	v[0]=V("Financial_Sector_Effective_Loans");
	v[1]=V("Financial_Sector_Demand_Loans");
	if(v[1]!=0)
		v[2]=v[0]/v[1];
	else
		v[2]=1;
RESULT(v[2])


EQUATION("Financial_Sector_Leverage")
/*
Total Stock of Loans over Total Stock of Deposits
Analysis Variable
*/
v[0]=V("Financial_Sector_Total_Stock_Loans");
v[1]=V("Financial_Sector_Stock_Deposits");
if(v[1]!=0)
	v[2]=v[0]/v[1];
else
	v[2]=0;
RESULT(v[2])


EQUATION("Financial_Sector_Effective_Capital_Ratio")
/*
Accumulated profits over total loans
Analysis Variable
*/
v[0]=V("Financial_Sector_Accumulated_Profits");
v[1]=V("Financial_Sector_Total_Stock_Loans");
if(v[1]!=0)
	v[2]=v[0]/v[1];
else
	v[2]=0;
RESULT(v[2])


EQUATION("Financial_Sector_Normalized_HHI")
/*
Financial Sector Variable for Analysis
*/
	v[0]=0;                           		//initializes the CYCLE    
	CYCLE(cur, "BANKS")               		//CYCLE trought all firms of the sector
	{
		v[1]=VS(cur, "Bank_Market_Share");  //firm's market share
		v[0]=v[0]+v[1]*v[1];            	//sums up firm's market share squared
	}
	v[2]=COUNT("BANKS");
	if (v[2]!=1)
		v[3]=(v[0]-(1/v[2]))/(1- (1/v[2]));
	else	
		v[3]=1;
RESULT(v[3])


EQUATION("Financial_Sector_Turbulence")
/*
Financial Sector Variable for Analysis
*/
	v[0]=0;                                           //initializes the CYCLE 
	CYCLE(cur, "BANKS")                               //CYCLE trough all firms 
	{
 	v[2]=VS(cur,"Bank_Market_Share");   			 //firm's effective market share in current period
 	v[3]=VLS(cur,"Bank_Market_Share",1);			 //firm's effective market share in the last period
 	v[4]=abs(v[2]-v[3]);                             //returns the absolute value of the difference
 	v[0]=v[0]+v[4];                                   //sums up all absolute values for all firms
	}
RESULT(v[0])


EQUATION("Total_Stock_Loans_Growth")
/*
Total credit growth
*/
v[0]=V("Financial_Sector_Total_Stock_Loans");
v[1]=VL("Financial_Sector_Total_Stock_Loans", 1);
if(v[1]!=0)
	v[2]=(v[0]-v[1])/v[1];
else
	v[2]=0;
RESULT(v[2])












