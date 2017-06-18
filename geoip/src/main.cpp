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

void SafeSetMember(GarrysMod::Lua::ILuaBase* LUA, const char * what,const char * val) {
	if (val!=NULL && what!=NULL) {
		LUA->PushString(val);
		LUA->SetField(-2, what);
	}
}
void SafeSetMember(GarrysMod::Lua::ILuaBase* LUA, const char * what,double val) {
	if (what!=NULL) {
		LUA->PushNumber(val);
		LUA->SetField(-2, what);
	}
}

bool GeoIPToLua(GarrysMod::Lua::ILuaBase* LUA, const char * ipstring) {
	if (!COUNTRY) return false;
	
	GeoIPRecord *gir;

	gir = GeoIP_record_by_addr(COUNTRY, ipstring);

	if (gir) {
		SafeSetMember(LUA, "city",gir->city);
		SafeSetMember(LUA, "country_name",gir->country_name);
		SafeSetMember(LUA, "region",gir->region);
		SafeSetMember(LUA, "country_code",gir->country_code);
		SafeSetMember(LUA, "postal_code",gir->postal_code);

		SafeSetMember(LUA, "latitude",gir->latitude);
		SafeSetMember(LUA, "longitude",gir->longitude);
		SafeSetMember(LUA, "netmask",gir->netmask);

		GeoIPRecord_delete(gir);
	}


	// other DBs

	if (ISP) 
		SafeSetMember(LUA, "ISP",GeoIP_org_by_name( ISP, ipstring ));

	if (ORG) 
		SafeSetMember(LUA, "org",GeoIP_name_by_name( ORG, ipstring ));

	if (CITY) {
		gir = GeoIP_record_by_addr(CITY, ipstring);

		if (gir) {
			SafeSetMember(LUA, "_city",gir->city);
			SafeSetMember(LUA, "_country_name",gir->country_name);
			SafeSetMember(LUA, "_region",gir->region);
			SafeSetMember(LUA, "_country_code",gir->country_code);
			SafeSetMember(LUA, "_postal_code",gir->postal_code);

			SafeSetMember(LUA, "_latitude",gir->latitude);
			SafeSetMember(LUA, "_longitude",gir->longitude);
			SafeSetMember(LUA, "_netmask",gir->netmask);
			GeoIPRecord_delete(gir);
		}
	}

	if (ASN)
		SafeSetMember(LUA, "asn",GeoIP_org_by_name( ASN, ipstring ));
	
	if (NET)
		SafeSetMember(LUA, "speed",GeoIP_id_by_name( NET, ipstring ));

	return true;
}


LUA_FUNCTION( time_zone_by_country_and_region )
{
	const char * ret = NULL;

	const char *country_code = LUA->CheckString(1);

	const char *region_code = LUA->CheckString( 2);

	ret = GeoIP_time_zone_by_country_and_region(country_code,region_code);

	if (!ret) return 0;

	LUA->PushString(ret);

	return 1;
}
LUA_FUNCTION( region_name_by_code )
{
	const char * ret = NULL;

	const char *country_code = LUA->CheckString( 1);

	const char *region_code = LUA->CheckString( 2);

	ret = GeoIP_region_name_by_code(country_code,region_code);

	if (!ret) return 0;

	LUA->PushString(ret);

	return 1;
}


LUA_FUNCTION( GeoIP_Get )
{
	const char *ipstring = LUA->CheckString(1); // should we copy this to account for gc?


	LUA->CreateTable();

		bool ret = GeoIPToLua(LUA, ipstring);
		if (!ret) {
			LUA->PushBool(true); LUA->SetField(-2,"error");
			LUA->PushBool(true); LUA->SetField(-2,"norecord");
		}

	return 1;
}


#define LOADGEO(what,datname,how) 	what = GeoIP_open("garrysmod/data/" datname ".dat", how);\
	if (what == NULL)\
		what = GeoIP_open(datname ".dat", how);\
	if (what == NULL)\
		what = GeoIP_open("geoip/" datname ".dat", how);\
	if (what != NULL) GeoIP_set_charset(what, GEOIP_CHARSET_UTF8);\

GMOD_MODULE_OPEN()
{
	LOADGEO(COUNTRY,"GeoIP",GEOIP_INDEX_CACHE);
	LOADGEO(ISP,"GeoIPISP",GEOIP_INDEX_CACHE);
	LOADGEO(ORG,"GeoIPOrg",GEOIP_INDEX_CACHE);
	LOADGEO(CITY,"GeoIPCity",GEOIP_INDEX_CACHE);
	LOADGEO(ASN,"GeoIPASNum",GEOIP_INDEX_CACHE);
	LOADGEO(NET,"GeoIPNetSpeed",GEOIP_INDEX_CACHE);

	LUA->CreateTable();
		LUA->PushCFunction( time_zone_by_country_and_region ); LUA->SetField( -2, "timezone" );
		LUA->PushCFunction(region_name_by_code); LUA->SetField(-2, "region_name");
		LUA->PushCFunction(GeoIP_Get); LUA->SetField(-2, "Get");
	LUA->SetField(-10002, "GeoIP"); // global table index

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