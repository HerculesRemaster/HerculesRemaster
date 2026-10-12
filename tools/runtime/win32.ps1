# Shared Win32 helpers for the runtime scripts (dot-source this file).
if (-not ("HercWin" -as [type])) {
    Add-Type @'
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

public class HercWin {
    public class Win { public IntPtr Handle; public string Title; public string Class; public int Left, Top, Right, Bottom; }
    delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential)] struct RECT { public int L, T, R, B; }
    [DllImport("user32.dll")] static extern bool EnumWindows(EnumProc p, IntPtr l);
    [DllImport("user32.dll")] static extern bool EnumChildWindows(IntPtr h, EnumProc p, IntPtr l);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetWindowText(IntPtr h, StringBuilder s, int n);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetClassName(IntPtr h, StringBuilder s, int n);
    [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr h, out uint p);
    [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
    [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr h, IntPtr hdc, uint flags);
    [StructLayout(LayoutKind.Sequential)] struct POINT { public int X, Y; }
    [DllImport("user32.dll")] static extern bool GetClientRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] static extern bool ClientToScreen(IntPtr h, ref POINT p);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetWindowTextW(IntPtr h, StringBuilder s, int n);
    // Client area in screen pixels: x, y, width, height.
    public static int[] ClientRect(IntPtr h) {
        RECT r; GetClientRect(h, out r); var p = new POINT();
        ClientToScreen(h, ref p);
        return new int[] { p.X, p.Y, r.R - r.L, r.B - r.T };
    }
    public static string Title(IntPtr h) { return Text(h); }
    public static uint Pid(IntPtr h) { uint p; GetWindowThreadProcessId(h, out p); return p; }
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
    [DllImport("user32.dll")] static extern void keybd_event(byte vk, byte scan, uint flags, UIntPtr extra);
    [DllImport("user32.dll")] static extern uint MapVirtualKey(uint code, uint type);

    static string Text(IntPtr h) { var s = new StringBuilder(512); GetWindowText(h, s, 512); return s.ToString(); }
    static string Cls(IntPtr h) { var s = new StringBuilder(256); GetClassName(h, s, 256); return s.ToString(); }

    public static List<Win> Windows(uint pid) {
        var r = new List<Win>();
        EnumWindows((h, l) => {
            uint p; GetWindowThreadProcessId(h, out p);
            if (p == pid && IsWindowVisible(h)) {
                RECT rc; GetWindowRect(h, out rc);
                r.Add(new Win { Handle = h, Title = Text(h), Class = Cls(h), Left = rc.L, Top = rc.T, Right = rc.R, Bottom = rc.B });
            }
            return true;
        }, IntPtr.Zero);
        return r;
    }

    public static string ChildText(IntPtr parent) {
        var parts = new List<string>();
        EnumChildWindows(parent, (h, l) => { var t = Text(h); if (t.Length > 0) parts.Add(t); return true; }, IntPtr.Zero);
        return string.Join(" | ", parts);
    }

    // Hardware-level key press (scan code), reaches GetAsyncKeyState of the foreground window.
    public static void Key(byte vk, int holdMs) {
        byte sc = (byte)MapVirtualKey(vk, 0);
        keybd_event(vk, sc, 0, UIntPtr.Zero);
        System.Threading.Thread.Sleep(holdMs);
        keybd_event(vk, sc, 2, UIntPtr.Zero);
    }
}
'@
}
[void][HercWin]::SetProcessDPIAware()  # physical pixels for window rects and screen capture
Add-Type -AssemblyName System.Drawing, System.Windows.Forms
function Save-Screenshot([string]$Path, [IntPtr]$Hwnd) {
    # Captures only the game window's own content (PrintWindow, PW_RENDERFULLCONTENT), never the
    # desktop or other applications, even when the window is covered.
    $wr = $null
    foreach ($w in [HercWin]::Windows([HercWin]::Pid($Hwnd))) { if ($w.Handle -eq $Hwnd) { $wr = $w } }
    if (-not $wr) { return }
    $w = $wr.Right - $wr.Left; $h = $wr.Bottom - $wr.Top
    $bmp = New-Object System.Drawing.Bitmap $w, $h
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $hdc = $g.GetHdc()
    [void][HercWin]::PrintWindow($Hwnd, $hdc, 2)
    $g.ReleaseHdc($hdc)
    $bmp.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
}
