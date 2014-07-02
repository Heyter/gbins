
#ifdef SHADERPLUGIN
	
#include "StdAfx.h"

#include "luareplace.h"

#define SHADER_DLL_INTERFACE_VERSION	"ShaderDLL004"

class IShader;

class IShaderDLLInternal {
public:
	virtual bool Connect( CreateInterfaceFn factory, bool bIsMaterialSystem ) = 0;
	virtual void Disconnect( bool bIsMaterialSystem ) = 0;
	virtual int ShaderCount() const = 0;
	virtual IShader *GetShader( int nShader ) = 0;
};

class IShaderDLL {
public:
	virtual void InsertShader( IShader *pShader ) = 0;
};

class CShaderDLL : public IShaderDLLInternal, public IShaderDLL {
public:
	CShaderDLL();

	// methods of IShaderDLL
	virtual bool Connect( CreateInterfaceFn factory );
	virtual void Disconnect();
	virtual int ShaderCount() const;
	virtual IShader *GetShader( int nShader );

	// methods of IShaderDLLInternal
	virtual bool Connect( CreateInterfaceFn factory, bool bIsMaterialSystem );
	virtual void Disconnect( bool bIsMaterialSystem );
	virtual void InsertShader( IShader *pShader );

private:
	CUtlVector< IShader * >	m_ShaderList;
};

static CShaderDLL *s_pShaderDLL;

IShaderDLLInternal *GetShaderDLLInternal() {

	if ( !s_pShaderDLL )
	{
		s_pShaderDLL = new CShaderDLL;
	}

	return static_cast<IShaderDLLInternal*>( s_pShaderDLL );
}

EXPOSE_INTERFACE_FN( (InstantiateInterfaceFn)GetShaderDLLInternal, IShaderDLLInternal, SHADER_DLL_INTERFACE_VERSION );

CShaderDLL::CShaderDLL() {}

bool CShaderDLL::Connect( CreateInterfaceFn factory, bool bIsMaterialSystem )
{
	Init();
	return true;
}

void CShaderDLL::Disconnect( bool bIsMaterialSystem )
{
	Shutdown();
}

bool CShaderDLL::Connect( CreateInterfaceFn factory )
{
	return Connect( factory, false );
}

void CShaderDLL::Disconnect()
{
	Disconnect( false );
}


//-----------------------------------------------------------------------------
// Iterates over all shaders
//-----------------------------------------------------------------------------
int CShaderDLL::ShaderCount() const
{
	return m_ShaderList.Count();
}

IShader *CShaderDLL::GetShader( int nShader ) 
{
	if ( ( nShader < 0 ) || ( nShader >= m_ShaderList.Count() ) )
		return NULL;

	return m_ShaderList[nShader];
}


//-----------------------------------------------------------------------------
// Adds to the shader lists
//-----------------------------------------------------------------------------
void CShaderDLL::InsertShader( IShader *pShader )
{
	Assert( pShader );
	m_ShaderList.AddToTail( pShader );
}

#endif