#include "ILuaModuleManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "tier0/dbg.h"
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "sys/times.h"
#include "sys/vtimes.h"
#include <sys/statvfs.h>
#include <sys/statfs.h>
GMOD_MODULE(Init, Shutdown);


void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
   using std::ifstream;
   using std::string;

   vm_usage     = 0.0;
   resident_set = 0.0;

   // 'file' stat seems to give the most reliable results
   //
   ifstream stat_stream("/proc/self/stat",ios_base::in);

   // dummy vars for leading entries in stat that we don't care about
   //
   string pid, comm, state, ppid, pgrp, session, tty_nr;
   string tpgid, flags, minflt, cminflt, majflt, cmajflt;
   string utime, stime, cutime, cstime, priority, nice;
   string O, itrealvalue, starttime;

   // the two fields we want
   //
   unsigned long vsize;
   long rss;

   stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
               >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
               >> utime >> stime >> cutime >> cstime >> priority >> nice
               >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

   stat_stream.close();

   long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
   vm_usage     = vsize / 1024.0;
   resident_set = rss * page_size_kb;
}


LUA_FUNCTION(GetMemoryUsage)
{
	double vm, rss;
	process_mem_usage(vm, rss);
	Lua()->Push(vm);
	Lua()->Push(rss);

	return 2;
}
LUA_FUNCTION(GetMemInfo)
{
	struct sysinfo memInfo;
    

    sysinfo (&memInfo);
    long long totalVirtualMem = memInfo.totalram;
    //Add other values in next statement to avoid int overflow on right hand side...
    totalVirtualMem += memInfo.totalswap;
    totalVirtualMem *= memInfo.mem_unit;
	
	Lua()->Push((double) totalVirtualMem / (double)1024 );

	long long virtualMemUsed = memInfo.totalram - memInfo.freeram;
   
    virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
    virtualMemUsed *= memInfo.mem_unit;
	
	Lua()->Push((double) virtualMemUsed/ (double)1024 );
	
	long long totalPhysMem = memInfo.totalram;
    //Multiply in next statement to avoid int overflow on right hand side...
    totalPhysMem *= memInfo.mem_unit;
	
	Lua()->Push((double) totalPhysMem/ (double)1024 );
	
	long long physMemUsed = memInfo.totalram - memInfo.freeram;
    //Multiply in next statement to avoid int overflow on right hand side...
    physMemUsed *= memInfo.mem_unit;
	
	Lua()->Push((double) physMemUsed/ (double)1024 );
	
	return 4;
}



// CPU USAGE


static unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;


void InitSysCPUUsage_c(){
	FILE* file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %Ld %Ld %Ld %Ld", &lastTotalUser, &lastTotalUserLow,
		&lastTotalSys, &lastTotalIdle);
	fclose(file);
}


double SysCPUUsage_c(){
	double percent;
	FILE* file;
	unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;


	file = fopen("/proc/stat", "r");
	fscanf(file, "cpu %Ld %Ld %Ld %Ld", &totalUser, &totalUserLow,
		&totalSys, &totalIdle);
	fclose(file);


	if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
		totalSys < lastTotalSys || totalIdle < lastTotalIdle){
		//Overflow detection. Just skip this value.
		percent = -1.0;
	}
	else{
		total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
			(totalSys - lastTotalSys);
		percent = total;
		total += (totalIdle - lastTotalIdle);
		percent /= total;
		percent *= 100;
	}


	lastTotalUser = totalUser;
	lastTotalUserLow = totalUserLow;
	lastTotalSys = totalSys;
	lastTotalIdle = totalIdle;


	return percent;
}

LUA_FUNCTION(InitSysCPUUsage)
{
	InitSysCPUUsage_c();
	return 0;
}
LUA_FUNCTION(SysCPUUsage)
{

	Lua()->Push(SysCPUUsage_c());

	return 1;
}




static clock_t lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;


void InitGameCPUUsage_c(){
	FILE* file;
	struct tms timeSample;
	char line[128];


	lastCPU = times(&timeSample);
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;


	file = fopen("/proc/cpuinfo", "r");
	numProcessors = 0;
	while(fgets(line, 128, file) != NULL){
		if (strncmp(line, "processor", 9) == 0) numProcessors++;
	}
	fclose(file);
}


double GameCPUUsage_c(){
	struct tms timeSample;
	clock_t now;
	double percent;


	now = times(&timeSample);
	if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
		timeSample.tms_utime < lastUserCPU){
		//Overflow detection. Just skip this value.
		percent = -1.0;
	}
	else{
		percent = (timeSample.tms_stime - lastSysCPU) +
			(timeSample.tms_utime - lastUserCPU);
		percent /= (now - lastCPU);
		percent /= numProcessors;
		percent *= 100;
	}
	lastCPU = now;
	lastSysCPU = timeSample.tms_stime;
	lastUserCPU = timeSample.tms_utime;


	return percent;
}


LUA_FUNCTION(InitGameCPUUsage)
{
	InitGameCPUUsage_c();
	return 0;
}
LUA_FUNCTION(GameCPUUsage)
{

	Lua()->Push(GameCPUUsage_c());

	return 1;
}


LUA_FUNCTION(DiskUsage)
{

    struct statvfs fiData;
	
	Lua()->CheckType(1, Type::STRING);
	const char *statwhat = Lua()->GetString(1);
	
	if((statvfs(statwhat,&fiData)) < 0 ) {
		return 0;
	}

	Lua()->Push((double)fiData.f_bsize); /* file system block size */
	Lua()->Push((double)fiData.f_bavail); /* # free blocks for unprivileged users */
	Lua()->Push((double)fiData.f_files); /* # inodes */
	Lua()->Push((double)fiData.f_favail); /* # free inodes for unprivileged users */
	Lua()->Push((double)fiData.f_fsid); // seeing if it is the same FS.
	return 5;
}


LUA_FUNCTION(sysinfo)
{
	struct sysinfo memInfo;
    
	sysinfo (&memInfo);

	ILuaObject *tbl = Lua()->GetNewTable();
		tbl->SetMember("uptime"		, (double) memInfo.uptime    );
		tbl->SetMember("totalram"	, (double) memInfo.totalram  );
		tbl->SetMember("freeram"	, (double) memInfo.freeram   );
		tbl->SetMember("sharedram"	, (double) memInfo.sharedram );
		tbl->SetMember("bufferram"	, (double) memInfo.bufferram );
		tbl->SetMember("totalswap"	, (double) memInfo.totalswap );
		tbl->SetMember("freeswap"	, (double) memInfo.freeswap  );
		tbl->SetMember("procs"		, (double) memInfo.procs     );
		tbl->SetMember("totalhigh"	, (double) memInfo.totalhigh );
		tbl->SetMember("freehigh"	, (double) memInfo.freehigh  );
		tbl->SetMember("mem_unit"	, (double) memInfo.mem_unit  );
		Lua()->Push(tbl);
	tbl->UnReference();

	return 1;
}



int Init(lua_State *L)
{

		
	ILuaObject *nixinfo = Lua()->GetNewTable();
	
		// !l require'nixinfo' local a,b=nixinfo.GetMemoryUsage() Msg"[Memory usage] "print(math.Round(a/1024)..' MB virt',math.Round(b/1024)..' MB Res')
		nixinfo->SetMember("GetMemoryUsage", GetMemoryUsage);
		nixinfo->SetMember("GetMemInfo", GetMemInfo);
		nixinfo->SetMember("SysCPUUsage", SysCPUUsage);
		nixinfo->SetMember("InitSysCPUUsage", InitSysCPUUsage);
		nixinfo->SetMember("GameCPUUsage", GameCPUUsage);
		nixinfo->SetMember("InitGameCPUUsage", InitGameCPUUsage);
		nixinfo->SetMember("sysinfo", sysinfo);
		nixinfo->SetMember("DiskUsage", DiskUsage);
		Lua()->Global()->SetMember("nixinfo", nixinfo);
		
	nixinfo->UnReference();

	return 0;

}

int Shutdown(lua_State *L)
{
	return 0;
}
