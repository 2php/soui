/**
 * Copyright (C) 2014-2050 SOUI团队
 * All rights reserved.
 * 
 * @file       SDropDown.h
 * @brief      一个通用的下拉窗口模块
 * @version    v1.0      
 * @author     soui      
 * @date       2014-05-25
 * 
 * Describe    此类是下拉窗口的父类 只需要派生该类即可
 */
#pragma once
#include "core/shostwnd.h"

namespace SOUI
{

    class SDropDownWnd;
    /**
     * @class      ISDropDownOwner
     * @brief      ISDropDownOwner接口类
     * 
     * Describe    下拉窗口接口类 里面只定义一些虚函数
     */
    class SOUI_EXP ISDropDownOwner
    {
    public:
        /**
         * ISDropDownOwner::GetDropDownOwner
         * @brief    获得下拉窗口所属者
         *
         * Describe  此函数是纯虚函数  
         */
        virtual SWindow * GetDropDownOwner() =0;
        
        /**
         * ISDropDownOwner::OnDropDown
         * @brief    下拉窗口发生DropDown,触发此函数
         *
         * Describe  此函数是纯虚函数
         */        
        virtual void OnDropDown(SDropDownWnd *pDropDown) = 0;
        
        /**
         * ISDropDownOwner::OnCloseUp
         * @brief    下拉窗口发生CloseUp,触发此函数
         *
         * Describe  此函数是纯虚函数  
         */
        virtual void OnCloseUp(SDropDownWnd *pDropDown,UINT uCode) = 0;
    };

    /**
     * @class      SDropDownWnd
     * @brief      下拉窗口类
     * 
     * Describe    下拉窗口类 只需要继承此类即可
     */
    class SOUI_EXP SDropDownWnd : public SHostWnd, public IMessageFilter
    {
    public:
    
        /**
         * SDropDownWnd::SDropDownWnd
         * @brief    构造函数
         * @param    ISDropDownOwner* pOwner -- 所属者指针
         *
         * Describe  SDropDownWnd类的构造函数
         */        
        SDropDownWnd(ISDropDownOwner* pOwner);

        /**
         * SDropDownWnd::~SDropDownWnd
         * @brief    析构函数
         *
         * Describe  SDropDownWnd类的析构函数
         */        
        virtual ~SDropDownWnd();

        /**
         * SDropDownWnd::Create
         * @brief    下拉窗口创建函数
         * @param     LPCRECT lpRect -- 下拉窗口大小
         * @param     LPVOID lParam -- 与下拉窗口关联的数据
         * @param     DWORD dwStyle -- 下拉窗口样式
         * @param     DWORD dwExStyle -- 下拉窗口扩展样式
         * 
         * Describe  此函数是虚函数，用于指定下拉窗口相关属性。比如样式，大小等
         */        
        virtual BOOL Create(LPCRECT lpRect,LPVOID lParam,DWORD dwStyle=WS_POPUP,DWORD dwExStyle=WS_EX_TOOLWINDOW|WS_EX_TOPMOST);

        /**
         * SDropDownWnd::EndDropDown
         * @brief    下拉窗口销毁
         * @param    UINT uCode -- 消息码 默认是IDCANCEL
         * 
         * Describe  此函数是用于销毁下拉窗口(注释可能不对 请大家指点)
         */
        void EndDropDown(UINT uCode=IDCANCEL);
    protected:
        virtual BOOL PreTranslateMessage(MSG* pMsg);

        /**
         * SDropDownWnd::OnLButtonDown
         * @brief    下拉窗口鼠标左键按下事件
         * @param    UINT nFlags -- 标志
         * @param    CPoint point -- 鼠标坐标
         *
         * Describe  此函数是消息响应函数
         */
        void OnLButtonDown(UINT nFlags, CPoint point);

        /**
         * SDropDownWnd::OnLButtonDown
         * @brief    下拉窗口鼠标左键抬起事件
         * @param    UINT nFlags -- 标志
         * @param    CPoint point -- 鼠标坐标
         *
         * Describe  此函数是消息响应函数
         */
        void OnLButtonUp(UINT nFlags, CPoint point);

        /**
         * SDropDownWnd::OnKeyDown
         * @brief    键盘按下事件
         * @param    UINT nChar -- 按键对应的码值 
         * @param    UINT nRepCnt -- 重复次数
         * @param    UINT nFlags -- 标志
         * 
         * Describe  此函数是消息响应函数
         */        
        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

        /**
         * SDropDownWnd::OnKillFocus
         * @brief    下拉窗口失去焦点消息响应函数
         * @param    HWND wndFocus -- 获得焦点窗口句柄
         * 
         * Describe  下拉窗口失去焦点时候调用此函数，参数是获得焦点的窗口句柄
         */        
        void OnKillFocus(HWND wndFocus);

        /**
         * SDropDownWnd::OnDestroy
         * @brief    下拉窗口销毁
         * 
         * Describe  此函数是用于销毁下拉窗口
         */        
        void OnDestroy();

        /**
         * SDropDownWnd::OnActivateApp
         * @brief    下拉窗口激活或者停止
         * @param    BOOL bActive  -- 窗口激活标志
         * @param    DWORD dwThreadID -- 线程ID
         * 
         * Describe  当下拉窗口激活时候调用此函数。详细说明，请查看MSDN
         */        
        void OnActivateApp(BOOL bActive, DWORD dwThreadID);

        /**
         * SDropDownWnd::OnMouseActivate
         * @brief    鼠标点击窗口使窗口活动
         * @param    HWND wndTopLevel  -- 顶级窗口
         * @param    UINT nHitTest -- 指定命中测试区号
         * @param    UINT message -- 鼠标消息码
         * 
         * Describe  当光标在无效窗口时，并且用户按下鼠标按钮调用此函数 
         *           详细说明，请查看MSDN
         */        
        int OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message);
        
         /**
         * SDropDownWnd::OnActivate
         * @brief    下拉窗口销毁
         * @param    UINT nState  -- 激活类型
         * @param    BOOL bMinimized -- 最小化标志
         * @param    HWND wndOther -- 原活动窗口
         * 
         * Describe  
         *           详细说明，请查看MSDN
         */        
        void OnActivate(UINT nState, BOOL bMinimized, HWND wndOther){}//中断消息处理，防止设置焦点

        virtual void OnFinalMessage(HWND);
        
    protected:
        ISDropDownOwner* m_pOwner; /**< 窗口所属者 */
        BOOL                m_bClick; /**< 单击状态 */
        UINT                m_uExitCode; /**< 退出消息码 */

        BEGIN_MSG_MAP_EX(SDropDownWnd)
            MSG_WM_LBUTTONDOWN(OnLButtonDown)
            MSG_WM_LBUTTONUP(OnLButtonUp)
            MSG_WM_KEYDOWN(OnKeyDown);
            MSG_WM_KILLFOCUS(OnKillFocus)
            MSG_WM_DESTROY(OnDestroy)
            MSG_WM_ACTIVATE(OnActivate)
            MSG_WM_ACTIVATEAPP(OnActivateApp)
            MSG_WM_MOUSEACTIVATE(OnMouseActivate)
            CHAIN_MSG_MAP(SHostWnd)
        END_MSG_MAP()
    };

}//end of namespace SOUI
