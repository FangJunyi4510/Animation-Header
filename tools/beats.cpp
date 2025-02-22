#include <bits/stdc++.h>
#include "../../../include/Char033.h"
using namespace std;
using namespace chrono;
using ll=long long;
ll startTime=0;
ll getTime(){
	return system_clock::now().time_since_epoch()/microseconds(1)-startTime;
}
double avg(const vector<ll>& a){
	double sum=0;
	for(const auto& i:a){
		sum+=i;
	}
	return sum/a.size();
}
double SD(const vector<ll>& a){
	double avga=avg(a);
	double sum=0;
	for(const auto& i:a){
		sum+=(i-avga)*(i-avga);
	}
	return sqrt(sum/(a.size()-1));
}

void printTime(ll t,int w,bool pos=false){
	cout<<(t>=0?(pos?"+":" "):"-");
	cout<<setw(w)<<fixed<<setprecision(3);
	cout<<abs(t/1000.0)<<"ms";
}
int main(){
	startTime=getTime();
	cout<<"Waiting! "<<endl;
	while(getchar() && getTime()<1000000);
	startTime+=getTime();

	cout<<"Start now! "<<endl;

	vector<ll> ticks{0};
	vector<ll> delta;

	int ch;
	while((ch=getchar())!=-1){
		if(ch!='\n'){
			continue;
		}
		cout<<c033::CursorMove(0,-10000)<<c033::ClearLine();
		ll cur=getTime();
		delta.push_back(cur-ticks.back());
		ticks.push_back(cur);
		ll curavg=avg(delta);
		ll bias=delta.back()-curavg;


		cout<<setw(4)<<delta.size();
		cout<<"   CurTime: ";	printTime(cur,10);
		cout<<"   Delta: ";		printTime(delta.back(),8);
		cout<<"   avg: ";		printTime(curavg,8);
		cout<<"   SD: ";		printTime(SD(delta),7);

		double rate=(bias)*100.0/curavg;
		switch(int(abs(rate))/2){
		case 0:cout<<c033::pBlue;break;
		case 1:cout<<c033::pGreen;break;
		case 2:cout<<c033::pLightGreen;break;
		case 3:cout<<c033::pYellow;break;
		case 4:cout<<c033::pLightRed;break;
		default:cout<<c033::pRed;break;
		}
		cout<<"   bias: ";		printTime(bias,7,true);
		cout<<"   "<<showpos<<setw(5)<<rate<<"%"<<noshowpos;
		cout<<c033::pNone;

	}

	return 0;
}