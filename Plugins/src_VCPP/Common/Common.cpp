#include "stdafx.h"
#include "Common.h"
#include <atlstr.h>

constexpr int DISPID_Translate = 1;
constexpr int DISPID_GetOption = 2;
constexpr int DISPID_SaveOption = 3;
constexpr int DISPID_LogError = 6;

HRESULT MergeApp_GetOption(IDispatch* pDispatch, BSTR bstrName, VARIANT& varDefault, VARIANT* pvarResult)
{
	VariantClear(pvarResult);
	if (!pDispatch)
	{
		return VariantCopy(pvarResult, &varDefault);
	}
	VARIANT varName{ VT_BSTR };
	varName.bstrVal = bstrName;
	VARIANT varDefaultRef;
	varDefaultRef.vt = VT_VARIANT | VT_BYREF;
	varDefaultRef.pvarVal = &varDefault;
	CComDispatchDriver drv(pDispatch);
	return drv.Invoke2(DISPID_GetOption, &varName, &varDefaultRef, pvarResult);
}

CString MergeApp_GetOptionString(IDispatch* pDispatch, const CString& sName, const CString& sDefault)
{
	CComBSTR bstrName{ sName };
	CComVariant varDefault{ sDefault };
	CComVariant varResult;
	MergeApp_GetOption(pDispatch, bstrName, varDefault, &varResult);
	varResult.ChangeType(VT_BSTR);
	return varResult.bstrVal;
}

HRESULT MergeApp_SaveOption(IDispatch* pDispatch, BSTR bstrName, VARIANT& varValue)
{
	if (!pDispatch)
	{
		UNREFERENCED_PARAMETER(bstrName);
		UNREFERENCED_PARAMETER(varValue);
		return S_OK;
	}
	VARIANT varName{ VT_BSTR };
	VARIANT varValueRef{ VT_VARIANT | VT_BYREF };
	varName.bstrVal = bstrName;
	varValueRef.pvarVal = &varValue;
	CComDispatchDriver drv(pDispatch);
	return drv.Invoke2(DISPID_SaveOption, &varName, &varValueRef);
}

BSTR MergeApp_Translate(IDispatch* pDispatch, BSTR bstrText)
{
	if (!pDispatch)
		return SysAllocString(bstrText);
	VARIANT varResult;
	VARIANT varText{ VT_BSTR };
	varText.bstrVal = bstrText;
	VariantInit(&varResult);
	CComDispatchDriver drv(pDispatch);
	HRESULT hr = drv.Invoke1(DISPID_Translate, &varText, &varResult);
	if (FAILED(hr))
		return SysAllocString(bstrText);
	return varResult.bstrVal;
}

HRESULT MergeApp_LogError(IDispatch* pDispatch, const CString& sMessage)
{
	if (!pDispatch)
		return E_FAIL;
	CComBSTR bstrText{ sMessage };
	VARIANT varText{ VT_BSTR };
	varText.bstrVal = bstrText;
	CComDispatchDriver drv(pDispatch);
	return drv.Invoke1(DISPID_LogError, &varText);
}

HRESULT MergeApp_SaveOptionString(IDispatch* pDispatch, const CString& sName, const CString& sValue)
{
	CComBSTR bstrName{ sName };
	CComVariant varValue{ sValue };
	return MergeApp_SaveOption(pDispatch, bstrName, varValue);
}

void TranslateDialog(HWND h, IDispatch* pDispatch)
{
	if (!pDispatch)
		return;
	CComDispatchDriver drv(pDispatch);
	UINT gw = GW_CHILD;
	do
	{
		constexpr size_t textbuf_size = 512;
		wchar_t text[textbuf_size];
		::GetWindowTextW(h, text, textbuf_size);
		CComVariant varResult;
		CComVariant varText(text);
		if (SUCCEEDED(drv.Invoke1(DISPID_Translate, &varText, &varResult)))
			::SetWindowTextW(h, varResult.bstrVal);
		h = ::GetWindow(h, gw);
		gw = GW_HWNDNEXT;
	} while (h != nullptr);
}
