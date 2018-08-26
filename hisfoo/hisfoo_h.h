

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0620 */
/* at Tue Jan 19 11:14:07 2038
 */
/* Compiler settings for hisfoo.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0620 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __hisfoo_h_h__
#define __hisfoo_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __Ihisfoo_FWD_DEFINED__
#define __Ihisfoo_FWD_DEFINED__
typedef interface Ihisfoo Ihisfoo;

#endif 	/* __Ihisfoo_FWD_DEFINED__ */


#ifndef __hisfoo_FWD_DEFINED__
#define __hisfoo_FWD_DEFINED__

#ifdef __cplusplus
typedef class hisfoo hisfoo;
#else
typedef struct hisfoo hisfoo;
#endif /* __cplusplus */

#endif 	/* __hisfoo_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __hisfoo_LIBRARY_DEFINED__
#define __hisfoo_LIBRARY_DEFINED__

/* library hisfoo */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_hisfoo;

#ifndef __Ihisfoo_DISPINTERFACE_DEFINED__
#define __Ihisfoo_DISPINTERFACE_DEFINED__

/* dispinterface Ihisfoo */
/* [uuid] */ 


EXTERN_C const IID DIID_Ihisfoo;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("04A76836-E4B7-4BB8-8FF2-AEC5DD5AA23C")
    Ihisfoo : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IhisfooVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Ihisfoo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Ihisfoo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Ihisfoo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Ihisfoo * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Ihisfoo * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Ihisfoo * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Ihisfoo * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IhisfooVtbl;

    interface Ihisfoo
    {
        CONST_VTBL struct IhisfooVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Ihisfoo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define Ihisfoo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define Ihisfoo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define Ihisfoo_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define Ihisfoo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define Ihisfoo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define Ihisfoo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __Ihisfoo_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_hisfoo;

#ifdef __cplusplus

class DECLSPEC_UUID("E15DCE55-7DFD-418F-8A09-960A02D1E97E")
hisfoo;
#endif
#endif /* __hisfoo_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


