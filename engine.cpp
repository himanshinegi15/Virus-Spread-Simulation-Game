#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <algorithm>
#include <climits>
#include <numeric>
#include <random>
#include <cctype>
#include <cstdint>

static constexpr int N = 12;

inline const char* CNAME[N] = {
    "China","India","USA","Brazil","Russia",
    "UK","Germany","Nigeria","Egypt","Australia","Japan","Mexico"
};
inline const char* CREG[N] = {
    "Asia","Asia","Americas","Americas","Europe",
    "Europe","Europe","Africa","Africa","Oceania","Asia","Americas"
};
static const long long CPOP[N] = {
    1412000000LL,1400000000LL,331000000LL,215000000LL,144000000LL,
    67000000LL,83000000LL,218000000LL,102000000LL,26000000LL,125000000LL,130000000LL
};

static const int LAND[N][N] = {
    {0,1,0,0,1,0,0,0,0,0,0,0},{1,0,0,0,0,0,0,0,1,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,1},{1,0,0,0,0,0,1,0,0,0,0,0},{0,0,0,0,0,0,1,0,0,0,0,0},
    {0,0,0,0,1,1,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,1,0,0,0},{0,1,0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0,0,0},{0,0,1,1,0,0,0,0,0,0,0,0}
};

static const int SEA[N][N] = {
    {0,0,0,0,0,0,0,0,0,0,1,0},{0,0,0,0,0,1,0,1,1,0,0,0},{0,0,0,1,0,1,0,0,0,1,0,0},
    {0,0,1,0,0,1,0,1,0,0,0,0},{0,0,0,0,0,1,0,0,0,0,0,0},{0,1,1,1,1,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0},{0,1,0,1,0,1,0,0,1,0,0,0},{0,1,0,0,0,1,0,1,0,0,0,0},
    {0,0,1,0,0,0,0,0,0,0,1,0},{1,0,0,0,0,0,0,0,0,1,0,0},{0,0,0,0,0,0,0,0,0,0,0,0}
};

static const int AIR[N][N] = {
    {0,2,3,0,2,4,0,0,0,4,1,0},{2,0,4,0,3,3,0,4,2,4,3,0},{3,4,0,2,3,2,2,3,4,4,2,1},
    {0,0,2,0,0,3,3,3,0,0,0,2},{2,3,3,0,0,2,1,0,2,0,2,0},{4,3,2,3,2,0,1,2,2,4,0,3},
    {0,0,2,3,1,1,0,3,2,0,0,4},{0,4,3,3,0,2,3,0,2,0,0,0},{0,2,4,0,2,2,2,2,0,0,0,0},
    {4,4,4,0,0,4,0,0,0,0,2,0},{1,3,2,0,2,0,0,0,0,2,0,3},{0,0,1,2,0,3,4,0,0,0,3,0}
};

static std::mt19937 rng(std::random_device{}());

static std::string qs(const std::string& s) { return '"' + s + '"'; }
static std::string jsonStr(const std::string& k, const std::string& v) { return qs(k)+':'+qs(v)+','; }
static std::string jsonLL(const std::string& k, long long v) { return qs(k)+':'+std::to_string(v)+','; }
static std::string jsonD(const std::string& k, double v) { std::ostringstream ss; ss<<v; return qs(k)+':'+ss.str()+','; }
static std::string jsonBool(const std::string& k, bool v) { return qs(k)+':'+(v?"true":"false")+','; }

static std::string jsonArrLL(const std::vector<long long>& v) {
    std::string s="[";
    for(int i=0;i<N;i++){s+=std::to_string(v[i]);if(i<N-1)s+=',';}
    return s+']';
}
static std::string jsonArrD(const std::vector<double>& v) {
    std::string s="[";
    for(int i=0;i<N;i++){std::ostringstream ss;ss<<v[i];s+=ss.str();if(i<N-1)s+=',';}
    return s+']';
}

static size_t findKey(const std::string& j, const std::string& k) {
    std::string needle='"'+k+'"';
    size_t p=0;
    while((p=j.find(needle,p))!=std::string::npos){
        size_t b=p;
        while(b>0&&j[b-1]==' ')b--;
        char before=b>0?j[b-1]:'{';
        if(before=='{'||before==',')return p;
        p+=needle.size();
    }
    return std::string::npos;
}

static long long parseLL(const std::string& j, const std::string& k) {
    size_t p=findKey(j,k);
    if(p==std::string::npos)return 0;
    p=j.find(':',p)+1;
    while(p<j.size()&&j[p]==' ')p++;
    bool neg=(p<j.size()&&j[p]=='-');if(neg)p++;
    long long v=0;
    while(p<j.size()&&std::isdigit(j[p]))v=v*10+(j[p++]-'0');
    return neg?-v:v;
}
static long long parseLLD(const std::string& j,const std::string& k,long long def){
    return findKey(j,k)==std::string::npos?def:parseLL(j,k);
}
static double parseD(const std::string& j,const std::string& k){
    size_t p=findKey(j,k);
    if(p==std::string::npos)return 0.0;
    p=j.find(':',p)+1;
    while(p<j.size()&&j[p]==' ')p++;
    size_t e=p;if(e<j.size()&&j[e]=='-')e++;
    while(e<j.size()&&(std::isdigit(j[e])||j[e]=='.'))e++;
    return std::stod(j.substr(p,e-p));
}
static double parseDD(const std::string& j,const std::string& k,double def){
    return findKey(j,k)==std::string::npos?def:parseD(j,k);
}
static bool parseBool(const std::string& j,const std::string& k,bool def=false){
    size_t p=findKey(j,k);
    if(p==std::string::npos)return def;
    size_t c=j.find(':',p)+1;
    while(c<j.size()&&j[c]==' ')c++;
    return j.substr(c,4)=="true";
}
static std::string parseStr(const std::string& j,const std::string& k){
    size_t p=findKey(j,k);
    if(p==std::string::npos)return "";
    p=j.find('"',j.find(':',p))+1;
    std::string v;
    while(p<j.size()&&j[p]!='"')v+=j[p++];
    return v;
}
static std::vector<long long> parseArrLL(const std::string& j,const std::string& k){
    std::vector<long long> a(N,0);
    size_t p=findKey(j,k);
    if(p==std::string::npos)return a;
    p=j.find('[',p)+1;
    for(int i=0;i<N;i++){
        while(p<j.size()&&!std::isdigit(j[p])&&j[p]!='-'){if(j[p]==']')return a;p++;}
        bool neg=(j[p]=='-');if(neg)p++;
        long long v=0;
        while(p<j.size()&&std::isdigit(j[p]))v=v*10+(j[p++]-'0');
        a[i]=neg?-v:v;
    }
    return a;
}
static std::vector<double> parseArrD(const std::string& j,const std::string& k){
    std::vector<double> a(N,0.0);
    size_t p=findKey(j,k);
    if(p==std::string::npos)return a;
    p=j.find('[',p)+1;
    for(int i=0;i<N;i++){
        while(p<j.size()&&!std::isdigit(j[p])&&j[p]!='-'){if(j[p]==']')return a;p++;}
        size_t st=p;if(j[p]=='-')p++;
        while(p<j.size()&&(std::isdigit(j[p])||j[p]=='.'))p++;
        a[i]=std::stod(j.substr(st,p-st));
    }
    return a;
}

static std::string serialise(
    long long day,long long pts,long long infectivity,long long severity,
    long long origin,long long aid,long long edl,
    bool gameOver,bool won,
    long long ti,long long td,long long tp,
    double cureProgress,bool cureDetected,
    const std::string& message,
    const std::vector<long long>& infected,const std::vector<long long>& dead,
    const std::vector<long long>& population,const std::vector<double>& spreadDelay,
    const std::vector<long long>& cured)
{
    std::string r="{";
    r+=jsonStr ("status","ok");
    r+=jsonLL  ("day",day);
    r+=jsonLL  ("points",pts);
    r+=jsonLL  ("infectivity",infectivity);
    r+=jsonLL  ("severity",severity);
    r+=jsonLL  ("origin",origin);
    r+=jsonLL  ("all_infected_day",aid);
    r+=jsonLL  ("eradication_deadline",edl);
    r+=jsonBool("game_over",gameOver);
    r+=jsonBool("won",won);
    r+=jsonLL  ("total_infected",ti);
    r+=jsonLL  ("total_dead",td);
    r+=jsonLL  ("total_population",tp);
    r+=jsonD   ("cure_progress",cureProgress);
    r+=jsonBool("cure_detected",cureDetected);
    r+=qs("message")+':'+qs(message)+',';
    r+=qs("infected")+':'+jsonArrLL(infected)+',';
    r+=qs("dead")+':'+jsonArrLL(dead)+',';
    r+=qs("population")+':'+jsonArrLL(population)+',';
    r+=qs("spread_delay")+':'+jsonArrD(spreadDelay)+',';
    r+=qs("cured")+':'+jsonArrLL(cured);
    return r+'}';
}

static std::vector<int> dijkstraAir(int src){
    std::vector<int> dist(N,INT_MAX);
    dist[src]=0;
    std::priority_queue<std::pair<int,int>,std::vector<std::pair<int,int>>,std::greater<std::pair<int,int>>> pq;
    pq.push({0,src});
    while(!pq.empty()){
        auto[d,u]=pq.top();pq.pop();
        if(d>dist[u])continue;
        for(int v=0;v<N;v++)
            if(AIR[u][v]&&dist[u]+AIR[u][v]<dist[v]){dist[v]=dist[u]+AIR[u][v];pq.push({dist[v],v});}
    }
    return dist;
}

static std::vector<int> bfsLandFrontier(const std::vector<bool>& inf){
    std::vector<bool> visited(N,false);
    std::queue<int> q;
    for(int i=0;i<N;i++)if(inf[i]){q.push(i);visited[i]=true;}
    std::vector<int> frontier;
    while(!q.empty()){
        int u=q.front();q.pop();
        for(int v=0;v<N;v++)
            if(LAND[u][v]&&!visited[v]){visited[v]=true;if(!inf[v])frontier.push_back(v);q.push(v);}
    }
    return frontier;
}

static std::vector<int> dfsSeaFrontier(const std::vector<bool>& inf){
    std::vector<bool> visited(N,false);
    std::stack<int> st;
    for(int i=0;i<N;i++)if(inf[i]){st.push(i);visited[i]=true;}
    std::set<int> frontier;
    while(!st.empty()){
        int u=st.top();st.pop();
        for(int v=0;v<N;v++)
            if(SEA[u][v]&&!visited[v]){visited[v]=true;if(!inf[v])frontier.insert(v);st.push(v);}
    }
    return{frontier.begin(),frontier.end()};
}

static std::vector<int> primsCureOrder(int src){
    int weight[N][N]={};
    for(int u=0;u<N;u++)for(int v=0;v<N;v++){
        if(LAND[u][v])weight[u][v]=std::max(weight[u][v],5);
        if(SEA[u][v]) weight[u][v]=std::max(weight[u][v],3);
        if(AIR[u][v]) weight[u][v]=std::max(weight[u][v],AIR[u][v]);
    }
    std::vector<bool> inMST(N,false);
    std::vector<int>  key(N,-1),order;
    order.reserve(N);
    inMST[src]=true;order.push_back(src);
    for(int v=0;v<N;v++)key[v]=weight[src][v];
    for(int iter=1;iter<N;iter++){
        int best=-1;
        for(int v=0;v<N;v++)if(!inMST[v]&&key[v]>0)if(best==-1||key[v]>key[best])best=v;
        if(best==-1){for(int v=0;v<N;v++)if(!inMST[v]){order.push_back(v);inMST[v]=true;}break;}
        inMST[best]=true;order.push_back(best);
        for(int v=0;v<N;v++)if(!inMST[v]&&weight[best][v]>key[v])key[v]=weight[best][v];
    }
    return order;
}

static double cureRateCalc(int countriesInfected,long long totalDead,long long totalPop,long long infectivity,long long severity){
    if(totalPop<=0)return 0.0;
    double awareness=static_cast<double>(countriesInfected)/12.0;
    double deathRatio=static_cast<double>(totalDead)/totalPop;
    double panicFunding=1.5+deathRatio*8.0;
    double severityBonus=1.0+(severity-1)*0.07;
    double infPenalty=1.0+(infectivity-1)*0.03;
    return 0.006*awareness*panicFunding*severityBonus/infPenalty;
}

static std::string doInit(const std::string&){
    std::string r='{'+jsonStr("status","ok")+'"'+"countries"+'"'+":[";
    for(int i=0;i<N;i++){
        r+='{';
        r+=jsonStr("name",CNAME[i]);
        r+=jsonLL("population",CPOP[i]);
        r+='"';r+="region";r+='"';r+=':';r+='"';r+=CREG[i];r+='"';
        r+='}';if(i<N-1)r+=',';
    }
    return r+"]}";
}

static std::string doStart(const std::string& json){
    int ci=static_cast<int>(parseLL(json,"country_index"));
    auto airDist=dijkstraAir(ci);
    int maxD=*std::max_element(airDist.begin(),airDist.end());
    if(maxD<=0)maxD=1;
    std::vector<long long> infected(N,0),dead(N,0),pop(N),cured(N,0);
    std::vector<double> spreadDelay(N);
    for(int i=0;i<N;i++){
        pop[i]=CPOP[i];
        spreadDelay[i]=(airDist[i]==INT_MAX)?20.0:static_cast<double>(airDist[i])/maxD*15.0+1.0;
    }
    spreadDelay[ci]=0.0;
    infected[ci]=static_cast<long long>(CPOP[ci]*0.0001);
    long long tp=std::accumulate(pop.begin(),pop.end(),0LL);
    return serialise(1,20,1,1,ci,-1,-1,false,false,infected[ci],0,tp,0.0,false,"",infected,dead,pop,spreadDelay,cured);
}

static std::string doNextDay(const std::string& json){
    long long day=parseLL(json,"day"),pts=parseLL(json,"points");
    long long infectivity=parseLL(json,"infectivity"),severity=parseLL(json,"severity");
    long long origin=parseLL(json,"origin");
    long long aid=parseLLD(json,"all_infected_day",-1),edl=parseLLD(json,"eradication_deadline",-1);
    double cureProgress=parseDD(json,"cure_progress",0.0);
    bool cureDetected=parseBool(json,"cure_detected",false);
    auto infected=parseArrLL(json,"infected"),dead=parseArrLL(json,"dead"),pop=parseArrLL(json,"population");
    auto spreadDelay=parseArrD(json,"spread_delay");
    auto cured=parseArrLL(json,"cured");

    std::vector<bool> isInf(N,false);
    for(int i=0;i<N;i++)if(infected[i]>0)isInf[i]=true;

    for(int u=0;u<N;u++){
        if(!isInf[u])continue;
        long long healthyAlive=std::max(0LL,pop[u]-dead[u]-infected[u]);
        if(aid<0){
            double rate=(severity>=7)?0.0:std::max(0.0,0.08+infectivity*0.04-(severity>3?(severity-3)*0.02:0.0));
            infected[u]+=std::min(healthyAlive,static_cast<long long>(healthyAlive*rate));
        }
        double dr=severity*severity*0.004+(aid>=0?0.15:0.0);
        long long nd=std::min(infected[u],static_cast<long long>(infected[u]*dr));
        infected[u]-=nd;dead[u]+=nd;
    }

    if(aid>=0){
        for(int u=0;u<N;u++){
            long long healthy=std::max(0LL,pop[u]-dead[u]-infected[u]);
            infected[u]+=healthy;
        }
    }

    if(aid<0&&severity<7){
        for(int v=0;v<N;v++){
            if(isInf[v]||spreadDelay[v]<=0)continue;
            for(int u=0;u<N;u++){
                if(!isInf[u]||!AIR[u][v])continue;
                long long au=pop[u]-dead[u];if(au<=0)continue;
                spreadDelay[v]-=static_cast<double>(infected[u])/au*(infectivity*0.3+0.2)/AIR[u][v];
            }
            if(spreadDelay[v]<=0)infected[v]=std::max(1LL,static_cast<long long>(pop[v]*0.0001));
        }
        for(int v:bfsLandFrontier(isInf)){
            if(spreadDelay[v]<=0)continue;
            for(int u=0;u<N;u++){
                if(!isInf[u]||!LAND[u][v])continue;
                long long au=pop[u]-dead[u];if(au<=0)continue;
                spreadDelay[v]-=static_cast<double>(infected[u])/au*(infectivity*0.5+0.3);
            }
            if(spreadDelay[v]<=0)infected[v]=std::max(1LL,static_cast<long long>(pop[v]*0.0001));
        }
        for(int v:dfsSeaFrontier(isInf)){
            if(spreadDelay[v]<=0)continue;
            for(int u=0;u<N;u++){
                if(!isInf[u]||!SEA[u][v])continue;
                long long au=pop[u]-dead[u];if(au<=0)continue;
                spreadDelay[v]-=static_cast<double>(infected[u])/au*(infectivity*0.4+0.2);
            }
            if(spreadDelay[v]<=0)infected[v]=std::max(1LL,static_cast<long long>(pop[v]*0.0001));
        }
    }

    long long tp=0,ti=0,td=0;
    for(int i=0;i<N;i++){tp+=pop[i];ti+=infected[i];td+=dead[i];}
    long long alive=tp-td;

    bool allCountries=true;
    for(int i=0;i<N;i++)if(infected[i]==0&&(pop[i]-dead[i])>0){allCountries=false;break;}
    double infRatio=(alive>0)?static_cast<double>(ti)/alive:0.0;
    if(allCountries&&infRatio>=0.99&&aid<0){
        aid=day;edl=day+20;
        severity=std::max(severity+3,static_cast<long long>(severity*1.3+0.5));
    }

    int countriesInfected=0;
    for(int i=0;i<N;i++)if(infected[i]>0)countriesInfected++;
    if(!cureDetected&&tp>0&&static_cast<double>(ti)/tp>0.005)cureDetected=true;
    if(cureDetected){
        double rate=cureRateCalc(countriesInfected,td,tp,infectivity,severity);
        cureProgress=std::min(1.0,cureProgress+rate);
        std::vector<int> cureOrder=primsCureOrder(static_cast<int>(origin));
        int targetCured=static_cast<int>(cureProgress*N);
        for(int slot=0;slot<targetCured&&slot<N;slot++){
            int ci=cureOrder[slot];
            if(!cured[ci]&&(pop[ci]-dead[ci])>0)cured[ci]=1;
        }
    }

    pts+=std::uniform_int_distribution<int>(3,10)(rng);
    day++;

    bool gameOver=false,won=false;
    std::string message;
    if(cureProgress>=1.0){gameOver=true;won=false;message="The cure reached every country. Humanity survived. You lose.";}
    else if(td>=tp||(td>0&&static_cast<double>(td)/tp>=0.9999)){gameOver=true;won=true;message="HUMANITY EXTINCT. You win.";}
    else if(ti<=500&&aid<0&&td>0){gameOver=true;won=false;message="The pathogen burned out before spreading worldwide. You lose.";}
    if(!gameOver&&aid>=0&&day==aid+2)message="All humans infected! Cure is racing. Severity boosted — kill fast!";

    return serialise(day,pts,infectivity,severity,origin,aid,edl,gameOver,won,ti,td,tp,cureProgress,cureDetected,message,infected,dead,pop,spreadDelay,cured);
}

static std::string doUpgrade(const std::string& json){
    std::string type=parseStr(json,"type");
    long long pts=parseLL(json,"points"),inf=parseLL(json,"infectivity"),sev=parseLL(json,"severity");
    double cp=parseDD(json,"cure_progress",0.0);
    bool cd=parseBool(json,"cure_detected",false);
    if(pts>=10){pts-=10;if(type=="infectivity")inf++;else sev++;}
    return serialise(
        parseLL(json,"day"),pts,inf,sev,parseLL(json,"origin"),
        parseLLD(json,"all_infected_day",-1),parseLLD(json,"eradication_deadline",-1),
        false,false,
        parseLL(json,"total_infected"),parseLL(json,"total_dead"),parseLL(json,"total_population"),
        cp,cd,"",
        parseArrLL(json,"infected"),parseArrLL(json,"dead"),parseArrLL(json,"population"),
        parseArrD(json,"spread_delay"),parseArrLL(json,"cured")
    );
}

int main(){
    std::string line;
    while(std::getline(std::cin,line)){
        if(line.empty())continue;
        const std::string action=parseStr(line,"action");
        std::string response;
        if     (action=="init")    response=doInit(line);
        else if(action=="start")   response=doStart(line);
        else if(action=="next_day")response=doNextDay(line);
        else if(action=="upgrade") response=doUpgrade(line);
        else response=R"({"status":"error","message":"unknown action"})";
        std::cout<<response<<'\n'<<std::flush;
    }
}
