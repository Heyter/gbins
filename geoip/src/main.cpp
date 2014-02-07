
#define IPV6STRICT // TODO: IpV6 D:

#include "GMLuaModule.h"
#include "libGeoIP/GeoIP.h"
#include "libGeoIP/GeoIPCity.h"

GeoIP * COUNTRY;
GeoIP * ISP;
GeoIP * ORG;
GeoIP * CITY;
GeoIP * ASN;
GeoIP * NET;

GMOD_MODULE(Init, Shutdown);

void SafeSetMember(ILuaObject *tbl,const char * what,const char * val) {
	if (val!=NULL) {
		tbl->SetMember(what,val);
	}
}

bool GeoIPToLua(const char * ipstring,ILuaObject *tbl) {
	
	if (!COUNTRY) return false;
	
	GeoIPRecord *gir;
	
	gir = GeoIP_record_by_addr(COUNTRY, ipstring);
	
	if (gir) {
		SafeSetMember(tbl,"city",gir->city);
		SafeSetMember(tbl,"country_name",gir->country_name);
		SafeSetMember(tbl,"region",gir->region);
		SafeSetMember(tbl,"country_code",gir->country_code);
		SafeSetMember(tbl,"postal_code",gir->postal_code);
		
		tbl->SetMember("latitude",gir->latitude);
		tbl->SetMember("longitude",gir->longitude);
		tbl->SetMember("netmask",(double)gir->netmask);
		
		GeoIPRecord_delete(gir);
	}
	
	
	// other DBs
	
	if (ISP) 
		SafeSetMember(tbl,"ISP",GeoIP_org_by_name( ISP, ipstring ));
		
	if (ORG) 
		SafeSetMember(tbl,"org",GeoIP_name_by_name( ORG, ipstring ));
	
	if (CITY) {
		gir = GeoIP_record_by_addr(CITY, ipstring);
		
		if (gir) {
			SafeSetMember(tbl,"_city",gir->city);
			SafeSetMember(tbl,"_country_name",gir->country_name);
			SafeSetMember(tbl,"_region",gir->region);
			SafeSetMember(tbl,"_country_code",gir->country_code);
			SafeSetMember(tbl,"_postal_code",gir->postal_code);
			
			tbl->SetMember("_latitude",gir->latitude);
			tbl->SetMember("_longitude",gir->longitude);
			tbl->SetMember("_netmask",(double)gir->netmask);
			GeoIPRecord_delete(gir);
		}
	}

	if (ASN)
		SafeSetMember(tbl,"asn",GeoIP_org_by_name( ASN, ipstring ));
	
	if (NET)
		tbl->SetMember("speed",(double)GeoIP_id_by_name( NET, ipstring ));
	
	return true;
	
}


LUA_FUNCTION(GetIPInfo)
{
	Lua()->CheckType(1, Type::STRING);
	const char *ipstring = Lua()->GetString(1); // should we copy this to account for gc?

	
	
	ILuaObject *tbl = Lua()->GetNewTable();
	
		bool ret = GeoIPToLua(ipstring,tbl);
		if (!ret) {
			tbl->SetMember("error",true);
			tbl->SetMember("norecord",true); // legacy
		}
	
	Lua()->Push(tbl);	
	tbl->UnReference();
	return 1;
}

	
#define LOADGEO(what,datname,how) 	what = GeoIP_open("garrysmod/data/"datname".dat", how);\
	if (what == NULL)\
		what = GeoIP_open(datname".dat", how);\
	if (what != NULL) GeoIP_set_charset(what, GEOIP_CHARSET_UTF8);\

int Init(lua_State *L)
{
	LOADGEO(COUNTRY,"GeoIP",GEOIP_INDEX_CACHE);
	LOADGEO(ISP,"GeoIPISP",GEOIP_INDEX_CACHE);
	LOADGEO(ORG,"GeoIPOrg",GEOIP_INDEX_CACHE);
	LOADGEO(CITY,"GeoIPCity",GEOIP_INDEX_CACHE);
	LOADGEO(ASN,"GeoIPASNum",GEOIP_INDEX_CACHE);
	LOADGEO(NET,"GeoIPNetSpeed",GEOIP_INDEX_CACHE);

	ILuaObject *tbl = Lua()->GetNewTable();
		tbl->SetMember("Get", GetIPInfo);
	Lua()->Global()->SetMember("GeoIP", tbl);

	tbl->UnReference();

	return 0;

}

int Shutdown(lua_State *L)
{
	GeoIP_delete(COUNTRY);
	GeoIP_delete(ISP);
	GeoIP_delete(ORG);
	GeoIP_delete(CITY);
	GeoIP_delete(ASN);
	GeoIP_delete(NET);
	return 0;
}
