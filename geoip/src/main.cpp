#include "GarrysMod/Lua/Interface.h"
#include <stdio.h>

#ifdef WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif

#include "libGeoIP/GeoIP.h"
#include "libGeoIP/GeoIPCity.h"

GeoIP * COUNTRY;
GeoIP * ISP;
GeoIP * ORG;
GeoIP * CITY;
GeoIP * ASN;
GeoIP * NET;

using namespace GarrysMod::Lua;

lua_State* state;

void SafeSetMember(const char * what,const char * val) {
	if (val!=NULL && what!=NULL) {
		LUA->PushString(val); 
		LUA->SetField(-2, what);
	}
}
void SafeSetMember(const char * what,double val) {
	if (what!=NULL) {
		LUA->PushNumber(val); 
		LUA->SetField(-2, what);
	}
}

bool GeoIPToLua(const char * ipstring) {
	
	if (!COUNTRY) return false;
	
	GeoIPRecord *gir;
	
	gir = GeoIP_record_by_addr(COUNTRY, ipstring);
	
	if (gir) {
		SafeSetMember("city",gir->city);
		SafeSetMember("country_name",gir->country_name);
		SafeSetMember("region",gir->region);
		SafeSetMember("country_code",gir->country_code);
		SafeSetMember("postal_code",gir->postal_code);
		
		SafeSetMember("latitude",gir->latitude);
		SafeSetMember("longitude",gir->longitude);
		SafeSetMember("netmask",gir->netmask);
		
		GeoIPRecord_delete(gir);
	}
	
	
	// other DBs
	
	if (ISP) 
		SafeSetMember("ISP",GeoIP_org_by_name( ISP, ipstring ));
		
	if (ORG) 
		SafeSetMember("org",GeoIP_name_by_name( ORG, ipstring ));
	
	if (CITY) {
		gir = GeoIP_record_by_addr(CITY, ipstring);
		
		if (gir) {
			SafeSetMember("_city",gir->city);
			SafeSetMember("_country_name",gir->country_name);
			SafeSetMember("_region",gir->region);
			SafeSetMember("_country_code",gir->country_code);
			SafeSetMember("_postal_code",gir->postal_code);
			
			SafeSetMember("_latitude",gir->latitude);
			SafeSetMember("_longitude",gir->longitude);
			SafeSetMember("_netmask",gir->netmask);
			GeoIPRecord_delete(gir);
		}
	}

	if (ASN)
		SafeSetMember("asn",GeoIP_org_by_name( ASN, ipstring ));
	
	if (NET)
		SafeSetMember("speed",GeoIP_id_by_name( NET, ipstring ));
	
	return true;
	
}



int GeoIP_Get( lua_State* S )
{
	state=S;

	LUA->CheckType(1, Type::STRING);
	const char *ipstring = LUA->GetString(1); // should we copy this to account for gc?

	
	LUA->CreateTable();
	
		bool ret = GeoIPToLua(ipstring);
		if (!ret) {
			LUA->PushBool(true); LUA->SetField(-2,"error");
			LUA->PushBool(true); LUA->SetField(-2,"norecord");
		}
	
	return 1;
}


#define LOADGEO(what,datname,how) 	what = GeoIP_open("garrysmod/data/"datname".dat", how);\
	if (what == NULL)\
		what = GeoIP_open(datname".dat", how);\
	if (what == NULL)\
		what = GeoIP_open("geoip/"datname".dat", how);\
	if (what != NULL) GeoIP_set_charset(what, GEOIP_CHARSET_UTF8);\

GMOD_MODULE_OPEN()
{
	LOADGEO(COUNTRY,"GeoIP",GEOIP_INDEX_CACHE);
	LOADGEO(ISP,"GeoIPISP",GEOIP_INDEX_CACHE);
	LOADGEO(ORG,"GeoIPOrg",GEOIP_INDEX_CACHE);
	LOADGEO(CITY,"GeoIPCity",GEOIP_INDEX_CACHE);
	LOADGEO(ASN,"GeoIPASNum",GEOIP_INDEX_CACHE);
	LOADGEO(NET,"GeoIPNetSpeed",GEOIP_INDEX_CACHE);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->CreateTable();

            LUA->PushCFunction(GeoIP_Get); LUA->SetField(-2, "Get");

    LUA->SetField(-2, "GeoIP");

	return 0;
}

GMOD_MODULE_CLOSE()
{
	GeoIP_delete(COUNTRY);
	GeoIP_delete(ISP);
	GeoIP_delete(ORG);
	GeoIP_delete(CITY);
	GeoIP_delete(ASN);
	GeoIP_delete(NET);
	return 0;
}