/**
* Copyright (C) 2014-2050 
* All rights reserved.
* 
* @file       Sobject.h
* @brief      
* @version    v1.0      
* @author     SOUI group   
* @date       2014/08/01
* 
* Describe    the base class used in SOUI, which provides type identify of class in runtime
*             and implement attributes dispatcher described in XML. 
*/

#pragma once
#include "../utilities-def.h"

#include "string/tstring.h"
#include "pugixml/pugixml.hpp"
#include "string/strcpcvt.h"

//////////////////////////////////////////////////////////////////////////

// SObject Class Name Declaration
#define SOUI_CLASS_NAME(theclass, classname)            \
public:                                                 \
    static LPCWSTR GetClassName()                       \
    {                                                   \
        return classname;                               \
    }                                                   \
    \
	static LPCWSTR BaseClassName()                      \
	{                                                   \
		return __super::GetClassName();                 \
	}                                                   \
	\
	virtual LPCWSTR GetObjectClass()  const             \
	{                                                   \
		return classname;                               \
	}                                                   \
	\
	virtual BOOL IsClass(LPCWSTR lpszName) const        \
	{                                                   \
		if(wcscmp(GetClassName(), lpszName)  == 0)      \
			return TRUE;                                \
		return __super::IsClass(lpszName);              \
	}                                                   \



namespace SOUI
{

	/**
	* @class      SObject
	* @brief      SOUIϵͳ�еĶ������
	* 
	* Describe    �ṩ��RTTI���ƣ�ʵ�ִ�XML�ڵ��и������������������
	*/
	struct UTILITIES_API IObject
    {
        /**
         * GetClassName
         * @brief    �������
         * @return   LPCWSTR -- ����
         * Describe  ��̬����
         */    
		static LPCWSTR GetClassName(){return L"object";}

		virtual ~IObject(){}


        /**
         * IsClass
         * @brief    �ж�this�ǲ�������ָ��������
         * @param    LPCWSTR lpszName --  ����������
         * @return   BOOL -- true�ǲ�������
         * Describe  
         */    
        virtual BOOL IsClass(LPCWSTR lpszName) const = 0;

        /**
         * GetObjectClass
         * @brief    ���������
         * @return   LPCWSTR -- ������
         * Describe  ����һ���麯����ע����GetClassName������
         */    
        virtual LPCWSTR GetObjectClass() const = 0;


        virtual HRESULT SetAttribute(const char*  strAttribName, const char*  strValue, BOOL bLoading) = 0;

        /**
         * SetAttribute
         * @brief    ����һ����������
         * @param    const SStringA & strAttribName --  ������
         * @param    const SStringA & strValue --  ����ֵ
         * @param    BOOL bLoading --  ���󴴽�ʱ��ϵͳ���ñ�־
         * @return   HRESULT -- �����������
         * Describe  
         */    
        virtual HRESULT SetAttribute(const SStringA &  strAttribName, const SStringA &  strValue, BOOL bLoading) = 0;

        /**
        * SetAttribute
        * @brief    ����һ����������
        * @param    const SStringA & strAttribName --  ������
        * @param    const SStringA & strValue --  ����ֵ
        * @param    BOOL bLoading --  ���󴴽�ʱ��ϵͳ���ñ�־
        * @return   HRESULT -- �����������
        * Describe  
        */    
        virtual HRESULT SetAttribute(const SStringW &  strAttribName, const SStringW &  strValue, BOOL bLoading) = 0;

        /**
         * OnAttribute
         * @brief    ���Դ�������õķ���
         * @param    const SStringW & strAttribName --  ������
         * @param    const SStringW & strValue --  ������
         * @param    HRESULT hr --  ���Դ������
         * @return   HRESULT -- ���Դ������
         * Describe  ����������ֱ�ӷ���
         */    
        virtual HRESULT AfterAttribute(const SStringW & strAttribName,const SStringW & strValue,HRESULT hr) = 0;

		/**
         * GetID
         * @brief    ��ȡ����ID
         * @return   int -- ����ID
         * Describe  
         */    
        virtual int GetID() const = 0;

        /**
         * GetName
         * @brief    ��ȡ����Name
         * @return   LPCWSTR -- ����Name
         * Describe  
         */    
        virtual LPCWSTR GetName() const = 0;

		virtual BOOL InitFromXml( pugi::xml_node xmlNode ) = 0;


		virtual HRESULT DefAttributeProc(const SStringW & strAttribName,const SStringW & strValue, BOOL bLoading) = 0;

		/**
         * OnInitFinished
         * @brief    ���Գ�ʼ����ɴ����ӿ�
         * @param    pugi::xml_node xmlNode --  ���Խڵ�
         * @return   void
         * Describe  
         */    
        virtual void OnInitFinished(pugi::xml_node xmlNode) = 0;

		virtual SStringW GetAttribute(const SStringW & strAttr) const = 0;


		/**
         * MarkAttributeHandled
         * @brief    ��־һ�������Ѿ�������
         * @param    pugi::xml_node xmlNode --  ���Խڵ�
         * @return   void
         * Describe  
         */
		static void MarkAttributeHandled(pugi::xml_attribute xmlAttr, boolean bHandled)
		{
			xmlAttr.set_userdata(bHandled?1:0);
		}


		/**
         * IsAttributeHandled
         * @brief    ���һ�������Ƿ��Ѿ�������
         * @param    pugi::xml_node xmlNode --  ���Խڵ�
         * @return   bool true-�Ѿ�������
         * Describe  
         */
		static bool IsAttributeHandled(pugi::xml_attribute xmlAttr)
		{
			return xmlAttr.get_userdata()!=0;
		}
    };

    /**
     * sobj_cast
     * @brief    SOUI Object �����Ͱ�ȫ������ת���ӿ�
     * @param    SObject * pObj --  Դ����
     * @return   T * -- ת����Ķ���
     * Describe  ���Դ�����Ǵ�ת���������ͣ�����NULL
     */    
    template<class T>
    T * sobj_cast(IObject *pObj)
    {
        if(!pObj)
            return NULL;

        if(pObj->IsClass(T::GetClassName()))
            return (T*)pObj;
        else
            return NULL;
    }

}//namespace SOUI