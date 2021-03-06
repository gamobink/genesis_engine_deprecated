/****************************************************************************
Copyright (c) 2011-2013,WebJet Business Division,CYOU
 
http://www.genesis-3d.com.cn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
using System;
using System.Runtime.CompilerServices;
using ScriptRuntime;

namespace ScriptRuntime
{ 
    public partial class Application    {
        // - internal call declare
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static bool ICall_Application_OpenScene(String sceneResID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static bool ICall_Application_CloseScene(String sceneResID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static void ICall_Application_Quit();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static Scene ICall_Application_GetCurrentScene();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static int ICall_Application_GetCurrentSceneState();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static int ICall_Application_GetSceneState(String sceneResID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static string ICall_Application_GetAssetPath();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static string ICall_Application_GetAppDicOnMobile();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static float ICall_Application_GetDPIHeight();

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern private static float ICall_Application_GetDPIWidth();
    }
} 
